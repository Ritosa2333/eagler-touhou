
// block 00428520
00428520  push       esi
00428521  mov        esi, eax
00428523  call       0x427ec0

// block 00428528
00428528  push       0x1e8
0042852d  lea        eax, [esi + 0x454]
00428533  push       0
00428535  push       eax
00428536  mov        dword ptr [esi], 0x4a0654
0042853c  call       0x477420

// block 00428541
00428541  add        esp, 0xc
00428544  lea        ecx, [esi + 0x63c]
0042854a  call       0x4027e0

// block 0042854f
0042854f  lea        ecx, [esi + 0xaf0]
00428555  call       0x4027e0

// block 0042855a
0042855a  mov        eax, esi
0042855c  pop        esi
0042855d  ret        
