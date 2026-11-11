export function gameVersion(language='cn'){
  if(language!=='cn'&&language!=='jp')throw Error('游戏语言无效，请重新选择汉化或日文版本');
  const suffix=language==='jp'?'-jp':'';
  return {language,label:language==='jp'?'日文':'汉化',
    soloDisk:`th04-solo${suffix}.hdi.gz`,soloPatch:`solo-patch${suffix}.json`,
    coopDisk:`th04-coop${suffix}.hdi.gz`,coopPatch:`patch${suffix}.json`,coopMeta:`disk${suffix}.json`};
}

// Standalone modes remember their own choice. Online language is room-owned.
export function rememberLanguage(select,mode){
  const key=`th04.language.${mode}`;
  try{const value=localStorage.getItem(key);if(value==='cn'||value==='jp')select.value=value;}catch{}
  select.addEventListener('change',()=>{try{localStorage.setItem(key,select.value);}catch{}});
}
