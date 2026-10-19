import {useLocale} from '../../i18n';

/** Copy and key meanings from the TH075 1.0.4 development-package guide. */
export function FightingGuide({open,onToggle}:{open:boolean;onToggle():void}){
  const {t}=useLocale();
  return <article className="guide-scene guide-panel" data-guide-panel="game-controls">
    <button className="guide-tab-card" id="guideTabGameControls" type="button" aria-controls="guideGameControls" data-guide-tab="game-controls" aria-expanded={open} onClick={onToggle}><span><strong>{t('help.gameControls')}</strong><small>{t('help.gameControlsSummary')}</small></span><i className="guide-tab-arrow mizuki-select-arrow" aria-hidden="true"><svg viewBox="0 0 24 24"><path d="m7 10 5 5 5-5"/></svg></i></button>
    <div className="guide-demo-body" id="guideGameControls" hidden={!open}>
      <div className="guide-info">
        <p className="guide-info-lead">{t('help.fightingIntro')}</p>
        <div className="guide-key-grid">
          <div className="guide-key-row"><kbd>{t('help.arrowKeys')}</kbd><span>{t('help.fightingMovement')}</span></div>
          <div className="guide-key-row"><kbd>Z</kbd><span>{t('help.fightingA')}</span></div>
          <div className="guide-key-row"><kbd>X</kbd><span>{t('help.fightingB')}</span></div>
          <div className="guide-key-row"><kbd>C / V</kbd><span>{t('help.fightingCD')}</span></div>
          <div className="guide-key-row"><kbd>Esc</kbd><span>{t('help.pauseBack')}</span></div>
          <div className="guide-key-row"><kbd>Ctrl</kbd><span>{t('help.skipDialogue')}</span></div>
        </div>
        <p className="guide-info-note">{t('help.fightingNote')}</p>
      </div>
    </div>
  </article>;
}
