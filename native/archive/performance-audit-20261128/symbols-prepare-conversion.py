from pathlib import Path
import json,hashlib
p=Path('D:/Temp/native-performance-20261128/symbols')
n=Path('C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native')
games=json.loads((n/'assets/game_catalog.json').read_text())
print('catalog keys',list(games[0]))
inputs=['']
for game in games:
    for key in ('title','subtitle','name','nameEn','englishName'):
        value=game.get(key)
        if isinstance(value,str) and value not in inputs:
            inputs.append(value)
assert len(inputs)>10
rows=[]
for value in inputs:
    utf8=value.encode('utf-8');utf16=value.encode('utf-16-le')
    units=[int.from_bytes(utf16[i:i+2],'little') for i in range(0,len(utf16),2)]
    byte_literal=''.join('\\x%02x'%byte for byte in utf8)
    rows.append('{std::string("'+byte_literal+'",'+str(len(utf8))+'),std::u16string{'+','.join('char16_t(0x%04x)'%unit for unit in units)+'}}')
(p/'conversion-inputs.h').write_text('const std::vector<std::pair<std::string,std::u16string>> inputs={\n'+',\n'.join(rows)+'\n};\n',encoding='utf-8')
(p/'conversion-inputs.json').write_text(json.dumps(inputs,ensure_ascii=False,indent=2),encoding='utf-8')
source=Path('D:/workspace/eagler/toolchains/flutter-stable/engine/src/flutter/fml/string_conversion.cc')
proof={'inputFile':'native/assets/game_catalog.json','inputSha256':hashlib.sha256((n/'assets/game_catalog.json').read_bytes()).hexdigest(),'strings':len(inputs),'expectedUtf16':'Python strict UTF-8 and UTF-16LE codecs','fmlSource':str(source),'fmlSourceSha256':hashlib.sha256(source.read_bytes()).hexdigest(),'scope':'Pinned FML source compiled independently with local MSVC; component comparison, not a modified production engine or whole-app performance test'}
(p/'conversion-source.json').write_text(json.dumps(proof,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(proof))
