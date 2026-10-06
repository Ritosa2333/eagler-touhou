
// block 00466710
00466710  cmp        dword ptr [esi + 0x1c], 2
00466714  jne        0x466740

// block 00466716
00466716  dec        dword ptr [esi + 0x14]
00466719  mov        eax, dword ptr [esi + 0x14]
0046671c  test       eax, eax
0046671e  jg         0x46672d

// block 00466720
00466720  mov        dword ptr [esi + 0x1c], 0
00466727  mov        eax, 1
0046672c  ret        

// block 0046672d
0046672d  imul       eax, eax, 0xffffec78
00466733  cdq        
00466734  idiv       dword ptr [esi + 0x18]
00466737  mov        ecx, eax
00466739  mov        eax, esi
0046673b  call       0x4663e0

// block 00466740
00466740  xor        eax, eax
00466742  ret        
