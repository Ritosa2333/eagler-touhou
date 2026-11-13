import {useState, type CSSProperties} from 'react';
import {useHref} from 'react-router';
import './profile-cover.css';

const coverAspect = 3;
const wideCoverAspect = coverAspect * 2;

export function ProfileCover({src, className, responsive}: {src?: string; className?: string; responsive?: boolean}) {
  const backgroundUrl = useHref('/assets/launcher-background.webp');
  const [wideSource, setWideSource] = useState<string>();
  return <div className={className ? `profile-cover ${className}` : 'profile-cover'}
    data-default={!src || undefined} data-responsive={responsive || undefined} data-wide={src && src === wideSource ? true : undefined}
    style={{
      aspectRatio: coverAspect,
      '--profile-cover-aspect': coverAspect,
    } as CSSProperties}>
    {responsive && <img className="profile-cover-backdrop" src={src || backgroundUrl} alt="" decoding="async" />}
    <div className="profile-cover-frame">
      <img src={src || backgroundUrl} alt="" decoding="async" onLoad={event => {
        const {naturalWidth, naturalHeight} = event.currentTarget;
        if (src && Math.abs(naturalWidth / naturalHeight - wideCoverAspect) < .15) setWideSource(src);
      }} />
    </div>
  </div>;
}
