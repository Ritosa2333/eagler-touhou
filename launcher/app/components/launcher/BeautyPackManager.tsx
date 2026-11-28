import {useId} from 'react';
import type {GameVersion} from '../../../src/contracts/package-read-models.mts';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {Toggle} from '../../ui/controls';
import {bookArtwork} from './book-artwork';
import type {LibraryProduct} from './products';
import './beauty-pack-manager.css';

type BeautyPackWork = Pick<LibraryProduct, 'gameId' | 'title'> & {version: GameVersion};

export function BeautyPackManager({works, onSetVersions}: {
  works: readonly BeautyPackWork[];
  onSetVersions(games: readonly GameId[], value: GameVersion): void;
}) {
  const {t} = useLocale();
  const titleId = useId();
  const enabled = works.filter(work => work.version === 'beauty').length;
  const allGames = works.map(work => work.gameId);
  return <section className="beauty-packs" aria-labelledby={titleId}>
    <header className="beauty-packs-header">
      <h3 id={titleId}>{t('beautyPacks.title')}</h3>
      {works.length > 0 && <div className="beauty-packs-tools">
        <Button variant="outline" size="sm" aria-disabled={enabled === works.length} onClick={() => {
          if (enabled < works.length) onSetVersions(allGames, 'beauty');
        }}>
          {t('beautyPacks.enableAll')}
        </Button>
        <Button variant="outline" size="sm" aria-disabled={enabled === 0} onClick={() => {
          if (enabled > 0) onSetVersions(allGames, 'original');
        }}>
          {t('beautyPacks.restoreAll')}
        </Button>
      </div>}
    </header>
    {works.length > 0 ? <ul className="beauty-packs-list">
      {works.map(work => <li className="beauty-packs-work" key={work.gameId}>
        <img src={bookArtwork(work)} alt="" width={42} height={42} loading="lazy"/>
        <div className="beauty-packs-name">
          <strong>{work.title}</strong>
          <small className="book-work-number">{work.gameId}</small>
        </div>
        <label className="beauty-packs-version">
          <span aria-hidden="true">{t(work.version === 'beauty' ? 'settings.version.beauty' : 'settings.version.original')}</span>
          <Toggle aria-label={work.title} checked={work.version === 'beauty'}
            onCheckedChange={checked => onSetVersions([work.gameId], checked ? 'beauty' : 'original')}/>
        </label>
      </li>)}
    </ul> : <p className="beauty-packs-empty">{t('beautyPacks.empty')}</p>}
  </section>;
}
