
// block 00402870
00402870  mov        eax, dword ptr [esi + 0x10]
00402873  push       edi
00402874  xor        edi, edi
00402876  cmp        eax, edi
00402878  je         0x402886

// block 0040287a
0040287a  push       eax
0040287b  call       0x46c941

// block 00402880
00402880  add        esp, 4
00402883  mov        dword ptr [esi + 0x10], edi

// block 00402886
00402886  mov        eax, dword ptr [esi + 0x14]
00402889  cmp        eax, edi
0040288b  je         0x402899

// block 0040288d
0040288d  push       eax
0040288e  call       0x46c941

// block 00402893
00402893  add        esp, 4
00402896  mov        dword ptr [esi + 0x14], edi

// block 00402899
00402899  mov        eax, dword ptr [esi]
0040289b  dec        eax
0040289c  xor        edx, edx
0040289e  test       eax, eax
004028a0  jle        0x40292f

// block 004028a6
004028a6  push       ebx
004028a7  push       ebp

// block 004028a8
004028a8  mov        ecx, dword ptr [esi + 8]
004028ab  lea        ebx, [ecx + edx*4]
004028ae  mov        ecx, dword ptr [ebx]
004028b0  cmp        ecx, edi
004028b2  je         0x40291f

// block 004028b4
004028b4  mov        eax, dword ptr [0x4ce8cc]
004028b9  mov        eax, dword ptr [eax + 0x8856b8]
004028bf  cmp        eax, edi
004028c1  je         0x4028d1

// block 004028c3
004028c3  mov        ebp, dword ptr [eax]
004028c5  cmp        dword ptr [ebp], ecx
004028c8  je         0x4028f0

// block 004028ca
004028ca  mov        eax, dword ptr [eax + 4]
004028cd  cmp        eax, edi
004028cf  jne        0x4028c3

// block 004028d1
004028d1  mov        eax, dword ptr [0x4ce8cc]
004028d6  mov        eax, dword ptr [eax + 0x8856c0]
004028dc  cmp        eax, edi
004028de  je         0x40291f

// block 004028e0
004028e0  mov        ebp, dword ptr [eax]
004028e2  cmp        dword ptr [ebp], ecx
004028e5  je         0x4028f0

// block 004028e7
004028e7  mov        eax, dword ptr [eax + 4]
004028ea  cmp        eax, edi
004028ec  jne        0x4028e0

// block 004028ee
004028ee  jmp        0x40291f

// block 004028f0
004028f0  mov        eax, ebp
004028f2  cmp        eax, edi
004028f4  je         0x40291f

// block 004028f6
004028f6  mov        ebp, 0x10000000
004028fb  or         dword ptr [eax + 0x47c], ebp
00402901  cmp        dword ptr [eax + 0x18], edi
00402904  jne        0x40291f

// block 00402906
00402906  mov        eax, dword ptr [eax + 0x14]
00402909  cmp        eax, edi
0040290b  je         0x40291f

// block 0040290d
0040290d  lea        ecx, [ecx]

// block 00402910
00402910  mov        ecx, dword ptr [eax]
00402912  or         dword ptr [ecx + 0x47c], ebp
00402918  mov        eax, dword ptr [eax + 4]
0040291b  cmp        eax, edi
0040291d  jne        0x402910

// block 0040291f
0040291f  mov        dword ptr [ebx], edi
00402921  mov        ecx, dword ptr [esi]
00402923  inc        edx
00402924  dec        ecx
00402925  cmp        edx, ecx
00402927  jl         0x4028a8

// block 0040292d
0040292d  pop        ebp
0040292e  pop        ebx

// block 0040292f
0040292f  mov        eax, dword ptr [esi + 8]
00402932  cmp        eax, edi
00402934  je         0x402942

// block 00402936
00402936  push       eax
00402937  call       0x46c941

// block 0040293c
0040293c  add        esp, 4
0040293f  mov        dword ptr [esi + 8], edi

// block 00402942
00402942  mov        eax, dword ptr [esi + 0xc]
00402945  cmp        eax, edi
00402947  je         0x402955

// block 00402949
00402949  push       eax
0040294a  call       0x46c941

// block 0040294f
0040294f  add        esp, 4
00402952  mov        dword ptr [esi + 0xc], edi

// block 00402955
00402955  pop        edi
00402956  ret        
