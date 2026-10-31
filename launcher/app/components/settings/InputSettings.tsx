import { ChevronRight, Gamepad2, Keyboard } from 'lucide-react';
import { useEffect, useId, useRef, useState } from 'react';
import type { GamepadMappingModel } from '../../../src/launcher/gamepad-mapping.mts';
import type { KeyboardPreferences } from '../../../src/launcher/keyboard-preferences.mts';
import type { Th09LocalInput } from '../../../src/launcher/th09-local-input.mts';
import { useLocale } from '../../i18n';
import { MainSelect } from '../launcher/MainSelect';
import { Button } from '../ui/button';
import { GamepadSettings } from './GamepadSettings';
import { KeyboardSettings } from './KeyboardSettings';
import { Th09LocalInputSettings } from './Th09LocalInputSettings';

/** Keep detailed capture in a bounded dialog, outside the options rail. */
export function InputSettings({
  keyboard,
  gamepad,
  th09Local,
}: {
  keyboard?: KeyboardPreferences;
  gamepad?: GamepadMappingModel;
  th09Local?: Th09LocalInput;
}) {
  const { t } = useLocale(),
    id = useId(),
    dialog = useRef<HTMLDialogElement>(null);
  const [open, setOpen] = useState(true),
    [editing, setEditing] = useState(false),
    [closing, setClosing] = useState(false);
  const [device, setDevice] = useState<'keyboard' | 'gamepad'>(keyboard ? 'keyboard' : 'gamepad');
  const [scope, setScope] = useState<'general' | 'local'>('general');
  useEffect(() => {
    if (!th09Local) setScope('general');
  }, [th09Local]);
  useEffect(() => {
    if (editing && !dialog.current?.open) dialog.current?.showModal();
  }, [editing]);
  useEffect(() => {
    if (!closing) return;
    const timer = setTimeout(() => {
      dialog.current?.close();
      setEditing(false);
      setClosing(false);
    }, 170);
    return () => clearTimeout(timer);
  }, [closing]);
  const title = t('input.text.input_settings');
  function edit(value: 'keyboard' | 'gamepad') {
    setDevice(value);
    setScope('general');
    setClosing(false);
    setEditing(true);
  }
  const choices = (['keyboard', 'gamepad'] as const).filter((value) => (value === 'keyboard' ? keyboard : gamepad));
  const label = (value: 'keyboard' | 'gamepad') =>
    value === 'keyboard' ? t('input.text.keyboard_51') : t('input.text.gamepad_52');
  return (
    <section className="options-group input-settings" data-open={open}>
      <Button
        variant="legacy"
        size="legacy"
        className="options-group-head input-settings-heading"
        type="button"
        aria-expanded={open}
        aria-controls={id}
        onClick={() => setOpen(!open)}
      >
        <span>{title}</span>
        <i aria-hidden="true">⌄</i>
      </Button>
      <div id={id} className="input-settings-collapse" inert={!open} aria-hidden={!open}>
        <div className="input-settings-content">
          <div className="input-settings-entries">
            {choices.map((value) => (
              <Button
                variant="legacy"
                size="legacy"
                className="input-settings-entry"
                type="button"
                key={value}
                onClick={() => edit(value)}
                aria-haspopup="dialog"
              >
                {value === 'keyboard' ? (
                  <Keyboard size={22} aria-hidden="true" focusable="false" />
                ) : (
                  <Gamepad2 size={22} aria-hidden="true" focusable="false" />
                )}
                <strong>{label(value)}</strong>
                <ChevronRight size={16} aria-hidden="true" focusable="false" />
              </Button>
            ))}
          </div>
        </div>
      </div>
      {editing && (
        <dialog
          className={`input-settings-dialog settings-ui${closing ? ' is-closing' : ''}`}
          ref={dialog}
          aria-labelledby={id + '-title'}
          onCancel={(event) => {
            event.preventDefault();
            setClosing(true);
          }}
          onClose={() => {
            setEditing(false);
            setClosing(false);
          }}
          onClick={(event) => {
            if (event.target !== event.currentTarget) return;
            const r = event.currentTarget.getBoundingClientRect();
            if (event.clientX < r.left || event.clientX > r.right || event.clientY < r.top || event.clientY > r.bottom)
              setClosing(true);
          }}
        >
          <header>
            <h2 id={id + '-title'}>{title}</h2>
            <Button
              variant="legacy"
              size="legacy"
              type="button"
              className="input-dialog-close"
              aria-label={t('input.text.close_input_settings')}
              onClick={() => setClosing(true)}
            >
              ×
            </Button>
          </header>
          {th09Local && gamepad && (
            <div className="input-context-toolbar">
              <label htmlFor={id + '-scope'}>{t('input.text.control_context')}</label>
              <MainSelect
                id={id + '-scope'}
                aria-label={t('input.text.control_context')}
                className="option-select"
                value={scope}
                onChange={(e) => setScope(e.target.value === 'local' ? 'local' : 'general')}
              >
                <option value="general">{t('input.text.single_player_online')}</option>
                <option value="local">{t('input.text.local_two_player')}</option>
              </MainSelect>
            </div>
          )}
          {scope === 'general' && (
            <div className="input-device-tabs" role="tablist" aria-label={t('input.text.input_device')}>
              {choices.map((value) => (
                <Button
                  variant="legacy"
                  size="legacy"
                  key={value}
                  id={id + '-' + value}
                  type="button"
                  role="tab"
                  tabIndex={device === value ? 0 : -1}
                  aria-controls={id + '-panel'}
                  aria-selected={device === value}
                  onClick={() => setDevice(value)}
                  onKeyDown={(event) => {
                    if (!['ArrowLeft', 'ArrowRight', 'Home', 'End'].includes(event.key)) return;
                    event.preventDefault();
                    const tabs = [
                      ...event.currentTarget.parentElement!.querySelectorAll<HTMLButtonElement>('[role=tab]'),
                    ];
                    const index =
                      event.key === 'Home'
                        ? 0
                        : event.key === 'End'
                          ? tabs.length - 1
                          : (tabs.indexOf(event.currentTarget) + (event.key === 'ArrowRight' ? 1 : tabs.length - 1)) %
                            tabs.length;
                    tabs[index]?.focus();
                    tabs[index]?.click();
                  }}
                >
                  {label(value)}
                </Button>
              ))}
            </div>
          )}
          <div className="input-dialog-scroll">
            <div
              className="input-device-panel"
              role={scope === 'general' ? 'tabpanel' : undefined}
              id={id + '-panel'}
              aria-labelledby={scope === 'general' ? id + '-' + device : undefined}
              key={scope + device}
            >
              {scope === 'local' && th09Local && gamepad ? (
                <Th09LocalInputSettings model={th09Local} gamepad={gamepad} active={!closing} />
              ) : (
                <>
                  {device === 'keyboard' && keyboard && <KeyboardSettings model={keyboard} active={!closing} />}
                  {device === 'gamepad' && gamepad && <GamepadSettings model={gamepad} active={!closing} />}
                </>
              )}
            </div>
          </div>
        </dialog>
      )}
    </section>
  );
}
