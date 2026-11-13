import type imageCompression from 'browser-image-compression';
import { launcherBaseUrl } from '../navigation/addresses';
import { createBrowserPorts } from './browser-ports';

export type ProfileImageKind = 'avatar' | 'cover';
export const PROFILE_IMAGE_MAX_BYTES = 150_000;
export const PROFILE_IMAGE_SOURCE_MAX_BYTES = 20 * 1024 * 1024;
const PROFILE_COVER_MAX_WIDTH = 1600;
const ROOM_AVATAR_SIZE = 64;
const ROOM_AVATAR_MAX_BYTES = 8 * 1024;
export class ProfileImageError extends Error {
  constructor(public readonly code: 'invalid' | 'sourceTooLarge' | 'readFailed') {
    super(code);
    this.name = 'ProfileImageError';
  }
}
export interface ProfileImageCrop {x:number;y:number;width:number;height:number}
export const PROFILE_IMAGE_ASPECT = {avatar:1,cover:3};
export const PROFILE_DESKTOP_COVER_ASPECT = 2 * PROFILE_IMAGE_ASPECT.cover;
export function validateProfileImage(file:File) {
  if (!['image/png', 'image/jpeg', 'image/webp'].includes(file.type) || !file.size) throw new ProfileImageError('invalid');
  if (file.size>PROFILE_IMAGE_SOURCE_MAX_BYTES) throw new ProfileImageError('sourceTooLarge');
}

async function withProfileImage<T>(url: string, render: (image: HTMLImageElement) => Promise<T>, signal?: AbortSignal) {
  abortIfNeeded(signal);
  const image=new Image();
  const cancel=()=>{image.src='';};
  try {
    await new Promise<void>((resolve,reject)=>{
      const abort=()=>{done(new DOMException('Image processing cancelled','AbortError'));cancel();};
      const done=(error?:Error)=>{image.onload=null;image.onerror=null;signal?.removeEventListener('abort',abort);if(error)reject(error);else resolve();};
      image.onload=()=>done();image.onerror=()=>done(new ProfileImageError('readFailed'));
      signal?.addEventListener('abort',abort,{once:true});image.src=url;
    });
    abortIfNeeded(signal);
    return await render(image);
  } finally {cancel();}
}

function profileCropBounds(image: HTMLImageElement, crop: ProfileImageCrop, aspect: number) {
  if (![crop.x,crop.y,crop.width,crop.height].every(Number.isFinite) || crop.x<0 || crop.y<0 || crop.width<=0 || crop.height<=0 || crop.x+crop.width>100.01 || crop.y+crop.height>100.01) throw new ProfileImageError('invalid');
  const width=image.naturalWidth*crop.width/100,height=image.naturalHeight*crop.height/100;
  if (!width || !height || image.naturalWidth*image.naturalHeight>60_000_000 || Math.abs(width/height-aspect)>0.01) throw new ProfileImageError('invalid');
  return {x:image.naturalWidth*crop.x/100,y:image.naturalHeight*crop.y/100,width,height};
}

function drawProfileCrop(image: HTMLImageElement, crop: ProfileImageCrop, aspect: number, maximum: number) {
  const {x,y,width,height}=profileCropBounds(image,crop,aspect);
  const scale=Math.min(1,maximum/Math.max(width,height)),canvas=document.createElement('canvas');
  canvas.width=Math.max(1,Math.round(width*scale));
  canvas.height=aspect===1?canvas.width:Math.max(1,Math.round(height*scale));
  const context=canvas.getContext('2d');if(!context)throw new ProfileImageError('readFailed');
  context.imageSmoothingQuality='high';
  context.drawImage(image,x,y,width,height,0,0,canvas.width,canvas.height);
  return canvas;
}

function drawCoverPadding(context: CanvasRenderingContext2D, sample: HTMLCanvasElement, image: HTMLImageElement,
  source: ProfileImageCrop, x: number, width: number) {
  const height = context.canvas.height;
  // Resample a reflected strip at low resolution for a soft edge on all canvas implementations.
  sample.height = Math.min(16, height);
  sample.width = Math.max(1, Math.round(width * sample.height / height));
  const preview = sample.getContext('2d');
  if (!preview) throw new ProfileImageError('readFailed');
  preview.imageSmoothingQuality = 'high';
  preview.setTransform(-1, 0, 0, 1, sample.width, 0);
  preview.drawImage(image, source.x, source.y, source.width, source.height, 0, 0, sample.width, sample.height);
  context.drawImage(sample, x, 0, width, height);
}

/** The central half of this 6:1 canvas is the selected 3:1 crop. */
export function drawProfileCover(canvas: HTMLCanvasElement, image: HTMLImageElement, crop: ProfileImageCrop, previewHeight?: number) {
  const aspect = PROFILE_IMAGE_ASPECT.cover;
  const selected = profileCropBounds(image, crop, aspect);
  const height = Math.min(selected.height, selected.width / aspect, image.naturalHeight, image.naturalWidth / aspect);
  const primaryWidth = height * aspect;
  const x = Math.max(0, Math.min(image.naturalWidth - primaryWidth, selected.x + (selected.width - primaryWidth) / 2));
  const y = Math.max(0, Math.min(image.naturalHeight - height, selected.y + (selected.height - height) / 2));
  const wideX = x - primaryWidth / 2;
  const wideRight = wideX + primaryWidth * 2;
  const sourceLeft = Math.max(0, wideX);
  const sourceRight = Math.min(image.naturalWidth, wideRight);
  const leftPadding = sourceLeft - wideX;
  const rightPadding = wideRight - sourceRight;

  const outputHeight = previewHeight ?? Math.max(1, Math.min(Math.floor(height), Math.floor(PROFILE_COVER_MAX_WIDTH / PROFILE_DESKTOP_COVER_ASPECT)));
  const outputWidth = outputHeight * PROFILE_DESKTOP_COVER_ASPECT;
  if (canvas.width !== outputWidth) canvas.width = outputWidth;
  if (canvas.height !== outputHeight) canvas.height = outputHeight;
  const context = canvas.getContext('2d');
  if (!context) throw new ProfileImageError('readFailed');
  context.clearRect(0, 0, outputWidth, outputHeight);
  context.imageSmoothingQuality = 'high';
  const scale = outputHeight / height;
  if (leftPadding > 0 || rightPadding > 0) {
    const sample = document.createElement('canvas');
    if (leftPadding > 0) drawCoverPadding(context, sample, image, {x:0, y, width:leftPadding, height}, 0, leftPadding * scale);
    if (rightPadding > 0) drawCoverPadding(context, sample, image, {x:image.naturalWidth-rightPadding, y, width:rightPadding, height}, outputWidth - rightPadding * scale, rightPadding * scale);
  }
  context.drawImage(image, sourceLeft, y, sourceRight - sourceLeft, height,
    leftPadding * scale, 0, (sourceRight - sourceLeft) * scale, outputHeight);
}

function encodeCanvas(canvas: HTMLCanvasElement, type: string, quality?: number) {
  return new Promise<Blob>((resolve,reject)=>canvas.toBlob(value=>value?resolve(value):reject(new ProfileImageError('readFailed')),type,quality));
}

/** Crop the original once, at its final dimensions, before lossy compression. */
export async function prepareCroppedProfileImage(file:File,kind:ProfileImageKind,crop:ProfileImageCrop,signal?:AbortSignal) {
  validateProfileImage(file);
  const url=URL.createObjectURL(file);
  try {
    const blob = await withProfileImage(url, async image => {
      const canvas = kind === 'cover' ? document.createElement('canvas') : drawProfileCrop(image, crop, PROFILE_IMAGE_ASPECT.avatar, 512);
      try {
        if (kind === 'cover') drawProfileCover(canvas, image, crop);
        return await encodeCanvas(canvas, 'image/png');
      } finally { canvas.width = canvas.height = 0; }
    }, signal);
    return await prepareProfileImage(new File([blob],'profile.png',{type:'image/png'}),kind,signal);
  } finally {URL.revokeObjectURL(url);}
}

export async function prepareRoomAvatarThumbnail(image: HTMLImageElement, crop: ProfileImageCrop) {
  const canvas = drawProfileCrop(image, crop, PROFILE_IMAGE_ASPECT.avatar, ROOM_AVATAR_SIZE);
  try {
    let thumbnail = await encodeCanvas(canvas, 'image/webp', 0.75);
    if (thumbnail.type === 'image/webp' && thumbnail.size > ROOM_AVATAR_MAX_BYTES)
      thumbnail = await encodeCanvas(canvas, 'image/webp', 0.45);
    if (thumbnail.type !== 'image/webp' || thumbnail.size > ROOM_AVATAR_MAX_BYTES)
      thumbnail = await encodeCanvas(canvas, 'image/jpeg', 0.65);
    if (!['image/webp', 'image/jpeg'].includes(thumbnail.type) || !thumbnail.size || thumbnail.size > ROOM_AVATAR_MAX_BYTES)
      throw new ProfileImageError('readFailed');
    return await new Promise<string>((resolve, reject) => {
      const reader = new window.FileReader();
      reader.onload = () => resolve(String(reader.result));
      reader.onerror = () => reject(new ProfileImageError('readFailed'));
      reader.readAsDataURL(thumbnail);
    });
  } finally { canvas.width = canvas.height = 0; }
}

export function prepareStoredAvatarThumbnail(url: string) {
  return withProfileImage(url, image => {
    const size = Math.min(image.naturalWidth, image.naturalHeight);
    const width = 100 * size / image.naturalWidth;
    const height = 100 * size / image.naturalHeight;
    return prepareRoomAvatarThumbnail(image, {x: (100 - width) / 2, y: (100 - height) / 2, width, height});
  });
}

let compressorLoad: Promise<typeof imageCompression> | undefined;
function abortIfNeeded(signal?: AbortSignal) {
  if (signal?.aborted) throw new DOMException('Image processing cancelled', 'AbortError');
}
function imageBaseUrl() {
  return launcherBaseUrl(document.querySelector<HTMLAnchorElement>('#launcherLink')?.href ?? document.baseURI);
}
async function loadCompressor() {
  if (!compressorLoad)
    compressorLoad = createBrowserPorts({ document, window, baseUrl: imageBaseUrl() })
      .loadImageCompressor()
      .catch((error) => {
        compressorLoad = undefined;
        throw error;
      });
  return compressorLoad;
}

/** Normalize local profile pictures before they enter the account request. */
export async function prepareProfileImage(file: File, kind: ProfileImageKind, signal?: AbortSignal): Promise<string> {
  validateProfileImage(file);
  abortIfNeeded(signal);
  try {
    const compress = await loadCompressor();
    abortIfNeeded(signal);
    const options = {
      maxSizeMB: PROFILE_IMAGE_MAX_BYTES / 1024 / 1024,
      maxWidthOrHeight: kind === 'avatar' ? 512 : PROFILE_COVER_MAX_WIDTH,
      useWebWorker: true,
      libURL: new URL('vendor/browser-image-compression.min.js', imageBaseUrl()).href,
      fileType: 'image/webp',
      initialQuality: 0.9,
      maxIteration: 30,
      preserveExif: false,
      signal,
    };
    let image = await compress(file, options);
    abortIfNeeded(signal);
    if (image.size > PROFILE_IMAGE_MAX_BYTES) image = await compress(image, { ...options, maxWidthOrHeight: 384 });
    abortIfNeeded(signal);
    if (
      !image.size ||
      image.size > PROFILE_IMAGE_MAX_BYTES ||
      !['image/png', 'image/jpeg', 'image/webp'].includes(image.type)
    )
      throw new ProfileImageError('readFailed');
    const dataUrl = await compress.getDataUrlFromFile(image);
    abortIfNeeded(signal);
    return dataUrl;
  } catch (error) {
    abortIfNeeded(signal);
    if (error instanceof ProfileImageError) throw error;
    throw new ProfileImageError('readFailed');
  }
}
