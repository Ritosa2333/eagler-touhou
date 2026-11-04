/** Native dialogs and fullscreen elements form browser top layers. */
export function uiPortalHost(anchor?: HTMLElement | null): HTMLElement | undefined {
  if (typeof document === 'undefined') return undefined;
  const fullscreen = document.fullscreenElement || (document as Document & {webkitFullscreenElement?: Element}).webkitFullscreenElement;
  const player = document.getElementById('player');
  return anchor?.closest<HTMLDialogElement>('dialog[open]') ??
    anchor?.closest<HTMLElement>('[data-launcher-transient-host]') ??
    (player && fullscreen instanceof HTMLElement && player.contains(fullscreen) ? fullscreen : null) ??
    anchor?.closest<HTMLElement>('#player') ?? document.body;
}
