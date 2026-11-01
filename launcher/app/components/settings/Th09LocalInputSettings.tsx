import { useEffect, useState, useSyncExternalStore } from 'react';
import type { GamepadMappingModel } from '../../../src/launcher/gamepad-mapping.mts';
import { configurableKeyboardCode } from '../../../src/launcher/keyboard-preferences.mts';
import { localInputActions, type Th09LocalInput } from '../../../src/launcher/th09-local-input.mts';
import { useLocale } from '../../i18n';
import { MainSelect } from '../launcher/MainSelect';
import { Button } from '../ui/button';
import { GamepadSettings } from './GamepadSettings';
import { type InputAction, InputBindingsTable } from './InputBindingsTable';
import { keyboardCodeName } from './KeyboardSettings';

export function Th09LocalInputSettings({
  model,
  gamepad,
  active,
}: {
  model: Th09LocalInput;
  gamepad: GamepadMappingModel;
  active: boolean;
}) {
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot),
    { locale, t } = useLocale();
  const [seat, setSeat] = useState(0),
    [learning, setLearning] = useState<InputAction | null>(null),
    [feedback, setFeedback] = useState(''),
    [naming, setNaming] = useState(false),
    [name, setName] = useState('');
  const p = state.players[seat]!;
  function result(error: string | null) {
    setFeedback(
      error
        ? error === 'gamepad'
          ? t('input.text.this_controller_belongs_to_the_other_player')
          : t('input.usedByOther', { key: keyboardCodeName(error) })
        : '',
    );
  }
  useEffect(() => {
    setLearning(null);
    setFeedback('');
    setNaming(false);
    gamepad.cancelLearning();
  }, [seat, gamepad]);
  useEffect(() => {
    if (!active || !learning) return;
    const capture = (event: KeyboardEvent) => {
      event.preventDefault();
      event.stopImmediatePropagation();
      if (event.repeat) return;
      if (event.code === 'Escape') {
        setLearning(null);
        return;
      }
      if (event.altKey || event.metaKey || !configurableKeyboardCode(event.code)) {
        setFeedback(t('input.text.this_key_or_chord_is_not_supported'));
        return;
      }
      const error = model.bind(seat, learning, event.code);
      result(error);
      if (!error) setLearning(null);
    };
    document.addEventListener('keydown', capture, true);
    return () => document.removeEventListener('keydown', capture, true);
  }, [active, learning, model, seat, locale]);
  const bindings = Object.fromEntries(
    Object.entries(p.bindings).map(([a, keys]) => [a, keys.map(keyboardCodeName)]),
  ) as Record<InputAction, string[]>;
  return (
    <div className="th09-local-input input-config">
      <label className="input-gamepad-enable">
        <input type="checkbox" checked={state.enabled} onChange={(e) => model.setEnabled(e.target.checked)} />
        {t('input.text.enable_local_two_player_controls')}
      </label>
      <p className="input-config-hint">
        {t('input.text.only_for_local_versus_overrides_both_players_in_game_device_setti')}
      </p>
      <div className="input-device-tabs input-local-players" role="tablist" aria-label={t('input.text.local_player')}>
        {[0, 1].map((n) => (
          <Button
            variant="legacy"
            size="legacy"
            type="button"
            key={n}
            role="tab"
            aria-selected={seat === n}
            tabIndex={seat === n ? 0 : -1}
            onClick={() => setSeat(n)}
            onKeyDown={(e) => {
              if (e.key === 'ArrowLeft' || e.key === 'ArrowRight') {
                e.preventDefault();
                setSeat(1 - seat);
                (e.currentTarget.parentElement?.children[1 - seat] as HTMLButtonElement)?.focus();
              }
            }}
          >
            {t('input.player', { number: n + 1 })}
          </Button>
        ))}
      </div>
      <div className="input-layout-toolbar">
        <label htmlFor="th09LocalDevice">{t('input.text.input_device')}</label>
        <MainSelect
          id="th09LocalDevice"
          className="option-select"
          aria-label={t('input.text.player_input_device')}
          value={p.device}
          onChange={(e) => {
            setLearning(null);
            result(model.setDevice(seat, e.target.value === 'gamepad' ? 'gamepad' : 'keyboard'));
          }}
        >
          <option value="keyboard">{t('input.text.keyboard')}</option>
          <option value="gamepad">{t('input.text.gamepad')}</option>
        </MainSelect>
      </div>
      {p.device === 'gamepad' ? (
        <GamepadSettings
          model={gamepad}
          active={active}
          assignment={{ key: p.gamepad, onChange: (key) => result(model.setDevice(seat, 'gamepad', key)) }}
        />
      ) : (
        <>
          <div className="input-layout-toolbar">
            <label htmlFor="th09LocalLayout">{t('input.text.layout')}</label>
            <MainSelect
              id="th09LocalLayout"
              className="option-select"
              aria-label={t('input.text.two_player_keyboard_layout')}
              value={p.selected}
              onChange={(e) => {
                setLearning(null);
                result(model.selectLayout(seat, e.target.value));
              }}
            >
              <option value="original">{t(seat === 0 ? 'input.originalLeft' : 'input.originalRight')}</option>
              <option value="wasd">WASD</option>
              <option value="arrows">{t('input.text.arrow_keys')}</option>
              {p.selected === 'custom' && (
                <option value="custom" disabled>
                  {t('input.text.custom')}
                </option>
              )}
              {p.saved.map((s) => (
                <option key={s.id} value={s.id}>
                  {s.name}
                </option>
              ))}
            </MainSelect>
            <Button
              variant="legacy"
              size="legacy"
              type="button"
              className="input-config-button"
              onClick={() => {
                setLearning(null);
                setNaming(!naming);
                setName('');
              }}
            >
              {t('input.text.save_layout')}
            </Button>
          </div>
          {naming && (
            <form
              className="input-save-layout"
              onSubmit={(e) => {
                e.preventDefault();
                if (model.saveLayout(seat, name)) {
                  setNaming(false);
                  setFeedback(t('input.text.layout_saved'));
                }
              }}
            >
              <input
                autoFocus
                aria-label={t('input.text.two_player_layout_name')}
                maxLength={32}
                value={name}
                onChange={(e) => setName(e.target.value)}
              />
              <Button
                variant="legacy"
                size="legacy"
                className="input-config-button primary"
                type="submit"
                disabled={!name.trim()}
              >
                {t('input.text.save')}
              </Button>
              <Button
                variant="legacy"
                size="legacy"
                className="input-config-button"
                type="button"
                onClick={() => setNaming(false)}
              >
                {t('input.text.cancel')}
              </Button>
            </form>
          )}
          <InputBindingsTable
            bindings={bindings}
            learning={learning}
            actions={localInputActions}
            names={{ bomb: 'input.action.quick', focus: 'input.action.charge' }}
            resetLabel={t('input.text.clear_binding')}
            onBind={(action) => {
              setNaming(false);
              setFeedback('');
              setLearning(learning === action ? null : action);
            }}
            onReset={(action) => {
              model.clear(seat, action);
              setLearning(null);
            }}
          />
          <footer className="input-config-footer">
            <span>
              {learning
                ? t('input.text.press_a_key_or_esc_to_cancel')
                : state.persisted
                  ? t('input.text.saved_automatically_for_each_player')
                  : t('input.text.changes_apply_only_to_this_page')}
            </span>
            {p.selected.startsWith('saved-') && (
              <Button
                variant="legacy"
                size="legacy"
                type="button"
                className="input-config-button"
                onClick={() => model.deleteLayout(seat, p.selected)}
              >
                {t('input.text.delete_layout')}
              </Button>
            )}
            <Button
              variant="legacy"
              size="legacy"
              type="button"
              className="input-config-button"
              onClick={() => {
                setLearning(null);
                result(model.selectLayout(seat, 'original'));
              }}
            >
              {t('input.text.restore_original')}
            </Button>
          </footer>
        </>
      )}
      <p className="input-config-hint" role="status">
        {feedback}
      </p>
    </div>
  );
}
