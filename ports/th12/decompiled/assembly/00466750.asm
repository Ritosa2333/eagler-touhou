
// block 00466750
00466750  cmp        dword ptr [esi + 0x1c], 3
00466754  jne        0x466780

// block 00466756
00466756  dec        dword ptr [esi + 0x14]
00466759  mov        eax, dword ptr [esi + 0x14]
0046675c  test       eax, eax
0046675e  jg         0x46676d

// block 00466760
00466760  mov        dword ptr [esi + 0x1c], 0
00466767  mov        eax, 1
0046676c  ret        

// block 0046676d
0046676d  imul       eax, eax, 0xfffffc18
00466773  cdq        
00466774  idiv       dword ptr [esi + 0x18]
00466777  mov        ecx, eax
00466779  mov        eax, esi
0046677b  call       0x4663e0

// block 00466780
00466780  xor        eax, eax
00466782  ret        
