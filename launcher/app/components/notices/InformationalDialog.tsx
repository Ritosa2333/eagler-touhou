import {Button as UiButton} from '../../ui/controls';
import {X} from 'lucide-react';
import type {ReactNode} from 'react';
import {useMainDialog, type MainDialogProps} from './use-main-dialog';
import './informational-dialog.css';

export const informationalDialogPresentation = {
  standard: {layout: 'standard', window: 'standard'},
  artwork: {layout: 'artwork', window: 'standard'},
  scrollable: {layout: 'scrollable', window: 'scrollable'},
} as const;
type Presentation = typeof informationalDialogPresentation[keyof typeof informationalDialogPresentation];
interface InformationalDialogProps extends MainDialogProps {
  id: string;
  titleId: string;
  closeId: string;
  title: ReactNode;
  closeLabel: string;
  presentation: Presentation;
  launcherDocument?: boolean;
  immediateClose?: boolean;
  children: ReactNode | ((actions: {closeImmediately(): void}) => ReactNode);
}
/** One native modal owner. Callers supply content and appearance, never lifecycle handlers. */
export function InformationalDialog({id, titleId, closeId, title, closeLabel, presentation, launcherDocument = false, immediateClose = false, children, ...lifecycle}: InformationalDialogProps) {
  const dialog = useMainDialog(lifecycle, {immediateClose});
  // biome-ignore lint/a11y/useKeyWithClickEvents: Native cancel and the labelled close button handle keyboard dismissal; this click only handles its backdrop.
  return <dialog ref={dialog.ref} id={id} aria-labelledby={titleId} data-launcher-document={launcherDocument ? '' : undefined}
    className={`informational-dialog informational-dialog--${presentation.layout}`}
    onCancel={dialog.onCancel} onClick={dialog.onClick}>
    <article className={`informational-window informational-window--${presentation.window}`}><header>
      <h1 id={titleId}>{title}</h1><UiButton variant="icon" id={closeId} type="button" aria-label={closeLabel} onClick={dialog.requestClose}><X aria-hidden="true"/></UiButton>
    </header>{typeof children === 'function' ? children({closeImmediately: dialog.requestImmediateClose}) : children}</article>
  </dialog>;
}
