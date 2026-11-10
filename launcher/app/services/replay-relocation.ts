import {gameIdForProduct, isMultiplayerProductId, type ProductId} from '../../src/contracts/product-catalog.mts';
import {RuntimeSessionSupersededError, type RuntimeService} from './runtime';
import type {RuntimeCommandPayloads} from '../../src/contracts/runtime-protocol.mts';

/** Update an active recording before its old file is removed. */
export async function relocateReplayPlayback(runtime: RuntimeService, product: ProductId, epoch: number, source: string, target: string,
  request = (payload: RuntimeCommandPayloads['playback']) => runtime.send('playback', payload)): Promise<void> {
  const current = () => {
    const state = runtime.getSnapshot();
    if (state.epoch !== epoch || state.game !== gameIdForProduct(product) || state.runtimeVariant !== (isMultiplayerProductId(product) ? 'multiplayer' : 'normal')) throw new RuntimeSessionSupersededError();
    return state;
  };
  const state = current(), playback = state.runtimeInfo.playback as Record<string, unknown> | undefined;
  if (!state.launched || playback?.kind !== 'replay' || playback.path !== source) return;
  let expectedPath = source;
  const assertPlayback = () => {if ((current().runtimeInfo.playback as Record<string, unknown> | undefined)?.path !== expectedPath) throw new RuntimeSessionSupersededError();};
  const send = async (payload: RuntimeCommandPayloads['playback'], nextPath = expectedPath) => {
    assertPlayback(); await request(payload); expectedPath = nextPath; assertPlayback();
  };
  if (playback.fileRelocation === 1) {
    await send({action: 'relocate', path: source, target}, target);
    return;
  }
  // Older Runtime packages use their existing playback commands. Keep their
  // selected stage, position and controls while reopening the new path.
  await send({action: 'pause'});
  const stage = typeof playback.startStage === 'number' ? playback.startStage : undefined;
  await send({action: 'open', path: target, ...(stage !== undefined ? {stage} : {})}, target);
  if (typeof playback.frame === 'number' && playback.frame > 0) await send({action: 'seek', value: playback.frame});
  if (playback.paused || playback.ended) await send({action: 'pause'});
  if (typeof playback.rate === 'number') await send({action: 'rate', value: playback.rate});
}
