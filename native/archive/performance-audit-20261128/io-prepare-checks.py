from pathlib import Path
import json,zipfile,hashlib,shutil

native=Path('C:/Users/Ritosa/AppData/Local/Packages/OpenAI.Codex_2p2nqsd0c76g0/LocalCache/Local/eagler-account-api-routing/native')
stage=Path('D:/workspace/eagler/artifacts/.native-room-client-win')
task=Path(__file__).parent
shutil.copy2(native/'lib/game_resources.dart',stage/'lib/game_resources.dart')
source=(native/'tools/verify_game_data_test.dart').read_text()
source=source.replace("'th06'","'th07'")
assert source.count("'game': 'th07'")==1
source=source.replace("'game': 'th07'","'game': 'th08'")
(stage/'tools/verify_th07_io_cost_test.dart').write_text(source)
(task/'verify_th07_io_cost_test.dart').write_text(source)
ui_source=(native/'test/game_import_test.dart').read_text().replace('th06','th07')
ui_source=ui_source.replace('attempt < 500','attempt < 1500')
(stage/'test/game_import_th07_io_cost_test.dart').write_text(ui_source)
(task/'game_import_th07_io_cost_test.dart').write_text(ui_source)
game=next(g for g in json.loads((native/'assets/game_catalog.json').read_text()) if g['id']=='th07')
data=game['data']
input_file=Path('D:/workspace/eagler/artifacts/launcher-ui-pencil-20261122/return-rail-validation/release-cbf99745/hosted-rehearsal/games/th07/th07.data')
assert input_file.stat().st_size==data['bytes']
assert hashlib.sha256(input_file.read_bytes()).hexdigest()==data['sha256']
manifest={'schema':'eagler-touhou/package/1','game':'th07','runtimeRequirement':{'protocol':'eagler-touhou/1','target':'th07','dataFile':data['name']},'files':{data['name']:{'source':data['name'],'target':'/'+data['name'],'bytes':data['bytes'],'sha256':data['sha256']}}}
with zipfile.ZipFile(task/'th07-reference.zip','w',compression=zipfile.ZIP_STORED) as z:
    z.writestr('package.json',json.dumps(manifest))
    z.write(input_file,data['name'])
record={'scope':'Existing release th07 DATA, independently matched to bundled published descriptor. Temporary ZIP fixture wraps the same real bytes in the existing package schema; it is not a separately published full release. Existing import verifier and widget entry fixtures use th07 with a distinct th08 wrong-game sample. Widget wait deadline scales for the larger real input and is not a performance assertion. No downloads or changes to the original file.','data':data,'zipSha256':hashlib.sha256((task/'th07-reference.zip').read_bytes()).hexdigest(),'zipBytes':(task/'th07-reference.zip').stat().st_size,'sourceSha256':hashlib.sha256((native/'lib/game_resources.dart').read_bytes()).hexdigest(),'verifierSha256':hashlib.sha256(source.encode()).hexdigest(),'uiTestSha256':hashlib.sha256(ui_source.encode()).hexdigest()}
(task/'inputs.json').write_text(json.dumps(record,indent=2)+'\n')
print(json.dumps(record))
