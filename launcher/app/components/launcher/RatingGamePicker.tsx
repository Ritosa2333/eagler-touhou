import {ChevronDown} from 'lucide-react';
import {PRODUCT_GAMES, type GameId} from '../../../src/contracts/product-catalog.mts';
import {useLocale} from '../../i18n';
import {ActionMenu, ActionMenuRadioGroup, ActionMenuRadioItem} from '../../ui/action-menu';
import {Button} from '../../ui/button';
import {bookGameArtwork} from './book-artwork';
import {BookMultiplayerMenuGroup} from './BookMultiplayerMenuGroup';
import {BookWorkNumber} from './BookWorkNumber';
import {compareWorkNumbers, groupMultiplayerWorks} from './products';
import './rating-game-picker.css';

const ratingGames = ['th03', 'th075', 'th09', 'th19'] as const satisfies readonly GameId[];
type RatingGame = typeof ratingGames[number];
const groups = groupMultiplayerWorks(ratingGames.map(gameId => ({gameId, ...PRODUCT_GAMES[gameId]})).sort(compareWorkNumbers));

export function RatingGamePicker({current}: {current: RatingGame}) {
  const {t} = useLocale();
  const label = t('lobby.game');
  return <ActionMenu label={label} variant="selection" align="start" trigger={
    <Button variant="outline" size="sm" className="rating-game-picker"
      aria-label={`${label}: ${PRODUCT_GAMES[current].title}`}>
      <RatingGameIcon game={current} />
      <BookWorkNumber gameId={current} number={PRODUCT_GAMES[current].number} />
      <ChevronDown aria-hidden="true" />
    </Button>
  }>
    <ActionMenuRadioGroup value={current}>
      {groups.map(({id, products}) => <BookMultiplayerMenuGroup key={id} group={id} data-rating-category={id}>
        {products.map(({gameId, title}) => <ActionMenuRadioItem key={gameId} value={gameId} data-game={gameId} disabled={gameId !== current}>
          <RatingGameIcon game={gameId} />
          <span>{title}</span>
        </ActionMenuRadioItem>)}
      </BookMultiplayerMenuGroup>)}
    </ActionMenuRadioGroup>
  </ActionMenu>;
}

function RatingGameIcon({game}: {game: RatingGame}) {
  return <img className="rating-game-picker-icon" src={bookGameArtwork(game)} width={22} height={22} alt="" />;
}
