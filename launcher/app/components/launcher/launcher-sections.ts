import {useLocation} from 'react-router';
import {isLauncherHomePath} from '../../navigation/addresses';

export type MessageSection = 'announcements' | 'messages';
export type LauncherSection = 'home' | MessageSection;

export function useLauncherSection(): LauncherSection | null {
  return launcherSection(useLocation());
}

export function launcherSection({pathname, search}: {pathname: string; search: string}): LauncherSection | null {
  if (!isLauncherHomePath(pathname)) return null;
  const section = new URLSearchParams(search).get('section');
  return section === 'home' || section === 'announcements' || section === 'messages' ? section : null;
}

export function primaryLauncherView(location: {pathname: string; search: string}) {
  const params = new URLSearchParams(location.search);
  if (params.has('r')) return null;
  if (/^\/lobby(?:\.html)?\/?$/.test(location.pathname)) return 'lobby';
  if (!isLauncherHomePath(location.pathname) || params.has('game')) return null;
  return launcherSection(location) ?? 'library';
}

export function profileSettingsPanel(hash: string) {
  return ['#edit', '#settings', '#security', '#game-settings'].includes(hash) ? hash : '';
}

export function launcherSectionHref(homeHref: string, section: LauncherSection): string {
  return `${homeHref}?${new URLSearchParams({section})}`;
}
