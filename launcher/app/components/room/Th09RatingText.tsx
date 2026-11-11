import type {CSSProperties} from 'react';
import {useLocale} from '../../i18n';
import type {Th09Rating} from '../../services/th09-ratings';
import './th09-rating-text.css';

export function Th09RatingText({player, mine, games}: {player?: Th09Rating; mine?: Th09Rating; games?: number}) {
  const {t, locale} = useLocale();
  const difference = player && mine && mine.group === player.group ? player.rating - mine.rating : 0;
  const excess = Math.max(0, Math.abs(difference) - (player?.interval ?? 0));
  const total = games ?? player?.games;
  return <>
    <strong className="th09-rating-number" data-strength={excess ? difference > 0 ? 'higher' : 'lower' : undefined}
      style={excess ? {'--rating-tint': `${Math.min(100, excess / 3)}%`} as CSSProperties : undefined}>{player?.rating ?? '—'}</strong>
    {total !== undefined && <> {t('th09.ratingGames', {games: new Intl.NumberFormat(locale).format(total)})}</>}
  </>;
}
