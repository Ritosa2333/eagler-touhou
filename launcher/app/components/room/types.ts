import type {
  ProbeLane,
  ProbeMetric,
  createRoomNetwork,
} from '../../../src/launcher/room-network.mts';
import type { MultiplayerRoomService } from '../../services/multiplayer-room';
import type { TouchMovementMode } from '../../../src/launcher/game-preferences.mts';
import type { MultiplayerQuickChatModel } from '../../../src/launcher/multiplayer-quick-chat-model.mts';
import type {ReactNode} from 'react';
export type RoomPanelKind = 'personal' | 'network' | 'spectators' | 'game';
export interface RoomNetworkPresentation {
  subscribe(listener: () => void): () => void;
  getSnapshot(): number;
  metric(peer: string, lane: ProbeLane): ProbeMetric;
  capabilities(): ReturnType<ReturnType<typeof createRoomNetwork>['capabilities']>;
  retry(peer?: string): void;
  testLoss(): void;
  loss: ReturnType<typeof createRoomNetwork>['loss'];
  lossReady: ReturnType<typeof createRoomNetwork>['lossReady'];
}
export interface RoomViewContext {
  transfer?: import('../../models/transfer').TransferSnapshot;
  launched: boolean;
  previewReady?: boolean;
  th09NetworkOverlayOpen: boolean;
  launchStage: 'path' | 'preparing' | null;
  touchEnabled: boolean;
  touchMovementMode: TouchMovementMode;
}
export interface RoomViewProps {
  quickChat?: MultiplayerQuickChatModel;
  service: MultiplayerRoomService;
  network: RoomNetworkPresentation;
  context: RoomViewContext;
  assetUrl(path: string): string;
  onLeave(): void | Promise<void>;
  onCopyRoomCode(code: string): void | Promise<void>;
  onGuide(): void;
  onOpenSettings(): void;
  settingsOpen: boolean;
  visible?: boolean;
  settingsClosing?: boolean;
  workspaceHost?: HTMLElement | null;
  workspaceStage?: HTMLElement | null;
  playbackControls?: ReactNode;
  reloadPending?: boolean;
  onReloadResources?(): void;
  onRestoreSettings?(): void;
  onFullscreen?(): void;
  panel: { kind: RoomPanelKind; peerId?: string } | null;
  onOpenPanel(kind: RoomPanelKind, peerId?: string): void;
  onClosePanel(): void;
}
