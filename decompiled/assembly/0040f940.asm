
// block 0040f940
0040f940  push       ebx
0040f941  push       ebp
0040f942  mov        ebp, dword ptr [esp + 0xc]
0040f946  push       edi
0040f947  mov        ebx, 0x49f69c
0040f94c  mov        ecx, 6
0040f951  call       0x45fe60

// block 0040f956
0040f956  xor        edi, edi
0040f958  mov        dword ptr [ebp + 0x10], eax
0040f95b  cmp        eax, edi
0040f95d  jne        0x40f97a

// block 0040f95f
0040f95f  push       0x49f8b8
0040f964  mov        ecx, 0x4b0ec8
0040f969  call       0x464220

// block 0040f96e
0040f96e  add        esp, 4
0040f971  pop        edi
0040f972  pop        ebp
0040f973  or         eax, 0xffffffff
0040f976  pop        ebx
0040f977  ret        4

// block 0040f97a
0040f97a  push       esi
0040f97b  push       0x24
0040f97d  call       0x46c9ea

// block 0040f982
0040f982  add        esp, 4
0040f985  cmp        eax, edi
0040f987  je         0x40f9a5

// block 0040f989
0040f989  and        dword ptr [eax + 4], 0xfffffffe
0040f98d  mov        dword ptr [eax + 8], edi
0040f990  mov        dword ptr [eax + 0xc], edi
0040f993  mov        dword ptr [eax + 0x10], edi
0040f996  mov        dword ptr [eax], edi
0040f998  mov        dword ptr [eax + 0x14], eax
0040f99b  mov        dword ptr [eax + 0x18], edi
0040f99e  mov        dword ptr [eax + 0x1c], edi
0040f9a1  mov        esi, eax
0040f9a3  jmp        0x40f9a7

// block 0040f9a5
0040f9a5  xor        esi, esi

// block 0040f9a7
0040f9a7  mov        eax, dword ptr [esi + 4]
0040f9aa  and        eax, 0xfffffffd
0040f9ad  or         eax, 1
0040f9b0  mov        ebx, 0x18
0040f9b5  mov        dword ptr [esi + 8], 0x40fbc0
0040f9bc  mov        dword ptr [esi + 0xc], edi
0040f9bf  mov        dword ptr [esi + 0x10], edi
0040f9c2  mov        dword ptr [esi + 0x20], ebp
0040f9c5  mov        dword ptr [esi + 4], eax
0040f9c8  call       0x462380

// block 0040f9cd
0040f9cd  push       0x24
0040f9cf  mov        dword ptr [ebp + 8], esi
0040f9d2  call       0x46c9ea

// block 0040f9d7
0040f9d7  add        esp, 4
0040f9da  cmp        eax, edi
0040f9dc  je         0x40f9fa

// block 0040f9de
0040f9de  and        dword ptr [eax + 4], 0xfffffffe
0040f9e2  mov        dword ptr [eax + 8], edi
0040f9e5  mov        dword ptr [eax + 0xc], edi
0040f9e8  mov        dword ptr [eax + 0x10], edi
0040f9eb  mov        dword ptr [eax], edi
0040f9ed  mov        dword ptr [eax + 0x14], eax
0040f9f0  mov        dword ptr [eax + 0x18], edi
0040f9f3  mov        dword ptr [eax + 0x1c], edi
0040f9f6  mov        esi, eax
0040f9f8  jmp        0x40f9fc

// block 0040f9fa
0040f9fa  xor        esi, esi

// block 0040f9fc
0040f9fc  mov        ecx, dword ptr [esi + 4]
0040f9ff  and        ecx, 0xfffffffd
0040fa02  or         ecx, 1
0040fa05  mov        ebx, 0x21
0040fa0a  mov        dword ptr [esi + 8], 0x40fbd0
0040fa11  mov        dword ptr [esi + 0xc], edi
0040fa14  mov        dword ptr [esi + 0x10], edi
0040fa17  mov        dword ptr [esi + 0x20], ebp
0040fa1a  mov        dword ptr [esi + 4], ecx
0040fa1d  call       0x462420

// block 0040fa22
0040fa22  mov        dword ptr [ebp + 0xc], esi
0040fa25  pop        esi
0040fa26  pop        edi
0040fa27  pop        ebp
0040fa28  xor        eax, eax
0040fa2a  pop        ebx
0040fa2b  ret        4
