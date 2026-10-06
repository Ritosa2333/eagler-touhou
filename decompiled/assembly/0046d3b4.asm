
// block 0046d3b4
0046d3b4  mov        edi, edi
0046d3b6  push       ebp
0046d3b7  mov        ebp, esp
0046d3b9  mov        eax, dword ptr [ebp + 8]
0046d3bc  push       esi
0046d3bd  mov        esi, ecx
0046d3bf  mov        byte ptr [esi + 0xc], 0
0046d3c3  test       eax, eax
0046d3c5  jne        0x46d42a

// block 0046d3c7
0046d3c7  call       0x475467

// block 0046d3cc
0046d3cc  mov        dword ptr [esi + 8], eax
0046d3cf  mov        ecx, dword ptr [eax + 0x6c]
0046d3d2  mov        dword ptr [esi], ecx
0046d3d4  mov        ecx, dword ptr [eax + 0x68]
0046d3d7  mov        dword ptr [esi + 4], ecx
0046d3da  mov        ecx, dword ptr [esi]
0046d3dc  cmp        ecx, dword ptr [0x4adab8]
0046d3e2  je         0x46d3f6

// block 0046d3e4
0046d3e4  mov        ecx, dword ptr [0x4ad9d4]
0046d3ea  test       dword ptr [eax + 0x70], ecx
0046d3ed  jne        0x46d3f6

// block 0046d3ef
0046d3ef  call       0x474181

// block 0046d3f4
0046d3f4  mov        dword ptr [esi], eax

// block 0046d3f6
0046d3f6  mov        eax, dword ptr [esi + 4]
0046d3f9  cmp        eax, dword ptr [0x4ad8d8]
0046d3ff  je         0x46d417

// block 0046d401
0046d401  mov        eax, dword ptr [esi + 8]
0046d404  mov        ecx, dword ptr [0x4ad9d4]
0046d40a  test       dword ptr [eax + 0x70], ecx
0046d40d  jne        0x46d417

// block 0046d40f
0046d40f  call       0x4739a5

// block 0046d414
0046d414  mov        dword ptr [esi + 4], eax

// block 0046d417
0046d417  mov        eax, dword ptr [esi + 8]
0046d41a  test       byte ptr [eax + 0x70], 2
0046d41e  jne        0x46d434

// block 0046d420
0046d420  or         dword ptr [eax + 0x70], 2
0046d424  mov        byte ptr [esi + 0xc], 1
0046d428  jmp        0x46d434

// block 0046d42a
0046d42a  mov        ecx, dword ptr [eax]
0046d42c  mov        dword ptr [esi], ecx
0046d42e  mov        eax, dword ptr [eax + 4]
0046d431  mov        dword ptr [esi + 4], eax

// block 0046d434
0046d434  mov        eax, esi
0046d436  pop        esi
0046d437  pop        ebp
0046d438  ret        4
