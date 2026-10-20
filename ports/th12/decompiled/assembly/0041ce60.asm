
// block 0041ce60
0041ce60  push       ebx
0041ce61  push       ebp
0041ce62  mov        ebp, dword ptr [esp + 0x10]
0041ce66  push       esi
0041ce67  push       edi
0041ce68  xor        edi, edi
0041ce6a  test       ebp, ebp
0041ce6c  jle        0x41ceaf

// block 0041ce6e
0041ce6e  mov        esi, dword ptr [esp + 0x14]
0041ce72  add        esi, 0x4a4
0041ce78  mov        ebx, ebp
0041ce7a  mov        edi, ebp
0041ce7c  lea        esp, [esp]

// block 0041ce80
0041ce80  mov        eax, dword ptr [esi]
0041ce82  test       eax, eax
0041ce84  je         0x41ce93

// block 0041ce86
0041ce86  lea        ecx, [esi - 0x494]
0041ce8c  mov        edx, 2
0041ce91  call       eax

// block 0041ce93
0041ce93  mov        eax, 2
0041ce98  mov        word ptr [esi - 0xd0], ax
0041ce9f  add        esi, 0x4b4
0041cea5  sub        ebx, 1
0041cea8  jne        0x41ce80

// block 0041ceaa
0041ceaa  cmp        ebp, 8
0041cead  jae        0x41cf2b

// block 0041ceaf
0041ceaf  mov        eax, dword ptr [esp + 0x1c]
0041ceb3  mov        edx, dword ptr [esp + 0x14]
0041ceb7  mov        ecx, edi
0041ceb9  imul       ecx, ecx, 0x4b4
0041cebf  add        eax, 7
0041cec2  lea        esi, [ecx + edx + 0x10]
0041cec6  movzx      ebx, ax
0041cec9  mov        eax, dword ptr [esi + 0x494]
0041cecf  test       eax, eax
0041ced1  je         0x41ceda

// block 0041ced3
0041ced3  movsx      edx, bx
0041ced6  mov        ecx, esi
0041ced8  call       eax

// block 0041ceda
0041ceda  inc        edi
0041cedb  mov        word ptr [esi + 0x3c4], bx
0041cee2  cmp        edi, 8
0041cee5  jae        0x41cf2b

// block 0041cee7
0041cee7  mov        edx, dword ptr [esp + 0x14]
0041ceeb  mov        ecx, edi
0041ceed  imul       ecx, ecx, 0x4b4
0041cef3  mov        ebx, 8
0041cef8  lea        esi, [ecx + edx + 0x4a4]
0041ceff  sub        ebx, edi

// block 0041cf01
0041cf01  mov        eax, dword ptr [esi]
0041cf03  test       eax, eax
0041cf05  je         0x41cf14

// block 0041cf07
0041cf07  lea        ecx, [esi - 0x494]
0041cf0d  mov        edx, 3
0041cf12  call       eax

// block 0041cf14
0041cf14  mov        eax, 3
0041cf19  mov        word ptr [esi - 0xd0], ax
0041cf20  add        esi, 0x4b4
0041cf26  sub        ebx, 1
0041cf29  jne        0x41cf01

// block 0041cf2b
0041cf2b  pop        edi
0041cf2c  pop        esi
0041cf2d  pop        ebp
0041cf2e  pop        ebx
0041cf2f  ret        0xc
