
// block 00461920
00461920  mov        ecx, dword ptr [esp + 4]
00461924  test       ecx, ecx
00461926  jne        0x46192d

// block 00461928
00461928  xor        eax, eax
0046192a  ret        4

// block 0046192d
0046192d  mov        eax, dword ptr [edx + 0x8856b8]
00461933  push       esi
00461934  test       eax, eax
00461936  je         0x461945

// block 00461938
00461938  mov        esi, dword ptr [eax]
0046193a  cmp        dword ptr [esi], ecx
0046193c  je         0x461963

// block 0046193e
0046193e  mov        eax, dword ptr [eax + 4]
00461941  test       eax, eax
00461943  jne        0x461938

// block 00461945
00461945  mov        eax, dword ptr [edx + 0x8856c0]
0046194b  test       eax, eax
0046194d  je         0x46195d

// block 0046194f
0046194f  nop        

// block 00461950
00461950  mov        edx, dword ptr [eax]
00461952  cmp        dword ptr [edx], ecx
00461954  je         0x461969

// block 00461956
00461956  mov        eax, dword ptr [eax + 4]
00461959  test       eax, eax
0046195b  jne        0x461950

// block 0046195d
0046195d  xor        eax, eax
0046195f  pop        esi
00461960  ret        4

// block 00461963
00461963  mov        eax, esi
00461965  pop        esi
00461966  ret        4

// block 00461969
00461969  mov        eax, edx
0046196b  pop        esi
0046196c  ret        4
