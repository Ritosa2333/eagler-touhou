
// block 00437680
00437680  push       0
00437682  push       0
00437684  call       0x463c10

// block 00437689
00437689  mov        dword ptr [esi + 0xa2c], eax
0043768f  test       eax, eax
00437691  jne        0x437697

// block 00437693
00437693  or         eax, 0xffffffff
00437696  ret        

// block 00437697
00437697  xor        ecx, ecx
00437699  xor        edx, edx
0043769b  cmp        cx, word ptr [eax + 2]
0043769f  jae        0x43771f

// block 004376a1
004376a1  mov        ecx, 0x268
004376a6  push       edi
004376a7  jmp        0x4376b0

// block 004376b0
004376b0  mov        eax, dword ptr [esi + 0xa2c]
004376b6  add        dword ptr [ecx + eax], eax
004376b9  mov        eax, dword ptr [esi + 0xa2c]
004376bf  mov        eax, dword ptr [ecx + eax]
004376c2  cmp        byte ptr [eax], 0
004376c5  jl         0x43770c

// block 004376c7
004376c7  jmp        0x4376d0

// block 004376d0
004376d0  mov        edi, dword ptr [eax + 0x24]
004376d3  mov        edi, dword ptr [edi*4 + 0x4aedf0]
004376da  mov        dword ptr [eax + 0x24], edi
004376dd  mov        edi, dword ptr [eax + 0x28]
004376e0  mov        edi, dword ptr [edi*4 + 0x4aebd4]
004376e7  mov        dword ptr [eax + 0x28], edi
004376ea  mov        edi, dword ptr [eax + 0x2c]
004376ed  mov        edi, dword ptr [edi*4 + 0x4ce8ac]
004376f4  mov        dword ptr [eax + 0x2c], edi
004376f7  mov        edi, dword ptr [eax + 0x30]
004376fa  mov        edi, dword ptr [edi*4 + 0x4aee10]
00437701  mov        dword ptr [eax + 0x30], edi
00437704  add        eax, 0x34
00437707  cmp        byte ptr [eax], 0
0043770a  jge        0x4376d0

// block 0043770c
0043770c  mov        eax, dword ptr [esi + 0xa2c]
00437712  movzx      eax, word ptr [eax + 2]
00437716  inc        edx
00437717  add        ecx, 8
0043771a  cmp        edx, eax
0043771c  jl         0x4376b0

// block 0043771e
0043771e  pop        edi

// block 0043771f
0043771f  xor        eax, eax
00437721  ret        
