import {useCallback, useEffect, useRef, useState} from 'react';
import type {SaveProgress} from '../../services/cloud-saves';
import {readThkkdProgress} from '../../services/thkkd-progress';

export type ThkkdProgressView = {
  owner?: string;
  status: 'local'; loading: boolean; failed: boolean; progress?: SaveProgress[]; modifiedAt?: number; retry(): void;
};

export function useThkkdProgress(enabled: boolean): ThkkdProgressView {
  const [result, setResult] = useState<Omit<ThkkdProgressView, 'status' | 'retry'>>({loading: true, failed: false});
  const refresh = useRef<(() => void) | undefined>(undefined);
  const retry = useCallback(() => refresh.current?.(), []);
  useEffect(() => {
    if (!enabled) return;
    let generation = 0;
    const load = () => {
      const current = ++generation;
      void readThkkdProgress(window.indexedDB).then(data => {
        if (current === generation) setResult({loading: false, failed: false, ...data});
      }, () => {
        if (current === generation) setResult({loading: false, failed: true});
      });
    };
    const message = (event: MessageEvent) => {
      const data = event.data;
      if (event.origin === location.origin && data?.protocol === 'eagler-touhou/1' && data.game === 'thkkd'
        && (data.event === 'ready' || data.event === 'exit')) load();
    };
    const visible = () => {if (!document.hidden) load();};
    refresh.current = load;
    load();
    window.addEventListener('message', message);
    window.addEventListener('focus', load);
    document.addEventListener('visibilitychange', visible);
    return () => {
      ++generation;
      refresh.current = undefined;
      window.removeEventListener('message', message);
      window.removeEventListener('focus', load);
      document.removeEventListener('visibilitychange', visible);
    };
  }, [enabled]);
  return {status: 'local', ...result, retry};
}
