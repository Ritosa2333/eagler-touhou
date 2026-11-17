import {useHref} from 'react-router';
import {StandalonePageShell} from '../standalone-pages';
import {UptimePage} from '../components/uptime/UptimePage';
import {translate} from '../i18n';
import {sitePageTitle} from '../navigation/document-locale';

export const meta = () => [{title: sitePageTitle(translate('zh-CN', 'nav.serviceStatus'))}];

export default function UptimeRoute() {
  const base = useHref('/');
  return <StandalonePageShell><UptimePage endpoint={base + 'uptime/summary'}/></StandalonePageShell>;
}
