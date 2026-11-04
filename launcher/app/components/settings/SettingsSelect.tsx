import type {ReactNode} from 'react';
import {Field, FieldContent, FieldLabel, FieldDescription} from '../../ui/field';
import {MainSelect, type MainSelectProps} from '../launcher/MainSelect';

export function SettingsSelect({id, label, description, hidden, className = '', ...props}: MainSelectProps & {id: string; label: ReactNode; description?: ReactNode}) {
  const descriptionId = description ? `${id}-description` : undefined;
  return <Field className={`settings-field ${className}`} hidden={hidden} orientation="vertical">
    <FieldContent><FieldLabel id={`${id}-label`} htmlFor={`${id}-trigger`}>{label}</FieldLabel>{description && <FieldDescription id={descriptionId}>{description}</FieldDescription>}</FieldContent>
    <MainSelect {...props} id={id} aria-labelledby={`${id}-label`} aria-describedby={descriptionId} className="option-select"/>
  </Field>;
}
