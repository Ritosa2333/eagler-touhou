import { Play, Upload } from 'lucide-react';
import type { ReactNode } from 'react';
import { Button } from '../ui/button';

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
export function LaunchActions({ primary, secondary, placement = 'standard' }: LaunchActionsProps) {
  return (
    <div className={`launch-wrap launch-actions${placement === 'compact' ? ' launch-actions-compact' : ''}`}>
      <Button
        size="legacy"
        className="launch"
        id={primary.id}
        type="button"
        disabled={primary.disabled}
        onClick={primary.onClick}
      >
        <span id={primary.textId}>{primary.label}</span>
        <span className="launch-icon" aria-hidden="true">
          <Play size={22} aria-hidden="true" focusable="false" />
        </span>
      </Button>
      <Button
        variant="outline"
        size="legacy"
        className="launch launch-secondary"
        id={secondary.id}
        type="button"
        disabled={secondary.disabled}
        onClick={secondary.onClick}
      >
        <span className="launch-icon" aria-hidden="true">
          <Upload size={20} aria-hidden="true" focusable="false" />
        </span>
        <span id={secondary.textId}>{secondary.label}</span>
      </Button>
    </div>
  );
}
