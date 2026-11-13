import {Info} from 'lucide-react';
import {useLocale} from '../i18n';
import {Button} from './button';
import {Popover, PopoverContent, PopoverTrigger} from './popover';

export function FieldHelp({id, label, children}: {id: string; label: string; children: string}) {
  const {t} = useLocale();
  return <>
    <span id={id} className="sr-only">{children}</span>
    <Popover>
      <PopoverTrigger asChild>
        <Button variant="ghost" size="icon-sm" className="size-6 pointer-coarse:size-8" aria-label={`${t('nav.help')} | ${label}`} aria-describedby={id}>
          <Info aria-hidden="true"/>
        </Button>
      </PopoverTrigger>
      <PopoverContent aria-label={label} className="ui-field-help" onOpenAutoFocus={event => event.preventDefault()}>
        {children}
      </PopoverContent>
    </Popover>
  </>;
}
