import {CircleHelp, Info} from 'lucide-react';
import type {ReactNode} from 'react';
import {useLocale} from '../i18n';
import {Button} from './button';
import {Popover, PopoverContent, PopoverTrigger} from './popover';
import './menu-motion.css';

export function FieldHelp({id, label, children, action, option}: {
  id: string; label?: string; children: string; action?: ReactNode; option?: boolean;
}) {
  const {t} = useLocale();
  const name = label ? `${t('nav.help')} | ${label}` : t('nav.help');
  const Icon = option ? CircleHelp : Info;
  return <>
    <span id={id} className="sr-only">{children}</span>
    <Popover>
      <PopoverTrigger asChild>
        <Button variant="ghost" size="icon-sm" className="size-6 pointer-coarse:size-8" aria-label={name}
          aria-describedby={id} title={option ? children : undefined}>
          <Icon aria-hidden="true"/>
        </Button>
      </PopoverTrigger>
      <PopoverContent aria-label={label ?? name} className="ui-field-help ui-hint-motion" onOpenAutoFocus={action ? undefined : event => event.preventDefault()}>
        {children}
        {action && <div className="ui-field-help-action">{action}</div>}
      </PopoverContent>
    </Popover>
  </>;
}
