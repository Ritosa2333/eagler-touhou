"""Read-only TH12 retail combat index. Static facts, never a game golden.

Opcode entries are basic blocks within 4154d0, not independent functions.
CFG boundaries/call edges remain machine inference unless separately reviewed.
"""
import collections, hashlib, json, pathlib, struct, sys

TITLE = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(TITLE.parent / 'tools/python'))
import capstone, pefile

EXE = TITLE.parent / 'th12/th12.exe'
SHA = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
assert hashlib.sha256(EXE.read_bytes()).hexdigest() == SHA
PE = pefile.PE(str(EXE))
BASE = PE.OPTIONAL_HEADER.ImageBase
CS = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
def data(a, n): return PE.get_data(a - BASE, n)
def words(a, n): return list(struct.unpack('<' + 'I' * n, data(a, n * 4)))

NAMES = {
 0x4359a0:'player_constructor', 0x435ae0:'player_register', 0x436100:'player_initialize',
 0x4364f0:'player_movement', 0x436ba0:'player_update', 0x437660:'player_tick_wrapper',
 0x437680:'sht_load_and_install_callbacks',
 0x437810:'player_rect_collision', 0x437980:'player_round_collision',
 0x437a80:'player_line_collision', 0x437d00:'player_curve_collision',
 0x4381e0:'player_confirm_miss', 0x438370:'player_collision_state',
 0x4385b0:'player_resource_reset', 0x4390f0:'damage_source_circle_create',
 0x439630:'friendly_create', 0x4399d0:'friendly_spawn_callback_dispatch',
 0x439a40:'player_fire', 0x439b10:'friendly_move_and_anchor', 0x439ed0:'friendly_damage_query',
 0x43a680:'marisa_laser_sound_start',0x43a6b0:'marisa_laser_sound_pan',
 0x43a810:'reimu_a_friendly_update',
 0x43a950:'sanae_b_missile_extra',0x43aa50:'sanae_b_particle_update',
 0x406b20:'bomb_allocate',0x406bb0:'bomb_tick_callback',0x406ce0:'bomb_update_dispatch',
 0x406de0:'bomb_damage_dispatch',0x406bf0:'bomb_start_dispatch',
 0x406880:'bomb_register',0x406fd0:'bomb_spell_loss_and_invulnerability',
 0x407010:'marisa_a_bomb_start',0x4072d0:'marisa_a_bomb_update',
 0x4073b0:'marisa_a_bomb_damage',0x407580:'marisa_a_bomb_clear',
 0x407780:'marisa_b_bomb_start',0x407950:'marisa_b_bomb_update',
 0x4079d0:'marisa_b_bomb_damage',0x407a30:'marisa_b_bomb_clear',
 0x408120:'reimu_b_bomb_start',0x4082e0:'reimu_b_bomb_update',
 0x407ea0:'reimu_b_orb_create',0x407b80:'reimu_b_orb_update',0x408030:'reimu_b_orb_end',
 0x4089c0:'sanae_a_bomb_start',0x408b90:'sanae_a_bomb_update',
 0x408c10:'sanae_a_bomb_damage',0x408c30:'sanae_a_bomb_clear',
 0x408cb0:'sanae_b_bomb_start',0x408f00:'sanae_b_bomb_update',
 0x46a5f0:'reimu_a_bomb_start',0x46a970:'reimu_a_bomb_update',
 0x46a250:'reimu_a_emitter_create',0x46a300:'reimu_a_emitter_custom_update',
 0x469750:'reimu_a_beam_create',0x469d40:'reimu_a_beam_custom_draw',
 0x4103f0:'sanae_b_leaf_custom_update',0x4105e0:'sanae_b_leaf_create',
 0x4099e0:'hostile_bullet_update',0x40a020:'hostile_bullet_manager_tick',
 0x40aa60:'hostile_extension_dispatch',0x40b5e0:'angle_resolve',
 0x40b670:'extension_speed_bonus',0x40b6f0:'extension_vector_acceleration',
 0x40b800:'extension_20000000_acceleration',0x40b920:'extension_polar_acceleration',
 0x40b9c0:'extension_10_turn',0x40baf0:'extension_40_turn',0x40bc20:'extension_20_turn',
 0x40bef0:'extension_boundary',0x40c000:'extension_protection',
 0x40c0b0:'extension_wait',0x40c170:'extension_800000_homing',
 0x40c230:'extension_2000000_position_interpolation',0x40c3b0:'extension_8000000_translation',
 0x40c480:'extension_400_protection_geometry',0x40c8b0:'hostile_cancel',
 0x40caa0:'hostile_circle_cancel',0x40cd00:'hostile_rectangle_cancel',
 0x427ec0:'laser_base_constructor',0x427fa0:'laser_manager_constructor',0x427fd0:'laser_register',
 0x4281c0:'laser_manager_create',
 0x428270:'laser_manager_tick',0x428450:'laser_factory',
 0x428520:'laser_type0_constructor',0x428560:'laser_type1_constructor',
 0x4285b0:'laser_type2_constructor',0x4285f0:'laser_rectangle_clear',
 0x428670:'laser_all_clear_no_items',0x4286f0:'laser_circle_clear',
 0x428750:'laser_all_clear',0x428780:'laser_vertical_clear',
 0x4287c0:'laser_type0_initialize',0x428b00:'laser_type0_extension_dispatch',
 0x429150:'laser_type0_spawn_child',0x429400:'laser_type0_acceleration',
 0x429520:'laser_type0_rotation',0x429610:'laser_type0_update',
 0x429af0:'laser_type0_draw',0x429bc0:'laser_type0_clear',
 0x42a1f0:'laser_type0_circle_clear',0x42a800:'laser_type0_all_clear',
 0x42aa60:'laser_type0_point_radius_query',0x42ab80:'laser_type1_initialize',
 0x42ada0:'laser_type1_update',0x42b100:'laser_type1_draw',
 0x42b1d0:'laser_type1_rectangle_clear',0x42b750:'laser_type1_circle_clear',
 0x42bd90:'laser_type1_all_clear',0x42c080:'laser_type1_point_radius_query',
 0x42c1a0:'laser_type1_extension_dispatch',0x42c260:'laser_type2_sample_motion',
 0x42c3f0:'laser_type2_initialize',0x42c770:'laser_type2_update',
 0x42cca0:'laser_type2_draw',0x42d030:'laser_type2_extension_dispatch',
 0x42d6e0:'laser_type2_acceleration',0x42d800:'laser_type2_motion',
 0x42d890:'laser_type2_rotation',0x42d9c0:'laser_type2_rectangle_clear',
 0x42df00:'laser_type2_circle_clear',0x42e3e0:'laser_type2_all_clear',
 0x42e5b0:'laser_type2_point_radius_query',0x42e770:'laser_common_angle',
 0x42e820:'laser_bounds',0x42e880:'laser_common_float',
 0x4128e0:'laser_type1_parameters_reset',0x412900:'laser_target_set',
 0x412960:'laser_find_id',0x4154d0:'enemy_high_opcode_dispatch',
 0x41a900:'ecl_integer_argument',0x41a950:'ecl_float_argument',
 0x4391c0:'player_graze_line',0x444890:'ufo_enemy_death_callback',
 0x412690:'player_invulnerable_query',0x4126b0:'player_flag4_query',
 0x412780:'spell_active_pending_bomb_query',
 0x41aa70:'ecl529_callback1',0x41b5f0:'ecl529_callback2',
 0x41b740:'ecl529_callback3',0x41bb40:'ecl529_callback4',
 0x41bd20:'ecl529_callback5',0x41bf50:'ecl529_callback6',
 0x41aba0:'ecl530_callback1',0x41b4d0:'ecl531_callback1',
}

def ecl_records(p):
 b=p.read_bytes(); count=struct.unpack_from('<H',b,16)[0]
 table=36+struct.unpack_from('<H',b,6)[0]
 offsets=struct.unpack_from('<'+'I'*count,b,table); names=[]; q=table+count*4
 for _ in range(count):
  end=b.index(0,q);names.append(b[q:end].decode('shift_jis'));q=end+1
 rows=[]
 for k,(start,name) in enumerate(zip(offsets,names)):
  pos=start+16; end=offsets[k+1] if k+1<count else len(b)
  while pos+16<=end:
   time,op,size,mask,diff,argc,pop=struct.unpack_from('<iHHHBBI',b,pos)
   if size<16 or pos+size>end: raise ValueError((p.name,name,hex(pos),size))
   raw=b[pos+16:pos+size]
   rows.append({'routine':name,'offset':pos,'time':time,'opcode':op,
                'difficultyMask':diff,'argumentCount':argc,'parameterMask':mask,
                'rawWords':list(struct.unpack('<'+'I'*(len(raw)//4),raw[:len(raw)//4*4]))})
   pos+=size
 return rows

high_idx=data(0x419890,356); high=words(0x419654,143)
dispatch={op:high[high_idx[op-256]] for op in range(256,612)}
OUT=TITLE/'artifacts/reverse/combat';OUT.mkdir(parents=True,exist_ok=True)
selected=list(range(500,536))+list(range(600,612))
oprows=[]
for op in selected:
 a=dispatch[op]; end=min([v for v in dispatch.values() if v>a]+[0x419654])
 ins=list(CS.disasm(data(a,end-a),a))
 asm='\n'.join(f'{i.address:08x} {i.mnemonic:10} {i.op_str}' for i in ins)+'\n'
 (OUT/f'ecl-{op}.asm').write_text(asm)
 oprows.append({'opcode':op,'basicBlock':hex(a),'endBound':hex(end),
   'calls':[i.op_str for i in ins if i.mnemonic=='call'],
   'warning':'Block range ends at next dispatch target, may omit shared continuation.'})

files=[];usage=collections.Counter(); references=[]
for p in sorted((TITLE/'local/retail').glob('*.ecl')):
 rows=ecl_records(p);c=collections.Counter(r['opcode'] for r in rows);usage.update(c)
 files.append({'file':p.name,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),
               'instructionCount':len(rows),'opcodeCounts':dict(sorted(c.items()))})
 references.extend({'file':p.name,**r} for r in rows if 529<=r['opcode']<=535 or r['opcode']>=600)

# Reuse the root's complete resource inventory for extension coverage. This is
# count of static literal records, not reachability or final evaluated operands.
extension_counts=collections.Counter();extension_dynamic=0
resource_index=TITLE/'local/reverse/th12-1.00b/full/resources/ecl-instructions.json'
if resource_index.exists():
 for f in json.loads(resource_index.read_text())['files']:
  for routine in f['routines']:
   for c in routine['commands']:
    if c['opcode']==509 and len(c['operandWords'])>3:
     if c['parameterMask']&8:extension_dynamic+=1
     else:extension_counts[c['operandWords'][3]]+=1

index=json.loads((TITLE/'local/reverse/th12-1.00b/functions.json').read_text())
functions=index['functions'] if isinstance(index,dict) else index
byaddr={int(f['address'],16) if isinstance(f['address'],str) else f['address']:f for f in functions}
manifest=[]
for a,name in sorted(NAMES.items()):
 f=byaddr.get(a)
 manifest.append({'address':hex(a),'researchName':name,
  'entryStatus':'manual research address; audit body/signature separately',
  'cfg':f,'cfgStatus':'machine inferred, indirect edges may be absent'})
vtables=[]
for kind,a in [(0,0x4a0654),(1,0x4a0704),(2,0x4a06ac)]:
 vtables.append({'factoryKind':kind,'address':hex(a),'entries':[hex(x) for x in words(a,10)],
   'virtualSlots':['extension dispatch','initialize','tick','draw','deleting destructor',
                   'all clear','rectangle clear','circle clear','reserved/unknown','point/radius query']})
callbacks=[]
for name,a,n in [('spawn',0x4aedf0,7),('update',0x4aebd4,6),('hit',0x4ce8ac,1),
                  ('extra',0x4aee10,2),('ecl529',0x4af0fc,7),
                  ('ecl530',0x4af118,2),('ecl531',0x4af120,2)]:
 raw=data(a,n*4)
 callbacks.append({'table':name,'address':hex(a),
                   'words':[hex(x) for x in struct.unpack('<'+'I'*n,raw)] if len(raw)==n*4 else None,
                   'warning':'Runtime BSS hit table is zero in PE image; raw-file words=null. Count is adjacent-table/retail-selector read bound, not a validated out-of-range guard.'})
result={'schema':'th12-research-full-combat-audit/1','exeSha256':SHA,
 'provider':'Static PE, Capstone, raw retail ECL, existing angr CFG index',
 'wholeGameOracle':False,'opcodeRange':{'lower':256,'upper':611,'rangeCheck':'41551e..415539',
    'outsideRangeTarget':'0x41962b','opcode700':'467170/468bd9 calls4154d0 unchanged; native probe confirms default return and PC advance'},
 'functions':manifest,'laserVtables':vtables,'callbackTables':callbacks,
 'eclDispatch':oprows,'eclFiles':files,'totalOpcodeCounts':dict(sorted(usage.items())),
 'highOpcodeReferences':references,
 'defaultCombatOpcodeRange':[op for op in range(500,612) if dispatch[op]==0x41962b],
 'extensionLiteralCounts':{hex(k):v for k,v in sorted(extension_counts.items())},
 'extensionDynamicKindRecords':extension_dynamic}
path=OUT/'index.json';path.write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'output':str(path),'functionCount':len(manifest),
 'eclFiles':len(files),'highOpcodeCounts':{op:usage[op] for op in list(range(529,532))+list(range(600,612))+[700]},
 'laserVtables':vtables},indent=2))
