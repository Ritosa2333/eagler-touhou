import {useLocale} from '../../i18n';
import type {MainDialogProps} from './use-main-dialog';
import {InformationalDialog, informationalDialogPresentation} from './InformationalDialog';
export interface DonationDialogProps extends MainDialogProps {
  assetUrl(path: string): string;
  /** Main hides both donation triggers after missing artwork, without fallback copy. */
  onArtworkUnavailable(): void;
  /** Main library animates; original standalone lobby closes immediately. */
  closeDurationMs?: 0 | 220;
}
/** Authored donation artwork; all surrounding controls use the site locale. */
export function DonationDialog({assetUrl, onArtworkUnavailable, closeDurationMs, ...props}: DonationDialogProps) {
  const {t} = useLocale();
  return <InformationalDialog {...props} id="donationDialog" titleId="donationTitle" closeId="donationClose"
    title={t('donation.title')} closeLabel={t('donation.close')} presentation={informationalDialogPresentation.artwork} immediateClose={closeDurationMs === 0}>
    {({closeImmediately}) =>
      <div className="donation-image-wrap"><p className="donation-scan-hint">{t('donation.scan')}</p><img id="donationImage" src={assetUrl('assets/donation.webp')} alt={t('donation.image')} decoding="async" onError={() => {closeImmediately(); onArtworkUnavailable();}}/></div>
    }
  </InformationalDialog>;
}
