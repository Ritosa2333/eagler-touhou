export const nativeLanguage=language=>({ja:0,lang_en:1,'lang_zh-hans':2,'lang_zh-hant':3}[language]??0);
export const nativeMusic=(music,options)=>music==='none'?0:options.musicSource==='akyu'?3:1;
export function nativeSettings({options,music,language}){
  return {language:nativeLanguage(language),music:nativeMusic(music,options),alwaysHitbox:options.alwaysHitbox===true,
    localPlayerGuide:options.multiplayerLocalPlayerVisibility===true};
}
