import { Progress as ProgressPrimitive } from 'radix-ui';
import type { ComponentProps } from 'react';
import { cn } from '../../lib/utils';

type ProgressProps = ComponentProps<typeof ProgressPrimitive.Root> & {
  indicatorProps?: ComponentProps<typeof ProgressPrimitive.Indicator>;
};
export function Progress({ className, value, indicatorProps, ...props }: ProgressProps) {
  const { className: indicatorClassName, style, ...indicatorRest } = indicatorProps ?? {};
  return (
    <ProgressPrimitive.Root
      data-slot="progress"
      value={value}
      className={cn('relative h-2 w-full overflow-hidden rounded-full bg-secondary', className)}
      {...props}
    >
      <ProgressPrimitive.Indicator asChild {...indicatorRest}>
        <span
          data-slot="progress-indicator"
          className={cn('block h-full w-full rounded-full bg-primary transition-transform', indicatorClassName)}
          style={{ transform: `translateX(-${100 - (value ?? 0)}%)`, ...style }}
        />
      </ProgressPrimitive.Indicator>
    </ProgressPrimitive.Root>
  );
}
