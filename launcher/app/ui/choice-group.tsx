import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {ToggleGroup} from 'radix-ui';
import {cn} from './utils';

export const ChoiceGroup = ToggleGroup.Root;
export const ChoiceItem = forwardRef<HTMLButtonElement, ComponentPropsWithoutRef<typeof ToggleGroup.Item>>(function ChoiceItem({className, ...props}, ref) {
  return <ToggleGroup.Item {...props} ref={ref} data-slot="choice-item" className={cn('ui-kit-choice inline-flex items-center justify-center gap-2 rounded-md border-0 bg-transparent px-3 py-2 font-sans text-sm font-medium text-muted-foreground outline-none transition-colors hover:bg-accent hover:text-foreground focus-visible:ring-2 focus-visible:ring-ring focus-visible:ring-inset data-[state=on]:bg-secondary data-[state=on]:text-foreground disabled:pointer-events-none disabled:opacity-50 [&_svg]:size-4 [&_svg]:shrink-0', className)}/>;
});
