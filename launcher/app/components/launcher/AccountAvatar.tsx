import {useState, type ReactNode} from 'react';
import {SiteBrand} from './SiteBrand';

const rootAssetUrl = (path: string) => '/' + path;
export function AccountAvatar({src,alt='',assetUrl=rootAssetUrl,loading,fallback}:{src?:string;alt?:string;assetUrl?:(path:string)=>string;loading?:'lazy'|'eager';fallback?:ReactNode}) {
  const [failed,setFailed] = useState<string>();
  return <span className="account-avatar">{src && src !== failed
    ? <img src={src} alt={alt} loading={loading} decoding="async" onError={() => setFailed(src)}/>
    : fallback ?? <SiteBrand assetUrl={assetUrl}/>}</span>;
}
