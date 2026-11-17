import {Tooltip} from 'radix-ui';
import type {ReactElement} from 'react';

export function MastheadTooltip({label, children}: {label: string; children: ReactElement}) {
  return <Tooltip.Root>
    <Tooltip.Trigger asChild>{children}</Tooltip.Trigger>
    <Tooltip.Portal>
      <Tooltip.Content className="masthead-tooltip" side="bottom" sideOffset={6} collisionPadding={8}>
        {label}
      </Tooltip.Content>
    </Tooltip.Portal>
  </Tooltip.Root>;
}
