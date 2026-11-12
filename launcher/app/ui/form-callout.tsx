import type {ComponentProps} from 'react';
import {Callout} from '@fumadocs/base-ui/components/callout';
import {cn} from './utils';
import './document-body.css';

export function FormCallout({className, ...props}: ComponentProps<typeof Callout>) {
  return <Callout {...props} className={cn('my-0 rounded-lg border-border py-2 pe-2 text-xs leading-relaxed [&>svg]:size-4 [&>svg]:shrink-0', className)}/>;
}
