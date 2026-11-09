import { Button as UiButton } from '../ui/controls';
import { InputGroup, InputGroupInput } from '../ui/input-group';
import { TriangleAlert } from 'lucide-react';
import { useEffect, useRef, useState } from 'react';
import { useLocale } from '../i18n';
import { closeMainSelectMenus } from './launcher/MainSelect';
import type { DecisionInput, DecisionComparison } from '../models/decisions';
import { animateSurface, type SurfaceAnimation } from '../ui/surface-motion';

const decisionActionClass = 'h-auto min-h-9 whitespace-normal px-3 py-1.5 pointer-coarse:min-h-11 max-[780px]:min-h-11';

/** Main's decision-dialog structure, focus and discard/cancel behavior. No history owner. */
export function ConfirmationDialog({
  open,
  title,
  message,
  confirmText,
  cancelText,
  tone = 'normal',
  secondaryText = '',
  variant = '',
  hideCancel = false,
  confirmOnEnter = false,
  input,
  comparison,
  onConfirm,
  onCancel,
  onSecondary,
  idPrefix = 'decision',
}: {
  open: boolean;
  title: string;
  message: string;
  confirmText: string;
  cancelText: string;
  tone?: 'normal' | 'danger';
  secondaryText?: string;
  variant?: string;
  hideCancel?: boolean;
  confirmOnEnter?: boolean;
  input?: DecisionInput;
  comparison?: DecisionComparison;
  onConfirm(value?: string): void;
  onCancel(): void;
  onSecondary?(): void;
  idPrefix?: string;
}) {
  const { t } = useLocale();
  const dialogRef = useRef<HTMLDialogElement>(null);
  const cancelRef = useRef<HTMLButtonElement>(null);
  const confirmRef = useRef<HTMLButtonElement>(null);
  const inputRef = useRef<HTMLInputElement>(null);
  const [inputError, setInputError] = useState('');
  const animation = useRef<SurfaceAnimation | null>(null);
  const finishRef = useRef<(() => void) | null>(null);
  const callbacks = useRef({ onConfirm, onCancel, onSecondary });
  callbacks.current = { onConfirm, onCancel, onSecondary };
  useEffect(() => {
    const dialog = dialogRef.current;
    if (!dialog || !open) return;
    setInputError('');
    closeMainSelectMenus();
    const focusReturn = document.activeElement instanceof HTMLElement ? document.activeElement : null;
    let settled = false;
    const closed = () => {
      // Main app.mts8972 settles native close(), not only our button path.
      // A queued close from an effect replay cannot settle a reopened dialog.
      if (dialog.open || settled) return;
      settled = true;
      animation.current?.cancel();
      animation.current = null;
      finishRef.current = null;
      dialog.classList.remove('closing');
      const choice = dialog.returnValue;
      if (choice === 'confirm') callbacks.current.onConfirm(inputRef.current?.value);
      else if (choice === 'secondary') callbacks.current.onSecondary?.();
      else callbacks.current.onCancel();
      if (focusReturn?.isConnected) focusReturn.focus({ preventScroll: true });
    };
    dialog.addEventListener('close', closed);
    dialog.returnValue = 'cancel';
    dialog.setAttribute('data-window-managed', '');
    dialog.showModal();
    animation.current = animateSurface(dialog, 'enter', { kind: 'dialog' });
    (inputRef.current ?? (hideCancel ? confirmRef.current : cancelRef.current))?.focus({ preventScroll: true });
    inputRef.current?.select();
    return () => {
      animation.current?.cancel();
      animation.current = null;
      finishRef.current = null;
      dialog.classList.remove('closing');
      dialog.removeEventListener('close', closed);
      if (dialog.open) dialog.close();
      if (!settled && focusReturn?.isConnected) focusReturn.focus({ preventScroll: true });
    };
  }, [open, hideCancel]);
  function close(choice: 'confirm' | 'cancel' | 'secondary') {
    const dialog = dialogRef.current;
    if (!dialog?.open || finishRef.current) return;
    if (choice === 'confirm' && input) {
      const error = input.validate?.(inputRef.current?.value ?? input.defaultValue);
      setInputError(error ?? '');
      if (error) {
        inputRef.current?.focus({ preventScroll: true });
        return;
      }
    }
    dialog.returnValue = choice;
    const finish = () => {
      if (finishRef.current !== finish) return;
      finishRef.current = null;
      const previous = animation.current;
      animation.current = null;
      dialog.classList.remove('closing');
      if (dialog.open) dialog.close(choice);
      previous?.cancel();
    };
    finishRef.current = finish;
    dialog.classList.add('closing');
    const next = animateSurface(dialog, 'exit', { kind: 'dialog', fromCurrent: true });
    animation.current = next;
    if (next) void next.finished.then(finish, finish);
    else finish();
  }
  return (
    <dialog
      ref={dialogRef}
      className="decision-dialog"
      id={`${idPrefix}Dialog`}
      aria-labelledby={`${idPrefix}Title`}
      aria-describedby={message ? `${idPrefix}Message` : undefined}
      data-tone={tone}
      data-options={String(Number(!hideCancel) + Number(!!secondaryText) + 1)}
      data-variant={variant}
      data-confirm-on-enter={String(confirmOnEnter)}
      data-input={input ? '' : undefined}
      onCancel={(event) => {
        event.preventDefault();
        close('cancel');
      }}
      onKeyDown={(event) => {
        if (
          event.key !== 'Enter' ||
          event.defaultPrevented ||
          event.target instanceof HTMLButtonElement ||
          (event.nativeEvent as KeyboardEvent).isComposing
        )
          return;
        event.preventDefault();
        if (confirmOnEnter) confirmRef.current?.click();
      }}
    >
      <form
        method="dialog"
        className="decision-window"
        onSubmit={(event) => {
          event.preventDefault();
          const submitter = (event.nativeEvent as SubmitEvent).submitter;
          const value = submitter instanceof HTMLButtonElement ? submitter.value : 'cancel';
          close(value === 'confirm' || value === 'secondary' ? value : 'cancel');
        }}
      >
        <div className="decision-body">
          {!input && (tone === 'danger' || variant === 'browser-warning') && (
            <div className="decision-mark" aria-hidden="true">
              <TriangleAlert />
            </div>
          )}
          <header>
            <strong id={`${idPrefix}Title`}>{title}</strong>
          </header>
          {(message || input) && <p className="decision-message" id={`${idPrefix}Message`}>{message}</p>}
          {comparison && <SaveComparisonCards value={comparison}/>}
          {input && (
            <div className="decision-input">
              <label htmlFor={`${idPrefix}Input`}>{input.label}</label>
              <InputGroup>
                <InputGroupInput
                  ref={inputRef}
                  id={`${idPrefix}Input`}
                  defaultValue={input.defaultValue}
                  autoComplete="off"
                  spellCheck={false}
                  aria-invalid={inputError ? true : undefined}
                  aria-describedby={inputError || input.description ? `${idPrefix}InputHelp` : undefined}
                  onChange={() => setInputError('')}
                />
              </InputGroup>
              {(input.description || inputError) && (
                <p
                  id={`${idPrefix}InputHelp`}
                  className="decision-input-help"
                  role={inputError ? 'alert' : undefined}
                  data-error={!!inputError}
                >
                  {inputError || input.description}
                </p>
              )}
            </div>
          )}
        </div>
        <footer>
          <UiButton
            type="submit"
            ref={cancelRef}
            className={`decision-cancel ${decisionActionClass}`}
            id={`${idPrefix}Cancel`}
            value="cancel"
            hidden={hideCancel}
          >
            {cancelText}
          </UiButton>
          <UiButton
            variant={variant === 'browser-warning' ? 'primary' : 'action'}
            type="submit"
            className={`decision-secondary ${decisionActionClass}`}
            id={`${idPrefix}Secondary`}
            value="secondary"
            hidden={!secondaryText}
          >
            {secondaryText || t('action.backgroundDownload')}
          </UiButton>
          <UiButton
            variant={variant === 'browser-warning' ? 'action' : tone === 'danger' ? 'danger' : 'primary'}
            type="submit"
            ref={confirmRef}
            className={`decision-confirm ${decisionActionClass}${tone === 'danger' ? ' bg-destructive text-destructive-foreground hover:bg-destructive/90' : ''}`}
            id={`${idPrefix}Confirm`}
            value="confirm"
          >
            {confirmText}
          </UiButton>
        </footer>
      </form>
    </dialog>
  );
}

export function SaveComparisonCards({value}: {value: DecisionComparison}) {
  return <div className="decision-comparison">
    <div className="decision-versions">
      {value.versions.map(version => <section className="decision-version" data-mark={version.mark?.tone} key={version.title}>
        <header>
          <h3>{version.title}</h3>
          {version.mark && <span className="decision-version-mark">{version.mark.label}</span>}
        </header>
        {version.empty ? <p className="decision-version-empty">{version.empty}</p> : <>
          <p className="decision-version-date"><span>{version.timestamp?.label}</span>{version.timestamp?.value}</p>
          <dl>{version.rows.map(row => <div key={row.label}><dt>{row.label}</dt><dd>{row.value}</dd></div>)}</dl>
        </>}
        {!!version.files.length && <details>
          <summary>{value.filesLabel} <span>{version.files.length}</span></summary>
          <ul>{version.files.map(file => <li key={file.name}><span>{file.name}</span><small>{file.size}</small></li>)}</ul>
        </details>}
      </section>)}
    </div>
  </div>;
}
