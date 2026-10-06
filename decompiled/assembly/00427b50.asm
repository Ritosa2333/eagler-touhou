
// block 00427b50
00427b50  push       esi
00427b51  mov        esi, dword ptr [0x4b4514]
00427b57  add        eax, 0x9c4
00427b5c  mov        edx, 0xa58

// block 00427b61
00427b61  mov        ecx, dword ptr [eax]
00427b63  test       ecx, ecx
00427b65  je         0x427b7e

// block 00427b67
00427b67  cmp        ecx, 1
00427b6a  jne        0x427b7e

// block 00427b6c
00427b6c  mov        ecx, dword ptr [esi + 0xa2c]
00427b72  fld        dword ptr [ecx + 8]
00427b75  mov        dword ptr [eax], 3
00427b7b  fstp       dword ptr [eax + 0xc]

// block 00427b7e
00427b7e  add        eax, 0x9d8
00427b83  sub        edx, 1
00427b86  jne        0x427b61

// block 00427b88
00427b88  pop        esi
00427b89  ret        
