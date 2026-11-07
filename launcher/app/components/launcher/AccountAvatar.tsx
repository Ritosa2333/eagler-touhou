import {useState} from 'react';
import {SiteBrand} from './SiteBrand';

export function AccountAvatar({src,alt=''}:{src?:string;alt?:string}) {
  const [failed,setFailed] = useState<string>();
  return <span className="account-avatar">{src && src !== failed
    ? <img src={src} alt={alt} onError={() => setFailed(src)}/>
    : <SiteBrand assetUrl={path => '/' + path.replace(/^\/+/, '')}/>}</span>;
}
