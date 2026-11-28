import {Tooltip} from 'radix-ui';
import type {ReactElement, ReactNode} from 'react';

export function MastheadActionLabel({children}: {children: ReactNode}) {
  return <span className="masthead-action-label" aria-hidden="true">{children}</span>;
}

export function MastheadTooltip({label, children, iconOnly}: {label: string; children: ReactElement; iconOnly?: boolean}) {
  return <Tooltip.Root>
    <Tooltip.Trigger asChild>{children}</Tooltip.Trigger>
    <Tooltip.Portal>
      <Tooltip.Content className="masthead-tooltip" data-icon-only={iconOnly || undefined} side="bottom" sideOffset={6} collisionPadding={8}>
        {label}
      </Tooltip.Content>
    </Tooltip.Portal>
  </Tooltip.Root>;
}
