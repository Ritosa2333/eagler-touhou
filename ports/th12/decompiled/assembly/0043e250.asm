
// block 0043e250
0043e250  push       ebx
0043e251  push       ebp
0043e252  mov        ebp, dword ptr [esp + 0xc]
0043e256  push       esi
0043e257  push       edi
0043e258  mov        edi, dword ptr [0x4b4524]
0043e25e  cmp        dword ptr [edi + 0x14], 0xa
0043e262  mov        ebx, eax
0043e264  jl         0x43e26d

// block 0043e266
0043e266  mov        dword ptr [edi + 0x14], 0

// block 0043e26d
0043e26d  mov        eax, dword ptr [edi + 0x14]
0043e270  shl        eax, 6
0043e273  lea        esi, [eax + edi + 0x4cc]
0043e27a  mov        byte ptr [esi + 0x38], 1
0043e27e  mov        dword ptr [esp + 0x14], 0
0043e286  test       ebx, ebx
0043e288  jl         0x43e32c

// block 0043e28e
0043e28e  je         0x43e2bf

// block 0043e290
0043e290  mov        eax, 0x66666667
0043e295  imul       ebx
0043e297  sar        edx, 2
0043e29a  mov        ecx, edx
0043e29c  shr        ecx, 0x1f
0043e29f  add        ecx, edx
0043e2a1  mov        al, cl
0043e2a3  mov        dl, 0xa
0043e2a5  imul       dl
0043e2a7  sub        bl, al
0043e2a9  mov        eax, dword ptr [esp + 0x14]
0043e2ad  mov        byte ptr [eax + esi], bl
0043e2b0  inc        eax
0043e2b1  mov        ebx, ecx
0043e2b3  mov        dword ptr [esp + 0x14], eax
0043e2b7  test       ebx, ebx
0043e2b9  jne        0x43e290

// block 0043e2bb
0043e2bb  test       eax, eax
0043e2bd  jne        0x43e2ca

// block 0043e2bf
0043e2bf  mov        byte ptr [esi], 0

// block 0043e2c2
0043e2c2  mov        dword ptr [esp + 0x14], 1

// block 0043e2ca
0043e2ca  mov        al, byte ptr [esp + 0x14]
0043e2ce  fldz       
0043e2d0  mov        ecx, dword ptr [esp + 0x18]
0043e2d4  mov        byte ptr [esi + 0x39], al
0043e2d7  mov        dword ptr [esi + 0x18], ecx
0043e2da  mov        eax, dword ptr [esi + 0x2c]
0043e2dd  test       al, 1
0043e2df  jne        0x43e2ff

// block 0043e2e1
0043e2e1  or         eax, 1
0043e2e4  fst        dword ptr [esi + 0x24]
0043e2e7  mov        dword ptr [esi + 0x20], 0
0043e2ee  mov        dword ptr [esi + 0x1c], 0xfff0bdc1
0043e2f5  mov        dword ptr [esi + 0x28], 0x4b2ed0
0043e2fc  mov        dword ptr [esi + 0x2c], eax

// block 0043e2ff
0043e2ff  fstp       dword ptr [esi + 0x24]
0043e302  mov        dword ptr [esi + 0x20], 0
0043e309  mov        dword ptr [esi + 0x1c], 0xffffffff
0043e310  mov        edx, dword ptr [ebp]
0043e313  mov        dword ptr [esi + 0xc], edx
0043e316  mov        eax, dword ptr [ebp + 4]
0043e319  mov        dword ptr [esi + 0x10], eax
0043e31c  mov        ecx, dword ptr [ebp + 8]
0043e31f  mov        dword ptr [esi + 0x14], ecx
0043e322  inc        dword ptr [edi + 0x14]
0043e325  pop        edi
0043e326  pop        esi
0043e327  pop        ebp
0043e328  pop        ebx
0043e329  ret        8

// block 0043e32c
0043e32c  mov        byte ptr [esi], 0xa
0043e32f  jmp        0x43e2c2
