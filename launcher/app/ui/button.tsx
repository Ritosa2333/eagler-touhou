import { forwardRef, type ComponentPropsWithoutRef } from 'react';
import { Slot } from 'radix-ui';
import { cva, type VariantProps } from 'class-variance-authority';
import { cn } from './utils';

// Variant and composition axes follow shadcn's Radix Button.
const buttonVariants = cva(
  'ui-kit-button inline-flex shrink-0 items-center justify-center gap-2 whitespace-nowrap rounded-md border border-solid border-transparent font-sans text-sm font-medium no-underline outline-none transition-colors select-none touch-manipulation focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-offset-2 focus-visible:ring-offset-background disabled:pointer-events-none disabled:opacity-50 [&_svg]:pointer-events-none [&_svg]:size-4 [&_svg]:shrink-0',
  {
    variants: {
      variant: {
        default: 'bg-primary text-primary-foreground hover:bg-primary/90',
        secondary: 'bg-secondary text-secondary-foreground hover:bg-secondary/75',
        outline: 'border-[var(--kit-control-border)] bg-card text-foreground hover:bg-accent active:bg-secondary',
        ghost: 'bg-transparent text-muted-foreground hover:bg-accent hover:text-foreground',
        destructive: 'bg-destructive/10 text-destructive hover:bg-destructive/20',
        link: 'bg-transparent text-foreground underline-offset-4 hover:underline',
      },
      size: {
        sm: 'h-8 px-3 text-xs',
        default: 'h-9 px-4',
        lg: 'h-11 px-5',
        icon: 'size-11 p-0',
        'icon-sm': 'size-8 p-0',
        row: 'h-auto min-h-10 w-full justify-start gap-2.5 whitespace-normal px-2.5 py-2 text-left text-[13px] pointer-coarse:min-h-11 max-[780px]:min-h-11 [&>svg:last-child]:ml-auto',
      },
    },
    defaultVariants: { variant: 'default', size: 'default' },
  },
);
export type ButtonProps = ComponentPropsWithoutRef<'button'> &
  VariantProps<typeof buttonVariants> & { asChild?: boolean };
export const Button = forwardRef<HTMLButtonElement, ButtonProps>(function Button(
  { variant = 'default', size = 'default', asChild, className, type = 'button', ...props },
  ref,
) {
  const Element = asChild ? Slot.Root : 'button';
  return (
    <Element
      ref={ref}
      type={asChild ? undefined : type}
      data-slot="button"
      data-variant={variant}
      data-size={size}
      className={cn(buttonVariants({ variant, size }), className)}
      {...props}
    />
  );
});
