
// block 00431700
00431700  mov        eax, dword ptr [0x4cea94]
00431705  push       esi
00431706  xor        esi, esi
00431708  cmp        eax, esi
0043170a  je         0x43171a

// block 0043170c
0043170c  mov        ecx, dword ptr [eax]
0043170e  mov        edx, dword ptr [ecx + 8]
00431711  push       eax
00431712  call       edx

// block 00431714
00431714  mov        dword ptr [0x4cea94], esi

// block 0043171a
0043171a  mov        eax, dword ptr [0x4cea98]
0043171f  cmp        eax, esi
00431721  je         0x431731

// block 00431723
00431723  mov        ecx, dword ptr [eax]
00431725  mov        edx, dword ptr [ecx + 8]
00431728  push       eax
00431729  call       edx

// block 0043172b
0043172b  mov        dword ptr [0x4cea98], esi

// block 00431731
00431731  mov        eax, dword ptr [0x4cea9c]
00431736  cmp        eax, esi
00431738  je         0x431748

// block 0043173a
0043173a  mov        ecx, dword ptr [eax]
0043173c  mov        edx, dword ptr [ecx + 8]
0043173f  push       eax
00431740  call       edx

// block 00431742
00431742  mov        dword ptr [0x4cea9c], esi

// block 00431748
00431748  mov        dword ptr [0x4cea94], esi
0043174e  pop        esi
0043174f  ret        
