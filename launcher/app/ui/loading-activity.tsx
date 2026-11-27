import {useState} from 'react';
import {SITE_BRAND_FACES, siteBrandPath} from '../../lib/site-brand.mjs';
import {useLocale, type UiMessageKey} from '../i18n';
import {accountsEnabled} from '../release-features';
import './loading-activity.css';

type LoadingContext = 'solo' | 'multiplayer' | 'install';
const tips: Record<LoadingContext, readonly UiMessageKey[]> = {
  solo: ['loadingTip.controls', 'loadingTip.touch', 'loadingTip.replay'],
  multiplayer: [...(accountsEnabled ? ['loadingTip.chat'] as const : []),
    'loadingTip.filter', 'loadingTip.spectate', 'loadingTip.touch'],
  install: ['loadingTip.queue', 'loadingTip.favorites'],
};

export function LoadingActivity({assetUrl, status, context, compact = false}: {
  assetUrl(path: string): string;
  status: string;
  context: LoadingContext;
  compact?: boolean;
}) {
  const {t} = useLocale();
  // Sample without replacement. The lineup and tip belong to this wait, not to progress updates.
  const [{faces, tip}] = useState(() => {
    const pool = [...SITE_BRAND_FACES];
    return {
      faces: Array.from({length:4}, () => pool.splice(Math.floor(Math.random() * pool.length), 1)[0]!),
      tip: tips[context][Math.floor(Math.random() * tips[context].length)]!,
    };
  });
  return <div className="loading-activity" data-loading-context={context} data-compact={compact || undefined}>
    <div className="loading-activity-faces" aria-hidden="true">
      {faces.map(face => <img key={face} src={assetUrl(siteBrandPath(face))} alt="" draggable={false}/>)}
    </div>
    <div className="loading-activity-copy">
      <strong className="loading-activity-status">{status}</strong>
      <p className="loading-activity-tip"><b>TIP</b><span>{t(tip)}</span></p>
    </div>
  </div>;
}
