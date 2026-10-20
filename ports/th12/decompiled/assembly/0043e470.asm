
// block 0043e470
0043e470  push       ebx
0043e471  push       ebp
0043e472  mov        ebp, dword ptr [esp + 0xc]
0043e476  push       esi
0043e477  push       edi
0043e478  push       0x24
0043e47a  call       0x46c9ea

// block 0043e47f
0043e47f  xor        edi, edi
0043e481  add        esp, 4
0043e484  cmp        eax, edi
0043e486  je         0x43e4a4

// block 0043e488
0043e488  and        dword ptr [eax + 4], 0xfffffffe
0043e48c  mov        dword ptr [eax + 8], edi
0043e48f  mov        dword ptr [eax + 0xc], edi
0043e492  mov        dword ptr [eax + 0x10], edi
0043e495  mov        dword ptr [eax], edi
0043e497  mov        dword ptr [eax + 0x14], eax
0043e49a  mov        dword ptr [eax + 0x18], edi
0043e49d  mov        dword ptr [eax + 0x1c], edi
0043e4a0  mov        esi, eax
0043e4a2  jmp        0x43e4a6

// block 0043e4a4
0043e4a4  xor        esi, esi

// block 0043e4a6
0043e4a6  or         dword ptr [esi + 4], 3
0043e4aa  mov        ebx, 5
0043e4af  mov        dword ptr [esi + 8], 0x43eea0
0043e4b6  mov        dword ptr [esi + 0xc], edi
0043e4b9  mov        dword ptr [esi + 0x10], edi
0043e4bc  mov        dword ptr [esi + 0x20], ebp
0043e4bf  call       0x462380

// block 0043e4c4
0043e4c4  push       0x24
0043e4c6  mov        dword ptr [ebp + 8], esi
0043e4c9  call       0x46c9ea

// block 0043e4ce
0043e4ce  add        esp, 4
0043e4d1  cmp        eax, edi
0043e4d3  je         0x43e4f1

// block 0043e4d5
0043e4d5  and        dword ptr [eax + 4], 0xfffffffe
0043e4d9  mov        dword ptr [eax + 8], edi
0043e4dc  mov        dword ptr [eax + 0xc], edi
0043e4df  mov        dword ptr [eax + 0x10], edi
0043e4e2  mov        dword ptr [eax], edi
0043e4e4  mov        dword ptr [eax + 0x14], eax
0043e4e7  mov        dword ptr [eax + 0x18], edi
0043e4ea  mov        dword ptr [eax + 0x1c], edi
0043e4ed  mov        esi, eax
0043e4ef  jmp        0x43e4f3

// block 0043e4f1
0043e4f1  xor        esi, esi

// block 0043e4f3
0043e4f3  or         dword ptr [esi + 4], 3
0043e4f7  mov        ebx, 0x2e
0043e4fc  mov        dword ptr [esi + 8], 0x43eeb0
0043e503  mov        dword ptr [esi + 0xc], edi
0043e506  mov        dword ptr [esi + 0x10], edi
0043e509  mov        dword ptr [esi + 0x20], ebp
0043e50c  call       0x462420

// block 0043e511
0043e511  pop        edi
0043e512  mov        dword ptr [ebp + 0xc], esi
0043e515  pop        esi
0043e516  pop        ebp
0043e517  xor        eax, eax
0043e519  pop        ebx
0043e51a  ret        4
