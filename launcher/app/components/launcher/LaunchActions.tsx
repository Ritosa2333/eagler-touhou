import {Button as UiButton} from '../../ui/button';
import {Play, Upload} from 'lucide-react';
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
    <UiButton size="lg" className="launch" id={primary.id} type="button" disabled={primary.disabled} onClick={primary.onClick}><span id={primary.textId}>{primary.label}</span><span className="launch-icon" aria-hidden="true"><Play/></span></UiButton>
    <UiButton variant="outline" size="lg" className="launch launch-secondary" id={secondary.id} type="button" disabled={secondary.disabled} onClick={secondary.onClick}><span className="launch-icon" aria-hidden="true"><Upload/></span><span id={secondary.textId}>{secondary.label}</span></UiButton>
  </div>;
}
