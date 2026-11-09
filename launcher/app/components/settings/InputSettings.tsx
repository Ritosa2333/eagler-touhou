import { Button as UiButton } from '../../ui/button';
import { Keyboard, Gamepad2, ChevronRight, X } from 'lucide-react';
import { Collapsible } from 'radix-ui';
import { Dialog, DialogClose, DialogContent, DialogTitle } from '../../ui/dialog';
import { Tabs, TabsContent, TabsList, TabsTrigger } from '../../ui/tabs';
import { useEffect, useId, useLayoutEffect, useRef, useState } from 'react';
import type { GamepadMappingModel } from '../../../src/launcher/gamepad-mapping.mts';
import type { KeyboardPreferences } from '../../../src/launcher/keyboard-preferences.mts';
import { useLocale } from '../../i18n';
import { KeyboardSettings } from './KeyboardSettings';
import { GamepadSettings } from './GamepadSettings';
import type { Th09LocalInput } from '../../../src/launcher/th09-local-input.mts';
import { Th09LocalInputSettings } from './Th09LocalInputSettings';
import { MainSelect } from '../launcher/MainSelect';
import { OptionsGroupHeading } from './OptionsGroup';
import { InputCapture } from './input-capture';

/** Keep detailed capture in a bounded dialog, outside the options rail. */
export function InputSettings({
  keyboard,
  gamepad,
  th09Local,
  th19=false,
}: {
  keyboard?: KeyboardPreferences;
  gamepad?: GamepadMappingModel;
  th09Local?: Th09LocalInput;
  th19?:boolean;
}) {
  const { t } = useLocale(),
    id = useId(),
    trigger = useRef<HTMLButtonElement>(null);
  const [open, setOpen] = useState(true),
    [editing, setEditing] = useState(false);
  const [device, setDevice] = useState<'keyboard' | 'gamepad'>(keyboard ? 'keyboard' : 'gamepad');
  const [scope, setScope] = useState<'general' | 'local'>('general');
  const capture = useRef<(() => void) | null>(null);
  const scroll = useRef<HTMLDivElement>(null);
  // biome-ignore lint/correctness/useExhaustiveDependencies: Reset the new device or control-context pane's scroll position.
  useLayoutEffect(() => {
    if (editing && scroll.current) scroll.current.scrollTop = 0;
  }, [device, scope, editing]);
  useEffect(() => {
    if (!th09Local) setScope('general');
  }, [th09Local]);
  const title = t(th19?'th19.input':'input.text.input_settings');
  function edit(value: 'keyboard' | 'gamepad', node: HTMLButtonElement) {
    trigger.current = node;
    setDevice(value);
    setScope('general');
    setEditing(true);
  }
  const choices = (['keyboard', 'gamepad'] as const).filter((value) => (value === 'keyboard' ? keyboard : gamepad));
  const label = (value: 'keyboard' | 'gamepad') =>
    value === 'keyboard' ? t('input.text.keyboard_51') : t('input.text.gamepad_52');
  return (
    <Collapsible.Root open={open} onOpenChange={setOpen} asChild>
      <section className="options-group input-settings" data-open={open}>
        <Collapsible.Trigger asChild>
          <OptionsGroupHeading className="input-settings-heading" aria-controls={id}>
            {title}
          </OptionsGroupHeading>
        </Collapsible.Trigger>
        <Collapsible.Content forceMount asChild>
          <div id={id} className="input-settings-collapse" inert={!open} aria-hidden={!open}>
            <div className="input-settings-content">
              <div className="input-settings-entries">
                {choices.map((value) => (
                  <UiButton
                    variant="ghost"
                    size="row"
                    className="input-settings-entry settings-entry"
                    type="button"
                    key={value}
                    onClick={(event) => edit(value, event.currentTarget)}
                    aria-haspopup="dialog"
                  >
                    {value === 'keyboard' ? <Keyboard aria-hidden="true" /> : <Gamepad2 aria-hidden="true" />}
                    <span>{label(value)}</span>
                    <ChevronRight aria-hidden="true" />
                  </UiButton>
                ))}
              </div>
            </div>
          </div>
        </Collapsible.Content>
        <Dialog open={editing} onOpenChange={setEditing}>
            <DialogContent
              className="input-settings-dialog settings-ui"
              showClose={false}
              portalAnchor={trigger.current}
              aria-labelledby={id + '-title'}
              aria-describedby={undefined}
              onEscapeKeyDown={(event) => {
                if (capture.current) {
                  event.preventDefault();
                  capture.current();
                }
              }}
              onCloseAutoFocus={(event) => {
                event.preventDefault();
                trigger.current?.focus({ preventScroll: true });
              }}
            >
              <InputCapture.Provider value={capture}>
                <Tabs value={device} onValueChange={(value) => setDevice(value as 'keyboard' | 'gamepad')} asChild>
                  <div className="input-settings-body">
                    <header>
                      <DialogTitle id={id + '-title'}>{title}</DialogTitle>
                      <DialogClose asChild>
                        <UiButton
                          variant="ghost"
                          size="icon"
                          type="button"
                          className="input-dialog-close"
                          aria-label={t('input.text.close_input_settings')}
                        >
                          <X aria-hidden="true" />
                        </UiButton>
                      </DialogClose>
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
                      <TabsList className="input-device-tabs" aria-label={t('input.text.input_device')}>
                        {choices.map((value) => (
                          <TabsTrigger id={id + '-' + value} value={value} key={value}>
                            {label(value)}
                          </TabsTrigger>
                        ))}
                      </TabsList>
                    )}
                    <div ref={scroll} className="input-dialog-scroll">
                      <TabsContent
                        value={device}
                        id={id + '-panel'}
                        aria-labelledby={scope === 'general' ? id + '-' + device : undefined}
                        asChild
                      >
                        <div className="input-device-panel" key={scope + device}>
                          {scope === 'local' && th09Local && gamepad ? (
                            <Th09LocalInputSettings model={th09Local} gamepad={gamepad} active={editing} />
                          ) : (
                            <>
                              {device === 'keyboard' && keyboard && (
                                <KeyboardSettings model={keyboard} active={editing} th19={th19} />
                              )}
                              {device === 'gamepad' && gamepad && <GamepadSettings model={gamepad} active={editing} th19={th19} />}
                            </>
                          )}
                        </div>
                      </TabsContent>
                    </div>
                  </div>
                </Tabs>
              </InputCapture.Provider>
            </DialogContent>
        </Dialog>
      </section>
    </Collapsible.Root>
  );
}
