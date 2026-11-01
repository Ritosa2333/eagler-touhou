/** Keep the Runtime's real controls/listeners, but lay them out in the Host's
 * information column. Only spectator-scoped styles cross the document boundary. */
export function mountSpectatorPanel(
  frame: HTMLIFrameElement,
  host: HTMLElement,
  changed: (score: HTMLElement | null) => void,
) {
  let observer: MutationObserver | null = null;
  let nativeBody: HTMLElement | null = null;
  let panel: HTMLElement | null = null;
  const styles = document.createElement('style');
  styles.dataset.spectatorPanelStyles = '';
  document.head.append(styles);
  function scopedRules(rules: CSSRuleList): string {
    return Array.from(rules)
      .map((rule) => {
        if (rule.type === CSSRule.STYLE_RULE)
          return (rule as CSSStyleRule).selectorText.split(',').every((selector) => /spectator/i.test(selector))
            ? rule.cssText
            : '';
        if (rule.type === CSSRule.MEDIA_RULE) {
          const media = rule as CSSMediaRule;
          const body = scopedRules(media.cssRules);
          return body ? `@media ${media.conditionText}{${body}}` : '';
        }
        return '';
      })
      .join('\n');
  }
  function find() {
    const next =
      host.querySelector<HTMLElement>('#spectatorTimeline') ??
      nativeBody?.querySelector<HTMLElement>('#spectatorTimeline') ??
      null;
    if (next && next !== panel) {
      panel = next;
      styles.textContent = Array.from(nativeBody!.ownerDocument.styleSheets)
        .map((sheet) => {
          try {
            return scopedRules(sheet.cssRules);
          } catch {
            return '';
          }
        })
        .join('\n');
      host.append(next);
    }
    changed(next?.querySelector<HTMLElement>('#spectatorScore') ?? null);
  }
  function bind() {
    observer?.disconnect();
    if (panel?.parentElement === host) panel.remove();
    panel = null;
    try {
      nativeBody = frame.contentDocument?.body ?? null;
    } catch {
      nativeBody = null;
    }
    observer = new MutationObserver(find);
    if (nativeBody) observer.observe(nativeBody, { childList: true });
    observer.observe(host, { childList: true });
    find();
  }
  frame.addEventListener('load', bind);
  bind();
  return () => {
    observer?.disconnect();
    frame.removeEventListener('load', bind);
    // React can retire the Host before the still-live Runtime closes itself.
    if (panel?.parentElement === host && nativeBody?.isConnected) nativeBody.append(panel);
    styles.remove();
  };
}
