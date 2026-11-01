import { useEffect, useSyncExternalStore } from 'react';
import {
  defaultGamepadProfile,
  type GamepadControl,
  type GamepadMappingModel,
  gamepadActions,
} from '../../../src/launcher/gamepad-mapping.mts';
import { localInputActions } from '../../../src/launcher/th09-local-input.mts';
import { useLocale } from '../../i18n';
import { MainSelect } from '../launcher/MainSelect';
import { Button } from '../ui/button';
import { type InputAction, InputBindingsTable } from './InputBindingsTable';
export function GamepadSettings({
  model,
  active = true,
  assignment,
}: {
  model: GamepadMappingModel;
  active?: boolean;
  assignment?: { key: string; onChange(key: string): void };
}) {
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot),
    { t } = useLocale();
  useEffect(() => {
    if (!active) return;
    model.refresh();
    const timer = setInterval(() => model.refresh(), 50);
    return () => {
      clearInterval(timer);
      model.cancelLearning();
    };
  }, [active, model]);
  const key = assignment?.key ?? state.seats[0] ?? '',
    device = state.devices.find((d) => d.key === key),
    profile = state.profiles[key] ?? defaultGamepadProfile(device?.standard === true);
  function controlName(c: GamepadControl) {
    return c.kind === 'button'
      ? t('input.button', { number: c.index + 1 })
      : t('input.axis', { number: c.index + 1, direction: c.value > c.rest ? '+' : '−' });
  }
  const bindings = Object.fromEntries(
    gamepadActions.map((action) => [action, profile.bindings[action].map(controlName)]),
  ) as Record<InputAction, string[]>;
  return (
    <div className="gamepad-settings input-config">
      {!assignment && (
        <label className="input-gamepad-enable">
          <input
            type="checkbox"
            checked={state.enabled}
            disabled={!state.supported}
            onChange={(event) => model.setEnabled(event.target.checked)}
          />
          {t('input.text.enable_gamepad_mapping')}
        </label>
      )}
      <div className="input-layout-toolbar">
        <label htmlFor="inputGamepadDevice">{t('input.text.controller')}</label>
        <MainSelect
          id="inputGamepadDevice"
          className="option-select"
          aria-label={t('input.text.gamepad_device')}
          value={key}
          onChange={(event) => {
            model.cancelLearning();
            if (assignment) assignment.onChange(event.target.value);
            else model.select(0, event.target.value);
          }}
        >
          <option value="">{t('input.text.select_a_controller')}</option>
          {key && !device && <option value={key}>{t('input.text.saved_controller_disconnected')}</option>}
          {state.devices.map((d) => (
            <option value={d.key} key={d.key}>
              {d.name} (#{d.index + 1})
            </option>
          ))}
        </MainSelect>
      </div>
      <p className="input-config-hint">
        {!state.supported
          ? t('input.text.this_browser_cannot_read_gamepads')
          : !device
            ? t('input.text.connect_your_controller_and_press_a_button_to_make_it_visible')
            : t('input.text.click_a_binding_then_press_a_button_or_move_an_axis')}
      </p>
      {device && (
        <>
          {!device.standard && (
            <p className="input-config-hint">
              {t('input.text.this_controller_has_no_standard_layout_set_each_action')}
            </p>
          )}
          <InputBindingsTable
            bindings={bindings}
            learning={state.learning}
            actions={assignment ? localInputActions : undefined}
            names={assignment ? { bomb: 'input.action.quick', focus: 'input.action.charge' } : undefined}
            resetLabel={t('input.text.clear_binding')}
            isActive={(action) => !!((state.deviceActive[key] ?? 0) & (1 << gamepadActions.indexOf(action)))}
            onBind={(action) => (state.learning === action ? model.cancelLearning() : model.learn(key, action))}
            onReset={(action) => {
              model.cancelLearning();
              model.bind(key, action, []);
            }}
          />
          <label className="input-axis-threshold">
            <span>{t('input.text.stick_trigger_threshold')}</span>
            <output>{Math.round(profile.threshold * 100)}%</output>
            <input
              aria-label={t('input.text.stick_trigger_threshold')}
              type="range"
              min="10"
              max="90"
              value={Math.round(profile.threshold * 100)}
              onChange={(event) => model.setThreshold(key, Number(event.target.value) / 100)}
            />
          </label>
          <footer className="input-config-footer">
            <span role="status">
              {state.learning
                ? t('input.text.release_controls_then_press_the_new_control')
                : t('input.text.saved_automatically_for_this_controller')}
            </span>
            <Button
              variant="legacy"
              size="legacy"
              className="input-config-button"
              type="button"
              onClick={() => {
                model.cancelLearning();
                model.reset(key);
              }}
            >
              {t('input.text.reset_defaults')}
            </Button>
          </footer>
        </>
      )}
    </div>
  );
}
