import { useEffect, useState } from 'react';
import { useLocale } from '../../i18n';
/** Wall-clock elapsed time is display-only; it never advances game/replay ticks. */
export function RoomElapsed({ phase, startedAt }: { phase: string; startedAt?: number }) {
  const { t } = useLocale(),
    [now, setNow] = useState(Date.now);
  const running = phase !== 'lobby' && !!startedAt;
  useEffect(() => {
    if (!running) return;
    setNow(Date.now());
    const timer = window.setInterval(() => setNow(Date.now()), 1000);
    return () => window.clearInterval(timer);
  }, [running, startedAt]);
  const seconds = running ? Math.max(0, Math.floor((now - startedAt!) / 1000)) : 0;
  return (
    <span className="room-elapsed">
      {t('room.elapsedTime', { time: `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}` })}
    </span>
  );
}
