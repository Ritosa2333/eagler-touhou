import { cva, type VariantProps } from 'class-variance-authority';
import { Slot } from 'radix-ui';
import type { ComponentProps } from 'react';
import { cn } from '../../lib/utils';

const buttonVariants = cva(
  'focus-visible:outline-2 focus-visible:outline-offset-2 focus-visible:outline-ring disabled:pointer-events-none disabled:opacity-50',
  {
    variants: {
      variant: {
        default:
          'inline-flex items-center justify-center gap-2 rounded-md border border-transparent bg-primary text-primary-foreground hover:bg-primary/90',
        destructive:
          'inline-flex items-center justify-center gap-2 rounded-md border border-transparent bg-destructive text-destructive-foreground hover:bg-destructive/90',
        outline:
          'inline-flex items-center justify-center gap-2 rounded-md border border-border bg-background text-foreground hover:bg-accent hover:text-accent-foreground',
        secondary:
          'inline-flex items-center justify-center gap-2 rounded-md border border-transparent bg-secondary text-secondary-foreground hover:bg-secondary/80',
        ghost: 'inline-flex items-center justify-center gap-2 rounded-md hover:bg-accent hover:text-accent-foreground',
        link: 'text-primary underline-offset-4 hover:underline',
        // Authored launcher controls retain their geometry, artwork and motion.
        // Only the semantic primitive changes; no second visual preset is applied.
        legacy: '',
      },
      size: { default: 'h-9 px-4 py-2 text-sm', sm: 'h-8 px-3 text-xs', lg: 'h-10 px-6', icon: 'size-9', legacy: '' },
    },
    defaultVariants: { variant: 'default', size: 'default' },
  },
);

export type ButtonProps = ComponentProps<'button'> & VariantProps<typeof buttonVariants> & { asChild?: boolean };
function Button({ className, variant, size, asChild = false, ...props }: ButtonProps) {
  const Comp = asChild ? Slot.Root : 'button';
  // Preserve native submit defaults and refs: decision forms and game controls
  // deliberately own their type, value, key handling and focus return.
  return <Comp data-slot="button" className={cn(buttonVariants({ variant, size }), className)} {...props} />;
}

export { Button, buttonVariants };
