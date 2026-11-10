import {useState} from 'react';
import {SiteBrand} from './SiteBrand';

const rootAssetUrl = (path: string) => '/' + path;
export function AccountAvatar({src,alt='',assetUrl=rootAssetUrl}:{src?:string;alt?:string;assetUrl?:(path:string)=>string}) {
  const [failed,setFailed] = useState<string>();
  return <span className="account-avatar">{src && src !== failed
    ? <img src={src} alt={alt} onError={() => setFailed(src)}/>
    : <SiteBrand assetUrl={assetUrl}/>}</span>;
}
