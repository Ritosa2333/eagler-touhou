
// block 004022a0
004022a0  push       ecx
004022a1  push       ebp
004022a2  mov        ebp, dword ptr [esp + 0xc]
004022a6  push       esi
004022a7  xor        esi, esi
004022a9  xor        eax, eax
004022ab  cmp        dword ptr [ebp + 0x18f7c], eax
004022b1  mov        dword ptr [esp + 0x10], esi
004022b5  lea        ecx, [ebp + 0x97c]
004022bb  jle        0x402310

// block 004022bd
004022bd  push       ebx
004022be  push       edi
004022bf  mov        edi, ecx
004022c1  lea        edx, [ecx + 0x12c]
004022c7  mov        dword ptr [esp + 0x10], edi
004022cb  mov        ebx, ecx
004022cd  lea        ecx, [ecx]

// block 004022d0
004022d0  add        dword ptr [edx], -1
004022d3  js         0x4022f9

// block 004022d5
004022d5  cmp        esi, eax
004022d7  je         0x4022e6

// block 004022d9
004022d9  mov        ecx, 0x4e
004022de  mov        esi, ebx

// block 004022e0
004022e0  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004022e2
004022e2  mov        esi, dword ptr [esp + 0x18]

// block 004022e6
004022e6  mov        edi, dword ptr [esp + 0x10]
004022ea  inc        esi
004022eb  add        edi, 0x138
004022f1  mov        dword ptr [esp + 0x18], esi
004022f5  mov        dword ptr [esp + 0x10], edi

// block 004022f9
004022f9  inc        eax
004022fa  add        ebx, 0x138
00402300  add        edx, 0x138
00402306  cmp        eax, dword ptr [ebp + 0x18f7c]
0040230c  jl         0x4022d0

// block 0040230e
0040230e  pop        edi
0040230f  pop        ebx

// block 00402310
00402310  mov        dword ptr [ebp + 0x18f7c], esi
00402316  pop        esi
00402317  pop        ebp
00402318  pop        ecx
00402319  ret        4
