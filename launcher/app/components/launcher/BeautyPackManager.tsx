import {Ellipsis, Sparkles} from 'lucide-react';
import {useRef, useState} from 'react';
import type {GameVersion} from '../../../src/contracts/package-read-models.mts';
import type {GameId} from '../../../src/contracts/product-catalog.mts';
import {useLocale} from '../../i18n';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';
import {Button} from '../../ui/button';
import {Toggle} from '../../ui/controls';
import {Dialog, DialogContent, DialogTitle, DialogTrigger} from '../../ui/dialog';
import {bookArtwork} from './book-artwork';
import type {LibraryProduct} from './products';
import './beauty-pack-manager.css';

type BeautyPackWork = Pick<LibraryProduct, 'gameId' | 'title'> & {version: GameVersion};

export function BeautyPackManager({works, onSetVersions, defaultOpen = false}: {
  works: readonly BeautyPackWork[];
  onSetVersions(games: readonly GameId[], value: GameVersion): void;
  defaultOpen?: boolean;
}) {
  const {t} = useLocale();
  const [open, setOpen] = useState(defaultOpen);
  const desktop = useRef<HTMLButtonElement>(null), mobile = useRef<HTMLButtonElement>(null);
  const title = t('beautyPacks.title');
  const more = t('nav.more');
  const enabled = works.filter(work => work.version === 'beauty').length;
  const allGames = works.map(work => work.gameId);
  return <Dialog open={open} onOpenChange={setOpen}>
    <DialogTrigger asChild>
      <Button ref={desktop} variant="outline" size="sm" className="beauty-packs-trigger" data-toolbar-show="wide">
        <Sparkles aria-hidden="true"/>{title}
      </Button>
    </DialogTrigger>
    <ActionMenu label={more} trigger={
      <Button ref={mobile} variant="outline" size="icon" className="beauty-packs-overflow" data-toolbar-show="compact" data-toolbar-icon="" aria-label={more}>
        <Ellipsis aria-hidden="true"/>
      </Button>
    }>
      <ActionMenuItem onAction={() => setOpen(true)}><Sparkles aria-hidden="true"/>{title}</ActionMenuItem>
    </ActionMenu>
    <DialogContent className="beauty-packs-dialog" aria-describedby={undefined}
      onCloseAutoFocus={event => {
        event.preventDefault();
        (desktop.current?.getClientRects().length ? desktop.current : mobile.current)?.focus({preventScroll: true});
      }}>
      <DialogTitle>{title}</DialogTitle>
      {works.length > 0 ? <>
        <div className="beauty-packs-tools">
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
        </div>
        <ul className="beauty-packs-list">
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
        </ul>
      </> : <p className="beauty-packs-empty">{t('beautyPacks.empty')}</p>}
    </DialogContent>
  </Dialog>;
}
