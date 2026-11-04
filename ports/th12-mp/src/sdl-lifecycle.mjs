// Native initialization and a launched multiplayer run have separate lifetimes.
export function sdlStopError(code,reason,details,{started=false,lastFailure='',logs=[]}={}){
  if(code===0&&started)return null;
  const message=reason||lastFailure||`SDL Runtime ${started?'异常停止':'未完成原生初始化'}（退出码 ${code}，来源 ${details?.source??'unknown'}）`;
  return new Error(message,{cause:{exitCode:code,started,nativeStop:details??null,recentLog:logs.slice(-40)}});
}
