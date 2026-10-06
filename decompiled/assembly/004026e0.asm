
// block 004026e0
004026e0  push       esi
004026e1  mov        esi, ecx
004026e3  mov        eax, dword ptr [esi + 0x478]
004026e9  test       eax, eax
004026eb  je         0x4026f6

// block 004026ed
004026ed  push       eax
004026ee  call       0x46c941

// block 004026f3
004026f3  add        esp, 4

// block 004026f6
004026f6  mov        dword ptr [esi + 0x478], 0
00402700  pop        esi
00402701  ret        
