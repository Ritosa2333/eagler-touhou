import {normalizeReactMountPath, validateReactAppShellDeployment, type ReactAppShellDeployment} from '../../app/services/app-shell-deployment.ts';

/** Normal builds include offline loading on their own origin and mount. */
export function reactDeploymentConfig(environment: NodeJS.ProcessEnv = process.env) {
  const mountPath = normalizeReactMountPath(environment.EAGLER_REACT_MOUNT_PATH || '/');
  const outputDirectory = environment.EAGLER_REACT_BUILD_DIRECTORY || '.cache/build/ui-rewrite';
  const mode = environment.EAGLER_REACT_APP_SHELL || 'enabled';
  if (!['enabled', 'isolated', 'disabled'].includes(mode)) throw new Error('EAGLER_REACT_APP_SHELL must be enabled, isolated or disabled');
  let appShell: ReactAppShellDeployment | null = null;
  if (mode === 'disabled') {
    if (environment.EAGLER_REACT_APP_SHELL_ORIGIN || environment.EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN) throw new Error('Disabled App Shell cannot declare an origin');
  } else if (environment.EAGLER_REACT_APP_SHELL_ORIGIN || mode === 'isolated') {
    if (!environment.EAGLER_REACT_APP_SHELL_ORIGIN) throw new Error('Isolated React App Shell requires EAGLER_REACT_APP_SHELL_ORIGIN');
    if (environment.EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN && environment.EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN !== 'true') throw new Error('EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN must be true');
    appShell = validateReactAppShellDeployment({schema: 'eagler-touhou/react-app-shell/1', origin: environment.EAGLER_REACT_APP_SHELL_ORIGIN, mountPath,
      ...(environment.EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN === 'true' ? {isolatedOrigin: true} : {})});
  } else {
    if (environment.EAGLER_REACT_APP_SHELL_ISOLATED_ORIGIN) throw new Error('An isolated origin declaration requires EAGLER_REACT_APP_SHELL_ORIGIN');
    appShell = validateReactAppShellDeployment({schema: 'eagler-touhou/react-app-shell/1', origin: 'same-origin', mountPath});
  }
  return Object.freeze({mountPath, outputDirectory, appShell});
}
