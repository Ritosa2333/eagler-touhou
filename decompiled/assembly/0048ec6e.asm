
// block 0048ec6e
0048ec6e  push       0x10
0048ec70  push       0x4ab1c8
0048ec75  call       0x46fcec

// block 0048ec7a
0048ec7a  mov        eax, dword ptr [ebp + 8]
0048ec7d  cmp        eax, -2
0048ec80  jne        0x48ec95

// block 0048ec82
0048ec82  call       0x46e96f

// block 0048ec87
0048ec87  mov        dword ptr [eax], 9

// block 0048ec8d
0048ec8d  or         eax, 0xffffffff
0048ec90  jmp        0x48ed3f

// block 0048ec95
0048ec95  xor        ebx, ebx
0048ec97  cmp        eax, ebx
0048ec99  jl         0x48eca3

// block 0048ec9b
0048ec9b  cmp        eax, dword ptr [0x4d6308]
0048eca1  jb         0x48ecbd

// block 0048eca3
0048eca3  call       0x46e96f

// block 0048eca8
0048eca8  mov        dword ptr [eax], 9
0048ecae  push       ebx
0048ecaf  push       ebx
0048ecb0  push       ebx
0048ecb1  push       ebx
0048ecb2  push       ebx
0048ecb3  call       0x470f2d

// block 0048ecb8
0048ecb8  add        esp, 0x14
0048ecbb  jmp        0x48ec8d

// block 0048ecbd
0048ecbd  mov        ecx, eax
0048ecbf  sar        ecx, 5
0048ecc2  lea        edi, [ecx*4 + 0x4d6320]
0048ecc9  mov        esi, eax
0048eccb  and        esi, 0x1f
0048ecce  shl        esi, 6
0048ecd1  mov        ecx, dword ptr [edi]
0048ecd3  movsx      ecx, byte ptr [esi + ecx + 4]
0048ecd8  and        ecx, 1
0048ecdb  je         0x48eca3

// block 0048ecdd
0048ecdd  push       eax
0048ecde  call       0x48cb3d

// block 0048ece3
0048ece3  pop        ecx
0048ece4  mov        dword ptr [ebp - 4], ebx
0048ece7  mov        eax, dword ptr [edi]
0048ece9  test       byte ptr [esi + eax + 4], 1
0048ecee  je         0x48ed21

// block 0048ecf0
0048ecf0  push       dword ptr [ebp + 8]
0048ecf3  call       0x48cac6

// block 0048ecf8
0048ecf8  pop        ecx
0048ecf9  push       eax
0048ecfa  call       dword ptr [0x4981b4]

// block 0048ed00
0048ed00  test       eax, eax
0048ed02  jne        0x48ed0f

// block 0048ed04
0048ed04  call       dword ptr [0x4980e4]

// block 0048ed0a
0048ed0a  mov        dword ptr [ebp - 0x1c], eax
0048ed0d  jmp        0x48ed12

// block 0048ed0f
0048ed0f  mov        dword ptr [ebp - 0x1c], ebx

// block 0048ed12
0048ed12  cmp        dword ptr [ebp - 0x1c], ebx
0048ed15  je         0x48ed30

// block 0048ed17
0048ed17  call       0x46e982

// block 0048ed1c
0048ed1c  mov        ecx, dword ptr [ebp - 0x1c]
0048ed1f  mov        dword ptr [eax], ecx

// block 0048ed21
0048ed21  call       0x46e96f

// block 0048ed26
0048ed26  mov        dword ptr [eax], 9
0048ed2c  or         dword ptr [ebp - 0x1c], 0xffffffff

// block 0048ed30
0048ed30  mov        dword ptr [ebp - 4], 0xfffffffe
0048ed37  call       0x48ed45

// block 0048ed3c
0048ed3c  mov        eax, dword ptr [ebp - 0x1c]

// block 0048ed3f
0048ed3f  call       0x46fd31

// block 0048ed44
0048ed44  ret        
