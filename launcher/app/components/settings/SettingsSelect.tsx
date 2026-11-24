import type {ReactNode} from 'react';
import {Field, FieldContent, FieldLabel} from '../../ui/field';
import {FieldHelp} from '../../ui/field-help';
import {MainSelect, type MainSelectProps} from '../launcher/MainSelect';

export function SettingsSelect({id, label, description, hidden, className = '', ...props}: MainSelectProps & {id: string; label: ReactNode; description?: string}) {
  const descriptionId = description ? `${id}-description` : undefined;
  return <Field className={`settings-field settings-choice-row ${className}`} hidden={hidden} orientation="responsive">
    <FieldContent>
      <div className="option-label-with-help">
        <FieldLabel id={`${id}-label`} htmlFor={`${id}-trigger`}>{label}</FieldLabel>
        {description && descriptionId && <FieldHelp id={descriptionId} label={typeof label === 'string' ? label : props['aria-label']}
          option>{description}</FieldHelp>}
      </div>
    </FieldContent>
    <MainSelect {...props} id={id} aria-labelledby={`${id}-label`} aria-describedby={descriptionId} className="option-select"/>
  </Field>;
}
