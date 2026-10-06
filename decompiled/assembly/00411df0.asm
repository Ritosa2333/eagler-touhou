
// block 00411df0
00411df0  push       esi
00411df1  mov        esi, dword ptr [eax + 0x1034]
00411df7  test       esi, esi
00411df9  je         0x411e1b

// block 00411dfb
00411dfb  push       edi
00411dfc  lea        esp, [esp]

// block 00411e00
00411e00  mov        ecx, dword ptr [esi]
00411e02  mov        edi, dword ptr [esi + 4]
00411e05  push       ecx
00411e06  call       0x46ca4f

// block 00411e0b
00411e0b  push       esi
00411e0c  call       0x46ca4f

// block 00411e11
00411e11  add        esp, 8
00411e14  mov        esi, edi
00411e16  test       edi, edi
00411e18  jne        0x411e00

// block 00411e1a
00411e1a  pop        edi

// block 00411e1b
00411e1b  pop        esi
00411e1c  ret        
