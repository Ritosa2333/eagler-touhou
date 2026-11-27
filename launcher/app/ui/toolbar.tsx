import type {ComponentPropsWithoutRef} from 'react';
import {cn} from './utils';
import './toolbar.css';

/** A shared surface for action buttons, selects and segmented controls. */
export function Toolbar({className, ...props}: ComponentPropsWithoutRef<'div'>) {
  return <div {...props} className={cn('ui-toolbar', className)}/>;
}
