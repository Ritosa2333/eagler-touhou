
// block 00430340
00430340  cmp        dword ptr [edi + 0x994], 1
00430347  je         0x430373

// block 00430349
00430349  push       esi
0043034a  mov        esi, dword ptr [0x4ce8cc]
00430350  call       0x45a3c0

// block 00430355
00430355  mov        eax, dword ptr [edi + 8]
00430358  push       1
0043035a  mov        dword ptr [edi + 0x994], 1
00430364  mov        ecx, dword ptr [eax]
00430366  mov        edx, dword ptr [ecx + 0xe4]
0043036c  push       0xe
0043036e  push       eax
0043036f  call       edx

// block 00430371
00430371  pop        esi
00430372  ret        

// block 00430373
00430373  xor        eax, eax
00430375  ret        
