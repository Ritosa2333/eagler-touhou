
// block 00453460
00453460  push       esi
00453461  xor        esi, esi

// block 00453463
00453463  cmp        dword ptr [0x4d4770], 2
0045346a  je         0x4534ad

// block 0045346c
0045346c  mov        eax, dword ptr [esi*4 + 0x4aea50]
00453473  push       0
00453475  push       0
00453477  call       0x463c10

// block 0045347c
0045347c  mov        dword ptr [esi*4 + 0x4d1410], eax
00453483  test       eax, eax
00453485  je         0x4534af

// block 00453487
00453487  inc        esi
00453488  cmp        esi, 0x31
0045348b  jl         0x453463

// block 0045348d
0045348d  cmp        dword ptr [0x4d4770], 0
00453494  jne        0x4534ad

// block 00453496
00453496  mov        esi, dword ptr [0x498084]
0045349c  lea        esp, [esp]

// block 004534a0
004534a0  push       1
004534a2  call       esi

// block 004534a4
004534a4  cmp        dword ptr [0x4d4770], 0
004534ab  je         0x4534a0

// block 004534ad
004534ad  pop        esi
004534ae  ret        

// block 004534af
004534af  mov        eax, dword ptr [esi*4 + 0x4aea50]
004534b6  push       eax
004534b7  push       0x4a2e34
004534bc  mov        ecx, 0x4b0ec8
004534c1  call       0x464220

// block 004534c6
004534c6  add        esp, 8
004534c9  pop        esi
004534ca  ret        
