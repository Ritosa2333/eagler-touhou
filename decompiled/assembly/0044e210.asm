
// block 0044e210
0044e210  sub        esp, 0x44
0044e213  push       ebx
0044e214  push       ebp
0044e215  mov        ebp, dword ptr [esp + 0x5c]
0044e219  push       edi
0044e21a  mov        edi, ecx
0044e21c  cmp        dword ptr [edi + 0x11c], 0
0044e223  mov        ebx, eax
0044e225  jne        0x44e232

// block 0044e227
0044e227  pop        edi
0044e228  pop        ebp
0044e229  xor        al, al
0044e22b  pop        ebx
0044e22c  add        esp, 0x44
0044e22f  ret        0x10

// block 0044e232
0044e232  push       esi
0044e233  mov        esi, dword ptr [esp + 0x58]
0044e237  mov        eax, dword ptr [esi]
0044e239  mov        edx, dword ptr [eax + 0x30]
0044e23c  lea        ecx, [esp + 0x34]
0044e240  push       ecx
0044e241  push       esi
0044e242  call       edx

// block 0044e244
0044e244  mov        edx, dword ptr [edi + 0x108]
0044e24a  mov        ecx, dword ptr [edi + 0x104]
0044e250  xor        eax, eax
0044e252  push       eax
0044e253  mov        dword ptr [esp + 0x34], edx
0044e257  lea        edx, [esp + 0x28]
0044e25b  mov        dword ptr [esp + 0x28], eax
0044e25f  mov        dword ptr [esp + 0x2c], eax
0044e263  push       edx
0044e264  mov        dword ptr [esp + 0x34], ecx
0044e268  mov        ecx, dword ptr [esi]
0044e26a  mov        ecx, dword ptr [ecx + 0x34]
0044e26d  lea        eax, [esp + 0x24]
0044e271  push       eax
0044e272  push       esi
0044e273  call       ecx

// block 0044e275
0044e275  test       eax, eax
0044e277  jne        0x44e2b8

// block 0044e279
0044e279  mov        edx, dword ptr [edi + 0x110]
0044e27f  mov        ecx, dword ptr [edi + 0x120]
0044e285  mov        edi, dword ptr [edi + 0x100]
0044e28b  mov        esi, dword ptr [esp + 0x1c]
0044e28f  mov        dword ptr [esp + 0x10], esi
0044e293  mov        dword ptr [esp + 0x64], edx
0044e297  cmp        dword ptr [esp + 0x34], edi
0044e29b  jne        0x44e489

// block 0044e2a1
0044e2a1  sub        edi, 0x15
0044e2a4  je         0x44e3f5

// block 0044e2aa
0044e2aa  sub        edi, 4
0044e2ad  je         0x44e399

// block 0044e2b3
0044e2b3  sub        edi, 1
0044e2b6  je         0x44e2c4

// block 0044e2b8
0044e2b8  pop        esi
0044e2b9  pop        edi
0044e2ba  pop        ebp
0044e2bb  xor        al, al
0044e2bd  pop        ebx
0044e2be  add        esp, 0x44
0044e2c1  ret        0x10

// block 0044e2c4
0044e2c4  mov        eax, dword ptr [esp + 0x5c]
0044e2c8  imul       esi, ebx
0044e2cb  add        esi, dword ptr [esp + 0x20]
0044e2cf  lea        edi, [esi + eax*2]
0044e2d2  test       ebp, ebp
0044e2d4  jle        0x44e489

// block 0044e2da
0044e2da  mov        dword ptr [esp + 0x14], ebp
0044e2de  mov        edi, edi

// block 0044e2e0
0044e2e0  mov        edx, dword ptr [esp + 0x60]
0044e2e4  xor        eax, eax
0044e2e6  test       edx, edx
0044e2e8  jle        0x44e377

// block 0044e2ee
0044e2ee  mov        dword ptr [esp + 0x5c], edx
0044e2f2  mov        dword ptr [esp + 0x18], edx

// block 0044e2f6
0044e2f6  movzx      edx, byte ptr [edi]
0044e2f9  movzx      eax, byte ptr [ecx]
0044e2fc  movzx      esi, byte ptr [ecx + 1]
0044e300  shr        eax, 4
0044e303  shr        edx, 4
0044e306  sub        eax, edx
0044e308  shr        esi, 4
0044e30b  imul       eax, esi
0044e30e  sar        eax, 4
0044e311  add        al, dl
0044e313  shl        al, 4
0044e316  mov        byte ptr [edi], al
0044e318  movzx      edx, byte ptr [ecx]
0044e31b  movzx      eax, al
0044e31e  and        eax, 0xf
0044e321  and        edx, 0xf
0044e324  sub        edx, eax
0044e326  imul       edx, esi
0044e329  sar        edx, 4
0044e32c  add        dl, al
0044e32e  movzx      eax, byte ptr [edi + 1]
0044e332  or         byte ptr [edi], dl
0044e334  movzx      ebp, byte ptr [ecx + 1]
0044e338  mov        edx, eax
0044e33a  shr        eax, 2
0044e33d  and        eax, 0x3c
0044e340  add        eax, esi
0044e342  and        edx, 0xf
0044e345  and        ebp, 0xf
0044e348  cmp        eax, 0x10
0044e34b  jl         0x44e352

// block 0044e34d
0044e34d  mov        eax, 0xf

// block 0044e352
0044e352  mov        ebx, ebp
0044e354  sub        ebx, edx
0044e356  imul       ebx, esi
0044e359  sar        ebx, 4
0044e35c  add        bl, dl
0044e35e  shl        al, 4
0044e361  or         bl, al
0044e363  mov        byte ptr [edi + 1], bl
0044e366  add        edi, 2
0044e369  add        ecx, 2
0044e36c  sub        dword ptr [esp + 0x5c], 1
0044e371  jne        0x44e2f6

// block 0044e373
0044e373  mov        eax, dword ptr [esp + 0x18]

// block 0044e377
0044e377  mov        edx, dword ptr [esp + 0x64]
0044e37b  add        eax, eax
0044e37d  sub        edx, eax
0044e37f  add        ecx, edx
0044e381  mov        edx, dword ptr [esp + 0x10]
0044e385  sub        edx, eax
0044e387  add        edi, edx
0044e389  sub        dword ptr [esp + 0x14], 1
0044e38e  jne        0x44e2e0

// block 0044e394
0044e394  jmp        0x44e489

// block 0044e399
0044e399  mov        eax, dword ptr [esp + 0x5c]
0044e39d  imul       esi, ebx
0044e3a0  add        esi, dword ptr [esp + 0x20]
0044e3a4  lea        edx, [esi + eax*2]
0044e3a7  test       ebp, ebp
0044e3a9  jle        0x44e489

// block 0044e3af
0044e3af  nop        

// block 0044e3b0
0044e3b0  mov        eax, dword ptr [esp + 0x60]
0044e3b4  xor        edi, edi
0044e3b6  test       eax, eax
0044e3b8  jle        0x44e3d8

// block 0044e3ba
0044e3ba  mov        esi, eax
0044e3bc  mov        edi, eax
0044e3be  mov        edi, edi

// block 0044e3c0
0044e3c0  movzx      eax, word ptr [ecx]
0044e3c3  test       eax, 0x8000
0044e3c8  je         0x44e3cd

// block 0044e3ca
0044e3ca  mov        word ptr [edx], ax

// block 0044e3cd
0044e3cd  add        edx, 2
0044e3d0  add        ecx, 2
0044e3d3  sub        esi, 1
0044e3d6  jne        0x44e3c0

// block 0044e3d8
0044e3d8  mov        esi, dword ptr [esp + 0x64]
0044e3dc  lea        eax, [edi + edi]
0044e3df  sub        esi, eax
0044e3e1  add        ecx, esi
0044e3e3  mov        esi, dword ptr [esp + 0x10]
0044e3e7  sub        esi, eax
0044e3e9  add        edx, esi
0044e3eb  sub        ebp, 1
0044e3ee  jne        0x44e3b0

// block 0044e3f0
0044e3f0  jmp        0x44e489

// block 0044e3f5
0044e3f5  mov        edx, dword ptr [esp + 0x5c]
0044e3f9  imul       esi, ebx
0044e3fc  add        esi, dword ptr [esp + 0x20]
0044e400  lea        esi, [esi + edx*4]
0044e403  test       ebp, ebp
0044e405  jle        0x44e489

// block 0044e40b
0044e40b  mov        dword ptr [esp + 0x5c], ebp
0044e40f  nop        

// block 0044e410
0044e410  mov        ebp, dword ptr [esp + 0x60]
0044e414  xor        edx, edx
0044e416  test       ebp, ebp
0044e418  jle        0x44e46b

// block 0044e41a
0044e41a  mov        edx, ebp
0044e41c  lea        esp, [esp]

// block 0044e420
0044e420  movzx      edi, byte ptr [ecx + 3]
0044e424  movzx      eax, byte ptr [esi]
0044e427  movzx      ebx, byte ptr [ecx]
0044e42a  sub        ebx, eax
0044e42c  imul       ebx, edi
0044e42f  sar        ebx, 8
0044e432  add        bl, al
0044e434  movzx      eax, byte ptr [esi + 1]
0044e438  mov        byte ptr [esi], bl
0044e43a  movzx      ebx, byte ptr [ecx + 1]
0044e43e  sub        ebx, eax
0044e440  imul       ebx, edi
0044e443  sar        ebx, 8
0044e446  add        bl, al
0044e448  movzx      eax, byte ptr [esi + 2]
0044e44c  mov        byte ptr [esi + 1], bl
0044e44f  movzx      ebx, byte ptr [ecx + 2]
0044e453  sub        ebx, eax
0044e455  imul       ebx, edi
0044e458  sar        ebx, 8
0044e45b  add        bl, al
0044e45d  mov        byte ptr [esi + 2], bl
0044e460  add        esi, 4
0044e463  add        ecx, 4
0044e466  sub        ebp, 1
0044e469  jne        0x44e420

// block 0044e46b
0044e46b  lea        eax, [edx*4]
0044e472  mov        edx, dword ptr [esp + 0x64]
0044e476  sub        edx, eax
0044e478  add        ecx, edx
0044e47a  mov        edx, dword ptr [esp + 0x10]
0044e47e  sub        edx, eax
0044e480  add        esi, edx
0044e482  sub        dword ptr [esp + 0x5c], 1
0044e487  jne        0x44e410

// block 0044e489
0044e489  mov        eax, dword ptr [esp + 0x58]
0044e48d  mov        ecx, dword ptr [eax]
0044e48f  mov        edx, dword ptr [ecx + 0x38]
0044e492  push       eax
0044e493  call       edx

// block 0044e495
0044e495  pop        esi
0044e496  pop        edi
0044e497  pop        ebp
0044e498  mov        al, 1
0044e49a  pop        ebx
0044e49b  add        esp, 0x44
0044e49e  ret        0x10
