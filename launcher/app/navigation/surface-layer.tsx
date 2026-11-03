import {createContext, useCallback, useContext, useId, useLayoutEffect, useRef, useState, type Dispatch, type SetStateAction} from 'react';

export type LayerValue = string | boolean | null;
export interface LayerEntry {value: Exclude<LayerValue, null>; parentPosition: number}
export interface SurfaceLayers {
  values: Record<string, LayerEntry>;
  set(id: string, value: LayerValue): void;
}
export const SurfaceLayersContext = createContext<SurfaceLayers | null>(null);

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

