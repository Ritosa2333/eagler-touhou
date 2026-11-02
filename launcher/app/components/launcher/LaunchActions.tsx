import {Button as UiButton} from '../../ui/button';
import {Play, FolderInput} from 'lucide-react';
import type {ReactNode} from 'react';

export interface LaunchAction {
  id: string;
  textId?: string;
  label: ReactNode;
  onClick(): void;
  disabled?: boolean;
}
export interface LaunchActionsProps {
  primary: LaunchAction;
  secondary: LaunchAction;
  /** Compact placement retains the original embedded replay row dimensions. */
  placement?: 'standard' | 'compact';
}
/** Shared play/import presentation; session and replay owners decide intent. */
export function LaunchActions({primary, secondary, placement = 'standard'}: LaunchActionsProps) {
  return <div className={`launch-wrap launch-actions${placement === 'compact' ? ' launch-actions-compact' : ''}`}>
    <UiButton size="lg" className="launch" id={primary.id} type="button" disabled={primary.disabled} onClick={primary.onClick}><Play aria-hidden="true"/><span id={primary.textId}>{primary.label}</span></UiButton>
    <UiButton variant="outline" size="lg" className="launch launch-secondary" id={secondary.id} type="button" disabled={secondary.disabled} onClick={secondary.onClick}><FolderInput aria-hidden="true"/><span id={secondary.textId}>{secondary.label}</span></UiButton>
  </div>;
}
