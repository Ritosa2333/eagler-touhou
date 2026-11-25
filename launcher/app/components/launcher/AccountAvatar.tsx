import {useEffect, useRef, useState, type ComponentProps, type ReactNode} from 'react';
import {accountRequest} from '../../services/account-client';
import {SiteBrand} from './SiteBrand';

const rootAssetUrl = (path: string) => '/' + path;
export function AccountAvatar({src,alt='',assetUrl=rootAssetUrl,loading,fallback}:{src?:string;alt?:string;assetUrl?:(path:string)=>string;loading?:'lazy'|'eager';fallback?:ReactNode}) {
  const [failed,setFailed] = useState<string>();
  return <span className="account-avatar">{src && src !== failed
    ? <img crossOrigin="anonymous" src={src} alt={alt} loading={loading} decoding="async" onError={() => setFailed(src)}/>
    : fallback ?? <SiteBrand assetUrl={assetUrl}/>}</span>;
}

export function PublicAccountAvatar({username, loading, className, ...props}: Omit<ComponentProps<typeof AccountAvatar>, 'src'> & {username?: string | null; className?: string}) {
  const element = useRef<HTMLSpanElement>(null);
  const [avatar, setAvatar] = useState<{username: string; src?: string}>();
  useEffect(() => {
    if (!username) return;
    const controller = new AbortController();
    const load = () => {
      void accountRequest<{media?: {avatar?: string}; details?: {avatar?: string} | null}>(
        'v1/profiles/' + encodeURIComponent(username), undefined,
        {query: new URLSearchParams({media: 'urls'}), signal: controller.signal},
      ).then(profile => {
        if (!controller.signal.aborted) setAvatar({username, src: profile?.media?.avatar || profile?.details?.avatar});
      }).catch(() => {});
    };
    if (loading !== 'lazy' || !element.current || typeof IntersectionObserver === 'undefined') {
      load();
      return () => controller.abort();
    }
    const observer = new IntersectionObserver(([entry]) => {
      if (!entry?.isIntersecting || controller.signal.aborted) return;
      observer.disconnect();
      load();
    }, {rootMargin: '120px'});
    observer.observe(element.current);
    return () => {observer.disconnect(); controller.abort();};
  }, [username, loading]);
  return <span ref={element} className={className}><AccountAvatar {...props} src={avatar?.username === username ? avatar?.src : undefined} loading={loading}/></span>;
}
