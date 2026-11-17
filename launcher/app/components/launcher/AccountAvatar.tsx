import {useState} from 'react';
import {SiteBrand} from './SiteBrand';

const rootAssetUrl = (path: string) => '/' + path;
export function AccountAvatar({src,alt='',assetUrl=rootAssetUrl,loading}:{src?:string;alt?:string;assetUrl?:(path:string)=>string;loading?:'lazy'|'eager'}) {
  const [failed,setFailed] = useState<string>();
  return <span className="account-avatar">{src && src !== failed
    ? <img src={src} alt={alt} loading={loading} decoding="async" onError={() => setFailed(src)}/>
    : <SiteBrand assetUrl={assetUrl}/>}</span>;
}
