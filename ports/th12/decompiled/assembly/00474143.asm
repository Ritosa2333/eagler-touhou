
// block 00474143
00474143  test       edi, edi
00474145  je         0x47417e

// block 00474147
00474147  test       eax, eax
00474149  je         0x47417e

// block 0047414b
0047414b  push       esi
0047414c  mov        esi, dword ptr [eax]
0047414e  cmp        esi, edi
00474150  je         0x47417a

// block 00474152
00474152  push       edi
00474153  mov        dword ptr [eax], edi
00474155  call       0x473ff5

// block 0047415a
0047415a  pop        ecx
0047415b  test       esi, esi
0047415d  je         0x47417a

// block 0047415f
0047415f  push       esi
00474160  call       0x474084

// block 00474165
00474165  cmp        dword ptr [esi], 0
00474168  pop        ecx
00474169  jne        0x47417a

// block 0047416b
0047416b  cmp        esi, 0x4ad9e0
00474171  je         0x47417a

// block 00474173
00474173  push       esi
00474174  call       0x473eac

// block 00474179
00474179  pop        ecx

// block 0047417a
0047417a  mov        eax, edi
0047417c  pop        esi
0047417d  ret        

// block 0047417e
0047417e  xor        eax, eax
00474180  ret        
