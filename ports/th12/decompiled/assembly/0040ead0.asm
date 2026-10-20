
// block 0040ead0
0040ead0  push       ebx
0040ead1  push       ebp
0040ead2  mov        ebp, dword ptr [esp + 0xc]
0040ead6  push       esi
0040ead7  push       edi
0040ead8  xor        edi, edi
0040eada  push       0x24
0040eadc  mov        dword ptr [0x4b452c], 0x4aebf0
0040eae6  mov        dword ptr [0x4b0cb0], edi
0040eaec  mov        dword ptr [0x4b0cb4], edi
0040eaf2  call       0x46c9ea

// block 0040eaf7
0040eaf7  add        esp, 4
0040eafa  cmp        eax, edi
0040eafc  je         0x40eb1a

// block 0040eafe
0040eafe  and        dword ptr [eax + 4], 0xfffffffe
0040eb02  mov        dword ptr [eax + 8], edi
0040eb05  mov        dword ptr [eax + 0xc], edi
0040eb08  mov        dword ptr [eax + 0x10], edi
0040eb0b  mov        dword ptr [eax], edi
0040eb0d  mov        dword ptr [eax + 0x14], eax
0040eb10  mov        dword ptr [eax + 0x18], edi
0040eb13  mov        dword ptr [eax + 0x1c], edi
0040eb16  mov        esi, eax
0040eb18  jmp        0x40eb1c

// block 0040eb1a
0040eb1a  xor        esi, esi

// block 0040eb1c
0040eb1c  or         dword ptr [esi + 4], 3
0040eb20  mov        ebx, 5
0040eb25  mov        dword ptr [esi + 8], 0x40f650
0040eb2c  mov        dword ptr [esi + 0xc], edi
0040eb2f  mov        dword ptr [esi + 0x10], edi
0040eb32  mov        dword ptr [esi + 0x20], ebp
0040eb35  call       0x462380

// block 0040eb3a
0040eb3a  push       0x24
0040eb3c  mov        dword ptr [ebp + 8], esi
0040eb3f  call       0x46c9ea

// block 0040eb44
0040eb44  add        esp, 4
0040eb47  cmp        eax, edi
0040eb49  je         0x40eb67

// block 0040eb4b
0040eb4b  and        dword ptr [eax + 4], 0xfffffffe
0040eb4f  mov        dword ptr [eax + 8], edi
0040eb52  mov        dword ptr [eax + 0xc], edi
0040eb55  mov        dword ptr [eax + 0x10], edi
0040eb58  mov        dword ptr [eax], edi
0040eb5a  mov        dword ptr [eax + 0x14], eax
0040eb5d  mov        dword ptr [eax + 0x18], edi
0040eb60  mov        dword ptr [eax + 0x1c], edi
0040eb63  mov        esi, eax
0040eb65  jmp        0x40eb69

// block 0040eb67
0040eb67  xor        esi, esi

// block 0040eb69
0040eb69  or         dword ptr [esi + 4], 3
0040eb6d  mov        ebx, 0x2e
0040eb72  mov        dword ptr [esi + 8], 0x40f660
0040eb79  mov        dword ptr [esi + 0xc], edi
0040eb7c  mov        dword ptr [esi + 0x10], edi
0040eb7f  mov        dword ptr [esi + 0x20], ebp
0040eb82  call       0x462420

// block 0040eb87
0040eb87  pop        edi
0040eb88  mov        dword ptr [ebp + 0xc], esi
0040eb8b  pop        esi
0040eb8c  pop        ebp
0040eb8d  xor        eax, eax
0040eb8f  pop        ebx
0040eb90  ret        4
