import {useCallback, useLayoutEffect, useState} from 'react';
import {readBookPreference, writeBookPreference} from './book-preferences';

type DisclosurePreferences = Readonly<Record<string, boolean>>;

function isDisclosurePreferences(value: unknown): value is DisclosurePreferences {
  return value !== null && typeof value === 'object' && !Array.isArray(value) &&
    Object.values(value).every(open => typeof open === 'boolean');
}

export function useBookDisclosures(key: string, defaults: DisclosurePreferences, interactive: boolean) {
  const [groups, setGroups] = useState(defaults);
  useLayoutEffect(() => {
    if (interactive) setGroups({...defaults, ...readBookPreference(key, {}, isDisclosurePreferences)});
  }, [defaults, interactive, key]);

  const setGroup = useCallback((group: string, open: boolean, remember = false) => {
    if (remember) {
      const saved = readBookPreference(key, {}, isDisclosurePreferences);
      writeBookPreference(key, {...saved, [group]: open});
    }
    setGroups(previous => previous[group] === open ? previous : {...previous, [group]: open});
  }, [key]);

  return [groups, setGroup] as const;
}
