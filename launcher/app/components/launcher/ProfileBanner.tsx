import {useLayoutEffect, useRef, type ReactNode} from 'react';
import {ProfileCover} from './ProfileCover';

export function ProfileBanner({src, loading = false, children}: {
  src?: string; loading?: boolean; children: ReactNode;
}) {
  const banner = useRef<HTMLDivElement>(null);
  useLayoutEffect(() => {
    const element = banner.current;
    const header = document.querySelector<HTMLElement>('.book-masthead');
    if (!element || !header || typeof IntersectionObserver === 'undefined') return;
    let intersection: IntersectionObserver | undefined;
    let headerHeight = -1;
    const observeCover = () => {
      const bounds = header.getBoundingClientRect();
      if (bounds.height === headerHeight) return;
      headerHeight = bounds.height;
      intersection?.disconnect();
      header.toggleAttribute('data-profile-cover-overlap', element.getBoundingClientRect().bottom > bounds.bottom);
      intersection = new IntersectionObserver(([entry], observer) => {
        if (observer === intersection) header.toggleAttribute('data-profile-cover-overlap', entry.isIntersecting);
      }, {rootMargin: `-${headerHeight}px 0px 0px`});
      intersection.observe(element);
    };
    observeCover();
    const resize = typeof ResizeObserver === 'undefined' ? null : new ResizeObserver(observeCover);
    resize?.observe(header, {box: 'border-box'});
    return () => {
      resize?.disconnect();
      intersection?.disconnect();
      intersection = undefined;
      header.removeAttribute('data-profile-cover-overlap');
    };
  }, []);
  return <section id={loading ? undefined : 'profile-overview'} className="profile-page-banner profile-public-section"
    aria-hidden={loading || undefined} aria-labelledby={loading ? undefined : 'profile-name'}>
    <div className="profile-page-banner-art" ref={banner}>
      {!loading && <ProfileCover src={src} className="profile-page-banner-cover" responsive/>}
    </div>
    {children}
  </section>;
}
