import {useLocale} from '../i18n';
import {forwardRef, type ComponentPropsWithoutRef} from 'react';
import {Dialog as Primitive} from 'radix-ui';
import {X} from 'lucide-react';
import {Button} from './button';
import {cn} from './utils';
import {FullscreenTransient} from '../components/FullscreenTransient';

export const Dialog = Primitive.Root;
export const DialogTrigger = Primitive.Trigger;
export const DialogClose = Primitive.Close;
export const DialogTitle = Primitive.Title;
export const DialogDescription = Primitive.Description;

/** Ordinary dialogs share Radix focus behavior and a stable fullscreen portal. */
export const DialogContent = forwardRef<HTMLDivElement, ComponentPropsWithoutRef<typeof Primitive.Content> & {
  closeLabel?: string; showClose?: boolean; portalAnchor?: HTMLElement | null;
}>(function DialogContent(
  {children, className, closeLabel, showClose = true, portalAnchor, ...props}, ref,
) {
  const {t} = useLocale();
  return <FullscreenTransient anchor={portalAnchor}><Primitive.Overlay className="ui-kit-dialog-overlay"/><Primitive.Content {...props} ref={ref} data-slot="dialog-content" className={cn('ui-kit-dialog', className)}>
    {children}{showClose && <Primitive.Close asChild><Button variant="ghost" size="icon" className="ui-kit-dialog-close" aria-label={closeLabel ?? t('action.close')}><X aria-hidden="true"/></Button></Primitive.Close>}
  </Primitive.Content></FullscreenTransient>;
});
