import {useState, type ReactNode} from 'react';
import {Disclosure, DisclosurePanel} from '../../ui/disclosure';
import {ChevronRight, type LucideIcon} from 'lucide-react';
import {Toggle} from '../../ui/controls';
import {OptionsGroupHeading} from './OptionsGroup';
import './settings-group.css';

export function SettingsGroup({children}: {children: ReactNode}) {
  return <div className="settings-group">{children}</div>;
}

export function SettingsRowContent({icon, children}: {icon?: ReactNode; children: ReactNode}) {
  return <>{icon}<span>{children}</span><ChevronRight className="settings-row-chevron" aria-hidden="true"/></>;
}

export function SettingsDisclosure({title, icon: Icon, children}: {
  title: string; icon?: LucideIcon; children: ReactNode;
}) {
  const [open, setOpen] = useState(false);
  const [visited, setVisited] = useState(false);
  return <Disclosure open={open} onOpenChange={next => {
    if (next) setVisited(true);
    setOpen(next);
  }}>{({trigger, panel}) => <div className="settings-disclosure">
    <OptionsGroupHeading icon={Icon} className="settings-disclosure-trigger" {...trigger}>{title}</OptionsGroupHeading>
    <DisclosurePanel {...panel}>
      {visited && <div className="settings-disclosure-body">{children}</div>}
    </DisclosurePanel>
  </div>}</Disclosure>;
}

export function SettingsField({label, labelId, children}: {label: string; labelId?: string; children: ReactNode}) {
  return <div className="settings-value-row"><span id={labelId}>{label}</span>{children}</div>;
}

export function SettingsSwitch({label, checked, onChange}: {label: string; checked: boolean; onChange(): void}) {
  return <SettingsField label={label}>
    <Toggle className="settings-switch" aria-label={label} checked={checked} onCheckedChange={onChange}/>
  </SettingsField>;
}
