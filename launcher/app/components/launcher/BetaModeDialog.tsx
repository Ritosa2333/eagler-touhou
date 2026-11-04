import { useState } from 'react';
import { Dialog, DialogContent, DialogTitle, DialogDescription } from '../../ui/dialog';
import { Button } from '../../ui/button';
import { useLocale } from '../../i18n';
import { betaProgramBase, isBetaPublication, reloadReleaseMode } from '../../../src/launcher/release-channel.mts';

export function BetaModeDialog({
  open,
  onOpenChange,
  onClosed,
  baseUrl,
}: {
  open: boolean;
  onOpenChange(open: boolean): void;
  onClosed?(): void;
  baseUrl: string;
}) {
  const { t } = useLocale(),
    [busy, setBusy] = useState(false),
    [error, setError] = useState('');
  if (!open) return null;
  async function enter() {
    setBusy(true);
    setError('');
    const controller = new AbortController(),
      timer = setTimeout(() => controller.abort(), 12000);
    try {
      const response = await fetch(new URL('site-manifest.json', betaProgramBase(baseUrl)), {
        cache: 'no-store',
        signal: controller.signal,
      });
      if (!response.ok || !isBetaPublication(await response.json()))
        throw Error(t('developer.betaUnavailable'));
      reloadReleaseMode(window, true);
    } catch {
      setError(t('developer.betaUnavailable'));
      setBusy(false);
    } finally {
      clearTimeout(timer);
    }
  }
  return (
    <Dialog
      open={open}
      onOpenChange={(value) => {
        if (!busy) {
          setError('');
          onOpenChange(value);
        }
      }}
    >
      <DialogContent
        onCloseAutoFocus={(event) => {
          event.preventDefault();
          onClosed?.();
        }}
      >
        <DialogTitle>{t('developer.betaFeatures')}</DialogTitle>
        <DialogDescription>{t('developer.betaWarning')}</DialogDescription>
        {error && <p role="alert">{error}</p>}
        <div className="flex justify-end gap-2">
          <Button
            variant="outline"
            disabled={busy}
            onClick={() => {
              setError('');
              onOpenChange(false);
            }}
          >
            {t('developer.betaCancel')}
          </Button>
          <Button
            disabled={busy}
            onClick={() => {
              void enter();
            }}
          >
            {t('developer.betaConfirm')}
          </Button>
        </div>
      </DialogContent>
    </Dialog>
  );
}
