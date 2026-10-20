
// block 00466af0
00466af0  cmp        dword ptr [esi + 0x78], 1
00466af4  jne        0x466b0d

// block 00466af6
00466af6  mov        eax, dword ptr [esi + 0x8c]
00466afc  push       eax
00466afd  call       dword ptr [0x4980a4]

// block 00466b03
00466b03  mov        dword ptr [esi + 0x8c], 0xffffffff

// block 00466b0d
00466b0d  ret        
