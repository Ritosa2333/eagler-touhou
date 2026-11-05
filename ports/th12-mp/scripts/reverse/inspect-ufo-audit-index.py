"""Read-only UFO/Items/Enemy audit inventory over genuine decompiler outputs.

File presence is decompiler coverage, not verified behavior. Explicit indirect
callback edges and manual verification levels are separate from CFG discovery.
"""
import hashlib,json,pathlib
TITLE=pathlib.Path(__file__).resolve().parents[2];EXE=TITLE.parent/'th12/th12.exe'
SHA='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256(EXE.read_bytes()).hexdigest()!=SHA:raise SystemExit('Wrong original EXE; refusing fixed entries')
base=TITLE/'local/reverse/th12-1.00b';source=base/'full-reviewed/functions.json'
if not source.exists():source=base/'full/functions.json'
if not source.exists():source=base/'functions.json'
functions={(int(x['address'],16) if isinstance(x['address'],str) else x['address']):x for x in json.loads(source.read_text())['functions']}
targets={0x425b10:'item_manager_construct',0x425c20:'item_manager_tick',0x4270b0:'ufo_token_collect',0x4271c0:'ufo_item_absorb_gate',0x4273f0:'item_spawn',0x422e80:'ufo_inventory_add',0x4214b0:'ufo_inventory_hud_update',0x421690:'ufo_inventory_blink',0x421a60:'ufo_inventory_rotate',0x41d560:'hud_vm_initialize',0x449f20:'boss_slots_any',0x449f50:'ufo_manager_reset',0x449f80:'ufo_manager_construct',0x449fa0:'ufo_manager_register',0x44a0f0:'ufo_manager_unregister_flag',0x44a120:'ufo_manager_destruct',0x44a230:'ufo_manager_create',0x44a290:'ufo_manager_destroy',0x44a2b0:'ufo_manager_delete',0x44a2d0:'ufo_manager_tick',0x44a4f0:'ufo_manager_draw',0x44a5d0:'ufo_manager_draw_aux',0x44a860:'ufo_tick_callback',0x44a870:'ufo_draw_callback',0x44a880:'ufo_draw_aux_callback',0x44a890:'ufo_death_callback',0x44a8a0:'ufo_spawn',0x44ab60:'ufo_relative_angle_candidate',0x44ac00:'ufo_target_angle',0x44aca0:'ufo_fill_reward',0x44af00:'ufo_finish_reward',0x412990:'enemy_spawn',0x413230:'enemy_construct',0x413190:'enemy_manager_tick',0x413840:'enemy_tick',0x414af0:'enemy_drop_and_death_callback',0x414c60:'enemy_global_clear',0x412140:'enemy_scattered_item_drop',0x41a6f0:'enemy_interrupt_check',0x412780:'spell_pending_bomb',0x412690:'player_invulnerability_or_flag'}
rows=[]
for address,name in targets.items():
 matches=list((base/'pseudocode').glob(f'{address:08x}_*.c'))+list((base/'full/pseudocode').glob(f'{address:08x}.c'))+list((base/'full-reviewed/pseudocode').glob(f'{address:08x}.c'))
 outputs=[]
 for path in matches:
  text=path.read_text(encoding='utf-8');outputs.append({'path':str(path.relative_to(TITLE)),'bytes':path.stat().st_size,'unsupportedInstructionMarkers':text.count('unsupported instruction'),'nanMarkers':text.count('nan'),'annotated':'.annotated.' in path.name})
 cfg=functions.get(address)
 rows.append({'address':hex(address),'researchName':name,'nameStatus':'manual research label, not retail source symbol','cfg':cfg,'pseudocodeFiles':outputs,'verification':'see FULL-UFO-AUDIT.md; file presence alone does not establish a verified ABI or behavior'})
result={'schema':'th12-full-ufo-audit-index/1','originalExeSha256':SHA,'cfgIndex':str(source.relative_to(TITLE)),'scope':'UFO/Items/Enemy related entry inventory, not full-game decompilation completion','originalSourceRecovered':False,'entries':rows,'manuallyVerifiedIndirectEdges':[{'site':'0x44a92f','field':'Enemy+0x103c','target':'0x44a890'},{'site':'0x414c4c','field':'Enemy+0x103c','dispatches':'0x44a890 for summoned UFOs'},{'site':'0x468be3','field':'ECL context vtable high-op dispatch','target':'0x4154d0'},{'site':'0x44a410','field':'sound54 buffer vtable+0x40','purpose':'audio pan, host output boundary'}]}
out=TITLE/'artifacts/reverse/full-ufo-audit-index.json';out.write_text(json.dumps(result,indent=2)+'\n');print(out)
