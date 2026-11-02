import {createContext, forwardRef, useContext, useLayoutEffect, useRef, useState, useCallback, type ReactNode, type SelectHTMLAttributes} from 'react';
import {Select} from 'radix-ui';
import {Check, ChevronDown, ChevronUp} from 'lucide-react';
import {useLocale} from '../../i18n';
import {uiPortalHost} from '../../ui/portal';
import {usePopupPadding} from '../../ui/use-popup-padding';

export type MainSelectProps = SelectHTMLAttributes<HTMLSelectElement> & {'data-trigger-i18n'?: string};
const Prefix = createContext<ReactNode>(null);
const menus = new Set<() => void>();
const emptyValue = '__eagler_empty_selection__';
export function MainSelectPrefix({children, prefix}: {children: ReactNode; prefix: ReactNode}) {return <Prefix value={prefix}>{children}</Prefix>;}
export function closeMainSelectMenus() {for (const close of menus) close();}

/** The native element owns forms, refs and change events; Radix owns the popup. */
export const MainSelect = forwardRef<HTMLSelectElement, MainSelectProps>(function MainSelect(props, ref) {
  const {t} = useLocale();
  const native = useRef<HTMLSelectElement | null>(null), trigger = useRef<HTMLButtonElement>(null);
  const prefix = useContext(Prefix);
  const [open, setOpen] = useState(false);
  const padding = usePopupPadding(open);
  const [value, setValue] = useState(String(props.value ?? props.defaultValue ?? ''));
  const [options, setOptions] = useState<{value: string; label: string; disabled: boolean}[]>([]);
  const [host, setHost] = useState<HTMLElement>();
  const [surface, setSurface] = useState<string>();
  const attach = useCallback((element: HTMLSelectElement | null) => {
    native.current = element;
    if (typeof ref === 'function') ref(element); else if (ref) ref.current = element;
  }, [ref]);
  useLayoutEffect(() => {
    const close = () => setOpen(false); menus.add(close);
    document.addEventListener('fullscreenchange', close);document.addEventListener('webkitfullscreenchange', close);
    return () => {menus.delete(close);document.removeEventListener('fullscreenchange', close);document.removeEventListener('webkitfullscreenchange', close);};
  }, []);
  useLayoutEffect(() => {
    if (!native.current) return;
    const next = [...native.current.options].filter(option => !option.hidden).map(option => ({value: option.value, label: option.text, disabled: option.disabled}));
    setOptions(previous => JSON.stringify(previous) === JSON.stringify(next) ? previous : next);
    setValue(native.current.value);
  });
  const change = (next: string) => {
    const select = native.current; if (!select) return;
    select.value = next === emptyValue ? '' : next;
    setValue(select.value);
    select.dispatchEvent(new Event('change', {bubbles: true}));
  };
  return <div className="ui-select mizuki-select">
    <select {...props} ref={attach} className={`${props.className ?? ''} custom-select-native`} tabIndex={-1} aria-hidden="true" onFocus={event => {props.onFocus?.(event); trigger.current?.focus();}}/>
    <Select.Root value={value || emptyValue} onValueChange={change} open={open} onOpenChange={next => {
      if (next) {closeMainSelectMenus(); setHost(uiPortalHost(trigger.current)); setSurface(trigger.current?.closest('[data-ui-surface]')?.getAttribute('data-ui-surface') ?? undefined);}
      setOpen(next);
    }} disabled={props.disabled} required={props.required}>
      <Select.Trigger ref={trigger} id={props.id ? `${props.id}-trigger` : undefined} data-slot="select-trigger" className="ui-select-trigger mizuki-select-trigger" aria-label={props['aria-label'] || (props['aria-labelledby'] ? undefined : t('common.selectOption'))} aria-labelledby={props['aria-labelledby']} aria-describedby={props['aria-describedby']} title={props.title}>
        {prefix}<Select.Value>{options.find(option => option.value === value)?.label ?? ''}</Select.Value><Select.Icon><ChevronDown aria-hidden="true"/></Select.Icon>
      </Select.Trigger>
      <Select.Portal container={host}><Select.Content className="ui-select-content" data-ui-surface={surface} data-launcher-document={native.current?.closest('[data-launcher-document]') ? '' : undefined} position="popper" sideOffset={5} collisionPadding={padding}>
        <Select.ScrollUpButton className="ui-select-scroll"><ChevronUp aria-hidden="true"/></Select.ScrollUpButton>
        <Select.Viewport className="ui-select-viewport">{options.map(option => <Select.Item key={option.value || emptyValue} value={option.value || emptyValue} disabled={option.disabled} className="ui-select-item" data-value={option.value}>
          <Select.ItemText>{option.label}</Select.ItemText><Select.ItemIndicator><Check aria-hidden="true"/></Select.ItemIndicator>
        </Select.Item>)}</Select.Viewport>
        <Select.ScrollDownButton className="ui-select-scroll"><ChevronDown aria-hidden="true"/></Select.ScrollDownButton>
      </Select.Content></Select.Portal>
    </Select.Root>
  </div>;
});
