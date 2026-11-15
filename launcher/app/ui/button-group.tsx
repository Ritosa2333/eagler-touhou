import type {ComponentProps} from 'react';
import {cn} from './utils';

export function ButtonGroup({className, orientation = 'horizontal', ...props}: ComponentProps<'fieldset'> & {orientation?: 'horizontal' | 'vertical'}) {
  return <fieldset data-slot="button-group" data-orientation={orientation} className={cn('ui-button-group', className)} {...props}/>;
}
