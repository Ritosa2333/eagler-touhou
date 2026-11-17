import {
  canonicalTouchLayout, cloneTouchLayout, cloneTouchLayoutProfile, emptyTouchLayout, loadTouchLayoutFromStorage,
  normalizeTouchLayoutPriorityOrder, persistTouchLayoutResult, touchLayoutControlMeta,
  touchLayoutControlNames, touchLayoutScaleMax, touchLayoutScaleMin,

} from './touch-layout-model.mjs';
import {createTouchLayoutWindowPositionStore} from './touch-layout-editor-state.mjs';

/** No navigation, browser measurement, confirmation or native capability owner.
 * The root decides exits; the React editor supplies measured original geometry. */
export function createTouchLayoutModel({storage}                                      )                   {
  const listeners = new Set            ();
  // Fighting controls do not inherit or overwrite the shared shooter layout.
  let scope = 'shared';
  const sessionLayouts = new Map                            ();
  const scopedKey = (key        ) => scope === 'th075' ? key + '-th075' : key;
  const layoutStorage                            = storage && {
    getItem: key => storage.getItem(scopedKey(key)),
    setItem: (key, value) => storage.setItem(scopedKey(key), value),
    removeItem: key => storage.removeItem(scopedKey(key)),
  };
  const windowPositions = createTouchLayoutWindowPositionStore({storage});
  let saved                     = null, draft                     = null, editing = false;
  const automaticDefaults = new Map                                                   ();
  let orientation                         = 'landscape', selected                         = 'bomb';
  let lastSave                                  = null, revision = 0;
  let snapshot                     ;
  function publish() {
    const changes = cloneTouchLayout(draft);
    if (changes) for (const [direction, defaults] of automaticDefaults) {
      if (!saved?.profiles[direction] && defaults &&
          JSON.stringify(changes.profiles[direction]) === JSON.stringify(defaults)) changes.profiles[direction] = null;
    }
    snapshot = Object.freeze({isEditing: editing, dirty: editing && JSON.stringify(canonicalTouchLayout(changes)) !== JSON.stringify(saved),
      saved: cloneTouchLayout(saved), draft: cloneTouchLayout(draft), orientation, selected, lastSave, revision: ++revision});
    for (const listener of listeners) listener();
  }
  function ensure(defaults                    ) {
    draft ??= emptyTouchLayout();
    const profile = draft.profiles[orientation];
    const previousDefaults = automaticDefaults.get(orientation);
    if (!profile || (!saved?.profiles[orientation] && previousDefaults && JSON.stringify(profile) === JSON.stringify(previousDefaults))) {
      // Untouched defaults follow authored responsive geometry. Resizing must
      // not turn them into a fixed layout captured at the previous resolution.
      draft.profiles[orientation] = cloneTouchLayoutProfile(defaults);
      automaticDefaults.set(orientation, cloneTouchLayoutProfile(defaults));
      return;
    }
    const missing = touchLayoutControlNames.filter(name => !profile.controls[name]);
    for (const name of missing) {
      const value = defaults.controls[name];
      if (value) profile.controls[name] = {...value};
    }
    normalizeTouchLayoutPriorityOrder(profile.controls);
    // Schema extensions are migration, not a user edit. Mirror only existing
    // saved profiles. Untouched default previews are not user edits.
    const baseline = saved?.profiles[orientation];
    if (missing.length && baseline) {
      for (const name of missing) if (defaults.controls[name]) baseline.controls[name] = {...defaults.controls[name] };
      normalizeTouchLayoutPriorityOrder(baseline.controls);
      saved = persistTouchLayoutResult(layoutStorage, saved).value;
    }
  }
  function profile() {
    if (!editing || !draft?.profiles[orientation]) throw new Error('touch.draftUnavailable');
    return draft.profiles[orientation] ;
  }
  publish();
  return Object.freeze({
    subscribe(listener            ) {listeners.add(listener); return () => {listeners.delete(listener);};},
    getSnapshot: () => snapshot,
    hydrate() {if (editing) return; saved = loadTouchLayoutFromStorage(layoutStorage); windowPositions.reload(); publish();},
    setGame(game        ) {
      const next = game === 'th075' ? 'th075' : 'shared';
      if (scope === next) return;
      sessionLayouts.set(scope, cloneTouchLayout(saved));
      scope = next; editing = false; draft = null; lastSave = null; automaticDefaults.clear();
      saved = sessionLayouts.has(scope) ? cloneTouchLayout(sessionLayouts.get(scope) ?? null) : loadTouchLayoutFromStorage(layoutStorage);
      publish();
    },
    begin(next                        , defaults                    , visible                                   ) {
      if (editing) return;
      automaticDefaults.clear();
      orientation = next; editing = true; lastSave = null; draft = cloneTouchLayout(saved) ?? emptyTouchLayout();
      ensure(defaults);
      selected = visible.reduce((best, name) => (profile().controls[name]?.priority ?? touchLayoutControlMeta[name].priority) >
        (profile().controls[best]?.priority ?? touchLayoutControlMeta[best].priority) ? name : best, visible[0] ?? 'bomb');
      publish();
    },
    ensureOrientation(next                        , defaults                    ) {if (!editing) return; orientation = next; ensure(defaults); publish();},
    reconcileVisibleControls(visible                                   ) {
      if (!editing || visible.includes(selected)) return;
      selected = visible.find(name => name !== 'escape') ?? visible[0] ?? selected; publish();
    },
    selectControl(name                        ) {
      const item = profile().controls[name]; if (!item) return;
      selected = name;
      item.priority = Math.max(...Object.values(profile().controls).map(value => value?.priority ?? 0)) + 1;
      normalizeTouchLayoutPriorityOrder(profile().controls); publish();
    },
    updateControl(name                        , update                                      ) {
      const item = profile().controls[name]; if (!item) return;
      if (update.x != null) item.x = Math.min(1, Math.max(0, update.x));
      if (update.y != null) item.y = Math.min(1, Math.max(0, update.y));
      if (update.scale != null) item.scale = Math.min(touchLayoutScaleMax, Math.max(touchLayoutScaleMin, update.scale));
      publish();
    },
    setViewport(x        ) {profile().viewport.x = Math.max(-.5, Math.min(.5, x)); publish();},
    resetOrientation() {if (!editing || !draft) return; draft.profiles[orientation] = null; publish();},
    save() {
      if (!editing || !draft) throw new Error('touch.draftUnavailable');
      lastSave = persistTouchLayoutResult(layoutStorage, draft);
      // A blocked durable write still commits the session's usable layout.
      saved = lastSave.value; draft = cloneTouchLayout(saved) ?? emptyTouchLayout(); automaticDefaults.clear(); publish(); return lastSave;
    },
    discard() {editing = false; draft = null; lastSave = null; automaticDefaults.clear(); publish();},
    windowPositions,
  });
}
