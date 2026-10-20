
// block 00466bc0
00466bc0  push       ebx
00466bc1  push       esi
00466bc2  mov        esi, ecx
00466bc4  cmp        dword ptr [esi + 0x7c], 0
00466bc8  mov        ebx, eax
00466bca  je         0x466bd6

// block 00466bcc
00466bcc  pop        esi
00466bcd  mov        eax, 0x80004005
00466bd2  pop        ebx
00466bd3  ret        4

// block 00466bd6
00466bd6  cmp        dword ptr [esi + 0x8c], -1
00466bdd  push       edi
00466bde  jne        0x466c0d

// block 00466be0
00466be0  mov        edi, dword ptr [esi + 0x94]
00466be6  mov        dword ptr [esi + 0x78], 1
00466bed  mov        dword ptr [esi + 0x7c], 0
00466bf4  call       0x466b10

// block 00466bf9
00466bf9  cmp        dword ptr [esi + 0x8c], -1
00466c00  jne        0x466c0d

// block 00466c02
00466c02  pop        edi
00466c03  pop        esi
00466c04  mov        eax, 0x80004005
00466c09  pop        ebx
00466c0a  ret        4

// block 00466c0d
00466c0d  mov        dword ptr [esi + 0x90], ebx
00466c13  mov        eax, dword ptr [ebx + 0x1c]
00466c16  mov        ecx, dword ptr [ebx + 0x18]
00466c19  push       eax
00466c1a  push       ecx
00466c1b  push       ebx
00466c1c  push       0x4a3af4
00466c21  call       0x466e90

// block 00466c26
00466c26  mov        edi, dword ptr [esp + 0x20]
00466c2a  add        esp, 0x10
00466c2d  xor        bl, bl
00466c2f  call       0x466c90

// block 00466c34
00466c34  mov        edx, dword ptr [esi + 8]
00466c37  pop        edi
00466c38  mov        dword ptr [esi + 0x2c], edx
00466c3b  pop        esi
00466c3c  xor        eax, eax
00466c3e  pop        ebx
00466c3f  ret        4
