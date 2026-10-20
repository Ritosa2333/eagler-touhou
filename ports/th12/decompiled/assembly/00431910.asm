
// block 00431910
00431910  push       esi
00431911  mov        esi, dword ptr [0x4ce8cc]
00431917  mov        eax, dword ptr [esi + 0x4b564c]
0043191d  test       eax, eax
0043191f  je         0x431933

// block 00431921
00431921  mov        ecx, dword ptr [eax]
00431923  mov        edx, dword ptr [ecx + 8]
00431926  push       eax
00431927  call       edx

// block 00431929
00431929  mov        dword ptr [esi + 0x4b564c], 0

// block 00431933
00431933  pop        esi
00431934  ret        
