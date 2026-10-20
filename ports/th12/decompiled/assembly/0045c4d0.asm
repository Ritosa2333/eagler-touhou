
// block 0045c4d0
0045c4d0  push       esi
0045c4d1  push       edi
0045c4d2  mov        edi, eax
0045c4d4  mov        eax, dword ptr [edi + 0x47c]
0045c4da  mov        esi, ecx
0045c4dc  test       al, 1
0045c4de  jne        0x45c4e8

// block 0045c4e0
0045c4e0  pop        edi
0045c4e1  or         eax, 0xffffffff
0045c4e4  pop        esi
0045c4e5  ret        4

// block 0045c4e8
0045c4e8  test       al, 2
0045c4ea  je         0x45c4e0

// block 0045c4ec
0045c4ec  cmp        byte ptr [edi + 0x3bf], 0
0045c4f3  je         0x45c4e0

// block 0045c4f5
0045c4f5  mov        eax, dword ptr [edi + 0x3f4]
0045c4fb  mov        eax, dword ptr [eax + 8]
0045c4fe  cmp        dword ptr [esi + 0x4b563c], eax
0045c504  je         0x45c52c

// block 0045c506
0045c506  mov        dword ptr [esi + 0x4b563c], eax
0045c50c  call       0x45a3c0

// block 0045c511
0045c511  mov        edx, dword ptr [esi + 0x4b563c]
0045c517  mov        edx, dword ptr [edx]
0045c519  mov        eax, dword ptr [0x4ce8f0]
0045c51e  mov        ecx, dword ptr [eax]
0045c520  push       edx
0045c521  push       0
0045c523  push       eax
0045c524  mov        eax, dword ptr [ecx + 0x104]
0045c52a  call       eax

// block 0045c52c
0045c52c  cmp        byte ptr [esi + 0x4b5642], 1
0045c533  je         0x45c541

// block 0045c535
0045c535  call       0x45a3c0

// block 0045c53a
0045c53a  mov        byte ptr [esi + 0x4b5642], 1

// block 0045c541
0045c541  mov        eax, esi
0045c543  call       0x459cf0

// block 0045c548
0045c548  mov        edx, dword ptr [esp + 0xc]
0045c54c  mov        eax, esi
0045c54e  call       0x45a4a0

// block 0045c553
0045c553  pop        edi
0045c554  xor        eax, eax
0045c556  pop        esi
0045c557  ret        4
