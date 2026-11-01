import { useLocale } from '../../i18n';
import { LaunchActions } from '../launcher/LaunchActions';
import { OptionRow } from '../settings/OptionRow';
import { OptionSwitch } from '../settings/TouchSettingsFields';

export interface MultiplayerSettingsControlsProps {
  onReplayViewer(): void | Promise<void>;
  onImport(): void;
  autoReplay?: { enabled: boolean; onChange(enabled: boolean): void };
}
/** Shared local actions. Room-wide timing belongs to RoomGameSettings. */
export function MultiplayerSettingsControls({
  onReplayViewer,
  onImport,
  autoReplay,
}: MultiplayerSettingsControlsProps) {
  const { t } = useLocale();
  return (
    <>
      {autoReplay && (
        <OptionRow
          className="item mp-shared-settings"
          label={t('th09.replayAutoSave')}
          hint={t('th09.replayAutoSaveHint')}
          hintId="mpReplayAutoSaveHint"
          control={
            <OptionSwitch
              id="mpReplayAutoSave"
              checked={autoReplay.enabled}
              label={t('th09.replayAutoSave')}
              describedBy="mpReplayAutoSaveHint"
              onChange={() => autoReplay.onChange(!autoReplay.enabled)}
            />
          }
        />
      )}
      <LaunchActions
        placement="compact"
        primary={{
          id: 'mpReplayViewer',
          label: t('action.watchReplay'),
          onClick: () => {
            void onReplayViewer();
          },
        }}
        secondary={{ id: 'mpGamePackageImport', label: t('action.import'), onClick: onImport }}
      />
    </>
  );
}
