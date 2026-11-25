import type {GameId} from '../../../src/contracts/product-catalog.mts';
import type {ReactNode} from 'react';
import {BookWorkNumber} from './BookWorkNumber';

export function WorkIdentity({game, title, number, subtitle, cover, titleId, children}: {
  game: GameId; title: string; number: string; subtitle?: string; cover?: string | null; titleId?: string; children?: ReactNode;
}) {
  return <div className="launcher-work-identity">
    {cover && <img src={cover} alt="" decoding="async"/>}
    <div>
      <div className="launcher-work-name"><strong id={titleId} title={title}>{title}</strong><BookWorkNumber gameId={game} number={number}/></div>
      {subtitle && <span>{subtitle}</span>}
      {children}
    </div>
  </div>;
}
