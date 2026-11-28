import {useEffect, useState, type ReactNode} from 'react';
import {SITE_BRAND_FACES, siteBrandPath} from '../../lib/site-brand.mjs';
import {useLocale, type UiMessageKey} from '../i18n';
import {accountsEnabled} from '../release-features';
import './loading-activity.css';

type LoadingContext = 'solo' | 'multiplayer' | 'install';
export const loadingTips: Record<LoadingContext, readonly UiMessageKey[]> = {
  solo: ['loadingTip.controls', 'loadingTip.touch', 'loadingTip.replay'],
  multiplayer: [...(accountsEnabled ? ['loadingTip.chat'] as const : []),
    'loadingTip.filter', 'loadingTip.spectate', 'loadingTip.touch'],
  install: ['loadingTip.queue', 'loadingTip.favorites'],
};

// Also runs in the document boot script, before the app module is available.
export function chooseLoadingActivity(pool: readonly string[], tipCount: number) {
  const remaining = [...pool];
  return {
    faces: Array.from({length: 4}, () => remaining.splice(Math.floor(Math.random() * remaining.length), 1)[0]!),
    tipIndex: Math.floor(Math.random() * tipCount),
  };
}

export function startLoadingTipRotation(tipCount: number, initialIndex: number, show: (index: number) => boolean | void) {
  let index = initialIndex;
  const interval = window.setInterval(() => {
    index = (index + 1) % tipCount;
    if (show(index) === false) stop();
  }, 5000);
  const stop = () => window.clearInterval(interval);
  return stop;
}

function RotatingLoadingTip({context, initialIndex}: {context: LoadingContext; initialIndex: number}) {
  const {t} = useLocale();
  const tips = loadingTips[context];
  const [index, setIndex] = useState(initialIndex);
  useEffect(() => startLoadingTipRotation(tips.length, initialIndex, setIndex), [tips, initialIndex]);
  return t(tips[index]);
}

interface LoadingActivityProps {
  assetUrl(path: string): string;
  status: string;
  context: LoadingContext;
  compact?: boolean;
  statusId?: string;
  artwork?: ReactNode;
}

export function LoadingCover(props: LoadingActivityProps) {
  return <div className="player-loading-cover" role="status"><LoadingActivity {...props}/></div>;
}

export function LoadingActivity({assetUrl, status, context, compact = false, statusId, artwork}: LoadingActivityProps) {
  const label = status.replace(/(?:…|\.{3})$/, '');
  const [{faces, tipIndex}] = useState(() => chooseLoadingActivity(SITE_BRAND_FACES, loadingTips[context].length));
  return <LoadingActivityContent faceSources={artwork == null ? faces.map(face => assetUrl(siteBrandPath(face))) : []} artwork={artwork}
    label={label} dots={label !== status} tip={<RotatingLoadingTip key={context} context={context}
      initialIndex={tipIndex % loadingTips[context].length}/>} context={context} compact={compact} statusId={statusId}/>;
}

export function LoadingActivityContent({faceSources, label, tip, dots, context, compact, statusId, artwork}: {
  faceSources: readonly (string | undefined)[];
  label: string;
  tip: ReactNode;
  dots: boolean;
  context?: LoadingContext;
  compact?: boolean;
  statusId?: string;
  artwork?: ReactNode;
}) {
  return <div className="loading-activity" data-loading-context={context} data-compact={compact || undefined} data-artwork={artwork != null || undefined}>
    {artwork ?? <div className="loading-activity-faces" aria-hidden="true">
      {faceSources.map((src, index) => <img key={src ?? index} src={src} alt="" draggable={false}/>)}
    </div>}
    <div className="loading-activity-copy">
      <strong className="loading-activity-status" id={statusId}><span>{label}</span>{dots &&
        <span className="loading-activity-dots" aria-hidden="true"><span>.</span><span>.</span><span>.</span></span>}
      </strong>
      <p className="loading-activity-tip" aria-live="off"><b>TIP</b><span>{tip}</span></p>
    </div>
  </div>;
}
