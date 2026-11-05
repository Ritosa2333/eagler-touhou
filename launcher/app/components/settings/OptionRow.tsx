import {cloneElement, isValidElement, useId, type ReactNode} from 'react';
import {Field, FieldContent, FieldLabel, FieldDescription} from '../../ui/field';

export interface OptionRowProps {
  className: string;
  id?: string;
  hidden?: boolean;
  label: ReactNode;
  hint?: ReactNode;
  hintId?: string;
  control: ReactNode;
}
/** Common toggle-row presentation; capability and mutation policy stay outside. */
export function OptionRow({className, id, hidden, label, hint, hintId, control}: OptionRowProps) {
  const generatedId = useId();
  const descriptionId = hint !== undefined ? hintId ?? `${generatedId}-hint` : undefined;
  const element = isValidElement<{id?: string; 'aria-describedby'?: string}>(control) ? control : undefined;
  const controlId = element?.props.id;
  const describedControl = element && descriptionId ? cloneElement(element, {'aria-describedby': element.props['aria-describedby'] ?? descriptionId}) : control;
  return <Field className={`settings-field ${className}`} id={id} hidden={hidden} orientation="horizontal" aria-labelledby={`${generatedId}-label`}>
    <FieldContent><FieldLabel id={`${generatedId}-label`} htmlFor={controlId}>{label}</FieldLabel>{hint !== undefined && <FieldDescription id={descriptionId}>{hint}</FieldDescription>}</FieldContent>{describedControl}
  </Field>;
}
