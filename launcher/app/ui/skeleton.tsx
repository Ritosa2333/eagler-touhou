import type {ComponentProps} from 'react';
import {cn} from './utils';

/** shadcn/ui Skeleton, using phrasing content so it also fits image links. */
export function Skeleton({className, ...props}: ComponentProps<'span'>) {
  return <span aria-hidden="true" {...props} data-slot="skeleton" className={cn('ui-skeleton block animate-pulse rounded-md bg-accent', className)}/>;
}
