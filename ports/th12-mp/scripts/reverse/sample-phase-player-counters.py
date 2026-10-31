"""Local original Player16/Bomb constructor checks; no replay capture."""
import gc, hashlib, json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf8').split('SPAWN=0x2070000;')[0]
rows = []
for loadout, name in enumerate(['reimu-a', 'reimu-b', 'marisa-a', 'marisa-b', 'sanae-a', 'sanae-b']):
 for mode in ['miss', 'deathbomb', 'bomb', 'empty-bomb']:
  saved = sys.argv
  sys.argv = [str(PROVIDER), '--frames', '0', '--native-player', '--native-items', '--native-bullets',
   '--native-lasers', '--active-collision', '--instruction-budget', '20000000', '--input',
   str(ROOT / f'artifacts/reverse/enemy-input-bomb-lifecycle-{name}.json')]
  ns = {'__file__': str(PROVIDER), '__name__': 'phase_counters_original'}
  try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
  finally: sys.argv = saved
  from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESI
  v, put, get, call = (ns[k] for k in ['v', 'put', 'get', 'call'])
  player, manager = ns['PLAYER'], ns['MANAGER']
  put(manager + 0x70, 'i', 1); put(manager + 0x10, '3I', 0, 0, 1)
  stock = int(mode != 'empty-bomb')
  put(0x4b0c98, '4i', 4, 2, stock, 2); put(0x4b0ccc, 'i', -300)
  put(0x4b0c48, 'i', 100); put(player + 0xa48, 'i', 120)
  put(player + 0x988, '2i', 111, 54144); put(player + 0x97c, '3f', 111 / 128, 423, 0)
  put(player + 0xc404, 'i', 0)
  def capture(): return list(get(manager + 0x10, '3I'))
  frames = [capture()]
  collision = mode in ['miss', 'deathbomb']
  if collision:
   v.reg_write(UC_X86_REG_ESI, player); call(0x438370)
   frames.append(capture())
  if mode == 'deathbomb':
   v.reg_write(UC_X86_REG_EAX, player + 0xa30); call(0x4067e0, 7)
  for tick in range(10):
   put(0x4d49d0, 'I', int(mode != 'miss' and tick == 0) * 2)
   call(0x436ba0, player); frames.append(capture())
  rows.append({'loadout': loadout, 'mode': mode, 'stock': stock, 'collision': collision, 'frames': frames})
  gc.collect()
 print(json.dumps({'loadout': loadout, 'cases': len(rows)}), flush=True)
out = ROOT / 'tests/fixtures/phase-player-counters-original.json'
out.write_text(json.dumps({'originalExeSha256': ns['SHA'],
 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'scope': 'Controlled Player16 inputs, actual438370/436ba0 and six original Bomb constructors; '
 'EnemyManager+10/+14/+18 observed. Ten ticks per case; no long replay, GPU or whole-game oracle. '
 'Provider resource/heap/audio boundaries retained; no original instruction or RNG replaced.',
 'rows': rows}, indent=2) + '\n', encoding='utf8')
print(json.dumps({'output': str(out), 'cases': len(rows)}))
