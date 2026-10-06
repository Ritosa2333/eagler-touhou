
// block 004530e0
004530e0  mov        edx, dword ptr [0x4d4774]
004530e6  mov        ecx, 0x4cf4e8
004530eb  call       0x453120

// block 004530f0
004530f0  cmp        dword ptr [0x4d4770], 0
004530f7  jne        0x45310e

// block 004530f9
004530f9  push       esi
004530fa  mov        esi, dword ptr [0x498084]

// block 00453100
00453100  push       1
00453102  call       esi

// block 00453104
00453104  cmp        dword ptr [0x4d4770], 0
0045310b  je         0x453100

// block 0045310d
0045310d  pop        esi

// block 0045310e
0045310e  mov        dword ptr [0x4d4778], 1
00453118  ret        
