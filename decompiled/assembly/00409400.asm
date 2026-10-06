
// block 00409400
00409400  push       esi
00409401  push       0x214
00409406  mov        esi, ecx
00409408  push       0
0040940a  push       esi
0040940b  call       0x477420

// block 00409410
00409410  add        esp, 0xc
00409413  mov        dword ptr [esi + 0x208], 0xffffffff
0040941d  mov        eax, esi
0040941f  pop        esi
00409420  ret        
