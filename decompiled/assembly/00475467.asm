
// block 00475467
00475467  mov        edi, edi
00475469  push       esi
0047546a  call       0x4753ee

// block 0047546f
0047546f  mov        esi, eax
00475471  test       esi, esi
00475473  jne        0x47547d

// block 00475475
00475475  push       0x10
00475477  call       0x472c0d

// block 0047547c
0047547c  pop        ecx

// block 0047547d
0047547d  mov        eax, esi
0047547f  pop        esi
00475480  ret        
