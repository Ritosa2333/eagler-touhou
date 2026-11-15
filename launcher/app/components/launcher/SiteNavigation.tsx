import {useId, useLayoutEffect, useState, type MouseEvent} from 'react';
import {Collapsible} from 'radix-ui';
import {ChevronDown, MoreHorizontal, type LucideIcon} from 'lucide-react';
import {Button} from '../../ui/button';
import {ActionMenu, ActionMenuItem} from '../../ui/action-menu';
import {useLocale} from '../../i18n';
import {readBookPreference, writeBookPreference} from './book-preferences';
import './site-navigation.css';

export interface SiteNavigationItem {id: string; label: string; href: string; icon: LucideIcon;}
export interface SiteNavigationGroup {id: string; label: string; items: readonly SiteNavigationItem[];}
interface NavigationProps {activeId: string; onSelect?(item: SiteNavigationItem): void;}

function intercept(event: MouseEvent<HTMLAnchorElement>, item: SiteNavigationItem, onSelect: NavigationProps['onSelect']) {
  if (!onSelect || event.button !== 0 || event.ctrlKey || event.metaKey || event.shiftKey || event.altKey) return;
  event.preventDefault(); onSelect(item);
}
function NavigationLink({item, activeId, onSelect, vertical = false}: NavigationProps & {item: SiteNavigationItem; vertical?: boolean}) {
  const Icon = item.icon;
  return <Button asChild variant={activeId === item.id ? 'secondary' : 'ghost'} className={vertical ? 'site-navigation-link h-auto min-h-11 min-w-0 flex-1 flex-col gap-1 whitespace-normal px-1 py-2 text-xs' : 'site-navigation-link h-11 w-full min-w-0 justify-start gap-3'}>
    <a href={item.href} aria-current={activeId === item.id ? 'page' : undefined} onClick={event => intercept(event, item, onSelect)}><Icon aria-hidden="true"/><span>{item.label}</span></a>
  </Button>;
}

/** The rail owns only group disclosure. Navigation and page state stay with the caller. */
export function SiteNavigationRail({groups, activeId, onSelect, storageKey}: NavigationProps & {groups: readonly SiteNavigationGroup[]; storageKey: string;}) {
  const {t} = useLocale();
  const id = useId();
  const [expanded, setExpanded] = useState<Record<string, boolean>>({});
  useLayoutEffect(() => {
    const valid = (value: unknown): value is Record<string, boolean> => !!value && typeof value === 'object' && !Array.isArray(value) && Object.values(value).every(item => typeof item === 'boolean');
    setExpanded(readBookPreference(storageKey, {}, valid));
  }, [storageKey]);
  return <nav className="site-navigation-rail" aria-label={t('nav.main')}>{groups.map(group => <Collapsible.Root key={group.id} open={expanded[group.id] !== false} onOpenChange={open => {
    const next = {...expanded, [group.id]: open}; setExpanded(next); writeBookPreference(storageKey, next);
  }}>
    <Collapsible.Trigger asChild><Button variant="ghost" className="site-navigation-group h-11 w-full justify-between text-xs" aria-controls={`${id}-${group.id}`}><span>{group.label}</span><ChevronDown aria-hidden="true"/></Button></Collapsible.Trigger>
    <Collapsible.Content id={`${id}-${group.id}`} forceMount hidden={expanded[group.id] === false}><ul>{group.items.map(item => <li key={item.id}><NavigationLink item={item} activeId={activeId} onSelect={onSelect}/></li>)}</ul></Collapsible.Content>
  </Collapsible.Root>)}</nav>;
}

export function SiteNavigationMore({items, activeId, onSelect, vertical = false}: NavigationProps & {items: readonly SiteNavigationItem[]; vertical?: boolean;}) {
  const {t} = useLocale();
  return <ActionMenu label={t('nav.more')} className="site-navigation-menu" trigger={<Button variant={items.some(item => item.id === activeId) ? 'secondary' : 'ghost'} className={vertical ? 'h-auto min-h-11 min-w-0 flex-1 flex-col gap-1 whitespace-normal px-1 py-2 text-xs' : 'h-11'}><MoreHorizontal aria-hidden="true"/>{t('nav.more')}</Button>}>
    {items.map(item => {const Icon = item.icon; return onSelect
      ? <ActionMenuItem key={item.id} onAction={() => onSelect(item)}><Icon aria-hidden="true"/>{item.label}</ActionMenuItem>
      : <ActionMenuItem key={item.id} asChild><a href={item.href} aria-current={activeId === item.id ? 'page' : undefined}><Icon aria-hidden="true"/>{item.label}</a></ActionMenuItem>;
    })}
  </ActionMenu>;
}

export function SiteBottomNavigation({items, more, activeId, onSelect}: NavigationProps & {items: readonly SiteNavigationItem[]; more: readonly SiteNavigationItem[];}) {
  const {t} = useLocale();
  return <nav className="site-bottom-navigation" aria-label={t('nav.main')}>{items.map(item => <NavigationLink key={item.id} item={item} activeId={activeId} onSelect={onSelect} vertical/>)}<SiteNavigationMore items={more} activeId={activeId} onSelect={onSelect} vertical/></nav>;
}
