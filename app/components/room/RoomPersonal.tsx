import {useEffect, useLayoutEffect, useRef, useState} from 'react';
import type {RoomOwnerSnapshot, MultiplayerRoomService} from '../../services/multiplayer-room';
import {useLocale} from '../../i18n';
import {roomPresentation} from './presentation';
export function RoomPersonal({state, service, assetUrl, th09NetworkOverlayOpen, hidden}: {state: RoomOwnerSnapshot; service: MultiplayerRoomService; assetUrl(path: string): string; th09NetworkOverlayOpen: boolean; hidden: boolean}) {
  const {t} = useLocale(), {room, loadout, label, config} = roomPresentation(state, t)!;
  const input = useRef<HTMLInputElement>(null), latest = useRef(service); latest.current = service;
  const moderated = !!state.nickname, moderatedRef = useRef(moderated); moderatedRef.current = moderated;
  const [clock, setClock] = useState(Date.now);
  useEffect(() => {if (!moderated || hidden) return; const timer=setInterval(()=>setClock(Date.now()),60000);return()=>clearInterval(timer);},[moderated,hidden]);
  const queued = state.nickname?.status === 'queued';
  const pending = queued || state.nickname?.status === 'reviewing', locked = !!state.nickname && state.nickname.nextChangeAt > clock;
  const statusKey = state.nickname?.attemptsLeft === 0 ? 'nickname.exhausted' : state.nickname ? `nickname.${state.nickname.status}` as const : 'nickname.idle';
  const detail = queued ? t('nickname.queuedHint') : locked ? t('nickname.nextChange',{time:new Date(state.nickname!.nextChangeAt).toLocaleString(undefined,{month:'numeric',day:'numeric',hour:'2-digit',minute:'2-digit'})}) : state.nickname?.attemptsLeft === 1 ? t('nickname.oneRetry') : state.nickname?.status === 'approved' && state.nickname.name ? t('nickname.firstApproved') : state.nickname?.status === 'idle' ? t('nickname.briefHint') : '';
  const draft = state.nickname?.pendingName || state.displayName;
  const save = () => {if (input.current) input.current.value = latest.current.setDisplayName(input.current.value);};
  const commit = () => {if (!moderatedRef.current) save();};
  useEffect(() => {const node = input.current; node?.addEventListener('change', commit); return () => node?.removeEventListener('change', commit);}, []);
  useLayoutEffect(() => {if (input.current && document.activeElement !== input.current) input.current.value = draft;}, [draft]);
  const editor = <label className="mp-name-editor" id="mpNameEditor"><span>{t('multiplayer.name')}</span><input ref={input} id="mpDisplayName" type="text" maxLength={12} autoComplete="off" placeholder={t('multiplayer.namePlaceholder')} aria-label={t('multiplayer.nameAria')} title={t(moderated ? 'nickname.hint' : 'multiplayer.nameEditableHint')} disabled={moderated && (locked || pending)} defaultValue={draft} onBlur={commit}/></label>;
  const arrow = (id: string, previous: boolean) => config.inGameSelection ? null : <button id={id} type="button" aria-label={t(previous ? 'multiplayer.previousCharacter' : 'multiplayer.nextCharacter')} onClick={() => service.changeLoadout(previous ? -1 : 1)}><img className="room-icon" src={assetUrl(previous ? 'assets/room-caret-left.svg' : 'assets/room-caret-right.svg')} alt=""/></button>;
  return <div className="mp-room-body" data-room-panel="personal" hidden={hidden}>
    {moderated ? <div className="mp-nickname-editor" data-status={state.nickname!.status}>
      {editor}
      <button id="mpNicknameSave" className="lobby-button" type="button" disabled={!state.connected || locked || pending} onClick={save}>{t(queued ? 'nickname.submitted' : state.nickname!.status === 'reviewing' ? 'nickname.reviewing' : 'nickname.save')}</button>
      <div className="mp-nickname-review" id="mpNicknameStatus" role="status"><strong>{t(statusKey)}</strong>{detail && <span>{detail}</span>}</div>
    </div> : editor}
    <section className="mp-spectator" id="mpUnseatedNote" hidden={room.synced === true && state.seat != null}>
      <div className="mp-spectator-title">{t(!room.synced ? 'multiplayer.connectingRoom' : state.spectatorRequested ? 'multiplayer.spectatorSeat' : 'multiplayer.notSeated')}</div>
      <div className="mp-spectator-hint">{t(!room.synced ? 'multiplayer.syncingState' : state.spectatorRequested ? 'multiplayer.waitSpectatorStream' : 'multiplayer.chooseSeatOrSpectate')}</div>
      <div className="mp-spectator-loadout" hidden={!room.synced || state.spectatorRequested}><div className="mp-local-character" aria-hidden="true" hidden={config.inGameSelection}><span id="mpLocalCharacterGlyph">{loadout.glyph}</span></div><div className="mp-local-copy"><strong>{t('multiplayer.you')}</strong><span id="mpLocalLoadoutLabel" hidden={config.inGameSelection}>{label(state.preferredLoadout)}</span></div><div className="mp-local-actions">{arrow('mpLoadoutPrev', true)}{arrow('mpLoadoutNext', false)}</div></div>
    </section>
    <div className="mp-local-player" id="mpLocalPlayer" hidden={!room.synced || state.seat == null}><button className="mp-stand-button" id="mpStandUp" type="button" onClick={service.standUp}>{t('multiplayer.leaveSeat')}</button><div className="mp-seated-loadout">{arrow('mpLoadoutPrevSeat', true)}<span id="mpLocalRoleLabel" hidden={config.inGameSelection}>{label(state.preferredLoadout)}</span>{arrow('mpLoadoutNextSeat', false)}</div></div>
    <button className="mp-game-check-button" id="mpCheckGame" type="button" hidden={state.seat == null || th09NetworkOverlayOpen} disabled={state.seat == null || !!room.phase && room.phase !== 'lobby' || state.ready || state.checkBusy || state.launchBusy} onClick={() => {void service.checkGame();}}>{t(state.checkBusy ? 'multiplayer.checkingGame' : 'multiplayer.checkGame')}</button>
  </div>;
}
