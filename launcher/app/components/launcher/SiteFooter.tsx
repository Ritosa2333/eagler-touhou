import {Button as UiButton} from '../../ui/controls';
import {useLocale} from '../../i18n';
/** Project links, server support and registration. */
export function SiteFooter({onDonation, donationAvailable = true}: {onDonation: () => void; donationAvailable?: boolean}) {
  const {t} = useLocale();
  return <footer className="site-footer">
    <div className="site-footer-inner">
      <p>{t('footer.license')}<a href="https://github.com/YomotsuHisami/eagler-touhou" target="_blank" rel="noopener noreferrer">{t('footer.repository')}</a><i aria-hidden="true">/</i><a href="https://qm.qq.com/q/eeUrxIltug" target="_blank" rel="noopener noreferrer">{t('footer.group')}</a><i aria-hidden="true">/</i><UiButton variant="unstyled" className="site-footer-link" id="donationOpen" type="button" hidden={!donationAvailable} onClick={onDonation}>{t('footer.donate')}</UiButton></p>
      <p className="site-footer-record"><a href="https://beian.miit.gov.cn/" target="_blank" rel="noopener noreferrer">赣ICP备2025074288号-1</a></p>
    </div>
  </footer>;
}
