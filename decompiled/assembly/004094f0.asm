
// block 004094f0
004094f0  push       esi
004094f1  mov        esi, ecx
004094f3  mov        eax, dword ptr [esi + 0x480]
004094f9  test       eax, eax
004094fb  je         0x409506

// block 004094fd
004094fd  push       eax
004094fe  call       0x46c941

// block 00409503
00409503  add        esp, 4

// block 00409506
00409506  mov        dword ptr [esi + 0x480], 0
00409510  pop        esi
00409511  ret        
