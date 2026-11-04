import { useState } from 'react';
import { Dialog, DialogContent, DialogTitle, DialogDescription } from '../../ui/dialog';
import { Button } from '../../ui/button';
import { useLocale } from '../../i18n';
import { BETA_OPT_IN_KEY, isBetaPublication, releaseChannelUrl } from '../../../src/launcher/release-channel.mts';

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
  async function enter() {
    setBusy(true);
    setError('');
    const controller = new AbortController(),
      timer = setTimeout(() => controller.abort(), 12000);
    try {
      const destination = releaseChannelUrl(window.location.href, baseUrl, true);
      const response = await fetch(new URL('site-manifest.json', new URL('./', destination)), {
        cache: 'no-store',
        signal: controller.signal,
      });
      if (!response.ok || !isBetaPublication(await response.json()))
        throw Error(t('developer.betaUnavailable'));
      window.localStorage.setItem(BETA_OPT_IN_KEY, '1');
      window.location.assign(destination);
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
