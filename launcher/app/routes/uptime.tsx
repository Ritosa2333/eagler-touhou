import {useHref} from 'react-router';
import {StandalonePageShell} from '../standalone-pages';
import {UptimePage} from '../components/uptime/UptimePage';
import {translate} from '../i18n';

export const meta = () => [{title: `${translate('zh-CN', 'nav.serviceStatus')} | EAGLER TOUHOU`}];

export default function UptimeRoute() {
  const base = useHref('/');
  return <StandalonePageShell><UptimePage endpoint={base + 'uptime/summary'}/></StandalonePageShell>;
}
