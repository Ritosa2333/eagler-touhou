
// block 00466e30
00466e30  cmp        dword ptr [esi + 0x78], 1
00466e34  jne        0x466e4d

// block 00466e36
00466e36  mov        eax, dword ptr [esi + 0x8c]
00466e3c  push       eax
00466e3d  call       dword ptr [0x4980a4]

// block 00466e43
00466e43  mov        dword ptr [esi + 0x8c], 0xffffffff

// block 00466e4d
00466e4d  xor        eax, eax
00466e4f  ret        
