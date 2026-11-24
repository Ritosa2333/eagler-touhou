import {cloneElement, isValidElement, useId, type ReactNode} from 'react';
import {Field, FieldContent, FieldLabel, FieldDescription} from '../../ui/field';
import {FieldHelp} from '../../ui/field-help';

export interface OptionRowProps {
  className: string;
  id?: string;
  hidden?: boolean;
  label: ReactNode;
  hint?: string;
  hintId?: string;
  hintAction?: ReactNode;
  status?: string;
  control: ReactNode;
}
/** Common toggle-row presentation; capability and mutation policy stay outside. */
export function OptionRow({className, id, hidden, label, hint, hintId, hintAction, status, control}: OptionRowProps) {
  const generatedId = useId();
  const descriptionId = hint || status ? hintId ?? `${generatedId}-hint` : undefined;
  const element = isValidElement<{id?: string; 'aria-describedby'?: string}>(control) ? control : undefined;
  const controlId = element?.props.id;
  const describedControl = element && descriptionId ? cloneElement(element, {'aria-describedby': element.props['aria-describedby'] ?? descriptionId}) : control;
  return <Field className={`settings-field ${className}`} id={id} hidden={hidden} orientation="horizontal" aria-labelledby={`${generatedId}-label`}>
    <FieldContent>
      <div className="option-label-with-help">
        <FieldLabel id={`${generatedId}-label`} htmlFor={controlId}>{label}</FieldLabel>
        {hint && descriptionId && <FieldHelp id={descriptionId} label={typeof label === 'string' ? label : undefined}
          action={hintAction} option>{hint}</FieldHelp>}
      </div>
      {status && <FieldDescription id={hint ? undefined : descriptionId} aria-live="polite">{status}</FieldDescription>}
    </FieldContent>
    {describedControl}
  </Field>;
}
