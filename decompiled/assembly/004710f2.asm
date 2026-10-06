
// block 004710f2
004710f2  mov        edi, edi
004710f4  push       ebp
004710f5  mov        ebp, esp
004710f7  test       byte ptr [edi + 0xc], 0x40
004710fb  push       ebx
004710fc  push       esi
004710fd  mov        esi, eax
004710ff  mov        ebx, ecx
00471101  je         0x471135

// block 00471103
00471103  cmp        dword ptr [edi + 8], 0
00471107  jne        0x471135

// block 00471109
00471109  mov        eax, dword ptr [ebp + 8]
0047110c  add        dword ptr [esi], eax
0047110e  jmp        0x47113b

// block 00471110
00471110  mov        al, byte ptr [ebx]
00471112  dec        dword ptr [ebp + 8]
00471115  mov        ecx, edi
00471117  call       0x471099

// block 0047111c
0047111c  inc        ebx
0047111d  cmp        dword ptr [esi], -1
00471120  jne        0x471135

// block 00471122
00471122  call       0x46e96f

// block 00471127
00471127  cmp        dword ptr [eax], 0x2a
0047112a  jne        0x47113b

// block 0047112c
0047112c  mov        ecx, edi
0047112e  mov        al, 0x3f
00471130  call       0x471099

// block 00471135
00471135  cmp        dword ptr [ebp + 8], 0
00471139  jg         0x471110

// block 0047113b
0047113b  pop        esi
0047113c  pop        ebx
0047113d  pop        ebp
0047113e  ret        
