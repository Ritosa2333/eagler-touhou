import {ListFilter} from 'lucide-react';
import {useLocale} from '../../i18n';
import type {DirectoryRoom} from '../../services/lobby-directory';
import {ActionMenu, ActionMenuLabel, ActionMenuRadioGroup, ActionMenuRadioItem} from '../../ui/action-menu';
import {BookButton} from '../launcher/BookButton';
import {directoryRoomDifficulties, directoryRoomStatuses, filterDirectoryRooms, hasDirectoryRoomFilters, type DirectoryRoomFiltersValue} from './directory-room-filter';

export function DirectoryRoomFilters({rooms, value, onChange}: {
  rooms: readonly DirectoryRoom[];
  value: DirectoryRoomFiltersValue;
  onChange(value: DirectoryRoomFiltersValue): void;
}) {
  const {t} = useLocale();
  const difficulties = ['', ...directoryRoomDifficulties];
  const playerCounts = ['0', '1', '2', '3'];
  const label = t('directory.filters');
  const count = (next: DirectoryRoomFiltersValue) => <span className="book-room-filter-count">{filterDirectoryRooms(rooms,next).length}</span>;
  const change = <K extends keyof DirectoryRoomFiltersValue>(key: K, next: string, options: readonly DirectoryRoomFiltersValue[K][]) => {
    const selected = options.find(option => option === next);
    if (selected !== undefined && selected !== value[key]) onChange({...value, [key]:selected});
  };
  return <ActionMenu variant="selection" align="start" sideOffset={10} minimumWidth={0} label={label}
    className="book-room-filter-popup" trigger={<BookButton variant="toolbar" type="button" className="book-room-filter-trigger px-4 text-xs"
      aria-label={label} title={label} data-active={hasDirectoryRoomFilters(value)}><ListFilter aria-hidden="true"/><span>{label}</span></BookButton>}>
    <div className="book-room-filter-groups">
      <ActionMenuRadioGroup aria-label={t('lobby.state')} value={value.status} onValueChange={next=>change('status',next,directoryRoomStatuses)}>
        <ActionMenuLabel className="book-room-filter-label">{t('lobby.state')}</ActionMenuLabel>
        {directoryRoomStatuses.map(status=><ActionMenuRadioItem key={status} value={status} data-room-filter={status}>
          {t(`directory.filter.${status}`)}{count({...value,status})}
        </ActionMenuRadioItem>)}
      </ActionMenuRadioGroup>
      <ActionMenuRadioGroup aria-label={t('lobby.difficulty')} value={value.difficulty} onValueChange={next=>change('difficulty',next,difficulties)}>
        <ActionMenuLabel className="book-room-filter-label">{t('lobby.difficulty')}</ActionMenuLabel>
        {difficulties.map(difficulty=><ActionMenuRadioItem key={difficulty} value={difficulty} data-room-difficulty={difficulty}>
          {difficulty || t('lobby.all')}{count({...value,difficulty})}
        </ActionMenuRadioItem>)}
      </ActionMenuRadioGroup>
      <ActionMenuRadioGroup aria-label={t('lobby.capacity')} value={value.players} onValueChange={next=>change('players',next,['',...playerCounts])}>
        <ActionMenuLabel className="book-room-filter-label">{t('lobby.capacity')}</ActionMenuLabel>
        {['',...playerCounts].map(players=><ActionMenuRadioItem key={players} value={players} data-room-players={players}>
          {players || t('lobby.all')}{count({...value,players})}
        </ActionMenuRadioItem>)}
      </ActionMenuRadioGroup>
    </div>
  </ActionMenu>;
}
