// Project only the browser game-audio surface. Normal Replay/save/practice UI shares the
// source file but has no owner in the co-op runtime; never copy its dynamic import closure.
export function projectCoopAudio(source,parse){const ast=parse(source,{sourceType:'module',ecmaVersion:'latest'}),records=ast.body.slice(0,6);
 const want=['StageMusic','directSoundPanGains','SoundIdentityHost','GameSoundQueue'];
 if(records.length!==6||records[0].type!=='ImportDeclaration'||records[0].source.value!=='../resource-fetch.mjs'||records[1].type!=='ImportDeclaration'||records[1].source.value!=='./music-output.mjs'||
    records.slice(2).some((v,i)=>(v.declaration??v).id?.name!==want[i])||ast.body[6]?.declarations?.[0]?.id?.name!=='keyBits')throw Error('Original game audio surface changed');
 return source.slice(0,records[5].end)+'\n';}
