import { useLocale } from '../../i18n';

/** Room-owned whole-match wins and cumulative confirmed play minutes. */
export function Th09MatchStats({wins, minutes, spectators}: {
  wins: readonly [number, number]; minutes: number; spectators: number;
}) {
  const { t } = useLocale();
  return <aside className="th09-match-stats" aria-label={t('th09.matchInformation')}>
    <strong className="th09-match-score" aria-label={t('th09.score')}>{`<${wins[0]} : ${wins[1]}>`}</strong>
    <span className="th09-match-stat"><svg viewBox="0 0 24 24" aria-hidden="true"><circle cx="12" cy="12" r="9"/><path d="M12 6v6l4 2"/></svg>{t('th09.elapsed',{minutes})}</span>
    <span className="th09-match-stat"><img src="assets/room-users.svg" alt=""/>{t('th09.watching',{count:spectators})}</span>
  </aside>;
}
