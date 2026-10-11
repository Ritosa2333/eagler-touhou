import {useLocale} from '../../i18n';
import {LaunchActions} from '../launcher/LaunchActions';

export interface MultiplayerSettingsControlsProps {
  onReplayViewer(): void | Promise<void>;
  onImport(): void;
}
/** Shared local actions. Room-wide timing belongs to RoomGameSettings. */
export function MultiplayerSettingsControls({onReplayViewer, onImport}: MultiplayerSettingsControlsProps) {
  const {t} = useLocale();
  return <LaunchActions placement="compact" primary={{id: 'mpReplayViewer', label: t('action.watchReplay'), onClick: () => {void onReplayViewer();}}}
    secondary={{id: 'mpGamePackageImport', label: t('action.import'), onClick: onImport}}/>;
}
