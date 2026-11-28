import {useEffect, useState, type ReactNode} from 'react';
import {useLocale, type UiMessageKey} from '../i18n';
import {accountsEnabled} from '../release-features';
import {LoadingScene, loadingSceneOptions, type LoadingSceneId} from './loading-scene';
import './loading-activity.css';

type LoadingContext = 'solo' | 'multiplayer' | 'install';
export const loadingTips: Record<LoadingContext, readonly UiMessageKey[]> = {
  solo: ['loadingTip.controls', 'loadingTip.touch', 'loadingTip.replay'],
  multiplayer: [...(accountsEnabled ? ['loadingTip.chat'] as const : []),
    'loadingTip.filter', 'loadingTip.spectate', 'loadingTip.touch'],
  install: ['loadingTip.queue', 'loadingTip.favorites'],
};

// The parser uses the same uniform draw before the app module is available.
export function chooseLoadingIndex(count: number) {
  return Math.floor(Math.random() * count);
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
  scene?: LoadingSceneId;
}

export function LoadingCover(props: LoadingActivityProps) {
  return <div className="player-loading-cover" role="status"><LoadingActivity {...props}/></div>;
}

export function LoadingActivity({assetUrl, status, context, compact = false, statusId, scene}: LoadingActivityProps) {
  const label = status.replace(/(?:…|\.{3})$/, '');
  const [randomScene] = useState(() => loadingSceneOptions[chooseLoadingIndex(loadingSceneOptions.length)].id);
  const [tipIndex] = useState(() => chooseLoadingIndex(loadingTips[context].length));
  return <LoadingActivityContent artwork={<LoadingScene key={scene ?? randomScene} scene={scene ?? randomScene} assetUrl={assetUrl} compact={compact}/>}
    label={label} dots={label !== status} tip={<RotatingLoadingTip key={context} context={context}
      initialIndex={tipIndex % loadingTips[context].length}/>} context={context} compact={compact} statusId={statusId}/>;
}

export function LoadingActivityContent({label, tip, dots, context, compact, statusId, artwork}: {
  label: string;
  tip: ReactNode;
  dots: boolean;
  context?: LoadingContext;
  compact?: boolean;
  statusId?: string;
  artwork: ReactNode;
}) {
  return <div className="loading-activity" data-loading-context={context} data-compact={compact || undefined}>
    {artwork}
    <div className="loading-activity-copy">
      <strong className="loading-activity-status" id={statusId}><span>{label}</span>{dots &&
        <span className="loading-activity-dots" aria-hidden="true"><span>.</span><span>.</span><span>.</span></span>}
      </strong>
      <p className="loading-activity-tip" aria-live="off"><b>TIP</b><span>{tip}</span></p>
    </div>
  </div>;
}
