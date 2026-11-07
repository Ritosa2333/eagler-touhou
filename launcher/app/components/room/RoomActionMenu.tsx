import type {ReactNode} from 'react';
import {Button} from '../../ui/button';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';

export interface RoomActionMenuItem {id: string; label: string; disabled?: boolean; danger?: boolean; select(): void;}
export function RoomActionMenu({enabled, label, triggerContent, triggerKind = 'action', menuClassName, items, minimumWidth = 260, openOnClick, align = 'start'}: {
  enabled: boolean; label: string; triggerContent: ReactNode; triggerKind?: 'action' | 'quick-chat' | 'player'; menuClassName: string;
  items: readonly RoomActionMenuItem[]; minimumWidth?: number; openOnClick?: boolean; align?: 'start' | 'end';
}) {
  return <ActionMenu label={label} openOnClick={openOnClick} side="top" align={align} className={`room-action-menu ${menuClassName}`} minimumWidth={minimumWidth}
    trigger={<Button variant={triggerKind === 'player' ? 'ghost' : triggerKind === 'quick-chat' ? 'secondary' : 'outline'} size={triggerKind === 'player' ? 'icon' : triggerKind === 'quick-chat' ? 'lg' : 'default'}
      className={triggerKind === 'player'
        ? 'room-member-menu-trigger text-inherit hover:bg-white/15 hover:text-inherit data-[state=open]:bg-white/15 focus-visible:ring-current focus-visible:ring-offset-0'
        : triggerKind === 'quick-chat' ? 'room-quick-chat-trigger h-12 w-full justify-between gap-3 data-[state=open]:bg-accent' : undefined}
      title={label} aria-label={label} disabled={!enabled}>{triggerContent}</Button>}>
    {items.map(item => <ActionMenuItem key={item.id} data-phrase={item.id} destructive={item.danger} disabled={item.disabled} onAction={item.select}>{item.label}</ActionMenuItem>)}
  </ActionMenu>;
}
