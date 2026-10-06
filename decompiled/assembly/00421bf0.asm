
// block 00421bf0
00421bf0  mov        eax, dword ptr [0x4b44f4]
00421bf5  push       esi
00421bf6  mov        esi, dword ptr [eax + 0x18]
00421bf9  test       esi, esi
00421bfb  je         0x421c32

// block 00421bfd
00421bfd  push       edi
00421bfe  mov        edi, edi

// block 00421c00
00421c00  mov        edx, dword ptr [esi]
00421c02  mov        eax, dword ptr [edx + 0x10]
00421c05  mov        edi, dword ptr [esi + 8]
00421c08  mov        ecx, esi
00421c0a  call       eax

// block 00421c0c
00421c0c  mov        ecx, dword ptr [esi + 4]
00421c0f  mov        edx, dword ptr [esi + 8]
00421c12  mov        dword ptr [ecx + 8], edx
00421c15  mov        eax, dword ptr [esi + 8]
00421c18  test       eax, eax
00421c1a  je         0x421c22

// block 00421c1c
00421c1c  mov        ecx, dword ptr [esi + 4]
00421c1f  mov        dword ptr [eax + 4], ecx

// block 00421c22
00421c22  push       esi
00421c23  call       0x46ca4f

// block 00421c28
00421c28  add        esp, 4
00421c2b  mov        esi, edi
00421c2d  test       edi, edi
00421c2f  jne        0x421c00

// block 00421c31
00421c31  pop        edi

// block 00421c32
00421c32  pop        esi
00421c33  ret        
