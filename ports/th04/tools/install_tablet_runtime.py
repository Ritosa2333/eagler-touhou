"""Install the verified 2026-10-15 core; update both distribution manifests."""
from pathlib import Path
import hashlib
import json

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / 'build/native-scheduler/runtime-tablet'
EXPECTED = {
    'dosbox-rollback.mjs': 'b86ffabafe90b43c0e12745ebd2d03e0da3353a53f765ad50a2008d12e291e8e',
    'dosbox-rollback-tracked.mjs': 'f7224bcb7e5ea881525760f3b71b194d01e127acf31103a5400a06d9d004d048',
    'wdosbox-x-rollback.wasm': '318af02f807f494613083c444a35c1abcde2823a9ad2c81a160e785d8d7d3948',
    'wdosbox-x-rollback-tracked.wasm': '075a3dead5f49cf3a38b7d382fd0460ac7ad23935cec0f55db0d4bb4ea5c848d',
    'snapshot-diff-tracked.wasm': 'fb69ccf0818caa9199ac3f357435528622413b690af90a353b99c88d81adb5c2',
}
def sha(data):
    return hashlib.sha256(data).hexdigest()

def main():
    assets = {name: (SOURCE / name).read_bytes() for name in EXPECTED}
    for name, data in assets.items():
        if sha(data) != EXPECTED[name]:
            raise SystemExit('Frozen candidate mismatch: ' + name)
    adapter_path = ROOT / 'web/dosbox-adapter.js'
    adapter = adapter_path.read_text(encoding='utf-8')
    flags = 'nativeScheduling:true,nativeCpu:true,nativeFramePresentation:false,'
    if flags not in adapter:
        if adapter.count('deterministic:net,') != 1:
            raise SystemExit('Adapter anchor changed')
        adapter = adapter.replace('deterministic:net,', 'deterministic:net,' + flags)
    adapter_bytes = adapter.encode('utf-8')
    vendor_path = ROOT / 'web/vendor/dosbox-x/runtime.json'
    network_path = ROOT / 'web/netplay/runtime.json'
    vendor = json.loads(vendor_path.read_text(encoding='utf-8'))
    network = json.loads(network_path.read_text(encoding='utf-8'))
    continuation = 'Asyncify boundary with native frame scheduling and guarded CPU'
    network.update(adapter='dosbox-x-native-tablet-v1',
        wasm_sha256=EXPECTED['wdosbox-x-rollback.wasm'],
        mobile_wasm_sha256=EXPECTED['wdosbox-x-rollback.wasm'],
        generated_js_sha256=EXPECTED['dosbox-rollback.mjs'],
        dosbox_adapter_sha256=sha(adapter_bytes), clock_sha256=sha(adapter_bytes))
    network['rollback_wasm'].update(sha256=EXPECTED['wdosbox-x-rollback.wasm'], continuation=continuation,
        source_sha256=sha((ROOT / 'build/native-scheduler/out-native/wdosbox-x-jspi.wasm').read_bytes()))
    network['write_journal'].update(wasm_sha256=EXPECTED['wdosbox-x-rollback-tracked.wasm'],
        glue_sha256=EXPECTED['dosbox-rollback-tracked.mjs'], diff_sha256=EXPECTED['snapshot-diff-tracked.wasm'])
    vendor['files'].update(EXPECTED)
    vendor['files']['NOTICE.md'] = sha((ROOT / 'web/vendor/dosbox-x/NOTICE.md').read_bytes())
    vendor['network'].update(continuation=continuation, nativeFrameBridge=False,
        optimizations=['guarded synchronous CPU', 'TH04 guarded idle fusion', 'word memcmp', 'PC98 planar lookup'],
        sourceRevision='788fde99d3d451eab3621009186e5abe516f2b9c')
    for name, data in assets.items():
        (ROOT / 'web/vendor/dosbox-x' / name).write_bytes(data)
    adapter_path.write_bytes(adapter_bytes)
    for path, value in [(vendor_path, vendor), (network_path, network)]:
        path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print('Installed verified tablet core and refreshed runtime identities')

if __name__ == '__main__':
    main()
