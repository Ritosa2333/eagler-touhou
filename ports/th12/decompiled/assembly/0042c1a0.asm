
// block 0042c1a0
0042c1a0  cmp        dword ptr [ecx + 0x42c], 0x12
0042c1a7  jge        0x42c252

// block 0042c1ad
0042c1ad  push       esi
0042c1ae  mov        edi, edi

// block 0042c1b0
0042c1b0  mov        edx, dword ptr [ecx + 0x42c]
0042c1b6  lea        eax, [edx + edx*2]
0042c1b9  lea        esi, [ecx + eax*8 + 0x4ac]
0042c1c0  mov        eax, dword ptr [esi + 0x10]
0042c1c3  test       eax, eax
0042c1c5  je         0x42c251

// block 0042c1cb
0042c1cb  cmp        dword ptr [esi + 0x14], 0
0042c1cf  jne        0x42c1da

// block 0042c1d1
0042c1d1  cmp        dword ptr [ecx + 0x430], 0
0042c1d8  jne        0x42c251

// block 0042c1da
0042c1da  cmp        eax, 0x80000000
0042c1df  je         0x42c23d

// block 0042c1e1
0042c1e1  cmp        eax, 0x200
0042c1e6  je         0x42c234

// block 0042c1e8
0042c1e8  cmp        eax, 0x2000
0042c1ed  je         0x42c22b

// block 0042c1ef
0042c1ef  cmp        eax, 0x10000000
0042c1f4  jne        0x42c23d

// block 0042c1f6
0042c1f6  cmp        dword ptr [esi + 8], 0
0042c1fa  je         0x42c219

// block 0042c1fc
0042c1fc  mov        edx, dword ptr [ecx + 0xadc]
0042c202  and        edx, 0xffffff3f
0042c208  or         edx, 0x20
0042c20b  mov        dword ptr [ecx + 0xadc], edx
0042c211  inc        dword ptr [ecx + 0x42c]
0042c217  jmp        0x42c244

// block 0042c219
0042c219  and        dword ptr [ecx + 0xadc], 0xffffff1f
0042c223  inc        dword ptr [ecx + 0x42c]
0042c229  jmp        0x42c244

// block 0042c22b
0042c22b  mov        dword ptr [ecx + 0xc], 3
0042c232  jmp        0x42c23d

// block 0042c234
0042c234  mov        eax, dword ptr [esi + 8]
0042c237  mov        dword ptr [ecx + 0x44c], eax

// block 0042c23d
0042c23d  inc        edx
0042c23e  mov        dword ptr [ecx + 0x42c], edx

// block 0042c244
0042c244  cmp        dword ptr [ecx + 0x42c], 0x12
0042c24b  jl         0x42c1b0

// block 0042c251
0042c251  pop        esi

// block 0042c252
0042c252  ret        
