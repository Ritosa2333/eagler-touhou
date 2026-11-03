import {BookButton} from '../launcher/BookButton';
import {useLocale} from '../../i18n';
import {MoreHorizontal} from 'lucide-react';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';

/** Shared original lobby.html room actions, including its disabled boot state. */
export function LobbyRoomTools({disabled, onCreate, onJoin, onRefresh, onReplay}: {
  disabled: boolean; onCreate?: () => void; onJoin?: () => void; onRefresh?: () => void; onReplay?: () => void;
}) {
  const {t} = useLocale();
  const buttonClass='lobby-button h-auto whitespace-normal px-3 max-[1000px]:px-1 [@media(pointer:coarse)]:px-1 text-xs';
  return <>
    <BookButton variant="primary" className={`${buttonClass} lobby-primary`} id="createButton" type="button" disabled={disabled} onClick={onCreate}>{t('lobby.create')}</BookButton>
    <BookButton variant="action" className={`${buttonClass} lobby-secondary`} id="codeButton" type="button" disabled={disabled} onClick={onJoin}>{t('lobby.byCode')}</BookButton>
    <ActionMenu label={t('nav.more')} trigger={<BookButton variant="action" size="icon" className="lobby-button lobby-more" type="button" aria-label={t('nav.more')}><MoreHorizontal aria-hidden="true"/></BookButton>}>
      {onReplay && <ActionMenuItem id="watchReplayButton" onAction={onReplay}>{t('lobby.watchReplay')}</ActionMenuItem>}
      <ActionMenuItem id="refreshRooms" onAction={onRefresh}>{t('lobby.refresh')}</ActionMenuItem>
    </ActionMenu>
  </>;
}
