
// block 00477813
00477813  mov        edi, edi
00477815  push       ebp
00477816  mov        ebp, esp
00477818  push       esi
00477819  push       edi
0047781a  xor        esi, esi

// block 0047781c
0047781c  push       0
0047781e  push       dword ptr [ebp + 0xc]
00477821  push       dword ptr [ebp + 8]
00477824  call       0x48b4e8

// block 00477829
00477829  mov        edi, eax
0047782b  add        esp, 0xc
0047782e  test       edi, edi
00477830  jne        0x477859

// block 00477832
00477832  cmp        dword ptr [0x4b41d0], eax
00477838  jbe        0x477859

// block 0047783a
0047783a  push       esi
0047783b  call       dword ptr [0x498084]

// block 00477841
00477841  lea        eax, [esi + 0x3e8]
00477847  cmp        eax, dword ptr [0x4b41d0]
0047784d  jbe        0x477852

// block 0047784f
0047784f  or         eax, 0xffffffff

// block 00477852
00477852  mov        esi, eax
00477854  cmp        eax, -1
00477857  jne        0x47781c

// block 00477859
00477859  mov        eax, edi
0047785b  pop        edi
0047785c  pop        esi
0047785d  pop        ebp
0047785e  ret        
