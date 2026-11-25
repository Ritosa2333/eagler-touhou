import darkWordmark from '../../../public/assets/launcher-wordmark-dark.png?url';
import lightWordmark from '../../../public/assets/launcher-wordmark-light.png?url';

export function LauncherWordmark() {
  return <span className="launcher-wordmark" aria-hidden="true">
    <img src={darkWordmark} width="98" height="29" alt=""/>
    <img src={lightWordmark} width="98" height="29" alt=""/>
  </span>;
}
