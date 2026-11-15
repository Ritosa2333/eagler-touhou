import {useMemo, useSyncExternalStore} from 'react';

interface ExternalStore<T> {
  subscribe(listener: () => void): () => void;
  getSnapshot(): T;
}

/** Subscribe to the fields a view presents, with a stable snapshot per value. */
export function useStoreSelector<T, S>(store: ExternalStore<T>, select: (snapshot: T) => S, equal: (left: S, right: S) => boolean = Object.is): S {
  const read = useMemo(() => {
    let initialized = false, snapshot: T, selected: S;
    return () => {
      const next = store.getSnapshot();
      if (initialized && Object.is(snapshot, next)) return selected;
      const value = select(next);
      if (!initialized || !equal(selected, value)) selected = value;
      snapshot = next; initialized = true;
      return selected;
    };
  }, [store, select, equal]);
  return useSyncExternalStore(store.subscribe, read, read);
}

export function shallowEqual(left: object, right: object): boolean {
  if (Object.is(left, right)) return true;
  const keys = Object.keys(left), other = Object.keys(right);
  const a = left as Record<string, unknown>, b = right as Record<string, unknown>;
  return keys.length === other.length && keys.every(key => Object.hasOwn(b, key) && Object.is(a[key], b[key]));
}
