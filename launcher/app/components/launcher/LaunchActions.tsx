import { Button as UiButton } from '../../ui/button';
import { Clock3, Download, Play, FolderInput, RotateCcw } from 'lucide-react';
import type { ReactNode } from 'react';
import { Progress } from '../../ui/progress';
import './launch-actions.css';

export interface LaunchAction {
  id: string;
  textId?: string;
  label: ReactNode;
  icon?: 'download' | 'clock';
  activity?: {state: 'active' | 'queued'; value: number | null};
  onClick(): void;
  disabled?: boolean;
}
export interface LaunchActionsProps {
  primary: LaunchAction;
  /** Keep the primary button sized for every state in the current language. */
  primaryStateLabels?: readonly string[];
  secondary?: Omit<LaunchAction, 'icon'> & {icon?: 'repeat'};
  loading?: ReactNode;
  /** Compact placement retains the original embedded replay row dimensions. */
  placement?: 'standard' | 'compact';
}
/** Shared play/import presentation; session and replay owners decide intent. */
export function LaunchActions({ primary, primaryStateLabels, secondary, loading, placement = 'standard' }: LaunchActionsProps) {
  const PrimaryIcon = primary.icon === 'clock' ? Clock3 : primary.icon === 'download' ? Download : Play;
  const SecondaryIcon = secondary?.icon === 'repeat' ? RotateCcw : FolderInput;
  const primaryLabel = <span id={primary.textId}>{primary.label}</span>;
  return (
    <div className={`launch-wrap launch-actions${placement === 'compact' ? ' launch-actions-compact' : ''}`}>
      <UiButton
        className={`launch min-h-9 px-3 pointer-coarse:min-h-11 max-[780px]:min-h-11${primary.activity ? ' disabled:opacity-100' : ''}`}
        id={primary.id}
        type="button"
        data-action-icon={primary.icon ?? 'play'}
        data-action-activity={primary.activity?.state}
        disabled={primary.disabled || !!loading}
        aria-busy={loading || primary.activity ? true : undefined}
        onClick={primary.onClick}
      >
        <PrimaryIcon aria-hidden="true" />
        {primaryStateLabels ? <span className="launch-action-label">
          {primaryLabel}
          {Array.from(new Set(primaryStateLabels), label => <span key={label} aria-hidden="true" data-measure-label={label} />)}
        </span> : primaryLabel}
        {primary.activity?.state === 'active' && <Progress className="launch-action-progress" value={primary.activity.value} inline smooth aria-hidden="true"/>}
      </UiButton>
      {secondary && <UiButton
        variant="outline"
        className="launch launch-secondary min-h-9 px-3 pointer-coarse:min-h-11 max-[780px]:min-h-11"
        id={secondary.id}
        type="button"
        disabled={secondary.disabled}
        onClick={secondary.onClick}
      >
        <SecondaryIcon aria-hidden="true" />
        <span id={secondary.textId}>{secondary.label}</span>
      </UiButton>}
      {loading && <div className="launch-actions-status" role="status">{loading}</div>}
    </div>
  );
}
