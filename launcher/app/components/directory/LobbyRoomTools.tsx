import {BookButton} from '../launcher/BookButton';
import {useLocale} from '../../i18n';

/** Shared original lobby.html room actions, including its disabled boot state. */
export function LobbyRoomTools({disabled, onCreate, onJoin, onRefresh}: {
  disabled: boolean; onCreate?: () => void; onJoin?: () => void; onRefresh?: () => void;
}) {
  const {t} = useLocale();
  const buttonClass='lobby-button h-auto whitespace-normal px-3 max-[1000px]:px-1 [@media(pointer:coarse)]:px-1 text-xs';
  return <>
    <BookButton variant="primary" className={`${buttonClass} lobby-primary`} id="createButton" type="button" disabled={disabled} onClick={onCreate}>{t('lobby.create')}</BookButton>
    <BookButton variant="action" className={`${buttonClass} lobby-secondary`} id="codeButton" type="button" disabled={disabled} onClick={onJoin}>{t('lobby.byCode')}</BookButton>
    <BookButton variant="action" size="icon" className="lobby-button lobby-refresh" id="refreshRooms" type="button" aria-label={t('lobby.refresh')} onClick={event=>{
      const button=event.currentTarget, icon=button.querySelector('svg');
      if (icon && !button.closest('.less-motion') && !window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
        icon.getAnimations().forEach(animation=>animation.cancel());
        icon.animate([{transform:'rotate(0deg)'},{transform:'rotate(360deg)'}],{duration:650,easing:'cubic-bezier(.22,.7,.2,1)'});
      }
      onRefresh?.();
    }}><svg viewBox="0 0 24 24" aria-hidden="true"><path d="M21.16 7v5h-5M20 12a8 8 0 1 0-2.3 5.7"/></svg></BookButton>
  </>;
}
