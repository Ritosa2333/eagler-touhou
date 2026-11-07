import { useRef, useSyncExternalStore } from 'react';
import { useLocale } from '../../i18n';
import type { GameSettingsModel } from '../../models/game-settings';
import { Button } from '../../ui/controls';
import { Popover, PopoverClose, PopoverContent, PopoverTrigger } from '../../ui/popover';
import { Slider } from '../../ui/slider';
import { PlayerExitIcon, PlayerSettingsIcon } from '../player/utility-icons';
import { OptionRow } from './OptionRow';
import './game-audio-controls.css';

export function GameAudioFields({ model }: { model: GameSettingsModel }) {
  const state = useSyncExternalStore(model.subscribe, model.getSnapshot, model.getSnapshot);
  const gesture = useRef<{ game: string; volume: number } | null>(null);
  const { t } = useLocale();
  if (!state) return null;
  return (['bgmVolume', 'sfxVolume'] as const).map((name) => {
    const label = t(name === 'bgmVolume' ? 'settings.bgmVolume' : 'settings.sfxVolume');
    const value = state.options[name];
    return <OptionRow key={name} className="item" label={label} control={
      <div className="game-audio-control flex w-36 shrink-0 items-center gap-2">
        <Slider className="min-w-0 flex-1" aria-label={label} aria-valuetext={`${value}%`}
          min={0} max={100} step={1} value={value}
          onPointerDownCapture={() => { gesture.current = { game: state.gameId, volume: value }; }}
          onValueChange={(volume) => model.previewVolume(name, volume)}
          onValueCommit={() => { gesture.current = null; model.commitPreferences(); }}
          onValueCancel={() => {
            const start = gesture.current;
            gesture.current = null;
            if (start && start.game === model.getSnapshot()?.gameId) model.previewVolume(name, start.volume);
          }} />
        <output className="w-9 shrink-0 text-right tabular-nums" aria-hidden="true">{value}%</output>
      </div>
    } />;
  });
}

export function GameSettingsPopover({ model, exit }: {
  model: GameSettingsModel;
  exit?: { label: string; onSelect(): void };
}) {
  const { t } = useLocale();
  const label = t('settings.title');
  return <Popover>
    <PopoverTrigger asChild>
      <Button variant="unstyled" type="button"
        className="game-settings-toggle" aria-label={label} title={label}><PlayerSettingsIcon /></Button>
    </PopoverTrigger>
    <PopoverContent align="end" aria-label={label} className="game-settings-popover">
      <GameAudioFields model={model} />
      {exit && <div className="game-settings-actions">
        <PopoverClose asChild>
          <Button variant="quiet" size="sm" type="button" className="game-settings-exit" onClick={exit.onSelect}>
            <PlayerExitIcon />{exit.label}
          </Button>
        </PopoverClose>
      </div>}
    </PopoverContent>
  </Popover>;
}
