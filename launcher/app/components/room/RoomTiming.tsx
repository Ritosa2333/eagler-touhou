import {Button as UiButton, Toggle} from '../../ui/controls';
import {ButtonGroup} from '../../ui/button-group';
import type {MultiplayerRoomService, RoomOwnerSnapshot} from '../../services/multiplayer-room';
import {multiplayerInputTimingPolicy} from '../../../src/contracts/product-catalog.mts';
import {useLocale} from '../../i18n';
import {roomPresentation} from './presentation';
import {OptionSwitch} from '../settings/TouchSettingsFields';
export type RoomTimingActions = Pick<MultiplayerRoomService, 'timingRecommendation' | 'setRollbackEnabled' | 'setTimingChoice'>;

export function RoomTiming({state, service, settings = false}: {state: RoomOwnerSnapshot; service: RoomTimingActions; settings?: boolean}) {
  const {t} = useLocale(), {room, ready, owner, config} = roomPresentation(state, t)!;
  const policy = multiplayerInputTimingPolicy(config), supported = !!config.inputTiming && policy.rollback;
  const rollback = supported && (room.phase === 'lobby' ? state.rollbackEnabled : room.adonisMode !== 1);
  const editable = ready && owner && room.phase === 'lobby' && !!config.inputTiming;
  const advice = service.timingRecommendation();
  const automatic = t(policy.measuredStartup ? room.phase !== 'lobby' ? room.timing ? 'room.inputDelayMeasured' : 'room.inputDelayMeasuring' : 'room.inputDelayMeasure' : 'room.inputDelayAutomatic',
    {frames: policy.measuredStartup ? room.inputDelay ?? 0 : advice.inputDelay, milliseconds: (advice.inputDelay * 16.67).toFixed(2)});
  const automaticChoice = room.phase === 'lobby' ? state.timingChoice === 'auto' : room.inputDelayAuto === true;
  const maximum = config.inputTiming?.manualDelayLimit ?? 8;
  const frames = !config.inputTiming || room.phase !== 'lobby' ? room.inputDelay ?? 0 : automaticChoice
    ? policy.measuredStartup ? room.inputDelay ?? 0 : advice.inputDelay : Number(state.timingChoice);
  const value = Math.max(0, Math.min(maximum, frames));
  const manualEditable = editable && !rollback && !(policy.measuredStartup && automaticChoice);
  return <div id={settings ? undefined : 'mpInputTiming'} className="mp-room-input-timing">
    {!settings && <label className="mp-rollback-toggle" hidden={!supported}>
      <span>{t('room.enableRollback')}</span><Toggle id="mpRollbackToggle" checked={rollback} aria-label={t('room.enableRollback')}
        hidden={!supported} disabled={!editable} onCheckedChange={value => service.setRollbackEnabled(value)}/>
    </label>}
    {settings && <section className="mp-room-delay" id="mpInputDelaySetting" hidden={rollback}>
      <span id="mpInputDelayLabel">{t('room.inputDelay')}：</span>
      <ButtonGroup className="mp-delay-stepper" aria-labelledby="mpInputDelayLabel">
        <UiButton size="icon-sm" id="mpInputDelayMinus" className="mp-delay-step" type="button" disabled={!manualEditable || value <= 0}
          aria-label={t('room.decreaseDelay')} onClick={() => service.setTimingChoice(value - 1)}>−</UiButton>
        <UiButton size="sm" id="mpInputDelay" className="mp-delay-value" type="button" value={value} disabled={!manualEditable}
          aria-label={t(policy.measuredStartup ? 'room.inputDelay' : 'room.resetAutoDelay')} title={automaticChoice ? automatic : undefined}
          onClick={policy.measuredStartup ? undefined : () => service.setTimingChoice('auto')}>{value}f</UiButton>
        <UiButton size="icon-sm" id="mpInputDelayPlus" className="mp-delay-step" type="button" disabled={!manualEditable || value >= maximum}
          aria-label={t('room.increaseDelay')} onClick={() => service.setTimingChoice(value + 1)}>+</UiButton>
      </ButtonGroup>
      {policy.measuredStartup && <label className="mp-delay-auto" htmlFor="mpInputDelayAuto"><span>{t('room.inputDelayAutoToggle')}</span>
        <OptionSwitch id="mpInputDelayAuto" label={t('room.inputDelayAutoToggle')} checked={automaticChoice} disabled={!editable || rollback}
          onChange={() => service.setTimingChoice(automaticChoice ? value : 'auto')}/>
      </label>}
    </section>}
  </div>;
}
