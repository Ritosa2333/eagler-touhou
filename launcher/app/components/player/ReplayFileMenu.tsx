import {MoreHorizontal} from 'lucide-react';
import {Button} from '../../ui/button';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';

export interface ReplayFileAction {label: string; run(): void; destructive?: boolean;}
export function ReplayFileMenu({label, actions, disabled}: {label: string; actions: ReplayFileAction[]; disabled?: boolean}) {
  return <ActionMenu label={label} className="replay-file-menu" trigger={<Button variant="ghost" size="icon" className="replay-file-menu-trigger" disabled={disabled} aria-label={label}><MoreHorizontal aria-hidden="true"/></Button>}>
    {actions.map(action => <ActionMenuItem key={action.label} destructive={action.destructive} onAction={action.run}>{action.label}</ActionMenuItem>)}
  </ActionMenu>;
}
