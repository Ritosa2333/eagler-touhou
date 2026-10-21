/** Parse only diagnostics emitted by TH08's localization activation and font fallback owners. */
export function classifyTh08LanguageDiagnostic({text,fontPaths=[]}={}){
  if(typeof text!=='string')return null;
  const activation=text.match(/^th08 thcrap localization active: font=(.+?) \((.+)\) utf8=(ok|FAILED) spells=(\d+) stages=(\d+) themes=(\d+) musiccmt=(\d+) spellcomments=(\d+) ascii=(\d+) strings=(\d+)$/);
  if(activation){
    const [,font,fontName,utf8,...counts]=activation;
    if(!font.trim()||!fontName.trim())return null;
    const diagnostic={font,fontName,utf8,counts:Object.fromEntries(['spells','stages','themes','musicComments','spellComments','ascii','strings']
      .map((name,index)=>[name,Number(counts[index])]))};
    return{classification:'localization-activation',disposition:utf8==='ok'?'expected':'blocking',diagnostic};
  }
  const fallback=text.match(/^th08 thcrap font fallback: U\+([0-9A-F]{4}) from pack font (\/thcrap\/th08\/fonts\/[A-Za-z0-9_.-]+\.(?:otf|ttf|ttc))$/);
  if(!fallback)return null;
  const codepoint=Number.parseInt(fallback[1],16),font=fallback[2];
  if(codepoint<32||(codepoint>=0xd800&&codepoint<=0xdfff)||!Array.isArray(fontPaths)||!fontPaths.includes(font))
    return{classification:'pack-font-fallback',disposition:'blocking',codepoint,font};
  return{classification:'pack-font-fallback',disposition:'expected',codepoint,font};
}
