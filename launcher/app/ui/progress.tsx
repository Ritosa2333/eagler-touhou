import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Progress as Primitive} from 'radix-ui';
import {cn} from './utils';

export const ProgressIndicator = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Indicator>>(
  function ProgressIndicator({className, ...props}, ref) {
    return <Primitive.Indicator ref={ref} data-slot="progress-indicator" className={cn('ui-progress-indicator', className)} {...props}/>;
  },
);

export const Progress = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Root> & {smooth?: boolean; inline?: boolean}>(
  function Progress({className, value, max = 100, smooth, inline, children, asChild, ...props}, ref) {
    const limit = Number.isFinite(max) && max > 0 ? max : 100;
    const current = typeof value === 'number' && Number.isFinite(value) ? Math.max(0, Math.min(limit, value)) : null;
    const ratio = current === null ? null : current / limit;
    const style = ratio === null ? undefined : smooth ? {transform: `scaleX(${ratio})`} : {width: `${ratio * 100}%`};
    const body = children ?? (inline ? <ProgressIndicator asChild><span style={style}/></ProgressIndicator> : <ProgressIndicator style={style}/>);
    return <Primitive.Root {...props} asChild={inline || asChild} ref={ref} value={current} max={limit} data-slot="progress" data-smooth={smooth || undefined} className={cn('ui-progress', className)}>
      {inline ? <span>{body}</span> : body}
    </Primitive.Root>;
  },
);
