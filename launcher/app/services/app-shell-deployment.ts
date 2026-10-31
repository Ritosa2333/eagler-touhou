/** Explicit artifact configuration, never inferred from a query or a parent SW. */
import {
  normalizeReactMountPath,
  REACT_APP_SHELL_META,
  validateReactAppShellDeployment as validateDeployment,
} from '../../lib/react-app-shell-deployment.mjs';

export { normalizeReactMountPath, REACT_APP_SHELL_META };
export interface ReactAppShellDeployment {
  schema: 'eagler-touhou/react-app-shell/1';
  origin: string;
  mountPath: string;
  isolatedOrigin?: boolean;
}

export function validateReactAppShellDeployment(value: unknown): ReactAppShellDeployment {
  return validateDeployment(value);
}

export function readReactAppShellDeployment(
  document: Pick<Document, 'querySelectorAll'>,
  location: Pick<Location, 'href'>,
) {
  try {
    const metadata = document.querySelectorAll(`meta[name="${REACT_APP_SHELL_META}"]`);
    if (metadata.length !== 1) return undefined;
    const configuration = validateReactAppShellDeployment(JSON.parse(metadata[0].getAttribute('content') ?? 'null'));
    const page = new URL(location.href);
    const loopback = ['localhost', '127.0.0.1', '[::1]'].includes(page.hostname);
    if (page.protocol !== 'https:' && !(page.protocol === 'http:' && loopback)) return undefined;
    const scope = new URL(
      configuration.mountPath,
      configuration.origin === 'same-origin' ? page.origin : configuration.origin,
    );
    if (page.origin !== scope.origin || new URL('./', page).href !== scope.href) return undefined;
    return Object.freeze({
      workerUrl: new URL('app-shell-sw.js', scope).href,
      scope: scope.href,
      previousWorkerUrls: [new URL('app-shell-sw-fontfix-1fefcaa790f0.js', scope).href],
    });
  } catch {
    return undefined;
  }
}
