"""Optimize dispatch for the pinned NP21 static function table.

NP21 has no dynamic linking or JS table writes. Its fixed 2244-entry table is
initialized when the instance is created; DOS RAM and rollback do not modify it.
Never apply this transformation to an unaudited upstream binary.
"""
import hashlib

SOURCE_WASM_SHA256='d64bbe39549a48686b1ed04fbadec68d8b33643d220395a9c09b8d54834f36e4'
OLD='var getWasmTableEntry=funcPtr=>wasmTable.get(funcPtr);'
NEW='var wasmTableMirror=[];var getWasmTableEntry=funcPtr=>wasmTableMirror[funcPtr]||(wasmTableMirror[funcPtr]=wasmTable.get(funcPtr));'

def optimize_dispatch(source, vendor):
    assert hashlib.sha256((vendor/'np21.wasm').read_bytes()).hexdigest()==SOURCE_WASM_SHA256,'Re-audit function table on NP21 upgrade'
    assert source.count(OLD)==1,'NP21 dispatch anchor changed'
    assert 'wasmTable.set(' not in source and 'wasmTable.grow(' not in source,'Dynamic table requires cache invalidation'
    return source.replace(OLD,NEW)
