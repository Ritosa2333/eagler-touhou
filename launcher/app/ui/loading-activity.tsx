import {useState} from 'react';
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
export function chooseLoadingActivity<T>(pool: readonly string[], tips: readonly T[]) {
  const remaining = [...pool];
  return {
    faces: Array.from({length: 4}, () => remaining.splice(Math.floor(Math.random() * remaining.length), 1)[0]!),
    tip: tips[Math.floor(Math.random() * tips.length)]!,
  };
}

interface LoadingActivityProps {
  assetUrl(path: string): string;
  status: string;
  context: LoadingContext;
  compact?: boolean;
  statusId?: string;
}

export function LoadingCover(props: LoadingActivityProps) {
  return <div className="player-loading-cover" role="status"><LoadingActivity {...props}/></div>;
}

export function LoadingActivity({assetUrl, status, context, compact = false, statusId}: LoadingActivityProps) {
  const {t} = useLocale();
  const label = status.replace(/(?:…|\.{3})$/, '');
  // Sample without replacement. The lineup and tip belong to this wait, not to progress updates.
  const [{faces, tip}] = useState(() => chooseLoadingActivity(SITE_BRAND_FACES, loadingTips[context]));
  return <LoadingActivityContent faceSources={faces.map(face => assetUrl(siteBrandPath(face)))}
    label={label} dots={label !== status} tip={t(tip)} context={context} compact={compact} statusId={statusId}/>;
}

export function LoadingActivityContent({faceSources, label, tip, dots, context, compact, statusId}: {
  faceSources: readonly (string | undefined)[];
  label: string;
  tip: string;
  dots: boolean;
  context?: LoadingContext;
  compact?: boolean;
  statusId?: string;
}) {
  return <div className="loading-activity" data-loading-context={context} data-compact={compact || undefined}>
    <div className="loading-activity-faces" aria-hidden="true">
      {faceSources.map((src, index) => <img key={src ?? index} src={src} alt="" draggable={false}/>)}
    </div>
    <div className="loading-activity-copy">
      <strong className="loading-activity-status" id={statusId}><span>{label}</span>{dots &&
        <span className="loading-activity-dots" aria-hidden="true"><span>.</span><span>.</span><span>.</span></span>}
      </strong>
      <p className="loading-activity-tip"><b>TIP</b><span>{tip}</span></p>
    </div>
  </div>;
}
