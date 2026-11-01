import { Switch as SwitchPrimitive } from 'radix-ui';
import type { ComponentProps } from 'react';
import { cn } from '../../lib/utils';

export function Switch({
  className,
  variant = 'default',
  ...props
}: ComponentProps<typeof SwitchPrimitive.Root> & { variant?: 'default' | 'legacy' }) {
  return (
    <SwitchPrimitive.Root
      data-slot="switch"
      className={cn(
        'focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-ring disabled:cursor-not-allowed disabled:opacity-50',
        variant === 'default' &&
          'inline-flex h-5 w-9 shrink-0 items-center rounded-full border border-input bg-secondary data-[state=checked]:bg-primary',
        className,
      )}
      {...props}
    >
      <SwitchPrimitive.Thumb asChild>
        <i
          data-slot="switch-thumb"
          className={
            variant === 'default'
              ? 'pointer-events-none block size-4 rounded-full bg-primary-foreground transition-transform data-[state=checked]:translate-x-4 data-[state=unchecked]:translate-x-0'
              : undefined
          }
        />
      </SwitchPrimitive.Thumb>
    </SwitchPrimitive.Root>
  );
}
