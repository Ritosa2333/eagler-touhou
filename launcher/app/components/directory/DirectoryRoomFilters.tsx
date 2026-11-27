import {Check, ListFilter} from 'lucide-react';
import {useMemo} from 'react';
import {useLocale} from '../../i18n';
import type {DirectoryRoom} from '../../services/lobby-directory';
import {ActionMenu, ActionMenuCheckboxItem, ActionMenuGroup, ActionMenuLabel} from '../../ui/action-menu';
import {BookButton} from '../launcher/BookButton';
import {countDirectoryRoomFacets, directoryRoomDifficulties, directoryRoomModes, directoryRoomStatuses, hasDirectoryRoomFilters, type DirectoryRoomFiltersValue} from './directory-room-filter';

type Facet = keyof DirectoryRoomFiltersValue;
const playerCounts = [0, 1, 2, 3];

interface RoomFilterProps {
  rooms: readonly DirectoryRoom[];
  value: DirectoryRoomFiltersValue;
  onChange(value: DirectoryRoomFiltersValue): void;
}

export function DirectoryRoomFilters({rooms, value, onChange}: RoomFilterProps) {
  const {t} = useLocale();
  const label = t('directory.filters');
  return <ActionMenu variant="selection" align="start" sideOffset={10} minimumWidth={0} label={label}
    className="book-room-filter-popup" trigger={<BookButton variant="toolbar" type="button" className="book-room-filter-trigger px-4 text-xs"
      aria-label={label} title={label} data-active={hasDirectoryRoomFilters(value)}><ListFilter aria-hidden="true"/><span>{label}</span></BookButton>}>
    <RoomFilterOptions rooms={rooms} value={value} onChange={onChange}/>
  </ActionMenu>;
}

function RoomFilterOptions({rooms, value, onChange}: RoomFilterProps) {
  const {t} = useLocale();
  const counts = useMemo(() => countDirectoryRoomFacets(rooms, value), [rooms, value]);
  function facet<K extends Facet>(key: K, heading: string, options: readonly DirectoryRoomFiltersValue[K][number][],
    text: (option: DirectoryRoomFiltersValue[K][number]) => string, allLabel = t('lobby.all')) {
    const selected: readonly DirectoryRoomFiltersValue[K][number][] = value[key];
    const forOption = (option: DirectoryRoomFiltersValue[K][number] | null) => {
      const checked = option === null ? !selected.length : selected.includes(option);
      const next = option === null ? [] : checked ? selected.filter(item => item !== option) : [...selected, option];
      const count = option === null ? counts[key].total : counts[key].options.get(option) ?? 0;
      return <ActionMenuCheckboxItem key={option ?? 'all'} checked={checked} showIndicator={false}
        className="book-room-filter-option" data-room-facet={key} data-room-value={option ?? 'all'}
        onSelect={event => event.preventDefault()} onCheckedChange={() => onChange({...value, [key]: next})}>
        <span className="book-room-filter-box" aria-hidden="true">{checked && <Check />}</span>
        <span>{option === null ? allLabel : text(option)}</span>
        <span className="book-room-filter-count">{count}</span>
      </ActionMenuCheckboxItem>;
    };
    return <ActionMenuGroup className="book-room-filter-facet" aria-label={heading}>
      <ActionMenuLabel className="book-room-filter-label">{heading}</ActionMenuLabel>
      {[forOption(null), ...options.map(forOption)]}
    </ActionMenuGroup>;
  }
  return (
    <div className="book-room-filter-groups">
      <div className="book-room-filter-column book-room-filter-status-mode">
        {facet('status', t('lobby.state'), directoryRoomStatuses, status => t(`directory.filter.${status}`), t('directory.filter.all'))}
        {facet('mode', t('directory.filterMode'), directoryRoomModes, mode => mode === 'pvp' ? 'PvP' : 'PvE')}
      </div>
      <div className="book-room-filter-column">
        {facet('difficulty', t('lobby.difficulty'), directoryRoomDifficulties, difficulty => difficulty)}
      </div>
      <div className="book-room-filter-column">
        {facet('players', t('lobby.capacity'), playerCounts, String)}
      </div>
    </div>
  );
}
