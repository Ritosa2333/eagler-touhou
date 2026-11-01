import { type Ref, useImperativeHandle } from 'react';
import { useLocale } from '../../i18n';
import { type MainDialogProps, useMainDialog } from '../notices/use-main-dialog';
import { LobbyDialogHeader } from './LobbyDialogHeader';
import { NetworkDiagnosticResults } from './NetworkDiagnosticResults';
import { type NetworkDiagnosticsPorts, useNetworkDiagnostics } from './use-network-diagnostics';

export interface NetworkDiagnosticsHandle {
  run(): Promise<void>;
  isRunning(): boolean;
}
export interface NetworkDiagnosticsDialogProps extends MainDialogProps, NetworkDiagnosticsPorts {
  /** Already normalized by original buildMultiplayerDiagnosticRelayUrl. */
  getRelayUrl(): string;
  getFallbackIceServers?(): RTCIceServer[];
  onRunningChange?(running: boolean): void;
  ref?: Ref<NetworkDiagnosticsHandle>;
}
/** Original lobby.html112–130. Forward restores the panel without rerunning;
 * only the original Network button action invokes the imperative run port.
 */
export function NetworkDiagnosticsDialog({
  model,
  getRelayUrl,
  getFallbackIceServers,
  onRunningChange,
  ref,
  ...props
}: NetworkDiagnosticsDialogProps) {
  const { t } = useLocale(),
    dialog = useMainDialog(props, 0);
  const { snapshot, actions } = useNetworkDiagnostics({ model, getRelayUrl, getFallbackIceServers, onRunningChange });
  useImperativeHandle(ref, () => actions, [actions]);
  return (
    <dialog
      ref={dialog.ref}
      className="lobby-dialog"
      id="lobbyNetworkDialog"
      aria-labelledby="lobbyNetworkTitle"
      onCancel={dialog.onCancel}
      onClick={dialog.onClick}
    >
      <LobbyDialogHeader
        titleId="lobbyNetworkTitle"
        title={t('networkCheck.title')}
        closeId="lobbyNetworkClose"
        closeLabel={t('action.close')}
        closeContent="×"
        onCloseRequest={dialog.requestClose}
      />
      <NetworkDiagnosticResults snapshot={snapshot} />
    </dialog>
  );
}
