import {useId, useState, type ReactNode} from 'react';
import {cn} from './utils';
import './disclosure.css';

type PanelState = {id: string; open: boolean};

type DisclosureState = {
  open: boolean;
  trigger: {'aria-expanded': boolean; 'aria-controls': string; onClick(): void};
  panel: PanelState;
};

/** The caller lays out the native button and chooses its label and indicator. */
export function Disclosure({id, open: controlledOpen, onOpenChange, children}: {
  id?: string; children(state: DisclosureState): ReactNode;
} & ({open: boolean; onOpenChange(open: boolean): void} | {open?: never; onOpenChange?: never})) {
  const generatedId = useId();
  const [expanded, setExpanded] = useState(false);
  const panelId = id ?? generatedId;
  const open = controlledOpen ?? expanded;
  return children({
    open,
    trigger: {'aria-expanded': open, 'aria-controls': panelId, onClick: () => (onOpenChange ?? setExpanded)(!open)},
    panel: {id: panelId, open},
  });
}

export function DisclosurePanel({id, open, className, clipClassName, children}: PanelState & {
  className?: string; clipClassName?: string; children: ReactNode;
}) {
  return <div className={cn('ui-disclosure-panel', className)} id={id} data-open={open} inert={!open} aria-hidden={!open || undefined}>
    <div className={cn('ui-disclosure-clip', clipClassName)}>{children}</div>
  </div>;
}
