
// block 00410270
00410270  mov        ecx, dword ptr [edx]
00410272  push       esi
00410273  mov        esi, eax
00410275  dec        ecx
00410276  xor        eax, eax
00410278  test       ecx, ecx
0041027a  jle        0x4102a6

// block 0041027c
0041027c  and        esi, 7
0041027f  shl        esi, 5
00410282  push       edi

// block 00410283
00410283  mov        ecx, dword ptr [edx + 0xc]
00410286  mov        ecx, dword ptr [ecx + eax*4]
00410289  mov        edi, dword ptr [ecx + 0x47c]
0041028f  and        edi, 0xffffff1f
00410295  or         edi, esi
00410297  mov        dword ptr [ecx + 0x47c], edi
0041029d  mov        ecx, dword ptr [edx]
0041029f  inc        eax
004102a0  dec        ecx
004102a1  cmp        eax, ecx
004102a3  jl         0x410283

// block 004102a5
004102a5  pop        edi

// block 004102a6
004102a6  pop        esi
004102a7  ret        
