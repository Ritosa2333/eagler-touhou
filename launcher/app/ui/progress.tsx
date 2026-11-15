import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Progress as Primitive} from 'radix-ui';
import {cn} from './utils';

export const ProgressIndicator = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Indicator>>(
  function ProgressIndicator({className, ...props}, ref) {
    return <Primitive.Indicator ref={ref} data-slot="progress-indicator" className={cn('ui-progress-indicator', className)} {...props}/>;
  },
);

export const Progress = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Root>>(
  function Progress({className, value, max = 100, children, ...props}, ref) {
    const limit = Number.isFinite(max) && max > 0 ? max : 100;
    const current = typeof value === 'number' && Number.isFinite(value) ? Math.max(0, Math.min(limit, value)) : null;
    return <Primitive.Root {...props} ref={ref} value={current} max={limit} data-slot="progress" className={cn('ui-progress', className)}>
      {children ?? <ProgressIndicator style={current === null ? undefined : {width: `${current / limit * 100}%`}}/>}
    </Primitive.Root>;
  },
);
