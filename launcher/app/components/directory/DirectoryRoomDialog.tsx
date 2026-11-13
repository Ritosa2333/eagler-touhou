import {Input} from '../../ui/input';
import {FieldHelp} from '../../ui/field-help';
import {MainSelect} from '../launcher/MainSelect';
import {BookButton} from '../launcher/BookButton';
import {BookProductMenu} from '../launcher/BookDirectoryGames';
import {ChevronDown, X} from 'lucide-react';
import {useLayoutEffect, useRef, useSyncExternalStore} from 'react';
import {PRODUCT_GAMES, gameIdForProduct, isMultiplayerProductId, multiplayerConfigForProduct} from '../../../src/contracts/product-catalog.mts';
import type {LobbyDirectoryService} from '../../services/lobby-directory';
import {LobbyDialogHeader} from './LobbyDialogHeader';
import {useLocale} from '../../i18n';
import {accountsEnabled} from '../../release-features';
import {OptionSwitch} from '../settings/TouchSettingsFields';
import '../room/room-presentation.css';

export interface DirectoryRoomDialogProps {
  service: LobbyDirectoryService;
  assetUrl(path: string): string;
  open: boolean;
  onCloseRequest(): void;
  completionContext?: {locationKey: string; parentKey: string | null; href: string};
}
/** Original room form lifecycle with shared selects and native form values. */
export function DirectoryRoomDialog({service, assetUrl, open, onCloseRequest, completionContext}: DirectoryRoomDialogProps) {
  const {t} = useLocale(), state = useSyncExternalStore(service.subscribe, service.getSnapshot, service.getSnapshot);
  const dialog = useRef<HTMLDialogElement>(null), code = useRef<HTMLInputElement>(null), lifetimeOpen = useRef(false);
  const rules = useRef<HTMLDetailsElement>(null);
  const submission = useRef<{owner: object; parentKey: string | null | undefined; href?: string} | null>(null);
  const form = state.form, created = form.mode === 'create';
  const policy = form.product ? multiplayerConfigForProduct(form.product) : null;
  const inGameDifficulty = policy?.inGameSelection && form.product !== 'th03mp';
  const inGameHelp = created && inGameDifficulty;
  const privateHelp = created && form.visibility === 'private';
  const disabled = state.connection !== 'live' || !!state.mine || state.leaving || !state.products.length;
  useLayoutEffect(() => {
    const node = dialog.current; if (!node) return;
    if (open) {
      const opening = !node.open;
      lifetimeOpen.current = true;
      if (opening) {
        if (rules.current) rules.current.open = false;
        node.showModal();
      }
      if (opening && form.mode === 'join') code.current?.focus();
    } else {
      if (node.open) node.close();
      if (lifetimeOpen.current) {
        lifetimeOpen.current = false;
        const pending = submission.current; submission.current = null;
        // Main closes synchronously before leaving its document. In the SPA,
        // only returning to this form's history parent may finish that intent.
        // Parentless restored entries retain the Router's same-address replace
        // fallback; standalone (non-Router) consumers keep the original API.
        const returnedToOwner = !pending || pending.parentKey === undefined || (pending.parentKey === null
          ? pending.href === completionContext?.href : pending.parentKey === completionContext?.locationKey);
        service.formDidClose(pending?.owner, returnedToOwner);
      }
    }
  }, [open, form.mode, service, completionContext?.locationKey, completionContext?.href]);
  useLayoutEffect(() => () => {
    if (dialog.current?.open) dialog.current.close();
    const pending = submission.current; submission.current = null;
    if (pending) service.formDidClose(pending.owner, false);
  }, [service]);
  useLayoutEffect(() => {code.current?.setCustomValidity(form.validationMessage);}, [form.validationMessage]);
  // biome-ignore lint/a11y/useKeyWithClickEvents: Native cancel and the labelled close button provide keyboard dismissal; this click handles the backdrop only.
  return <dialog ref={dialog} className="lobby-dialog" id="roomDialog" data-mode={form.mode} aria-labelledby="dialogTitle" onCancel={event => {event.preventDefault(); onCloseRequest();}}
    onClick={event => {const rect = event.currentTarget.getBoundingClientRect(); if (event.target === event.currentTarget && (event.clientX < rect.left || event.clientX > rect.right || event.clientY < rect.top || event.clientY > rect.bottom)) onCloseRequest();}}>
    <form id="roomForm" onSubmit={event => {
      event.preventDefault();
      const owner = {};
      const result = service.submitForm(owner);
      if (result === 'invalid-code') {code.current?.setCustomValidity(service.getSnapshot().form.validationMessage); code.current?.reportValidity();}
      else if (result === 'close') {submission.current = {owner, parentKey: completionContext?.parentKey, href: completionContext?.href}; onCloseRequest();}
    }}>
      <LobbyDialogHeader titleId="dialogTitle" title={t(created ? 'lobby.create' : 'lobby.byCode')} closeId="closeDialog" closeLabel={t('lobby.cancel')} closeContent={<X size={18} aria-hidden="true"/>} onCloseRequest={onCloseRequest}/>
      <div className="lobby-field" hidden={!created && state.globalRoomCodes}>
        <div className="flex items-center gap-1">
          <label htmlFor="gameSelect-trigger">{t('lobby.game')}</label>
          {inGameHelp && <FieldHelp id="createRoomGameHelp" label={t('lobby.game')}>{t('room.inGameSelectionHint')}</FieldHelp>}
        </div>
        <MainSelect id="gameSelect" aria-label={t('lobby.game')} aria-describedby={inGameHelp ? 'createRoomGameHelp' : undefined} required value={form.product}
          onChange={event => {if (isMultiplayerProductId(event.target.value)) service.updateForm({product: event.target.value});}}
          renderPicker={created ? (change, ref) => <BookProductMenu products={state.products} value={form.product}
            assetUrl={assetUrl} onChange={change} trigger={<BookButton ref={ref} id="gameSelect-trigger" variant="unstyled"
              className="ui-select-trigger mizuki-select-trigger" aria-label={t('lobby.game')} aria-describedby={inGameHelp ? 'gameSelect-value createRoomGameHelp' : 'gameSelect-value'} disabled={!state.products.length}>
              <span id="gameSelect-value">{form.product ? PRODUCT_GAMES[gameIdForProduct(form.product)].title : t('common.selectOption')}</span>
              <ChevronDown aria-hidden="true"/>
            </BookButton>}/> : undefined}>
          {state.products.map(product => <option key={product} value={product}>{PRODUCT_GAMES[gameIdForProduct(product)].title}</option>)}
        </MainSelect>
      </div>
      <div id="createFields" className="lobby-form-pair" hidden={!created}><label className="lobby-field"><span>{t('lobby.capacity')}</span><MainSelect id="capacitySelect" aria-label={t('lobby.capacity')} value={form.capacity} onChange={event => service.updateForm({capacity: Number(event.target.value) as 2 | 3})}>{policy?.playerCounts.map(count => <option key={count} value={count}>{t('lobby.playersCount', {count})}</option>)}</MainSelect></label><label className="lobby-field" hidden={inGameDifficulty}><span>{t('lobby.difficulty')}</span><MainSelect id="difficultySelect" aria-label={t('lobby.difficulty')} className="difficulty-label" data-difficulty={(policy?.difficulties[form.difficulty] || '').toLowerCase()} disabled={inGameDifficulty} value={form.difficulty} onChange={event => service.updateForm({difficulty: Number(event.target.value)})}>{policy?.difficulties.map((name, index) => <option key={name} value={index} className="difficulty-label" data-difficulty={name.toLowerCase()}>{name}</option>)}</MainSelect></label></div>
      {created && accountsEnabled && form.product === 'th09mp' && <div className="flex min-h-9 items-center justify-between gap-3">
        <label className="text-sm font-semibold text-foreground" htmlFor="createRoomUnrated">{t('room.unrated')}</label>
        <OptionSwitch id="createRoomUnrated" label={t('room.unrated')} checked={form.unrated === true}
          onChange={() => service.updateForm({unrated: !form.unrated})}/>
      </div>}
      <label className="lobby-field" id="codeField" hidden={created}><span className="sr-only">{t('lobby.roomCode')}</span><Input ref={code} id="roomCodeInput" type="text" inputMode="numeric" autoComplete="off" pattern="[0-9]{4,8}" minLength={4} maxLength={8} placeholder="0000" required={!created} value={form.code} onChange={event => {event.currentTarget.setCustomValidity(''); service.updateForm({code: event.target.value});}}/></label>
      <details ref={rules} className="lobby-create-rules" hidden={!created}>
        <summary>
          <span>{t('room.ruleSettings')}</span>
          <ChevronDown size={16} aria-hidden="true"/>
        </summary>
        <div className="lobby-form-pair" id="policyFields" hidden={!created}>
          <div className="lobby-field">
            <div className="flex items-center gap-1">
              <label htmlFor="visibilitySelect-trigger">{t('room.visibility')}</label>
              {privateHelp && <FieldHelp id="formNote" label={t('room.visibility')}>{t('room.privateHint')}</FieldHelp>}
            </div>
            <MainSelect id="visibilitySelect" aria-label={t('room.visibility')} aria-describedby={privateHelp ? 'formNote' : undefined} value={form.visibility} onChange={event => service.updateForm({visibility: event.target.value === 'private' ? 'private' : 'public'})}>
              <option value="public">{t('room.public')}</option><option value="private">{t('room.private')}</option>
            </MainSelect>
          </div>
          <label className="lobby-field"><span>{t('room.cheatMovement')}</span><MainSelect id="cheatSelect" aria-label={t('room.cheatMovement')} value={form.disableCheatMovement ? '1' : '0'} onChange={event => service.updateForm({disableCheatMovement: event.target.value === '1'})}><option value="0">{t('room.cheatAllowed')}</option><option value="1">{t('room.cheatDisabled')}</option></MainSelect></label>
        </div>
      </details>
      <p className="lobby-form-error" id="formError" role="alert" hidden/>
      <BookButton variant="primary" className="lobby-button lobby-primary lobby-submit" id="submitRoom" type="submit" disabled={disabled}>{t(created ? 'lobby.create' : 'lobby.joinRoom')}</BookButton>
    </form>
  </dialog>;
}
