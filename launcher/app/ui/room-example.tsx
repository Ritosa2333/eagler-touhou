import { Ellipsis } from 'lucide-react';
import { type ReactNode, useEffect, useRef, useState } from 'react';
import { multiplayerConfigForProduct } from '../../src/contracts/product-catalog.mts';
import { normalizeMultiplayerDisplayName } from '../../src/launcher/multiplayer-identity.mts';
import type { MultiplayerLobbySeat } from '../../src/launcher/multiplayer-lobby-snapshot.mts';
import {
  createMultiplayerQuickChatModel,
  type MultiplayerQuickChatModel,
} from '../../src/launcher/multiplayer-quick-chat-model.mts';
import { RoomActionMenu } from '../components/room/RoomActionMenu';
import { RoomGameSettings } from '../components/room/RoomGameSettings';
import { RoomNetwork } from '../components/room/RoomNetwork';
import { RoomPanel } from '../components/room/RoomPanel';
import { RoomPersonal } from '../components/room/RoomPersonal';
import { ResourceTracks } from '../components/room/RoomProgress';
import { RoomQuickChat } from '../components/room/RoomQuickChat';
import { RoomSeats } from '../components/room/RoomSeats';
import { RoomSpectators } from '../components/room/RoomSpectators';
import type { RoomNetworkPresentation, RoomPanelKind, RoomViewContext } from '../components/room/types';
import type { RoomOwnerSnapshot } from '../services/multiplayer-room';
import { Button } from './button';
import { ChoiceGroup, ChoiceItem } from './choice-group';
import '../components/room/room-workspace.css';

const context: RoomViewContext = {
  launched: false,
  previewReady: true,
  th09NetworkOverlayOpen: false,
  launchStage: null,
  touchEnabled: false,
  touchMovementMode: 'joystick',
};
type DemoSeat = MultiplayerLobbySeat & { resource: NonNullable<MultiplayerLobbySeat['resource']> };
const seat = (clientId: string, name: string, loadout: number, ready: boolean): DemoSeat => ({
  clientId,
  name,
  loadout,
  ready,
  offline: false,
  controlMode: 'normal',
  mobileDevice: false,
  resource: { status: 'ready', stage: 'runtime', percent: 100 },
});
type Room = NonNullable<RoomOwnerSnapshot['room']>;
type DemoState = Omit<RoomOwnerSnapshot, 'room'> & {
  room: Room & { seats: NonNullable<Room['seats']>; spectators: NonNullable<Room['spectators']> };
};
function initial(): DemoState {
  const seats = [
    seat('handbook-host', '演示房主', 0, false),
    seat('handbook-player', '演示玩家', 1, false),
    seat('handbook-ready', '已准备玩家', 0, true),
  ];
  seats[1].resource = { status: 'preparing', stage: 'package', percent: 64 };
  return {
    product: 'th10mp',
    room: {
      code: '1024',
      playerCount: 3,
      difficulty: 1,
      phase: 'lobby',
      created: true,
      synced: true,
      visibility: 'public',
      seats,
      spectators: [{ clientId: 'handbook-viewer', name: '演示观众' }],
    },
    epoch: 1,
    fromDirectory: false,
    connected: true,
    directorySupported: true,
    controlModesSupported: true,
    seat: 0,
    ready: false,
    spectatorRequested: false,
    preferredLoadout: 0,
    loadoutPending: false,
    displayName: '演示房主',
    roomSettingsOpen: false,
    startSerial: 0,
    preparation: { status: 'ready', stage: 'runtime', percent: 100, error: '' },
    launchBusy: false,
    checkBusy: false,
    timingChoice: 'auto',
    rollbackEnabled: true,
    preferTurn: false,
    canStart: false,
    localClientId: 'handbook-host',
  };
}

/** Chinese maintainer examples use production views with document-local state. */
export function RoomExample({
  announce,
  chatDemo = false,
  renderRegion = (id, children) => (
    <section id={`page-room-${id}`} key={id}>
      {children}
    </section>
  ),
}: {
  announce(message: string): void;
  chatDemo?: boolean;
  renderRegion?(id: string, children: ReactNode): ReactNode;
}) {
  const [state, setState] = useState(initial);
  const [chatSerial, setChatSerial] = useState(0);
  const [chatMember,setChatMember]=useState(chatDemo);
  const [networkState, setNetworkState] = useState<'connected' | 'checking' | 'unavailable'>('connected');
  const [networkPeer, setNetworkPeer] = useState<string | undefined>();
  const [personalOpen, setPersonalOpen] = useState(false),
    [chat, setChat] = useState<MultiplayerQuickChatModel | null>(null);
  const trigger = useRef<HTMLElement | null>(null);
  useEffect(() => {
    let messageId=1;
    const pending = new Set<number>();
    const model = createMultiplayerQuickChatModel({
      send: (message) => {
        const current = model.getSnapshot().context;
        if(message.type==='chat-report'){
          const entry=model.getSnapshot().entries.find(item=>item.messageId===message.messageId);
          if(entry)model.receive({type:'chat-retract',authorId:entry.authorId,throughId:entry.messageId,bannedUntil:Date.now()+60000});
          model.receive({type:'chat-result',action:'chat-report',ok:true});
          return;
        }
        const deliver = () => model.receive({
          ...message,
          ...(message.type==='free-chat'?{messageId:messageId++,authorId:'handbook-member',name:'演示房主'}:{}),
          room: current?.room,
          seat: current?.localSeat ?? -1,
          spectator: current?.localSpectator,
          clientId: current?.localClientId ?? 'handbook-host',
        });
        if(message.type==='free-chat') {
          const timer=window.setTimeout(()=>{pending.delete(timer);deliver();model.receive({type:'chat-result',action:'free-chat',ok:true});}, 1500);
          pending.add(timer);
        } else deliver();
      },
      voice() {},
      notify:code=>announce(code==='reported'?'示例发言已撤回':'操作未完成'),
      timers: {
        setTimeout: (callback, delay) => window.setTimeout(callback, delay),
        clearTimeout: (timer) => window.clearTimeout(timer),
      },
    });
    setChat(model);
    return () => {pending.forEach(timer=>{window.clearTimeout(timer);});model.dispose();};
  }, [announce]);
  useEffect(() => {
    chat?.update({
      visible: true,
      room: '1024',
      serial: chatSerial,
      localSeat: state.seat,
      seats: state.room?.seats ?? [],
      spectators: state.room?.spectators,
      localClientId: state.localClientId,
      localSpectator: state.spectatorRequested,
      connected: state.connected,
      language: 'zh-CN',
      inRoom: true,
    });
  }, [chat, state, chatSerial]);
  useEffect(()=>{
    if(!chat)return;
    chat.receive({type:'chat-status',registered:chatMember,authorId:'handbook-member',bannedUntil:0});
    if(chatMember)chat.receive({type:'free-chat',messageId:100,authorId:'handbook-other',clientId:'handbook-player',name:'演示玩家',room:'1024',serial:chatSerial,seat:1,spectator:false,text:'这局打完一起试试下一作？'});
  },[chat,chatMember,chatSerial]);
  function updateRoom(patch: Partial<Room>) {
    setState((previous) => ({
      ...previous,
      room: {
        ...previous.room,
        ...patch,
        seats: patch.seats ?? previous.room.seats,
        spectators: patch.spectators ?? previous.room.spectators,
      },
    }));
  }
  function removePlayer(index: number, spectator = false) {
    const player = state.room?.seats?.[index];
    if (!player) return;
    updateRoom({
      seats: state.room.seats.map((value, i) => (i === index ? null : value)),
      spectators: spectator ? [...(state.room.spectators ?? []), player] : state.room.spectators,
    });
    announce(spectator ? '演示玩家已移到观战席' : '演示玩家已移除');
  }
  const actions = {
    getSnapshot: () => state,
    async takeSeat(index: number) {
      updateRoom({
        seats: state.room.seats.map((value, i) =>
          i === index
            ? seat(state.localClientId, state.displayName, 0, false)
            : value?.clientId === state.localClientId
              ? null
              : value,
        ),
      });
      setState((previous) => ({ ...previous, seat: index, ready: false, spectatorRequested: false }));
    },
    movePlayerToSpectator: (index: number) => removePlayer(index, true),
    async removePlayer(index: number) {
      removePlayer(index);
    },
    async removeSpectator(clientId: string) {
      updateRoom({ spectators: state.room.spectators.filter((value) => value.clientId !== clientId) });
    },
    async spectate() {
      updateRoom({
        seats: state.room.seats.map((value) => (value?.clientId === state.localClientId ? null : value)),
        spectators: [...(state.room.spectators ?? []), { clientId: state.localClientId, name: state.displayName }],
      });
      setState((previous) => ({ ...previous, seat: null, ready: false, spectatorRequested: true }));
    },
    async leaveSpectator() {
      updateRoom({ spectators: state.room.spectators.filter((value) => value.clientId !== state.localClientId) });
      setState((previous) => ({ ...previous, ready: false, spectatorRequested: false }));
    },
    setRoomSettings: updateRoom,
    timingRecommendation: () => ({ inputDelay: 0, targetRollbackFrames: 12, networkFrames: 0, mobileSeats: 0 }),
    setRollbackEnabled: (value: boolean) => setState((previous) => ({ ...previous, rollbackEnabled: value })),
    setTimingChoice: (value: 'auto' | number) => setState((previous) => ({ ...previous, timingChoice: value })),
    setDisplayName(value: string) {
      const displayName = normalizeMultiplayerDisplayName(value);
      setState((previous) => ({
        ...previous,
        displayName,
        room: {
          ...previous.room,
          seats: previous.room.seats.map((member) =>
            member?.clientId === previous.localClientId ? { ...member, name: displayName } : member,
          ),
        },
      }));
      return displayName;
    },
    changeLoadout(delta: number) {
      const config = multiplayerConfigForProduct('th10mp');
      if (!config) return;
      const count = config.loadouts.length;
      setState((previous) => {
        const preferredLoadout = (previous.preferredLoadout + delta + count) % count;
        return {
          ...previous,
          preferredLoadout,
          room: {
            ...previous.room,
            seats: previous.room.seats.map((member) =>
              member?.clientId === previous.localClientId ? { ...member, loadout: preferredLoadout } : member,
            ),
          },
        };
      });
    },
    async standUp() {
      updateRoom({
        seats: state.room.seats.map((member) => (member?.clientId === state.localClientId ? null : member)),
      });
      setState((previous) => ({ ...previous, seat: null, ready: false }));
    },
    async checkGame() {
      announce('游戏检查从实际联机房间进入。');
    },
    setPreferTurn: (preferTurn: boolean) => setState((previous) => ({ ...previous, preferTurn })),
  };
  const network: Pick<
    RoomNetworkPresentation,
    'capabilities' | 'metric' | 'retry' | 'loss' | 'lossReady' | 'testLoss'
  > = {
    capabilities: () => ({ supported: true, rtcAvailable: true, turnConfigured: true }),
    metric: (_peer, lane) => ({
      state: networkState,
      rtt: networkState === 'connected' ? (lane === 'direct' ? 32 : 86) : null,
      jitter: networkState === 'connected' ? 3 : null,
      at: Date.now(),
    }),
    retry: () => setNetworkState('checking'),
    loss: () => ({ state: 'idle', sent: 0 }),
    lossReady: () => false,
    testLoss() {},
  };
  const player = state.room?.seats?.[1];
  const appearance = !player
    ? 'empty'
    : player.offline
      ? 'reconnecting'
      : player.resource?.status === 'failed'
        ? 'failed'
        : player.resource?.status !== 'ready'
          ? 'loading'
          : player.ready
            ? 'ready'
            : 'unready';
  const role = state.spectatorRequested
    ? 'spectator'
    : state.seat === 0
      ? 'host'
      : state.seat == null
        ? 'unseated'
        : 'member';
  const owner = state.seat === 0,
    waiting = state.room?.phase === 'lobby',
    connected = state.connected && state.room?.synced === true;
  const canManage = connected && owner && waiting && !!player && player.clientId !== state.localClientId;
  const prepared = state.preparation?.status === 'ready';
  const canStart =
    connected &&
    owner &&
    waiting &&
    state.room.seats
      .slice(0, state.room.playerCount)
      .every((member) => member?.ready && member.resource?.status === 'ready');
  const viewContext = { ...context, launched: !waiting };
  const openPanel = (kind: RoomPanelKind, element: HTMLElement, peerId?: string) => {
    if (kind === 'network') {
      setNetworkPeer(peerId);
      document.getElementById('page-room-network')?.scrollIntoView({ block: 'start' });
      return;
    }
    if (kind === 'personal') {
      trigger.current = element;
      setPersonalOpen(true);
    }
  };
  const selectRole = (value: string) => {
    const next = initial();
    if (value === 'member') {
      next.seat = 1;
      next.localClientId = 'handbook-player';
      next.displayName = '演示玩家';
      next.preferredLoadout = 1;
      next.preparation = { status: 'preparing', stage: 'package', percent: 64, error: '' };
    }
    if (value === 'spectator') {
      next.seat = null;
      next.localClientId = 'handbook-viewer';
      next.displayName = '演示观众';
      next.spectatorRequested = true;
    }
    if (value === 'unseated') {
      next.seat = null;
      next.localClientId = 'handbook-unseated';
      next.displayName = '未入座玩家';
    }
    setState(next);
    setPersonalOpen(false);
    setNetworkPeer(undefined);
    setChatSerial((serial) => serial + 1);
  };
  const setPreparation = (status: 'preparing' | 'ready' | 'failed' | 'cancelled') => {
    setState((previous) => ({
      ...previous,
      ready: false,
      preparation: {
        status,
        stage: 'package',
        percent: status === 'ready' ? 100 : 64,
        error: status === 'failed' ? '示例资源不可用' : '',
      },
      room: {
        ...previous.room,
        seats: previous.room.seats.map((member) =>
          member?.clientId === previous.localClientId
            ? {
                ...member,
                ready: false,
                resource: { status, stage: 'package', percent: status === 'ready' ? 100 : 64 },
              }
            : member,
        ),
      },
    }));
  };
  return (
    <>
      <p className="catalog-muted">操作只改变本页的房间。资源加载、启动和保存交接从实际联机大厅检查。</p>
      <div className="catalog-room-controls">
        <div>
          <span>当前身份</span>
          <ChoiceGroup
            type="single"
            className="catalog-choice-group"
            aria-label="房间示例身份"
            value={role}
            onValueChange={(value) => {
              if (value) selectRole(value);
            }}
          >
            {[
              ['host', '房主'],
              ['member', '普通成员'],
              ['spectator', '观战'],
              ['unseated', '未入座'],
            ].map(([value, label]) => (
              <ChoiceItem key={value} value={value}>
                {label}
              </ChoiceItem>
            ))}
          </ChoiceGroup>
        </div>
        <div>
          <span>房间阶段</span>
          <ChoiceGroup
            type="single"
            className="catalog-choice-group"
            aria-label="房间示例阶段"
            value={waiting ? 'lobby' : 'running'}
            onValueChange={(value) => {
              if (value === 'lobby' || value === 'running') updateRoom({ phase: value });
            }}
          >
            {[
              ['lobby', '等待准备'],
              ['running', '游戏中'],
            ].map(([value, label]) => (
              <ChoiceItem key={value} value={value}>
                {label}
              </ChoiceItem>
            ))}
          </ChoiceGroup>
        </div>
        <div>
          <span>成员数量</span>
          <ChoiceGroup
            type="single"
            className="catalog-choice-group"
            aria-label="房间示例人数"
            value={String(state.room?.playerCount)}
            onValueChange={(value) => {
              if (value === '2' || value === '3') updateRoom({ playerCount: Number(value) as 2 | 3 });
            }}
          >
            {['2', '3'].map((value) => (
              <ChoiceItem value={value} key={value}>
                {value}P
              </ChoiceItem>
            ))}
          </ChoiceGroup>
        </div>
        <Button
          variant="outline"
          onClick={() => {
            setState(initial());
            setNetworkState('connected');
            setNetworkPeer(undefined);
            setPersonalOpen(false);
            setChatSerial((serial) => serial + 1);
            announce('');
          }}
        >
          重置房间示例
        </Button>
      </div>
      <div id="player" className="room-workspace catalog-room-workspace" data-launcher-document="">
        <div id="mpRoomView" className="room-workspace-view">
          {renderRegion(
            'seats',
            <>
              <div className="catalog-row">
                <span>P2 状态</span>
                <ChoiceGroup
                  type="single"
                  className="catalog-choice-group"
                  aria-label="P2 示例状态"
                  value={appearance}
                  onValueChange={(value) => {
                    if (!value) return;
                    const next = seat('handbook-player', '演示玩家', 1, value === 'ready');
                    next.offline = value === 'reconnecting';
                    if (value === 'loading') next.resource = { status: 'preparing', stage: 'package', percent: 64 };
                    if (value === 'failed') next.resource = { status: 'failed', stage: 'package', percent: 64 };
                    updateRoom({
                      seats: state.room.seats.map((valueAtSeat, i) =>
                        i === 1 ? (value === 'empty' ? null : next) : valueAtSeat,
                      ),
                    });
                    if (state.seat === 1) {
                      if (value === 'empty') setState((previous) => ({ ...previous, seat: null, ready: false }));
                      else
                        setState((previous) => ({
                          ...previous,
                          ready: next.ready,
                          preparation: { ...next.resource, error: '' },
                        }));
                    }
                  }}
                >
                  {[
                    ['loading', '加载'],
                    ['failed', '失败'],
                    ['reconnecting', '重连'],
                    ['unready', '未准备'],
                    ['ready', '已准备'],
                    ['empty', '空席'],
                  ].map(([value, label]) => (
                    <ChoiceItem value={value} key={value}>
                      {label}
                    </ChoiceItem>
                  ))}
                </ChoiceGroup>
              </div>
              <div className="room-workspace-roster">
                <RoomSeats
                  state={state}
                  service={actions}
                  network={network}
                  context={viewContext}
                  openPanel={openPanel}
                />
              </div>
            </>,
          )}
          {renderRegion(
            'management',
            <div className="catalog-row">
              <RoomActionMenu
                enabled={canManage}
                label="管理 P2 样例"
                triggerContent={<Ellipsis aria-hidden="true" />}
                triggerKind="player"
                menuClassName="room-member-menu"
                items={[
                  { id: 'move-spectator', label: '移到观战席', select: () => removePlayer(1, true) },
                  { id: 'remove', label: '移除玩家', danger: true, select: () => removePlayer(1) },
                ]}
              />
              <Button variant="outline" onClick={(event) => openPanel('personal', event.currentTarget)}>
                本人角色与昵称
              </Button>
              <span className="catalog-muted">{owner ? '房主可管理其他玩家' : '当前身份不能管理其他玩家'}</span>
            </div>,
          )}
          {renderRegion('chat', chat && <><ChoiceGroup type="single" value={chatMember?'member':'guest'} onValueChange={value=>setChatMember(value==='member')} aria-label="聊天身份示例">
            <ChoiceItem value="guest">游客</ChoiceItem><ChoiceItem value="member">注册用户</ChoiceItem>
          </ChoiceGroup>{chatDemo ? <div className="room-workspace-chat" style={{height:300}}><RoomQuickChat model={chat} part="log"/><RoomQuickChat model={chat} part="actions"/></div> : <RoomQuickChat model={chat}/>}</>)}
          {renderRegion('spectators', <RoomSpectators state={state} service={actions} hidden={false} />)}
          {renderRegion('settings', <RoomGameSettings state={state} service={actions} hidden={false} />)}
          {renderRegion(
            'network',
            <>
              <ChoiceGroup
                type="single"
                className="catalog-choice-group"
                aria-label="网络示例状态"
                value={networkState}
                onValueChange={(value) => {
                  if (value === 'connected' || value === 'checking' || value === 'unavailable') setNetworkState(value);
                }}
              >
                {[
                  ['connected', '已测量'],
                  ['checking', '检查中'],
                  ['unavailable', '不可用'],
                ].map(([value, label]) => (
                  <ChoiceItem value={value} key={value}>
                    {label}
                  </ChoiceItem>
                ))}
              </ChoiceGroup>
              <div className="catalog-row">
                <Button
                  variant="outline"
                  onClick={() => setState((previous) => ({ ...previous, connected: !previous.connected }))}
                >
                  {state.connected ? '模拟断线' : '恢复连接'}
                </Button>
                {networkPeer && (
                  <Button variant="ghost" onClick={() => setNetworkPeer(undefined)}>
                    查看所有玩家
                  </Button>
                )}
              </div>
              <RoomNetwork state={state} network={network} context={viewContext} peerId={networkPeer} hidden={false} />
            </>,
          )}
          {renderRegion(
            'session',
            <div className="catalog-recipe">
              <ChoiceGroup
                type="single"
                className="catalog-choice-group"
                aria-label="本人资源状态"
                value={state.preparation?.status ?? 'preparing'}
                onValueChange={(value) => {
                  if (value === 'preparing' || value === 'ready' || value === 'failed' || value === 'cancelled')
                    setPreparation(value);
                }}
              >
                {[
                  ['preparing', '准备中'],
                  ['ready', '就绪'],
                  ['failed', '失败'],
                  ['cancelled', '已取消'],
                ].map(([value, label]) => (
                  <ChoiceItem key={value} value={value}>
                    {label}
                  </ChoiceItem>
                ))}
              </ChoiceGroup>
              <ResourceTracks progress={state.preparation} />
              <div className="catalog-row">
                <Button
                  className="mp-ready-button"
                  disabled={!connected || !waiting || state.seat == null || state.loadoutPending || !prepared}
                  aria-pressed={state.ready}
                  onClick={() => {
                    setState((previous) => ({
                      ...previous,
                      ready: !previous.ready,
                      room: {
                        ...previous.room,
                        seats: previous.room.seats.map((member) =>
                          member?.clientId === previous.localClientId ? { ...member, ready: !previous.ready } : member,
                        ),
                      },
                    }));
                  }}
                >
                  {state.ready ? '取消准备' : '准备'}
                </Button>
                <Button
                  className="mp-start-button"
                  disabled={!canStart}
                  onClick={() => {
                    updateRoom({ phase: 'running' });
                    announce('已切换游戏中样例 | 不启动游戏');
                  }}
                >
                  开始游戏样例
                </Button>
                <Button
                  variant="outline"
                  onClick={() => {
                    updateRoom({ phase: 'lobby' });
                    setState((previous) => ({
                      ...previous,
                      ready: false,
                      room: {
                        ...previous.room,
                        seats: previous.room.seats.map((member) =>
                          member?.clientId === previous.localClientId ? { ...member, ready: false } : member,
                        ),
                      },
                    }));
                    announce('已返回等待样例 | 实际离房与保存从联机大厅检查');
                  }}
                >
                  返回等待样例
                </Button>
              </div>
            </div>,
          )}
          <RoomPanel
            open={personalOpen}
            kind="personal"
            onCloseRequest={() => setPersonalOpen(false)}
            onClosed={() => {}}
            trigger={trigger}
          >
            {personalOpen && (
              <RoomPersonal
                state={state}
                service={actions}
                network={network}
                assetUrl={(path) => path}
                th09NetworkOverlayOpen={false}
                hidden={false}
              />
            )}
          </RoomPanel>
        </div>
      </div>
    </>
  );
}
