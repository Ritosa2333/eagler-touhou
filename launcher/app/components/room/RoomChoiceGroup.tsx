import {ChoiceGroup, ChoiceItem} from '../../ui/choice-group';
import type {ReactNode} from 'react';

export interface RoomChoice<Value extends string | number> {
  value: Value;
  label: ReactNode;
  selected: boolean;
  disabled: boolean;
  hidden?: boolean;
}
export interface RoomChoiceGroupProps<Value extends string | number> {
  title: ReactNode;
  name?: string;
  valueAttribute: `data-${string}`;
  choices: readonly RoomChoice<Value>[];
  onSelect(value: Value): void;
}
/** Shared room choices; supported values and phase/owner gates remain caller policy. */
export function RoomChoiceGroup<Value extends string | number>({title, name, valueAttribute, choices, onSelect}: RoomChoiceGroupProps<Value>) {
  return <><div className="mp-room-setting-title">{title}</div><ChoiceGroup type="single" className="mp-room-choice-group" data-mp-room-choice={name}
    value={String(choices.find(choice => choice.selected)?.value ?? '')} aria-label={typeof title === 'string' ? title : name}
    onValueChange={value => {const choice = choices.find(choice => String(choice.value) === value); if (choice) onSelect(choice.value);}}>
    {choices.map(choice => <ChoiceItem key={choice.value} value={String(choice.value)} disabled={choice.disabled} type="button"
      {...{[valueAttribute]: choice.value}} hidden={choice.hidden} className={choice.selected ? 'selected' : undefined} onClick={() => {if (choice.selected) onSelect(choice.value);}}>{choice.label}</ChoiceItem>)}
  </ChoiceGroup></>;
}
