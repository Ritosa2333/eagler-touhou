import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Slot} from 'radix-ui';
import {cva, type VariantProps} from 'class-variance-authority';
import {cn} from './utils';

const badgeVariants = cva('ui-badge', {
  variants: {variant: {secondary: 'ui-badge-secondary', outline: 'ui-badge-outline', destructive: 'ui-badge-destructive'}},
  defaultVariants: {variant: 'secondary'},
});
type BadgeProps = ComponentPropsWithoutRef<'span'> & VariantProps<typeof badgeVariants> & {asChild?: boolean};

export const Badge = forwardRef<HTMLSpanElement, BadgeProps>(function Badge({className, variant = 'secondary', asChild, ...props}, ref) {
  const Element = asChild ? Slot.Root : 'span';
  return <Element {...props} ref={ref} data-slot="badge" data-variant={variant} className={cn(badgeVariants({variant}), className)}/>;
});
