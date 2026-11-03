import {
  createContext,
  forwardRef,
  useCallback,
  useContext,
  useLayoutEffect,
  useRef,
  useState,
  type ComponentPropsWithoutRef,
  type ForwardedRef,
  type KeyboardEvent as ReactKeyboardEvent,
  type MouseEvent as ReactMouseEvent,
  type PointerEvent as ReactPointerEvent,
  type RefObject,
} from 'react';
import { Select as Primitive } from 'radix-ui';
import { Check, ChevronDown, ChevronUp } from 'lucide-react';
import { uiPortalHost } from './portal';
import { useSurfaceMotion } from './use-surface-motion';
import { cn } from './utils';
import { listPopupIndicatorClass } from './list-popup';

type SelectContextValue = {
  trigger: RefObject<HTMLButtonElement | null>;
  content: RefObject<HTMLDivElement | null>;
  host?: HTMLElement;
  surface?: string;
  open: boolean;
  setOpen(open: boolean): void;
};

const SelectContext = createContext<SelectContextValue | null>(null);
const closers = new Set<() => void>();

export function closeSelectMenus(except?: () => void) {
  for (const close of closers) if (close !== except) close();
}

export const closeMainSelectMenus = closeSelectMenus;

function useSelectContext() {
  const context = useContext(SelectContext);
  if (!context) throw new Error('Select components must be nested under Select.');
  return context;
}

function assignRef<T>(ref: ForwardedRef<T> | undefined, value: T | null) {
  if (typeof ref === 'function') ref(value);
  else if (ref) ref.current = value;
}

function mergeRefs<T>(...refs: Array<ForwardedRef<T> | undefined>) {
  return (value: T | null) => {
    for (const ref of refs) assignRef(ref, value);
  };
}

function runAfterConsumer<E extends { defaultPrevented: boolean }>(
  consumer: ((event: E) => void) | undefined,
  behavior: (event: E) => void,
) {
  return (event: E) => {
    consumer?.(event);
    if (!event.defaultPrevented) behavior(event);
  };
}

/** A single Radix select implementation shared by native form adapters and UI-kit fields. */
export function Select({
  open: openProp,
  defaultOpen,
  onOpenChange,
  disabled,
  children,
  ...props
}: ComponentPropsWithoutRef<typeof Primitive.Root>) {
  const trigger = useRef<HTMLButtonElement>(null);
  const content = useRef<HTMLDivElement>(null);
  const [uncontrolledOpen, setUncontrolledOpen] = useState(defaultOpen ?? false);
  const [host, setHost] = useState<HTMLElement>();
  const [surface, setSurface] = useState<string>();
  const controlled = openProp !== undefined;
  const open = controlled ? openProp : uncontrolledOpen;
  const openRef = useRef(open);
  const controlledRef = useRef(controlled);
  const onOpenChangeRef = useRef(onOpenChange);
  const suppressRetargetedClick = useRef(false);
  openRef.current = open;
  controlledRef.current = controlled;
  onOpenChangeRef.current = onOpenChange;

  const setOpen = useCallback((next: boolean) => {
    if (openRef.current === next) return;
    if (next) {
      closeSelectMenus(closeRef.current);
      const anchor = trigger.current;
      setHost(uiPortalHost(anchor));
      setSurface(anchor?.closest('[data-ui-surface]')?.getAttribute('data-ui-surface') ?? undefined);
    }
    if (!controlledRef.current) {
      setUncontrolledOpen(next);
      openRef.current = next;
    }
    onOpenChangeRef.current?.(next);
  }, []);
  const close = useCallback(() => setOpen(false), [setOpen]);
  const closeRef = useRef(close);
  closeRef.current = close;

  useSurfaceMotion(content, open ? 'enter' : null, { kind: 'menu' });

  useLayoutEffect(() => {
    if (disabled) return;
    closers.add(close);
    document.addEventListener('fullscreenchange', close);
    document.addEventListener('webkitfullscreenchange', close);
    return () => {
      closers.delete(close);
      document.removeEventListener('fullscreenchange', close);
      document.removeEventListener('webkitfullscreenchange', close);
    };
  }, [close, disabled]);

  useLayoutEffect(() => {
    if (disabled) return;
    const closeFromRetargetedTouch = (event: PointerEvent) => {
      // A stale marker belongs to the previous pointer sequence. Clear it before
      // deciding whether this new touch needs the compatibility-click guard.
      suppressRetargetedClick.current = false;
      if (!openRef.current || event.pointerType !== 'touch' || event.button !== 0 || event.ctrlKey) return;
      const target = event.target;
      if (target instanceof Node && trigger.current?.contains(target)) return;
      const bounds = trigger.current?.getBoundingClientRect();
      if (
        !bounds ||
        event.clientX < bounds.left ||
        event.clientX > bounds.right ||
        event.clientY < bounds.top ||
        event.clientY > bounds.bottom
      ) return;
      suppressRetargetedClick.current = true;
      event.preventDefault();
      close();
    };
    const consumeRetargetedClick = (event: MouseEvent) => {
      if (!suppressRetargetedClick.current || event.detail === 0) return;
      suppressRetargetedClick.current = false;
      const target = event.target;
      if (!(target instanceof Node) || !trigger.current?.contains(target)) return;
      event.preventDefault();
      event.stopPropagation();
      close();
      trigger.current?.focus();
    };
    document.addEventListener('pointerdown', closeFromRetargetedTouch, true);
    document.addEventListener('click', consumeRetargetedClick, true);
    return () => {
      document.removeEventListener('pointerdown', closeFromRetargetedTouch, true);
      document.removeEventListener('click', consumeRetargetedClick, true);
    };
  }, [close, disabled]);

  const context: SelectContextValue = { trigger, content, host, surface, open, setOpen };
  return (
    <SelectContext.Provider value={context}>
      <Primitive.Root {...props} disabled={disabled} open={open} onOpenChange={setOpen}>
        {children}
      </Primitive.Root>
    </SelectContext.Provider>
  );
}

export const SelectValue = Primitive.Value;

export const SelectTrigger = forwardRef<HTMLButtonElement, ComponentPropsWithoutRef<typeof Primitive.Trigger>>(
  function SelectTrigger({ children, className, onPointerDown, onPointerCancel, onClick, onKeyDown, ...props }, forwardedRef) {
    const { trigger, open, setOpen } = useSelectContext();
    const triggerActivation = useRef({ pointerSequence: false, closeOnClick: false });
    const handlePointerDown = (event: ReactPointerEvent<HTMLButtonElement>) => {
      triggerActivation.current = {
        pointerSequence: true,
        closeOnClick: open && event.button === 0 && !event.ctrlKey,
      };
      if (triggerActivation.current.closeOnClick) {
        event.preventDefault();
        setOpen(false);
      }
    };
    const handlePointerCancel = () => {
      triggerActivation.current = { pointerSequence: false, closeOnClick: false };
    };
    const handleClick = (event: ReactMouseEvent<HTMLButtonElement>) => {
      const activation = triggerActivation.current;
      if (activation.closeOnClick || (open && !activation.pointerSequence)) {
        triggerActivation.current = { pointerSequence: false, closeOnClick: false };
        event.preventDefault();
        if (!activation.closeOnClick) setOpen(false);
        trigger.current?.focus();
      } else if (activation.pointerSequence) {
        triggerActivation.current = { pointerSequence: false, closeOnClick: false };
      }
    };
    const handleKeyDown = (event: ReactKeyboardEvent<HTMLButtonElement>) => {
      triggerActivation.current = { pointerSequence: false, closeOnClick: false };
      if (open && (event.key === 'Enter' || event.key === ' ')) {
        event.preventDefault();
        setOpen(false);
        trigger.current?.focus();
      }
    };

    return (
      <Primitive.Trigger
        {...props}
        ref={mergeRefs(forwardedRef, trigger)}
        data-slot="select-trigger"
        className={cn('ui-select-trigger ui-kit-select-trigger', className)}
        onPointerDown={runAfterConsumer(onPointerDown, handlePointerDown)}
        onPointerCancel={runAfterConsumer(onPointerCancel, handlePointerCancel)}
        onClick={runAfterConsumer(onClick, handleClick)}
        onKeyDown={runAfterConsumer(onKeyDown, handleKeyDown)}
      >
        {children}
        <Primitive.Icon asChild>
          <ChevronDown aria-hidden="true" />
        </Primitive.Icon>
      </Primitive.Trigger>
    );
  },
);

type SelectContentProps = ComponentPropsWithoutRef<typeof Primitive.Content> & { 'data-ui-surface'?: string };

export const SelectContent = forwardRef<HTMLDivElement, SelectContentProps>(
  function SelectContent({ children, className, 'data-ui-surface': dataUiSurface, ...props }, forwardedRef) {
    const { content, host, surface } = useSelectContext();
    return (
      <Primitive.Portal container={host}>
        <Primitive.Content
          {...props}
          ref={mergeRefs(forwardedRef, content)}
          position="popper"
          sideOffset={6}
          collisionPadding={12}
          data-slot="select-content"
          data-ui-surface={dataUiSurface ?? surface}
          className={cn('ui-select-content ui-kit-select-content', className)}
        >
          <Primitive.ScrollUpButton className="ui-select-scroll">
            <ChevronUp aria-hidden="true" />
          </Primitive.ScrollUpButton>
          <Primitive.Viewport className="ui-select-viewport">{children}</Primitive.Viewport>
          <Primitive.ScrollDownButton className="ui-select-scroll">
            <ChevronDown aria-hidden="true" />
          </Primitive.ScrollDownButton>
        </Primitive.Content>
      </Primitive.Portal>
    );
  },
);

export const SelectItem = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Item>>(
  function SelectItem({ children, className, ...props }, ref) {
    return (
      <Primitive.Item
        {...props}
        ref={ref}
        data-slot="select-item"
        className={cn('ui-select-item', className)}
      >
        <Primitive.ItemText>{children}</Primitive.ItemText>
        <Primitive.ItemIndicator className={listPopupIndicatorClass}>
          <Check aria-hidden="true" />
        </Primitive.ItemIndicator>
      </Primitive.Item>
    );
  },
);
