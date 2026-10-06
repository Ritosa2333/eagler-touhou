
// block 0047b31d
0047b31d  mov        edi, edi
0047b31f  push       esi
0047b320  mov        eax, 0x4aa9ec
0047b325  mov        esi, 0x4aa9ec
0047b32a  push       edi
0047b32b  mov        edi, eax
0047b32d  cmp        eax, esi
0047b32f  jae        0x47b340

// block 0047b331
0047b331  mov        eax, dword ptr [edi]
0047b333  test       eax, eax
0047b335  je         0x47b339

// block 0047b337
0047b337  call       eax

// block 0047b339
0047b339  add        edi, 4
0047b33c  cmp        edi, esi
0047b33e  jb         0x47b331

// block 0047b340
0047b340  pop        edi
0047b341  pop        esi
0047b342  ret        
