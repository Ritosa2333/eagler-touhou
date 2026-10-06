
// block 0041d3b0
0041d3b0  mov        eax, dword ptr [0x4b452c]
0041d3b5  push       ebx
0041d3b6  mov        ebx, dword ptr [eax + 0x30]
0041d3b9  push       ebp
0041d3ba  mov        ebp, dword ptr [esp + 0xc]
0041d3be  mov        ecx, 0x1b
0041d3c3  call       0x45fe60

// block 0041d3c8
0041d3c8  xor        ebx, ebx
0041d3ca  mov        dword ptr [ebp + 0x6ce4], eax
0041d3d0  cmp        eax, ebx
0041d3d2  jne        0x41d3ee

// block 0041d3d4
0041d3d4  push       0x49f434
0041d3d9  mov        ecx, 0x4b0ec8
0041d3de  call       0x464220

// block 0041d3e3
0041d3e3  add        esp, 4
0041d3e6  pop        ebp
0041d3e7  or         eax, 0xffffffff
0041d3ea  pop        ebx
0041d3eb  ret        4

// block 0041d3ee
0041d3ee  mov        eax, dword ptr [0x4ce8a0]
0041d3f3  cmp        eax, ebx
0041d3f5  je         0x41d42c

// block 0041d3f7
0041d3f7  mov        dword ptr [ebp + 0x6d34], eax
0041d3fd  mov        ecx, dword ptr [0x4b0c94]
0041d403  mov        edx, dword ptr [0x4b0c90]
0041d409  lea        eax, [ecx + edx*2 + 6]
0041d40d  mov        ecx, dword ptr [0x4b452c]
0041d413  mov        edx, dword ptr [ecx + eax*4]
0041d416  push       edx
0041d417  push       0x49fcf0
0041d41c  call       0x421980

// block 0041d421
0041d421  add        esp, 8
0041d424  mov        dword ptr [0x4ce8a0], ebx
0041d42a  jmp        0x41d495

// block 0041d42c
0041d42c  mov        eax, dword ptr [0x4b0c94]
0041d431  mov        ecx, dword ptr [0x4b0c90]
0041d437  lea        edx, [eax + ecx*2 + 6]
0041d43b  mov        eax, dword ptr [0x4b452c]
0041d440  mov        eax, dword ptr [eax + edx*4]
0041d443  mov        byte ptr [0x4d4f38], bl
0041d449  mov        ecx, eax
0041d44b  jmp        0x41d450

// block 0041d450
0041d450  mov        dl, byte ptr [eax]
0041d452  inc        eax
0041d453  cmp        dl, bl
0041d455  jne        0x41d450

// block 0041d457
0041d457  push       esi
0041d458  push       edi
0041d459  mov        edi, 0x4d4f38
0041d45e  sub        eax, ecx
0041d460  mov        esi, ecx
0041d462  dec        edi

// block 0041d463
0041d463  mov        cl, byte ptr [edi + 1]
0041d466  inc        edi
0041d467  cmp        cl, bl
0041d469  jne        0x41d463

// block 0041d46b
0041d46b  mov        ecx, eax
0041d46d  shr        ecx, 2

// block 0041d470
0041d470  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0041d472
0041d472  mov        ecx, eax
0041d474  push       ebx
0041d475  and        ecx, 3
0041d478  push       ebx
0041d479  mov        eax, 0x4d4f38

// block 0041d47e
0041d47e  rep movsb  byte ptr es:[edi], byte ptr [esi]

// block 0041d480
0041d480  call       0x463c10

// block 0041d485
0041d485  pop        edi
0041d486  mov        dword ptr [ebp + 0x6d34], eax
0041d48c  pop        esi
0041d48d  cmp        eax, ebx
0041d48f  je         0x41d3d4

// block 0041d495
0041d495  mov        eax, dword ptr [ebp + 0x6cd0]
0041d49b  fldz       
0041d49d  test       al, 1
0041d49f  jne        0x41d4ca

// block 0041d4a1
0041d4a1  or         eax, 1
0041d4a4  fst        dword ptr [ebp + 0x6cc8]
0041d4aa  mov        dword ptr [ebp + 0x6cc4], ebx
0041d4b0  mov        dword ptr [ebp + 0x6cc0], 0xfff0bdc1
0041d4ba  mov        dword ptr [ebp + 0x6ccc], 0x4b2ed0
0041d4c4  mov        dword ptr [ebp + 0x6cd0], eax

// block 0041d4ca
0041d4ca  mov        dword ptr [ebp + 0x6cc4], ebx
0041d4d0  fstp       dword ptr [ebp + 0x6cc8]
0041d4d6  or         eax, 0xffffffff
0041d4d9  mov        dword ptr [ebp + 0x6cc0], eax
0041d4df  mov        ecx, dword ptr [0x4b0c44]
0041d4e5  mov        dword ptr [ebp + 0x6d38], eax
0041d4eb  mov        dword ptr [ebp + 0x6d40], eax
0041d4f1  mov        dword ptr [ebp + 0x6cdc], ecx
0041d4f7  pop        ebp
0041d4f8  xor        eax, eax
0041d4fa  pop        ebx
0041d4fb  ret        4
