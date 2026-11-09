import {createContext, useCallback, useContext, useId, useLayoutEffect, useRef, useState, type Dispatch, type ReactNode, type SetStateAction} from 'react';
import {useLocation, useNavigate, type Location, type NavigateOptions, type To} from 'react-router';
import {LOCAL_LAYERS_KEY, nextPageHistory, PAGE_HISTORY_KEY, pageOf, readPageHistory} from './page-history';

export type LayerValue = string | boolean | null;
export interface LayerEntry {value: Exclude<LayerValue, null>; parentPosition: number}
export interface SurfaceLayers {
  values: Record<string, LayerEntry>;
  set(id: string, value: LayerValue): void;
}
export const SurfaceLayersContext = createContext<SurfaceLayers | null>(null);

const layerAddress = ({pathname, search, hash}: Location) => pathname + search + hash;

export function navigateSurfaceLayer(location: Location, navigate: (to: To | number, options?: NavigateOptions) => void | Promise<void>,
  session: string, current: SurfaceLayers['values'], id: string, value: LayerValue) {
  const prior = current[id];
  if ((prior?.value ?? null) === value) return;
  const position = readPageHistory(location.state)?.position;
  if (value === null && prior && position !== undefined && prior.parentPosition < position) {
    void navigate(prior.parentPosition - position);
    return;
  }
  const values = {...current};
  if (value === null) delete values[id];
  else values[id] = {value, parentPosition: prior?.parentPosition ?? position ?? -1};
  const source = location.state;
  const state = source && typeof source === 'object' && !Array.isArray(source) ? {...source} : {};
  if (Object.keys(values).length) state[LOCAL_LAYERS_KEY] = {session, href: layerAddress(location), values};
  else delete state[LOCAL_LAYERS_KEY];
  void navigate(layerAddress(location), {replace: !!prior, preventScrollReset: true, state});
}

let standaloneSession = 0;
/** Read-only pages use the same history entries without mounting game navigation. */
export function StandaloneSurfaceLayers({children}: {children: ReactNode}) {
  const location = useLocation(), navigate = useNavigate();
  const [session] = useState(() => `site-${Date.now()}-${++standaloneSession}`);
  const entry = location.state?.[LOCAL_LAYERS_KEY];
  const values: SurfaceLayers['values'] = entry?.session === session && entry.href === layerAddress(location) ? entry.values ?? {} : {};
  const pending = useRef<{location: Location; id: string; value: LayerValue} | null>(null);
  useLayoutEffect(() => {
    if (pending.current?.location !== location) pending.current = null;
  }, [location]);
  function navigateLayer(to: To | number, options?: NavigateOptions) {
    if (typeof to === 'number') return navigate(to);
    const record = nextPageHistory(readPageHistory(location.state), pageOf(location), options?.replace === true);
    return navigate(to, {...options, state: {...options?.state, [PAGE_HISTORY_KEY]: record}});
  }
  function set(id: string, value: LayerValue) {
    if ((values[id]?.value ?? null) === value) return;
    if (pending.current?.location === location && pending.current.id === id && pending.current.value === value) return;
    pending.current = {location, id, value};
    navigateSurfaceLayer(location, navigateLayer, session, values, id, value);
  }
  return <SurfaceLayersContext.Provider value={{values, set}}>{children}</SurfaceLayersContext.Provider>;
}

/** Small child views use the same Router entries as their enclosing surface. */
export function useSurfaceLayerState<T extends LayerValue>(name: string, initial: T): [T, Dispatch<SetStateAction<T>>] {
  const layers = useContext(SurfaceLayersContext), instance = useId(), id = `${name}:${instance}`;
  const [local, setLocal] = useState(initial);
  const value = layers ? (layers.values[id]?.value ?? initial) as T : local;
  // Keep setter identity stable while reading the latest committed route.
  const current = useRef({layers, id, value, initial});
  useLayoutEffect(() => {current.current = {layers, id, value, initial};});
  const setValue = useCallback<Dispatch<SetStateAction<T>>>(next => {
    const {layers, id, value, initial} = current.current;
    if (!layers) {setLocal(next); return;}
    const resolved = typeof next === 'function' ? next(value) : next;
    layers.set(id, resolved === initial ? null : resolved);
  }, []);
  return [value, setValue];
}

