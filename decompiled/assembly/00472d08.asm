
// block 00472d08
00472d08  mov        edi, edi
00472d0a  push       ebp
00472d0b  mov        ebp, esp
00472d0d  mov        ecx, dword ptr [ebp + 8]
00472d10  push       esi
00472d11  xor        esi, esi
00472d13  cmp        ecx, esi
00472d15  jne        0x472d34

// block 00472d17
00472d17  call       0x46e96f

// block 00472d1c
00472d1c  push       esi
00472d1d  push       esi
00472d1e  push       esi
00472d1f  push       esi
00472d20  push       esi
00472d21  mov        dword ptr [eax], 0x16
00472d27  call       0x470f2d

// block 00472d2c
00472d2c  add        esp, 0x14
00472d2f  push       0x16
00472d31  pop        eax
00472d32  jmp        0x472d41

// block 00472d34
00472d34  mov        eax, dword ptr [0x4b3d90]
00472d39  cmp        eax, esi
00472d3b  je         0x472d17

// block 00472d3d
00472d3d  mov        dword ptr [ecx], eax
00472d3f  xor        eax, eax

// block 00472d41
00472d41  pop        esi
00472d42  pop        ebp
00472d43  ret        
