import {useState} from 'react';
import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import {Button as UiButton} from '../../ui/controls';
import {SettingsSelect} from '../settings/SettingsSelect';
import {FieldHelp} from '../../ui/field-help';
import {useLocale} from '../../i18n';
import {BRO_SYSTEMS, BRO_STAGES} from '../../../src/launcher/bro-room-options.mts';

export interface BroRoomSettingsProps {
  systems: readonly number[];
  queue: readonly number[];
  disabled: boolean;
  onSystems(value: number[]): void;
  onQueue(value: number[]): void;
}

/**
 * BossRush-only room options: the shared system loadout and the ordered
 * chapter play queue. Rendered only for `bromp`; every other product keeps its
 * native room settings untouched.
 */
export function BroRoomSettings({systems, queue, disabled, onSystems, onQueue}: BroRoomSettingsProps) {
  const {t} = useLocale();
  const [draftStage, setDraftStage] = useState('');

  const move = (index: number, delta: number) => {
    const target = index + delta;
    if (target < 0 || target >= queue.length) return;
    const next = [...queue];
    [next[index], next[target]] = [next[target], next[index]];
    onQueue(next);
  };

  const remove = (index: number) => onQueue(queue.filter((_, i) => i !== index));
  const addStage = () => {
    const level = Number(draftStage);
    if (Number.isInteger(level) && BRO_STAGES.some(s => s.level === level)) onQueue([...queue, level]);
    setDraftStage('');
  };

  const stageName = (level: number) => BRO_STAGES.find(s => s.level === level)?.name ?? `CH.${level}`;
  const available = BRO_STAGES.filter(stage => !queue.includes(stage.level));

  return <div className="bro-room-settings" data-bro-room-settings>
    <div className="bro-room-systems">
      <div className="flex items-center gap-1">
        <span className="mp-room-setting-title">{t('room.broSystems')}</span>
        <FieldHelp id="mpBroSystemsHint" label={t('room.broSystems')}>{t('room.broSystemsHint')}</FieldHelp>
      </div>
      <ChoiceGroup type="multiple" className="bro-system-choices flex flex-wrap gap-1 rounded-lg bg-muted p-1"
        value={systems.map(String)} disabled={disabled}
        aria-label={t('room.broSystems')}
        onValueChange={values => onSystems(values.map(Number).sort((a, b) => a - b))}>
        {BRO_SYSTEMS.map(system => <ChoiceItem key={system.id} value={String(system.id)} type="button"
          data-bro-system={system.id} disabled={disabled}
          className="box-border min-h-11 whitespace-nowrap px-2 py-2 text-xs">
          {system.name}
        </ChoiceItem>)}
      </ChoiceGroup>
    </div>

    <div className="bro-room-queue">
      <div className="flex items-center gap-1">
        <span className="mp-room-setting-title">{t('room.broStageQueue')}</span>
        <FieldHelp id="mpBroQueueHint" label={t('room.broStageQueue')}>{t('room.broStageQueueHint')}</FieldHelp>
      </div>
      <div className="bro-queue-add flex items-end gap-1">
        <SettingsSelect id="mpBroQueueStage" label={t('room.broQueueAdd')} value={draftStage}
          disabled={disabled || available.length === 0}
          onChange={event => setDraftStage(event.target.value)}>
          <option value="">—</option>
          {available.map(stage => <option key={stage.level} value={stage.level}>{stage.level}. {stage.name}</option>)}
        </SettingsSelect>
        <UiButton size="sm" type="button" disabled={disabled || !draftStage} onClick={addStage}>{t('room.broQueueAddButton')}</UiButton>
      </div>
      {queue.length === 0
        ? <p className="bro-queue-empty room-setting-description">{t('room.broQueueEmpty')}</p>
        : <ol className="bro-queue-list">
          {queue.map((level, index) => <li key={`${index}-${level}`} className="bro-queue-item">
            <span className="bro-queue-index">{index + 1}</span>
            <span className="bro-queue-name">{stageName(level)}</span>
            <span className="bro-queue-actions">
              <UiButton size="icon-sm" type="button" disabled={disabled || index === 0}
                aria-label={t('room.broQueueUp')} onClick={() => move(index, -1)}>↑</UiButton>
              <UiButton size="icon-sm" type="button" disabled={disabled || index === queue.length - 1}
                aria-label={t('room.broQueueDown')} onClick={() => move(index, 1)}>↓</UiButton>
              <UiButton size="icon-sm" type="button" disabled={disabled}
                aria-label={t('room.broQueueRemove')} onClick={() => remove(index)}>×</UiButton>
            </span>
          </li>)}
        </ol>}
      {queue.length > 0 && <UiButton size="sm" type="button" variant="quiet" disabled={disabled}
        onClick={() => onQueue([])}>{t('room.broQueueClear')}</UiButton>}
      <p className="bro-queue-note room-setting-description">{t('room.broQueueNote')}</p>
    </div>
  </div>;
}
