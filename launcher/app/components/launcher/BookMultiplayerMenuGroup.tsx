import type {ComponentPropsWithoutRef} from 'react';
import {useLocale} from '../../i18n';
import {ActionMenuGroup, ActionMenuLabel} from '../../ui/action-menu';
import {multiplayerGroupLabelKey, type MultiplayerGroup} from './products';

export function BookMultiplayerMenuGroup({group, children, ...props}: ComponentPropsWithoutRef<typeof ActionMenuGroup> & {group: MultiplayerGroup}) {
  const {t} = useLocale();
  const label = t(multiplayerGroupLabelKey(group));
  return <ActionMenuGroup {...props} aria-label={label}>
    <ActionMenuLabel className="px-2 pt-3 pb-1 text-xs font-medium text-muted-foreground">{label}</ActionMenuLabel>
    {children}
  </ActionMenuGroup>;
}
