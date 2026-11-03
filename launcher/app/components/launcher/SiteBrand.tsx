import {useLayoutEffect,useSyncExternalStore,type CSSProperties} from 'react';
import {sessionSiteBrandFace,resetSiteBrandFace,siteBrandPath,SITE_BRAND_DEFAULT,SITE_BRAND_RATIO} from '../../../lib/site-brand.mjs';

let face=SITE_BRAND_DEFAULT;
const listeners=new Set<()=>void>();
function subscribe(listener:()=>void){listeners.add(listener);return()=>{listeners.delete(listener);};}
const relative=(path:string)=>path;
function fallback(){resetSiteBrandFace(window);if(face!==SITE_BRAND_DEFAULT){face=SITE_BRAND_DEFAULT;for(const listener of listeners)listener();}}

/** All document instances and page loads in this tab share one character. */
export function SiteBrand({assetUrl=relative,className=''}:{assetUrl?:(path:string)=>string;className?:string}){
  const selected=useSyncExternalStore(subscribe,()=>face,()=>SITE_BRAND_DEFAULT);
  useLayoutEffect(()=>{
    const selected=sessionSiteBrandFace(window);
    if(face!==selected){face=selected;for(const listener of listeners)listener();}
  },[]);
  return <span className={`site-brand ${className}`} style={{'--site-brand-ratio':SITE_BRAND_RATIO} as CSSProperties} aria-hidden="true"><img suppressHydrationWarning src={assetUrl(siteBrandPath(selected))} width="1000" height={Math.round(1000/SITE_BRAND_RATIO)} alt="" decoding="async" data-brand-face={selected} onError={fallback}/></span>;
}
