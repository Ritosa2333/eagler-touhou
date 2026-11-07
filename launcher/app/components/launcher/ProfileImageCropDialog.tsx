import {lazy, Suspense, useEffect, useRef, useState, type RefObject} from 'react';
import type {Area, MediaSize} from 'react-easy-crop';
import {LoaderCircle, Minus, Plus, RotateCcw} from 'lucide-react';
import {Dialog, DialogContent, DialogDescription, DialogTitle} from '../../ui/dialog';
import {Button} from '../../ui/button';
import {Slider} from '../../ui/slider';
import {useLocale} from '../../i18n';
import {prepareRoomAvatarThumbnail, PROFILE_IMAGE_ASPECT, type ProfileImageCrop, type ProfileImageKind} from '../../services/profile-image';
import 'react-easy-crop/react-easy-crop.css';
import './profile-image-crop.css';
const Cropper=lazy(()=>import('react-easy-crop'));

export default function ProfileImageCropDialog({file,kind,onConfirm,onClose}: {
  file:File;kind:ProfileImageKind;onConfirm(crop:ProfileImageCrop, thumbnail?:string):void;onClose():void;
}) {
  const {t}=useLocale();
  const [source,setSource]=useState('');
  useEffect(()=>{const url=URL.createObjectURL(file);setSource(url);return()=>URL.revokeObjectURL(url);},[file]);
  const [crop,setCrop]=useState({x:0,y:0}),[zoom,setZoom]=useState(1),[area,setArea]=useState<Area>(),[failed,setFailed]=useState(false);
  const [cropSize,setCropSize]=useState<{width:number;height:number}>();
  const media = useRef<RefObject<HTMLImageElement> | null>(null);
  const [preview, setPreview] = useState<{area: Area; image: string}>();
  useEffect(() => {
    const image = media.current?.current;
    if (kind !== 'avatar' || failed || !area || !image) return;
    let current = true;
    const timer = window.setTimeout(async () => {
      const thumbnail = await prepareRoomAvatarThumbnail(image, area).catch(() => '');
      if (current) setPreview({area, image: thumbnail});
    }, 100);
    return () => { current = false; window.clearTimeout(timer); };
  }, [area, failed, kind]);
  const thumbnailReady = kind === 'cover' || (preview?.area === area && !!preview?.image);
  const thumbnailFailed = kind === 'avatar' && preview && preview.area === area && !preview.image;
  const zoomTo=(value:number)=>setZoom(Math.max(1,Math.min(4,value)));
  return <Dialog open onOpenChange={open=>{if(!open)onClose();}}><DialogContent className="profile-image-crop-dialog">
    <DialogTitle>{t(kind==='avatar'?'profile.image.cropAvatar':'profile.image.cropCover')}</DialogTitle>
    <DialogDescription>{t('profile.image.cropHint')}</DialogDescription>
    <div className="profile-image-crop-stage" data-kind={kind}>
      {!failed && source && <Suspense fallback={<span className="profile-image-crop-loading" role="status"><LoaderCircle className="animate-spin motion-reduce:animate-none"/>{t('profile.image.processing')}</span>}><Cropper image={source} crop={crop} zoom={zoom} aspect={PROFILE_IMAGE_ASPECT[kind]} maxZoom={4}
        onCropChange={setCrop} onZoomChange={setZoom} onCropAreaChange={setArea} onCropSizeChange={setCropSize}
        setImageRef={ref=>{media.current=ref;}} disableAutomaticStylesInjection
        zoomWithScroll={false} onMediaLoaded={(size:MediaSize)=>{if(size.naturalWidth*size.naturalHeight>60_000_000)setFailed(true);}}
        mediaProps={{onError:()=>setFailed(true)}}/></Suspense>}
      {kind==='cover'&&cropSize&&!failed&&<div className="profile-image-mobile-safe" aria-hidden="true" style={{width:cropSize.width*(2.8/PROFILE_IMAGE_ASPECT.cover),height:cropSize.height}}/>}
      {failed && <p role="alert">{t('profile.image.readFailed')}</p>}
    </div>
    <div className="profile-image-crop-tools">
      <Button variant="ghost" size="icon" aria-label={t('profile.image.zoomOut')} disabled={failed||zoom<=1} onClick={()=>zoomTo(zoom-0.1)}><Minus/></Button>
      <Slider aria-label={t('profile.image.zoom')} min={1} max={4} step={0.01} value={zoom} onValueChange={zoomTo} disabled={failed}/>
      <Button variant="ghost" size="icon" aria-label={t('profile.image.zoomIn')} disabled={failed||zoom>=4} onClick={()=>zoomTo(zoom+0.1)}><Plus/></Button>
      <Button variant="ghost" size="icon" aria-label={t('profile.image.resetCrop')} onClick={()=>{setCrop({x:0,y:0});setZoom(1);}}><RotateCcw/></Button>
    </div>
    {kind==='cover' && <p className="profile-image-crop-safe-hint">{t('profile.image.coverSafeArea')}</p>}
    {thumbnailFailed && <p role="alert" className="profile-image-crop-safe-hint">{t('profile.image.readFailed')}</p>}
    <div className="profile-image-crop-footer">
      {kind==='avatar' && <div className="profile-image-crop-preview">
        <span>{preview?.image && <img src={preview.image} width={32} height={32} alt=""/>}</span>
        {t('profile.image.roomPreview')}
      </div>}
      <div className="profile-image-crop-actions"><Button variant="outline" onClick={onClose}>{t('action.cancel')}</Button><Button disabled={failed||!area||!thumbnailReady} onClick={()=>{if(area&&thumbnailReady)onConfirm(area,preview?.image);}}>{t('profile.image.useCrop')}</Button></div>
    </div>
  </DialogContent></Dialog>;
}
