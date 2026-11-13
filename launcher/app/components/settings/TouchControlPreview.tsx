import {Button as UiButton} from '../../ui/controls';
import {PRODUCT_GAMES} from '../../../src/contracts/product-catalog.mts';
import {touchMovementUsesJoystick} from '../../../src/launcher/game-preferences.mts';
import {functionKeyGames} from '../../../src/launcher/touch-function-key.mts';
import type {PlayerTouchSnapshot} from '../../services/player-touch';
import type {SettingsSnapshot} from '../../models/game-settings';
import {useLocale} from '../../i18n';
const Resize = () => <span className="touch-layout-resize-handle" aria-hidden="true">↘</span>;
/** Authored touch geometry/labels. Editor controls select/move; they never send
 * live Runtime input. All native input ownership stays with the Runtime host. */
export function TouchControlPreview({settings, live, spectator = false, fireEnabled = true, editing = false}: {settings: SettingsSnapshot; live?: PlayerTouchSnapshot; spectator?: boolean; fireEnabled?: boolean; editing?: boolean}) {
  const {t} = useLocale(), {options} = settings, game = PRODUCT_GAMES[settings.gameId];
  const th19Charge = settings.gameId === 'th19';
  const actions = 'touchActions' in game ? game.touchActions : null;
  const heldFire = game.touchFire.mode === 'held-key', heldFocus = options.touchFocusMode === 'hold-button';
  const focusOn = live?.focusEnabled ?? false, fireOn = heldFire ? live?.heldFire ?? false : live?.fireEnabled ?? fireEnabled;
  const touchSurfaceVisible = !spectator && options.touchEnabled || editing;
  const thpracVisible = !spectator && options.thpracTouchControlsEnabled && (options.touchEnabled || editing);
  const extraU = 'thpracExtraFunctionKeys' in game && game.thpracExtraFunctionKeys.includes('U');
  return <>
    <nav className={`touch-hud${actions ? " native-actions" : ""}`} id="touchHud" aria-label={t('touch.leftControls')}>
      {actions ? <>
        <UiButton variant="unstyled" className={`touch-focus${(live?.actionButtons||0)&actions.focus.bit?' is-on':''}`} id="touchFocus" data-touch-layout-control="focus" type="button" aria-pressed={!!((live?.actionButtons||0)&actions.focus.bit)} aria-label={actions.focus.label+' '+t(actions.focus.descriptionKey)}><strong>{actions.focus.label}</strong><small>{t(actions.focus.descriptionKey)}</small><Resize/></UiButton>
        <UiButton variant="unstyled" className={`touch-fire${(live?.actionButtons||0)&actions.fire.bit?' is-on':''}`} id="touchFire" data-touch-layout-control="fire" type="button" aria-pressed={!!((live?.actionButtons||0)&actions.fire.bit)} aria-label={actions.fire.label+' '+t(actions.fire.descriptionKey)}><strong>{actions.fire.label}</strong><small>{t(actions.fire.descriptionKey)}</small><Resize/></UiButton>
        <UiButton variant="unstyled" className={`touch-bomb${(live?.actionButtons||0)&actions.bomb.bit?' is-on':''}`} id="touchBomb" data-touch-layout-control="bomb" type="button" aria-pressed={!!((live?.actionButtons||0)&actions.bomb.bit)} aria-label={actions.bomb.label+' '+t(actions.bomb.descriptionKey)}><span className="touch-icon">{actions.bomb.label}</span><strong>{t(actions.bomb.descriptionKey)}</strong><Resize/></UiButton>
        <UiButton variant="unstyled" className={`touch-dash${(live?.actionButtons||0)&actions.function.bit?' is-on':''}`} id="touchFunction" data-touch-layout-control="function" type="button" aria-pressed={!!((live?.actionButtons||0)&actions.function.bit)} aria-label={actions.function.label+' '+t(actions.function.descriptionKey)}><strong>{actions.function.label}</strong><small>{t(actions.function.descriptionKey)}</small><Resize/></UiButton>
      </> : <>
      <UiButton variant="unstyled" className={`touch-focus${focusOn ? ' is-on' : ''}`} id="touchFocus" data-touch-layout-control="focus" type="button" aria-pressed={focusOn} hidden={options.touchFocusMode === 'two-finger'} aria-label={t('touch.focus') + t(heldFocus ? 'touch.actionHold' : 'touch.actionToggle')}><strong><span className="touch-action-name">{t('touch.focus')}</span><span className="touch-action-mode">{t(heldFocus ? 'touch.actionHold' : 'touch.actionToggle')}</span></strong><small>{t(heldFocus ? 'touch.holdFocus' : 'touch.tapToggle')}</small><Resize/></UiButton>
      <UiButton variant="unstyled" className={`touch-fire${fireOn ? ' is-on' : ''}`} id="touchFire" data-touch-layout-control="fire" type="button" aria-pressed={fireOn} aria-label={t('touch.fire') + t(heldFire ? 'touch.actionHold' : 'touch.actionToggle')}><strong><span className="touch-action-name">{t('touch.fire')}</span><span className="touch-action-mode">{t(heldFire ? 'touch.actionHold' : 'touch.actionToggle')}</span></strong><small>{t(game.touchFire.labelKey)}</small><Resize/></UiButton>
      <UiButton variant="unstyled" className={`touch-function touch-fire${th19Charge?' touch-charge':''}${live?.functionDown?' is-on':''}`} id="touchFunction" data-touch-layout-control="function" type="button" aria-pressed={live?.functionDown ?? false} aria-label={t(th19Charge?'th19.touchChargeHint':'touch.specialFunction')} hidden={spectator || !functionKeyGames.has(settings.gameId)}><span className="touch-icon" aria-hidden="true">{th19Charge?'X':'C'}</span><strong>{t(th19Charge?'th19.charge':'touch.specialFunction')}</strong><small>{t(th19Charge?'th19.touchChargeHint':'touch.functionKeyHint')}</small><Resize/></UiButton>
      <UiButton variant="unstyled" className="touch-bomb" id="touchBomb" data-touch-layout-control="bomb" type="button"><span className="touch-icon">{th19Charge?'C':'B'}</span><strong>BOMB</strong><Resize/></UiButton>
      </>}
    </nav>
    <div className={`touch-joystick${actions ? " native-joystick" : ""}`} id="touchJoystick" data-touch-layout-control="joystick" aria-label={t('touch.joystickAria')} role="application" hidden={!(touchMovementUsesJoystick(options.touchMovementMode) && touchSurfaceVisible)}><div className="touch-joystick-base"><i className="touch-joystick-ring" aria-hidden="true"/><b className="touch-joystick-knob" id="touchJoystickKnob" aria-hidden="true"/></div><Resize/></div>
    <UiButton variant="unstyled" className="touch-escape" id="touchEscape" data-touch-layout-control="escape" type="button" aria-label={t('touch.escapeAria')} title={t('touch.escapeTitle')}>ESC<Resize/></UiButton>
    <UiButton variant="unstyled" className="touch-restart" id="touchRestart" data-touch-layout-control="restart" type="button" aria-label="R" title={t('touch.restartTitle')} hidden={!options.restartButtonEnabled}>R<Resize/></UiButton>
    <UiButton variant="unstyled" className="touch-thprac-tab" id="touchThpracTab" data-touch-layout-control="thpracTab" data-thprac-key="Tab" type="button" hidden={!(settings.gameId === 'th14' && thpracVisible)}><strong>Tab</strong><small>Tracker</small><Resize/></UiButton>
    <div className="touch-thprac-menu" id="touchThpracMenu" data-touch-layout-control="thpracMenu" hidden={!thpracVisible || (!editing && !!live?.thpracPracticeOpen)}>
      <UiButton variant="unstyled" className="touch-thprac-backspace" id="touchThpracBackspace" type="button" aria-expanded={live?.thpracMenuOpen ?? false}>{t('touch.cheatsAndSettings')}</UiButton>
      {settings.gameId === 'th14' && <UiButton variant="unstyled" className="touch-thprac-backspace mt-1" data-thprac-key="F12" type="button" aria-label={t('touch.advancedMenu')}>F12 · {t('touch.advancedMenu')}</UiButton>}
      <div className="touch-thprac-function-keys" id="touchThpracFunctionKeys" hidden>
        {([['F12','touch.advancedMenu'],['F1','touch.invincible'],['F2','touch.infiniteLives'],['F3','touch.infiniteBombs'],['F4','touch.infinitePower'],['F5','touch.timeLock'],['F6','touch.autoBomb'],['F7','touch.enemyBgm'],['U','touch.enemyInvincible']] as const).map(([key, label]) => <UiButton variant="unstyled" key={key} type="button" data-thprac-key={key} hidden={key === 'U' && !extraU}><strong>{key}</strong><small>{t(label)}</small></UiButton>)}
      </div><Resize/>
    </div>
  </>;
}
