import {execFile} from 'node:child_process';
import {promisify} from 'node:util';
import {fileURLToPath} from 'node:url';
import {resolve} from 'node:path';
const run=promisify(execFile);

/** Ask Windows to restore the one window owned by this BrowserServer process. */
export async function activateOwnedBrowserWindow({processId,title,platform=process.platform,execute=run}={}){
  return setOwnedBrowserWindowState({processId,title,platform,execute,state:'normal'});
}

export async function setOwnedBrowserWindowState({processId,title,state,platform=process.platform,execute=run}={}){
  if(!['normal','minimized'].includes(state))throw Error('Unsupported owned window state');
  if(platform!=='win32')return{status:'not-required',platform,source:'native CDP activation'};
  if(!Number.isSafeInteger(processId)||processId<1||typeof title!=='string'||!title.trim())
    throw Error('Native window activation needs the owned browser process and observed page title');
  const executable=resolve(process.env.SystemRoot??'C:/Windows','System32/WindowsPowerShell/v1.0/powershell.exe');
  const args=['-NoProfile','-NonInteractive','-ExecutionPolicy','Bypass','-File',
    fileURLToPath(new URL('./activate-owned-window.ps1',import.meta.url)),'-OwnerProcessId',String(processId),'-PageTitle',title,'-WindowState',state];
  const {stdout}=await execute(executable,args,{windowsHide:true,timeout:10000,maxBuffer:256*1024});
  const receipt=JSON.parse(stdout.trim().replace(/^\uFEFF/,''));
  if(receipt.processId!==processId||!['requested','incomplete'].includes(receipt.status))throw Error('Native activation returned another process identity');
  return receipt;
}
