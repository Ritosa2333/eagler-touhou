import type { GameVersion } from '../../src/contracts/package-read-models.mts';
import type { ProductId } from '../../src/contracts/product-catalog.mts';
import type { Translate } from '../i18n';
import type { DecisionStore } from './decisions';

export interface GameUpdateContext {
  product: ProductId;
  gameVersion: GameVersion;
  epoch: number;
}
const key = (context: GameUpdateContext) => `${context.product}:${context.gameVersion}:${context.epoch}`;

/** Completed downloads share one prompt per running game. Never displace an
 * existing decision or let a late response restart a replacement Runtime. */
export function createGameUpdatePrompt({
  decisions,
  translate: t,
  current,
  busy,
  restart,
  onError,
}: {
  decisions: Pick<DecisionStore, 'getSnapshot' | 'askDecision' | 'subscribe' | 'resolve'>;
  translate: Translate;
  current(): GameUpdateContext | null;
  busy(): boolean;
  restart(context: GameUpdateContext): Promise<void>;
  onError(error: unknown): void;
}) {
  const pending = new Map<string, GameUpdateContext>(),
    shown = new Set<string>();
  let disposed = false,
    prompting = false,
    scheduled = false;
  let promptKey: string | null = null,
    requestId: number | undefined;
  function flush() {
    if (disposed || scheduled) return;
    scheduled = true;
    queueMicrotask(() => {
      scheduled = false;
      void present();
    });
  }
  async function present() {
    if (disposed) return;
    const active = current(),
      activeKey = active && key(active);
    if (prompting) {
      if (promptKey !== activeKey && decisions.getSnapshot()?.requestId === requestId) decisions.resolve('cancel');
      return;
    }
    for (const id of pending.keys()) if (id !== activeKey) pending.delete(id);
    const context = activeKey && pending.get(activeKey);
    if (!context || decisions.getSnapshot() || busy()) return;
    pending.delete(activeKey);
    shown.add(activeKey);
    prompting = true;
    promptKey = activeKey;
    try {
      const choicePending = decisions.askDecision({
        title: t('update.readyTitle', { game: context.product.toUpperCase() }),
        message: t('update.readyMessage'),
        confirmText: t('update.restartNow'),
        cancelText: t('update.nextLaunch'),
        confirmOnEnter: true,
      });
      requestId = decisions.getSnapshot()?.requestId;
      const choice = await choicePending;
      const latest = current();
      if (choice === 'confirm' && !disposed && latest && key(latest) === activeKey && !busy()) await restart(context);
    } catch (error) {
      if (!disposed) onError(error);
    } finally {
      prompting = false;
      promptKey = null;
      requestId = undefined;
      flush();
    }
  }
  const unsubscribe = decisions.subscribe(flush);
  return {
    ready(context: GameUpdateContext) {
      if (disposed || shown.has(key(context))) return;
      pending.set(key(context), { ...context });
      flush();
    },
    flush,
    dispose() {
      disposed = true;
      pending.clear();
      unsubscribe();
    },
  };
}
