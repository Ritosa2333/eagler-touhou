
// block 00454ab0
00454ab0  mov        eax, dword ptr [edi + esi*4 + 0x18c0]
00454ab7  test       eax, eax
00454ab9  je         0x454acf

// block 00454abb
00454abb  push       eax
00454abc  call       0x46c941

// block 00454ac1
00454ac1  add        esp, 4
00454ac4  mov        dword ptr [edi + esi*4 + 0x18c0], 0

// block 00454acf
00454acf  ret        
