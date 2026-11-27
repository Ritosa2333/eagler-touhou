import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Checkbox, Switch, Slot} from 'radix-ui';
import {CheckIcon} from '@radix-ui/react-icons';
import {Button as KitButton, type ButtonProps as KitButtonProps} from './button';

export type ButtonVariant = 'action' | 'primary' | 'quiet' | 'toolbar' | 'tab' | 'choice' | 'icon' | 'danger' | 'unstyled';
export type ButtonProps = Omit<KitButtonProps, 'variant'> & {variant?: ButtonVariant};
const buttonAppearance = {action: 'outline', primary: 'default', danger: 'destructive', quiet: 'ghost', icon: 'ghost', toolbar: 'toolbar'} as const;

export const Button = forwardRef<HTMLButtonElement, ButtonProps>(function Button(
  {variant = 'action', className = '', type = 'button', asChild, size, ...props}, ref,
) {
  if (variant === 'tab' || variant === 'choice' || variant === 'unstyled') {
    const Element = asChild ? Slot.Root : 'button';
    return <Element {...props} ref={ref} type={asChild ? undefined : type} className={`${variant === 'unstyled' ? 'ui-control' : 'ui-button ui-control'} ${className}`.trim()} data-ui-variant={variant}/>;
  }
  return <KitButton {...props} ref={ref} type={type} asChild={asChild} variant={buttonAppearance[variant]} size={size ?? (variant === 'icon' ? 'icon' : variant === 'toolbar' ? 'lg' : 'default')}
    data-ui-variant={variant} className={`ui-button ui-control ${className}`}/>;
});

type CheckProps = Omit<ComponentPropsWithoutRef<typeof Checkbox.Root>, 'onCheckedChange'> & {onCheckedChange?(checked: boolean): void};
export const Check = forwardRef<HTMLButtonElement, CheckProps>(function Check({className = '', onCheckedChange, ...props}, ref) {
  return <Checkbox.Root {...props} data-slot="checkbox" onCheckedChange={value => onCheckedChange?.(value === true)} ref={ref} className={`ui-checkbox ${className}`.trim()}>
    <Checkbox.Indicator><CheckIcon/></Checkbox.Indicator>
  </Checkbox.Root>;
});

export const Toggle = forwardRef<HTMLButtonElement, ComponentPropsWithoutRef<typeof Switch.Root>>(function Toggle({className = '', ...props}, ref) {
  return <Switch.Root {...props} data-slot="switch" ref={ref} className={`ui-switch ${className}`.trim()}><Switch.Thumb className="ui-switch-thumb"/></Switch.Root>;
});
