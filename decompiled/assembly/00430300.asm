
// block 00430300
00430300  cmp        dword ptr [edi + 0x990], 0
00430307  je         0x430333

// block 00430309
00430309  push       esi
0043030a  mov        esi, dword ptr [0x4ce8cc]
00430310  call       0x45a3c0

// block 00430315
00430315  mov        eax, dword ptr [edi + 8]
00430318  push       0
0043031a  mov        dword ptr [edi + 0x990], 0
00430324  mov        ecx, dword ptr [eax]
00430326  mov        edx, dword ptr [ecx + 0xe4]
0043032c  push       0x1c
0043032e  push       eax
0043032f  call       edx

// block 00430331
00430331  pop        esi
00430332  ret        

// block 00430333
00430333  xor        eax, eax
00430335  ret        
