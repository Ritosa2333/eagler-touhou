import { Button as UiButton } from '../../ui/button';
import { ButtonGroup } from '../../ui/button-group';
import { Field, FieldContent, FieldGroup } from '../../ui/field';
import { Download, Upload, FolderOpen, Trash2, ChevronRight, SlidersHorizontal } from 'lucide-react';
import { useEffect, useSyncExternalStore, type ReactNode } from 'react';
import { PRODUCT_GAMES, productFeatureAvailable } from '../../../src/contracts/product-catalog.mts';
import { isMusicMode } from '../../../src/launcher/game-preferences.mts';
import { resolveMusicAvailability } from '../../../src/launcher/music-availability.mts';
import type { GameSettingsModel } from '../../models/game-settings';
import { useLocale } from '../../i18n';
import { OptionsGroup, OptionsGroupHeading } from './OptionsGroup';
import { InputSettings } from './InputSettings';
import { OptionRow } from './OptionRow';
import { OptionSwitch } from './TouchSettingsFields';
import { SettingsSelect } from './SettingsSelect';
import type { SettingsActions, SettingsFileAction } from './types';

/** Both library and lobby mount this exact component against the same model.
 * Room timing/launch rows are composed by their existing domain owner. */
export function SettingsBody({
  model,
  actions,
  onOpenTouchLayout,
  multiplayerControls,
}: {
  model: GameSettingsModel;
  actions: SettingsActions;
  onOpenTouchLayout(): void;
  multiplayerControls?: ReactNode;
}) {
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const midi = useSyncExternalStore(
    actions.externalMidi.subscribe,
    actions.externalMidi.getSnapshot,
    actions.externalMidi.getSnapshot,
  );
  const { t } = useLocale();
  const midiVisible = !!state && PRODUCT_GAMES[state.gameId].musicCapabilities.midi && state.music === 'midi';
  useEffect(() => {
    if (!midiVisible && midi.enabled) void actions.externalMidi.setEnabled(false).catch(actions.reportError);
  }, [midiVisible, midi.enabled, actions]);
  if (!state) return null;
  const { context, options, multiplayer } = state,
    mobileOpen = state.disclosure.touch,
    product = PRODUCT_GAMES[state.gameId];
  const availability = resolveMusicAvailability(context.musicAvailability);
  const id = (name: string) => (multiplayer ? `mp${name[0].toUpperCase()}${name.slice(1)}` : name);
  const item = (extra: string) => `item ${extra}`;
  const hasFaith = 'display' in product && product.display.faithBar;
  const hasHints = 'hintFiles' in product.storage && product.storage.hintFiles.length > 0;
  const th05Display = options.th05MultiplayerDisplay;
  const setTh05Display = (patch: Partial<typeof th05Display>) =>
    model.setOption('th05MultiplayerDisplay', {...th05Display, ...patch});
  const switchOption =
    (
      name:
        | 'alwaysHitbox'
        | 'magnifierEnabled'
        | 'focusHitboxEnabled'
        | 'faithBarEnabled'
        | 'thpracEnabled'
        | 'multiplayerLocalPlayerVisibility'
        | 'pixelPerfectEnabled',
    ) =>
    () =>
      model.setOption(name, !options[name]);
  function actionButton(action: SettingsFileAction, title: string) {
    const Icon =
      action === 'manage-replay'
        ? FolderOpen
        : action === 'delete-hint'
          ? Trash2
          : action.startsWith('export')
            ? Download
            : Upload;
    return (
      <UiButton
        variant={action === 'delete-hint' ? 'destructive' : 'outline'}
        size="sm"
        type="button"
        data-action={action}
        onClick={() => {
          void actions.file(action, context.productId).catch(actions.reportError);
        }}
      >
        <Icon aria-hidden="true" />
        <span>{title}</span>
      </UiButton>
    );
  }
  const pack = context.languages.find((entry) => entry.id === state.language);
  const selectedPack = pack?.offlinePack || pack?.pack;
  const packBytes = Number((selectedPack as { bytes?: number } | undefined)?.bytes) || 0;
  const packSize = selectedPack
    ? packBytes < 1024 * 1024
      ? `${(packBytes / 1024).toFixed(1)} KiB`
      : `${(packBytes / 1024 / 1024).toFixed(2)} MiB`
    : t('settings.builtin');
  // Main reorders this one section for mobile, rather than rebuilding it.
  // Its stable sibling key lets React move the same controls across the file,
  // display and advanced groups while preserving focus and local owners.
  const touch = (
    <section
      key="touch-options"
      className={`item mobile-options options-group${mobileOpen ? ' open' : ''}`}
      id={id('mobileOptions')}
    >
      <OptionsGroupHeading
        className="mobile-options-head"
        id={id('mobileOptionsToggle')}
        aria-expanded={mobileOpen}
        aria-controls={id('mobileOptionsBody')}
        onClick={() => model.setDisclosure('touch', !mobileOpen)}
      >
        {t('options.touch')}
      </OptionsGroupHeading>
      <FieldGroup className="mobile-options-body" id={id('mobileOptionsBody')} hidden={!mobileOpen} inert={!mobileOpen}>
        <OptionRow
          className="mobile-option"
          label={t('settings.touchEnabled')}
          control={
            <OptionSwitch
              id={id('touchToggle')}
              label={t('settings.touchEnabled')}
              checked={options.touchEnabled}
              onChange={() => model.setOption('touchEnabled', !options.touchEnabled)}
            />
          }
        />
        <OptionRow
          className="mobile-option"
          label={t('settings.alwaysHitbox')}
          control={
            <OptionSwitch
              id={id('alwaysHitboxToggle')}
              label={t('settings.alwaysHitbox')}
              checked={options.alwaysHitbox}
              onChange={switchOption('alwaysHitbox')}
            />
          }
        />
        <OptionRow
          className="mobile-option"
          id={id('magnifierOption')}
          label={t('settings.magnifier')}
          hint={t('settings.magnifierHint')}
          hintId={id('magnifierHint')}
          control={
            <OptionSwitch
              id={id('magnifierToggle')}
              describedBy={id('magnifierHint')}
              label={t('settings.magnifier')}
              checked={options.magnifierEnabled}
              onChange={switchOption('magnifierEnabled')}
            />
          }
        />
        <UiButton
          variant="outline"
          size="row"
          className="settings-entry touch-layout-option"
          id={id('touchLayoutEdit')}
          type="button"
          onClick={onOpenTouchLayout}
        >
          <SlidersHorizontal aria-hidden="true" />
          <span className="min-w-0 flex-1">{t('settings.touchLayout')}</span>
          <ChevronRight aria-hidden="true" />
        </UiButton>
      </FieldGroup>
    </section>
  );
  return (
    <div className={multiplayer ? 'mp-settings-body options-settings' : 'options-singleplayer options-settings'}>
      {multiplayer && multiplayerControls}
      {context.mobile && touch}
      <OptionsGroup
        id={id('fileOptions')}
        title={t('options.files')}
        open={state.disclosure.files}
        onOpenChange={(open) => model.setDisclosure('files', open)}
        bodyClassName="file-tools-grid"
      >
        <Field className="file-tool-item" id={id('saveFileTool')} orientation="horizontal">
          <FieldContent>
            <span className="ui-field-label">{t('settings.save')}</span>
          </FieldContent>
          <ButtonGroup aria-label={t('settings.save')}>
            {actionButton('export-save', t('settings.download'))}
            {actionButton('import-save', t('settings.import'))}
          </ButtonGroup>
        </Field>
        <Field className="file-tool-item" id={id('replayFileTool')} orientation="horizontal">
          <FieldContent>
            <span className="ui-field-label">{t('settings.replay')}</span>
          </FieldContent>
          <ButtonGroup aria-label={t('settings.replay')}>
            {actionButton('export-replay', t('settings.download'))}
            {actionButton('manage-replay', t('settings.manage'))}
          </ButtonGroup>
        </Field>
        <Field className="file-tool-item" id={id('hintFileTool')} hidden={!hasHints} orientation="horizontal">
          <FieldContent>
            <span className="ui-field-label">{t('settings.hint')}</span>
          </FieldContent>
          <ButtonGroup aria-label={t('settings.hint')}>
            {actionButton('import-hint', t('settings.import'))}
            {actionButton('delete-hint', t('action.delete'))}
          </ButtonGroup>
        </Field>
      </OptionsGroup>
      <OptionsGroup
        id={id('displayOptions')}
        title={t('options.display')}
        open={state.disclosure.display}
        onOpenChange={(open) => model.setDisclosure('display', open)}
        bodyClassName="options-advanced-body"
      >
        <OptionRow
          className={item('option-frame-limit')}
          hidden={!product.support.highRefreshRate}
          label={t('settings.frameLimit')}
          hint={
            <>
              <span id={id('frameLimitHintText')}>{t('settings.frameLimitHint')}</span>{' '}
              <UiButton
                variant="link"
                size="sm"
                className="frame-limit-apple-note"
                id={id('frameLimitAppleNote')}
                type="button"
                onClick={actions.showAppleNotice}
              >
                {t('settings.appleNotice')}
              </UiButton>
            </>
          }
          hintId={id('frameLimitHint')}
          control={
            <OptionSwitch
              id={id('frameLimitToggle')}
              describedBy={id('frameLimitHint')}
              label={t('settings.frameLimit')}
              checked={product.support.highRefreshRate && !options.frameLimit60Enabled}
              disabled={!product.support.highRefreshRate}
              onChange={() => model.setOption('frameLimit60Enabled', !options.frameLimit60Enabled)}
            />
          }
        />
        <div id={id('languageOption')} hidden={!multiplayer && context.languages.length <= 1}>
          <SettingsSelect
            id={id('languageSelect')}
            label={
              <>
                {t('settings.language')}
                {!multiplayer && <small id="languagePackSize">{packSize}</small>}
              </>
            }
            aria-label={t('settings.gameLanguage')}
            value={state.language}
            onChange={(event) => model.setLanguage(event.target.value)}
          >
            {context.languages.map((entry) => (
              <option key={entry.id} value={entry.id}>
                {entry.title || entry.id}
              </option>
            ))}
          </SettingsSelect>
        </div>
        <OptionRow
          className={item('option-pixel-perfect')}
          label={t('settings.pixelPerfect')}
          control={
            <OptionSwitch
              id={id('pixelPerfectToggle')}
              label={t('settings.pixelPerfect')}
              checked={options.pixelPerfectEnabled}
              onChange={switchOption('pixelPerfectEnabled')}
            />
          }
        />
        <OptionRow
          className={item('option-focus-hitbox')}
          id={id('focusHitboxOption')}
          hidden={!productFeatureAvailable(state.gameId, 'focusHitbox', context.hostFeatures)}
          label={t('settings.focusHitbox')}
          control={
            <OptionSwitch
              id={id('focusHitboxToggle')}
              label={t('settings.focusHitbox')}
              checked={options.focusHitboxEnabled}
              onChange={switchOption('focusHitboxEnabled')}
            />
          }
        />
        <OptionRow
          className={item('option-focus-hitbox')}
          id={id('faithBarOption')}
          hidden={!hasFaith}
          label={t('settings.faithBar')}
          control={
            <OptionSwitch
              id={id('faithBarToggle')}
              label={t('settings.faithBar')}
              checked={options.faithBarEnabled}
              onChange={switchOption('faithBarEnabled')}
            />
          }
        />
        {state.gameId === 'th05' && multiplayer && <>
          <SettingsSelect id="mpTh05BossBarSelect" label={t('settings.th05.bossBar')} value={th05Display.bossBar}
            onChange={event => {
              const bossBar = event.target.value;
              if (bossBar === 'native' || bossBar === 'eosd' || bossBar === 'ring') setTh05Display({bossBar});
            }}>
            {(['native', 'eosd', 'ring'] as const).map(style => <option key={style} value={style}>{t(`settings.th05.bar.${style}`)}</option>)}
          </SettingsSelect>
          {(['phaseMarkers', 'phaseStars', 'phaseTimeout', 'timeoutAsTime', 'numericDream', 'numericPower'] as const).map(key =>
            <OptionRow key={key} className="item" label={t(`settings.th05.${key}`)} control={
              <OptionSwitch id={`mpTh05${key[0].toUpperCase()}${key.slice(1)}Toggle`} label={t(`settings.th05.${key}`)} checked={th05Display[key]}
                disabled={key === 'phaseMarkers' && th05Display.bossBar !== 'native' || key === 'phaseStars' && th05Display.bossBar !== 'eosd'
                  || key === 'phaseTimeout' && th05Display.bossBar !== 'native' || key === 'timeoutAsTime' && !th05Display.phaseTimeout}
                onChange={() => setTh05Display({[key]: !th05Display[key]})}/>
            }/>)}
        </>}
      </OptionsGroup>
      {(actions.keyboard || actions.gamepad) && (
        <InputSettings
          keyboard={state.gameId === 'th19' ? actions.th19Keyboard : actions.keyboard}
          gamepad={state.gameId === 'th19' ? actions.th19Gamepad : actions.gamepad}
          th19={state.gameId === 'th19'}
          th09Local={state.gameId === 'th09' && !multiplayer ? actions.th09Local : undefined}
        />
      )}
      <OptionsGroup
        id={id('advancedOptions')}
        title={t('options.advanced')}
        open={state.disclosure.advanced}
        onOpenChange={(open) => model.setDisclosure('advanced', open)}
        bodyClassName="options-advanced-body"
      >
        <div id={id('gameVersionOption')}>
          <SettingsSelect
            id={id('gameVersionSelect')}
            label={t('settings.gameVersion')}
            description={t('settings.version.hint')}
            aria-label={t('settings.gameVersion')}
            value={state.gameVersion}
            onChange={(event) => {
              if (event.target.value === 'original' || event.target.value === 'beauty')
                model.setGameVersion(event.target.value);
            }}
          >
            <option value="original">{t('settings.version.original')}</option>
            {state.gameId !== 'th075' && (
              <option value="beauty" disabled={context.beautyInstalled === false}>
                {t('settings.version.beauty')}
              </option>
            )}
          </SettingsSelect>
        </div>
        <div id={id('musicOption')} hidden={!multiplayer && !availability.ogg}>
          <SettingsSelect
            id={id('musicSelect')}
            label={t('settings.music')}
            aria-label={t('settings.music')}
            title={availability.audio ? '' : t('settings.webAudioUnavailable')}
            value={state.music}
            onChange={(event) => {
              if (isMusicMode(event.target.value)) model.setMusic(event.target.value);
            }}
          >
            <option value="ogg-stream" disabled={!availability.ogg}>
              {t('settings.music.oggStream')}
            </option>
            <option value="ogg-full" disabled={!availability.ogg}>
              {t('settings.music.oggFull')}
            </option>
            {'sources' in product.musicRuntime && product.musicRuntime.sources.includes('akyu') && (
              <option value="ogg-akyu" disabled={!availability.ogg}>{t('settings.music.akyu')}</option>
            )}
            <option value="midi" disabled={!availability.midi}>
              midi
            </option>
            <option value="none">{t('settings.music.none')}</option>
          </SettingsSelect>
        </div>
        <OptionRow
          className="item feature-section"
          id={id('thpracOption')}
          hidden={!productFeatureAvailable(state.gameId, 'thprac', context.hostFeatures)}
          label={'thprac'}
          hint={t(multiplayer ? 'settings.thpracMultiplayerDisabled' : 'settings.thpracHint')}
          control={
            <OptionSwitch
              id={id('thpracToggle')}
              label="thprac"
              disabled={multiplayer}
              checked={!multiplayer && options.thpracEnabled}
              onChange={switchOption('thpracEnabled')}
            />
          }
        />
        {multiplayer && (
          <OptionRow
            className={item('option-frame-limit')}
            label={t('settings.localPlayerVisibility')}
            hint={t('settings.localPlayerVisibilityHint')}
            control={
              <OptionSwitch
                id="mpLocalPlayerVisibilityToggle"
                label={t('settings.localPlayerVisibility')}
                checked={options.multiplayerLocalPlayerVisibility}
                onChange={switchOption('multiplayerLocalPlayerVisibility')}
              />
            }
          />
        )}
        <OptionRow
          className={item('option-frame-limit option-external-midi')}
          id={id('externalMidiOption')}
          hidden={!midiVisible}
          label={t('settings.externalMidi')}
          hint={midi.hint}
          hintId={id('externalMidiHint')}
          control={
            <OptionSwitch
              id={id('externalMidiToggle')}
              label={t('settings.externalMidi')}
              describedBy={id('externalMidiHint')}
              checked={midiVisible && midi.enabled}
              disabled={!midiVisible || !midi.supported}
              title={midiVisible && !midi.supported ? t('settings.externalMidiUnsupported') : ''}
              onChange={() => {
                void actions.externalMidi.setEnabled(!midi.enabled).catch(actions.reportError);
              }}
            />
          }
        />
        <div id={id('externalMidiDeviceOption')} hidden={!midiVisible || !midi.enabled || !midi.granted}>
          <SettingsSelect
            id={id('externalMidiDeviceSelect')}
            label={t('settings.externalMidiDevice')}
            aria-label={t('settings.externalMidiDevice')}
            value={midi.selectedId}
            disabled={!midi.outputs.length}
            onChange={(event) => {
              const value = event.target.value;
              void actions.externalMidi.selectOutput(value).catch(actions.reportError);
            }}
          >
            {midi.outputs.length ? (
              midi.outputs.map((output) => (
                <option key={output.id} value={output.id}>
                  {output.name}
                </option>
              ))
            ) : (
              <option value="" disabled>
                {t('settings.externalMidiNoDeviceOption')}
              </option>
            )}
          </SettingsSelect>
        </div>
      </OptionsGroup>
      {!context.mobile && touch}
    </div>
  );
}
