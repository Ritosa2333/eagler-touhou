import type {ComponentProps} from 'react';
import {cn} from './utils';

export function FieldGroup({className, ...props}: ComponentProps<'div'>) {
  return <div data-slot="field-group" className={cn('ui-field-group', className)} {...props}/>;
}
export function Field({className, orientation = 'vertical', ...props}: ComponentProps<'fieldset'> & {orientation?: 'vertical' | 'horizontal' | 'responsive'}) {
  return <fieldset data-slot="field" data-orientation={orientation} className={cn('ui-field', className)} {...props}/>;
}
export function FieldContent({className, ...props}: ComponentProps<'div'>) {
  return <div data-slot="field-content" className={cn('ui-field-content', className)} {...props}/>;
}
export function FieldLabel({className, htmlFor, children, ...props}: ComponentProps<'label'>) {
  return <label htmlFor={htmlFor} data-slot="field-label" className={cn('ui-field-label', className)} {...props}>{children}</label>;
}
export function FieldDescription({className, ...props}: ComponentProps<'div'>) {
  return <div data-slot="field-description" className={cn('ui-field-description', className)} {...props}/>;
}
