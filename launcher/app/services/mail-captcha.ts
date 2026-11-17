type CaptchaSdk = {
  open(options: {appId: string}): Promise<string>;
  close(): void;
  isCancelled(error: unknown): boolean;
};

const sdkUrl = 'https://cv.naids.com/sdk/load.min.js';
let sdkLoad: Promise<CaptchaSdk> | undefined;
let opening = false;

function loadSdk() {
  sdkLoad ??= new Promise<CaptchaSdk>((resolve, reject) => {
    const script = document.createElement('script');
    script.src = sdkUrl;
    script.dataset.preload = 'false';
    const fail = () => {
      clearTimeout(timeout);
      script.remove();
      sdkLoad = undefined;
      reject(new Error('Captcha SDK unavailable'));
    };
    const timeout = setTimeout(fail, 10000);
    script.onerror = fail;
    script.onload = () => {
      const sdk = (window as Window & {CaptchaSDK?: CaptchaSdk}).CaptchaSDK;
      if (!sdk || typeof sdk.open !== 'function' || typeof sdk.close !== 'function' || typeof sdk.isCancelled !== 'function') {
        fail();
        return;
      }
      clearTimeout(timeout);
      resolve(sdk);
    };
    document.head.append(script);
  });
  return sdkLoad;
}

export async function openMailCaptcha(appId: string, signal?: AbortSignal) {
  signal?.throwIfAborted();
  if (opening) throw new DOMException('Verification already open', 'AbortError');
  opening = true;
  try {
    const sdk = await loadSdk();
    signal?.throwIfAborted();
    const close = () => sdk.close();
    signal?.addEventListener('abort', close, {once: true});
    try {
      const token = await sdk.open({appId});
      signal?.throwIfAborted();
      if (typeof token !== 'string' || !token.trim() || token.length > 4096) throw new Error('Captcha token missing');
      return token;
    } catch (error) {
      if (sdk.isCancelled(error)) throw new DOMException('Verification cancelled', 'AbortError');
      throw error;
    } finally {
      signal?.removeEventListener('abort', close);
    }
  } finally {
    opening = false;
  }
}
