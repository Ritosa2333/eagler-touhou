
// block 00477afe
00477afe  mov        edi, edi
00477b00  push       ebp
00477b01  mov        ebp, esp
00477b03  mov        eax, dword ptr [ebp + 8]
00477b06  mov        ecx, dword ptr [0x4adb9c]
00477b0c  push       esi

// block 00477b0d
00477b0d  cmp        dword ptr [eax], edx
00477b0f  je         0x477b20

// block 00477b11
00477b11  mov        esi, ecx
00477b13  imul       esi, esi, 0xc
00477b16  add        esi, dword ptr [ebp + 8]
00477b19  add        eax, 0xc
00477b1c  cmp        eax, esi
00477b1e  jb         0x477b0d

// block 00477b20
00477b20  imul       ecx, ecx, 0xc
00477b23  add        ecx, dword ptr [ebp + 8]
00477b26  pop        esi
00477b27  cmp        eax, ecx
00477b29  jae        0x477b2f

// block 00477b2b
00477b2b  cmp        dword ptr [eax], edx
00477b2d  je         0x477b31

// block 00477b2f
00477b2f  xor        eax, eax

// block 00477b31
00477b31  pop        ebp
00477b32  ret        
