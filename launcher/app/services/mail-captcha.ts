import {linkAbortSignals} from './abort-signals';
import './mail-captcha.css';

type TacResponse = {code?: number; data?: {id?: string}};
type TacConfig = {
  bindEl: string;
  requestCaptchaDataUrl: string;
  validCaptchaUrl: string;
  validSuccess(response: TacResponse): void;
  btnCloseFun(): void;
};
type TacTransport = {
  doSendRequest(request: {url: string; data: unknown}): Promise<unknown>;
};
type TacWidget = {init(): void; destroyWindow(): void};
type TacSdk = {
  TAC: new (config: TacTransport, style: object) => TacWidget;
  CaptchaConfig: new (config: TacConfig) => TacTransport;
};

let sdkLoad: Promise<TacSdk> | undefined;
let opening = false;

function loadSdk(prefix: string) {
  sdkLoad ??= new Promise<TacSdk>((resolve, reject) => {
    const script = document.createElement('script');
    script.src = `${prefix}/tac/js/tac.min.js`;
    script.crossOrigin = 'anonymous';
    const css = document.createElement('link');
    css.rel = 'stylesheet';
    css.href = `${prefix}/tac/css/tac.css`;
    css.crossOrigin = 'anonymous';
    const fail = () => {
      clearTimeout(timeout);
      script.remove();
      css.remove();
      sdkLoad = undefined;
      reject(new Error('Captcha SDK unavailable'));
    };
    let pending = 2;
    const loaded = () => {
      if (--pending) return;
      const sdk = window as Window & Partial<TacSdk>;
      if (!sdk.TAC || !sdk.CaptchaConfig) return fail();
      clearTimeout(timeout);
      resolve({TAC: sdk.TAC, CaptchaConfig: sdk.CaptchaConfig});
    };
    const timeout = setTimeout(fail, 10000);
    script.onload = css.onload = loaded;
    script.onerror = css.onerror = fail;
    document.head.append(css, script);
  });
  return sdkLoad;
}

export async function openMailCaptcha(origin: string, signal?: AbortSignal): Promise<string> {
  signal?.throwIfAborted();
  if (opening) throw new DOMException('Verification already open', 'AbortError');
  opening = true;
  const prefix = `${origin}/api/account/mail-captcha`;
  const dialog = document.createElement('dialog');
  dialog.className = 'mail-captcha-dialog';
  dialog.setAttribute('aria-label', 'CAPTCHA');
  const box = document.createElement('div');
  box.id = 'mail-captcha-widget';
  dialog.append(box);
  const lifetime = new AbortController();
  let widget: TacWidget | undefined;
  let cancel: (() => void) | undefined;
  try {
    return await new Promise<string>((resolve, reject) => {
      const cancelDialog = () => {
        lifetime.abort();
        reject(new DOMException('Verification cancelled', 'AbortError'));
      };
      cancel = cancelDialog;
      signal?.addEventListener('abort', cancelDialog, {once: true});
      dialog.addEventListener('cancel', cancelDialog, {once: true});
      void loadSdk(prefix).then(({TAC, CaptchaConfig}) => {
        lifetime.signal.throwIfAborted();
        document.body.append(dialog);
        dialog.showModal();
        const config = new CaptchaConfig({
          bindEl: '#mail-captcha-widget',
          requestCaptchaDataUrl: `${prefix}/challenge`,
          validCaptchaUrl: `${prefix}/check`,
          validSuccess({data}) {
            if (typeof data?.id === 'string' && data.id) resolve(data.id);
            else reject(new Error('Captcha token missing'));
          },
          btnCloseFun: cancelDialog,
        });
        config.doSendRequest = async ({url, data}) => {
          const request = linkAbortSignals(lifetime.signal, AbortSignal.timeout(8000));
          try {
            const response = await fetch(url, {
              method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(data),
              signal: request.signal,
            });
            if (!response.ok) throw new Error('Captcha request failed');
            return await response.json();
          } catch (error) {
            reject(error);
            // The SDK should stop updating its view once this dialog closes.
            return new Promise<never>(() => {});
          } finally {
            request.dispose();
          }
        };
        widget = new TAC(config, {});
        widget.init();
      }).catch(reject);
    });
  } finally {
    lifetime.abort();
    if (cancel) signal?.removeEventListener('abort', cancel);
    dialog.close();
    try {
      widget?.destroyWindow();
    } finally {
      dialog.remove();
      opening = false;
    }
  }
}
