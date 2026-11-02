/** Main app.mts askDecision contract. Decisions own no browser history entry. */
export type DecisionChoice = 'confirm' | 'cancel' | 'secondary';
export interface DecisionInput {
  readonly label: string;
  readonly defaultValue: string;
  readonly description?: string;
  readonly validate?: (value: string) => string | null;
}
export interface DecisionOptions {
  title?: string;
  message?: string;
  confirmText?: string;
  cancelText?: string;
  secondaryText?: string;
  tone?: 'normal' | 'danger';
  variant?: string;
  hideCancel?: boolean;
  confirmOnEnter?: boolean;
  input?: DecisionInput;
}
export interface DecisionRequest extends Readonly<DecisionOptions> {readonly requestId: number;}
export interface DecisionStore {
  subscribe(listener: () => void): () => void;
  getSnapshot(): DecisionRequest | null;
  askDecision(options?: DecisionOptions): Promise<DecisionChoice>;
  askConfirmation(options?: DecisionOptions): Promise<boolean>;
  askText(options: DecisionOptions & {input: DecisionInput}): Promise<string | null>;
  resolve(choice: DecisionChoice, value?: string): void;
  /** Reserves the one native decision surface for Router's dirty-close prompt. */
  setNavigationDecisionOpen(open: boolean): void;
  dispose(): void;
}
export function createDecisionStore(): DecisionStore {
  let pending: DecisionRequest | null = null;
  let requestSequence = 0;
  let resolver: ((choice: DecisionChoice, value?: string) => void) | null = null;
  let navigationDecisionOpen = false;
  let disposed = false;
  const listeners = new Set<() => void>();
  const emit = () => {for (const listener of listeners) listener();};
  function request(options: DecisionOptions, resolve: NonNullable<typeof resolver>) {
    // Main does not replace the existing request or queue a second prompt.
    if (disposed || resolver || navigationDecisionOpen) {resolve('cancel'); return;}
    resolver = resolve;
    pending = Object.freeze({...options, requestId: ++requestSequence});
    emit();
  }
  function askDecision(options: DecisionOptions = {}): Promise<DecisionChoice> {
    return new Promise(resolve => request(options, resolve));
  }
  function settle(choice: DecisionChoice, value?: string) {
    const resolve = resolver;
    if (!resolve) return;
    resolver = null;
    pending = null;
    emit();
    resolve(choice, value);
  }
  return {
    subscribe(listener) {listeners.add(listener); return () => {listeners.delete(listener);};},
    getSnapshot: () => pending,
    askDecision,
    askConfirmation: options => askDecision(options).then(choice => choice === 'confirm'),
    askText: options => new Promise(resolve => request(options, (choice, value) => resolve(choice === 'confirm' ? value ?? options.input.defaultValue : null))),
    resolve: settle,
    setNavigationDecisionOpen(open) {navigationDecisionOpen = open;},
    dispose() {
      if (disposed) return;
      // Mark teardown before notifying subscribers: their async/reentrant
      // requests must cancel rather than reopen an owner with no mounted view.
      disposed = true;
      settle('cancel'); navigationDecisionOpen = false; listeners.clear();
    },
  };
}
