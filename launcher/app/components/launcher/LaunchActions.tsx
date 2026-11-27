import { Button as UiButton } from '../../ui/button';
import { Download, Play, FolderInput } from 'lucide-react';
import type { ReactNode } from 'react';
import './launch-actions.css';

export interface LaunchAction {
  id: string;
  textId?: string;
  label: ReactNode;
  icon?: 'download';
  onClick(): void;
  disabled?: boolean;
}
export interface LaunchActionsProps {
  primary: LaunchAction;
  /** Keep the primary button sized for every state in the current language. */
  primaryStateLabels?: readonly string[];
  secondary?: LaunchAction;
  loading?: ReactNode;
  /** Compact placement retains the original embedded replay row dimensions. */
  placement?: 'standard' | 'compact';
}
/** Shared play/import presentation; session and replay owners decide intent. */
export function LaunchActions({ primary, primaryStateLabels, secondary, loading, placement = 'standard' }: LaunchActionsProps) {
  const PrimaryIcon = primary.icon === 'download' ? Download : Play;
  const primaryLabel = <span id={primary.textId}>{primary.label}</span>;
  return (
    <div className={`launch-wrap launch-actions${placement === 'compact' ? ' launch-actions-compact' : ''}`}>
      <UiButton
        className="launch min-h-9 px-3 pointer-coarse:min-h-11 max-[780px]:min-h-11"
        id={primary.id}
        type="button"
        disabled={primary.disabled || !!loading}
        aria-busy={loading ? true : undefined}
        onClick={primary.onClick}
      >
        <PrimaryIcon aria-hidden="true" />
        {primaryStateLabels ? <span className="launch-action-label">
          {primaryLabel}
          {Array.from(new Set(primaryStateLabels), label => <span key={label} aria-hidden="true" data-measure-label={label} />)}
        </span> : primaryLabel}
      </UiButton>
      {secondary && <UiButton
        variant="outline"
        className="launch launch-secondary min-h-9 px-3 pointer-coarse:min-h-11 max-[780px]:min-h-11"
        id={secondary.id}
        type="button"
        disabled={secondary.disabled}
        onClick={secondary.onClick}
      >
        <FolderInput aria-hidden="true" />
        <span id={secondary.textId}>{secondary.label}</span>
      </UiButton>}
      {loading && <div className="launch-actions-status" role="status">{loading}</div>}
    </div>
  );
}
