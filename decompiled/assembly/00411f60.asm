
// block 00411f60
00411f60  mov        eax, dword ptr [esi + 0x8c]
00411f66  mov        dword ptr [esi], 0x49fc28
00411f6c  test       eax, eax
00411f6e  je         0x411f83

// block 00411f70
00411f70  push       eax
00411f71  call       0x46c941

// block 00411f76
00411f76  add        esp, 4
00411f79  mov        dword ptr [esi + 0x8c], 0

// block 00411f83
00411f83  ret        
