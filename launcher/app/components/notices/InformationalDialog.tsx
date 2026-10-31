import type { ReactNode } from 'react';
import { Button } from '../ui/button';
import { informationalDialogMotion } from './informational-dialog-motion';
import { type MainDialogProps, useMainDialog } from './use-main-dialog';
import './informational-dialog.css';

export const informationalDialogPresentation = {
  standard: { layout: 'standard', window: 'standard' },
  artwork: { layout: 'artwork', window: 'standard' },
  scrollable: { layout: 'scrollable', window: 'scrollable' },
} as const;
type Presentation = (typeof informationalDialogPresentation)[keyof typeof informationalDialogPresentation];
interface InformationalDialogProps extends MainDialogProps {
  id: string;
  titleId: string;
  closeId: string;
  title: ReactNode;
  closeLabel: string;
  presentation: Presentation;
  launcherDocument?: boolean;
  immediateClose?: boolean;
  children: ReactNode | ((actions: { closeImmediately(): void }) => ReactNode);
}
/** One native modal owner. Callers supply content and appearance, never lifecycle handlers. */
export function InformationalDialog({
  id,
  titleId,
  closeId,
  title,
  closeLabel,
  presentation,
  launcherDocument = false,
  immediateClose = false,
  children,
  ...lifecycle
}: InformationalDialogProps) {
  const dialog = useMainDialog(lifecycle, { motion: informationalDialogMotion, immediateClose });
  return (
    <dialog
      ref={dialog.ref}
      id={id}
      aria-labelledby={titleId}
      data-launcher-document={launcherDocument ? '' : undefined}
      className={`informational-dialog informational-dialog--${presentation.layout}`}
      onCancel={dialog.onCancel}
      onClick={dialog.onClick}
    >
      <article className={`informational-window informational-window--${presentation.window}`}>
        <header>
          <h1 id={titleId}>{title}</h1>
          <Button
            variant="legacy"
            size="legacy"
            id={closeId}
            type="button"
            aria-label={closeLabel}
            onClick={dialog.requestClose}
          >
            ×
          </Button>
        </header>
        {typeof children === 'function' ? children({ closeImmediately: dialog.requestImmediateClose }) : children}
      </article>
    </dialog>
  );
}
