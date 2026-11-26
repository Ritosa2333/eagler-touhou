import {ArrowLeft} from 'lucide-react';
import type {ComponentProps} from 'react';
import {useLocale} from '../../i18n';
import {Button} from '../../ui/button';
import {AppLink} from './AppLink';

export function LauncherBack({className = '', ...link}: Pick<ComponentProps<typeof AppLink>, 'href' | 'replace' | 'returnToEntry' | 'onClick' | 'preventScrollReset'> & {className?: string}) {
  const {t} = useLocale();
  return <Button variant="ghost" size="icon" asChild className={`launcher-back ${className}`}>
    <AppLink {...link} aria-label={t('action.back')}><ArrowLeft aria-hidden="true"/></AppLink>
  </Button>;
}
