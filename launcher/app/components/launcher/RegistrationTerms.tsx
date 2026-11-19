import {useCallback, useEffect, useRef, useState} from 'react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {Check} from '../../ui/controls';
import {Dialog, DialogClose, DialogContent, DialogTitle} from '../../ui/dialog';
import {renderHelpContent} from '../documents/content';
import '../../ui/document-body.css';

export function RegistrationTerms({accepted, disabled, onChange}: {
  accepted: boolean;
  disabled: boolean;
  onChange(accepted: boolean): void;
}) {
  const {locale, t} = useLocale();
  const checkbox = useRef<HTMLButtonElement>(null);
  const readingPane = useRef<HTMLElement>(null);
  const [open, setOpen] = useState(false);
  const [reachedEnd, setReachedEnd] = useState(false);

  const checkEnd = useCallback(() => {
    const pane = readingPane.current;
    if (pane && pane.clientHeight > 0 && pane.scrollTop + pane.clientHeight >= pane.scrollHeight - 1) setReachedEnd(true);
  }, []);

  useEffect(() => {
    const pane = readingPane.current;
    if (!open || !pane || typeof ResizeObserver === 'undefined') return;
    const observer = new ResizeObserver(checkEnd);
    observer.observe(pane);
    return () => observer.disconnect();
  }, [open, checkEnd]);

  return <>
    <label className="account-terms-check" htmlFor="registration-terms">
      <Check id="registration-terms" ref={checkbox} checked={accepted} disabled={disabled} aria-haspopup="dialog"
        onCheckedChange={checked => {
          if (checked) {
            setReachedEnd(false);
            setOpen(true);
          } else {
            onChange(false);
          }
        }}/>
      {t('account.termsConsent')}
    </label>
    <Dialog open={open} onOpenChange={setOpen}>
      <DialogContent className="account-terms-dialog" showClose={false} aria-describedby={undefined}
        onOpenAutoFocus={event => {
          event.preventDefault();
          readingPane.current?.focus({preventScroll: true});
          checkEnd();
        }}
        onCloseAutoFocus={event => {
          event.preventDefault();
          checkbox.current?.focus({preventScroll: true});
        }}>
        <DialogTitle className="sr-only">{t('document.terms')}</DialogTitle>
        <section className="account-terms-reading ui-document ui-document--fumadocs prose" lang={locale}
          aria-label={t('document.terms')} ref={readingPane} onScroll={checkEnd}
          // biome-ignore lint/a11y/noNoninteractiveTabindex: The scroll pane must be reachable by keyboard to read the terms.
          tabIndex={0}>
          {renderHelpContent(t('content.terms'), new URL('https://eagler.invalid/terms.html'), locale)}
        </section>
        <div className="account-terms-actions">
          <DialogClose asChild><Button variant="outline">{t('action.cancel')}</Button></DialogClose>
          <Button disabled={!reachedEnd} onClick={() => {
            onChange(true);
            setOpen(false);
          }}>{t('account.termsAgree')}</Button>
        </div>
      </DialogContent>
    </Dialog>
  </>;
}
