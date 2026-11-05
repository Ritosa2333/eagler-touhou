import type imageCompression from 'browser-image-compression';
import { launcherBaseUrl } from '../navigation/addresses';
import { createBrowserPorts } from './browser-ports';

export type ProfileImageKind = 'avatar' | 'cover';
export const PROFILE_IMAGE_MAX_BYTES = 150_000;
export const PROFILE_IMAGE_SOURCE_MAX_BYTES = 20 * 1024 * 1024;
export class ProfileImageError extends Error {
  constructor(public readonly code: 'invalid' | 'sourceTooLarge' | 'readFailed') {
    super(code);
    this.name = 'ProfileImageError';
  }
}
export interface ProfileImageCrop {x:number;y:number;width:number;height:number}
export const PROFILE_IMAGE_ASPECT = {avatar:1,cover:17/4};
export function validateProfileImage(file:File) {
  if (!['image/png', 'image/jpeg', 'image/webp'].includes(file.type) || !file.size) throw new ProfileImageError('invalid');
  if (file.size>PROFILE_IMAGE_SOURCE_MAX_BYTES) throw new ProfileImageError('sourceTooLarge');
}

/** Crop the original once, at its final dimensions, before lossy compression. */
export async function prepareCroppedProfileImage(file:File,kind:ProfileImageKind,crop:ProfileImageCrop,signal?:AbortSignal) {
  validateProfileImage(file);abortIfNeeded(signal);
  if (![crop.x,crop.y,crop.width,crop.height].every(Number.isFinite) || crop.x<0 || crop.y<0 || crop.width<=0 || crop.height<=0 || crop.x+crop.width>100.01 || crop.y+crop.height>100.01) throw new ProfileImageError('invalid');
  const url=URL.createObjectURL(file),image=new Image();
  const cancel=()=>{image.src='';};
  try {
    await new Promise<void>((resolve,reject)=>{
      const abort=()=>{done(new DOMException('Image processing cancelled','AbortError'));cancel();};
      const done=(error?:Error)=>{image.onload=null;image.onerror=null;signal?.removeEventListener('abort',abort);if(error)reject(error);else resolve();};
      image.onload=()=>done();image.onerror=()=>done(new ProfileImageError('readFailed'));
      signal?.addEventListener('abort',abort,{once:true});image.src=url;
    });
    abortIfNeeded(signal);
    const width=image.naturalWidth*crop.width/100,height=image.naturalHeight*crop.height/100;
    if (!width || !height || image.naturalWidth*image.naturalHeight>60_000_000) throw new ProfileImageError('invalid');
    if(Math.abs(width/height-PROFILE_IMAGE_ASPECT[kind])>0.01)throw new ProfileImageError('invalid');
    const scale=Math.min(1,(kind==='avatar'?512:1600)/Math.max(width,height)),canvas=document.createElement('canvas');
    canvas.width=Math.max(1,Math.round(width*scale));canvas.height=Math.max(1,Math.round(height*scale));
    const context=canvas.getContext('2d');if(!context)throw new ProfileImageError('readFailed');
    context.imageSmoothingQuality='high';
    context.drawImage(image,image.naturalWidth*crop.x/100,image.naturalHeight*crop.y/100,width,height,0,0,canvas.width,canvas.height);
    const blob=await new Promise<Blob>((resolve,reject)=>canvas.toBlob(value=>value?resolve(value):reject(new ProfileImageError('readFailed')),'image/png'));
    canvas.width=canvas.height=0;abortIfNeeded(signal);
    return prepareProfileImage(new File([blob],'profile.png',{type:'image/png'}),kind,signal);
  } finally {cancel();URL.revokeObjectURL(url);}
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
      maxWidthOrHeight: kind === 'avatar' ? 512 : 1600,
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
