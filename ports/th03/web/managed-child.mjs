import {installManagedWorkers} from './managed-data.mjs';
const content = parent.__eaglerPC98Content;
if (!content) throw Error('请从主项目加载游戏');
const base = new URL('.', parent.location.href).href;
content.install(globalThis, base);
installManagedWorkers(globalThis, content, base);
