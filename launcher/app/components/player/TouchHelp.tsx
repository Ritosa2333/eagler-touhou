import {memo, useLayoutEffect, useState, type ReactNode} from 'react';
import type {TouchFocusMode} from '../../../src/launcher/game-preferences.mts';
import {useLocale, type UiMessageKey} from '../../i18n';
import {InformationalDialog, informationalDialogPresentation} from '../notices/InformationalDialog';
import {OptionsGroup} from '../settings/OptionsGroup';
import '../../ui/document-body.css';
import './touch-help-ui.css';

export interface TouchHelpProps {
  open: boolean;
  onCloseRequest(): void;
  touchEnabled: boolean;
  focusMode: TouchFocusMode;
  showIosFullscreenHelp: boolean;
  nativeControls?: boolean;
  th19Charge?: boolean;
}
type HelpSection = 'controls' | 'touch' | 'orientation' | 'thprac';

/** The Player owns seen gating and game refocus; help adds no history entry. */
export const TouchHelp = memo(function TouchHelp({open, onCloseRequest, touchEnabled, focusMode, showIosFullscreenHelp, nativeControls = false, th19Charge = false}: TouchHelpProps) {
  const {t} = useLocale(), [expanded, setExpanded] = useState<HelpSection | null>(null);
  useLayoutEffect(() => {
    const player = document.getElementById('player');
    player?.classList.toggle('help-visible', open);
    if (open) setExpanded(touchEnabled && !nativeControls ? 'touch' : 'controls');
    return () => {player?.classList.remove('help-visible');};
  }, [open, touchEnabled, nativeControls]);
  const group = (id: HelpSection, title: string, children: ReactNode) => <OptionsGroup id={`help-${id}`} title={title} open={expanded === id} onOpenChange={value => setExpanded(value ? id : null)} bodyClassName="game-help-section">
    {children}
  </OptionsGroup>;
  const keys = (rows: readonly (readonly [string, UiMessageKey])[]) => <dl className="game-help-keys not-prose">{rows.map(([key, label]) => <div key={key}><dt><kbd>{key}</kbd></dt><dd>{t(label)}</dd></div>)}</dl>;
  return <InformationalDialog id="touchHelp" titleId="touchHelpTitle" closeId="touchHelpClose" title={t('help.title')} closeLabel={t('help.close')}
    presentation={informationalDialogPresentation.scrollable} launcherDocument open={open} onCloseRequest={onCloseRequest} onClosed={() => setExpanded(null)}>
    <div className="game-help-content ui-document prose">
      {group('controls', t('help.gameControls'), <>
        <p>{t(nativeControls ? 'help.fightingIntro' : 'help.gameControlsIntro')}</p>
        {keys(nativeControls ? [[t('help.arrowKeys'), 'help.fightingMovement'], ['Z', 'help.fightingA'], ['X', 'help.fightingB'], ['C / V', 'help.fightingCD'], ['Esc', 'help.pauseBack'], ['Ctrl', 'help.skipDialogue']] : th19Charge ? [[t('help.arrowKeys'), 'help.moveSelect'], ['Z', 'help.fireConfirm'], ['X', 'th19.chargeCancel'], ['C', 'input.action.bomb'], ['Shift', 'help.focusMove'], ['Esc', 'help.pauseBack'], ['Ctrl', 'help.skipDialogue']] : [[t('help.arrowKeys'), 'help.moveSelect'], ['Z', 'help.fireConfirm'], ['X', 'help.bombCancel'], ['Shift', 'help.focusMove'], ['Esc', 'help.pauseBack'], ['Ctrl', 'help.skipDialogue']])}
        <p>{t(nativeControls ? 'help.fightingNote' : 'help.gameControlsNote')}</p>
      </>)}
      {touchEnabled && !nativeControls && group('touch', t('help.touchControls'), <>
        <h3>{t('help.inGame')}</h3>
        <p>{t('help.touchMove')}</p>
        {th19Charge && <p>{t('th19.touchChargeHint')}</p>}
        <p>{t(focusMode === 'two-finger' ? 'help.focusTwoFingerSummary' : focusMode === 'toggle-button' ? 'help.focusToggleSummary' : 'help.focusHoldSummary')}</p>
        <h3>{t('help.menu')}</h3>
        <ul><li>{t('help.slideConfirm')}</li><li>{t('help.twoFingerBack')}</li></ul>
        <h3>{t('help.dialogue')}</h3>
        <p>{t('help.dialogueControls')}</p>
      </>)}
      {(touchEnabled || showIosFullscreenHelp) && group('orientation', t(showIosFullscreenHelp ? 'help.iphoneFullscreen' : 'help.manualLandscape'), showIosFullscreenHelp ? <ol>
        {(['iosSafariShare', 'iosAddHome', 'iosWebApp'] as const).map(step => <li key={step}><strong>{t(`help.${step}`)}</strong><p>{t(`help.${step}Step`)}</p></li>)}
      </ol> : <>
        <ol><li>{t('help.turnPhone')}</li><li>{t('help.systemRotate')}</li></ol>
        <p>{t('help.rotateVaries')}</p>
        <h3>{t('help.autoRotate')}</h3><p>{t('help.autoRotateStep')}</p>
      </>)}
      {!nativeControls && group('thprac', 'thprac', <>
        <p>{t('help.thpracIntro')}</p>
        {touchEnabled ? <><p><strong>{t('touch.cheatsAndSettings')}</strong> | {t('help.thpracMobilePanels')}</p><p>{t('help.thpracReplayMobile')}</p></> : <>
          {keys([['Backspace', 'touch.cheatMenu'], ['Tab', 'help.tracker'], ['F12', 'touch.advancedMenu'], ['F1', 'touch.invincible'], ['F2', 'touch.infiniteLives'], ['F3', 'touch.infiniteBombs'], ['F4', 'touch.infinitePower'], ['F5', 'touch.timeLock'], ['F6', 'touch.autoBomb'], ['F7', 'touch.enemyBgm']])}
          <p>{t('help.thpracReplayDesktop')}</p>
        </>}
      </>)}
    </div>
  </InformationalDialog>;
});
