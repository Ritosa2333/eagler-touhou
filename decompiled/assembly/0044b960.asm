
// block 0044b960
0044b960  sub        esp, 0x14
0044b963  mov        ecx, dword ptr [esi + 0xc]
0044b966  mov        dword ptr [esp], 0
0044b96d  test       ecx, ecx
0044b96f  jne        0x44b977

// block 0044b971
0044b971  xor        al, al
0044b973  add        esp, 0x14
0044b976  ret        

// block 0044b977
0044b977  mov        eax, dword ptr [ecx]
0044b979  mov        eax, dword ptr [eax]
0044b97b  push       ebx
0044b97c  push       ebp
0044b97d  push       edi
0044b97e  push       0x4a2334
0044b983  push       edx
0044b984  call       eax

// block 0044b986
0044b986  test       al, al
0044b988  je         0x44ba90

// block 0044b98e
0044b98e  mov        ecx, dword ptr [esi + 0xc]
0044b991  mov        edx, dword ptr [ecx]
0044b993  mov        edx, dword ptr [edx + 8]
0044b996  push       0x10
0044b998  lea        eax, [esp + 0x14]
0044b99c  push       eax
0044b99d  call       edx

// block 0044b99f
0044b99f  test       eax, eax
0044b9a1  je         0x44ba90

// block 0044b9a7
0044b9a7  push       0x10
0044b9a9  push       0x10
0044b9ab  push       0x37
0044b9ad  lea        eax, [esp + 0x1c]
0044b9b1  push       0x10
0044b9b3  push       eax
0044b9b4  mov        al, 0x1b
0044b9b6  call       0x4639d0

// block 0044b9bb
0044b9bb  cmp        dword ptr [esp + 0x10], 0x31414854
0044b9c3  jne        0x44ba90

// block 0044b9c9
0044b9c9  mov        ecx, dword ptr [esp + 0x1c]
0044b9cd  sub        dword ptr [esp + 0x14], 0x75bcd15
0044b9d5  sub        dword ptr [esp + 0x18], 0x3ade68b1
0044b9dd  add        ecx, 0xf7e7f8ac
0044b9e3  mov        dword ptr [esi + 4], ecx
0044b9e6  mov        ecx, dword ptr [esi + 0xc]
0044b9e9  mov        edx, dword ptr [ecx]
0044b9eb  mov        eax, dword ptr [edx + 0x14]
0044b9ee  call       eax

// block 0044b9f0
0044b9f0  sub        eax, dword ptr [esp + 0x18]
0044b9f4  mov        ecx, dword ptr [esi + 0xc]
0044b9f7  mov        edx, dword ptr [ecx]
0044b9f9  mov        ebp, eax
0044b9fb  mov        eax, dword ptr [edx + 0x18]
0044b9fe  push       0
0044ba00  push       ebp
0044ba01  call       eax

// block 0044ba03
0044ba03  mov        ebx, dword ptr [esp + 0x18]
0044ba07  push       ebx
0044ba08  call       0x46d04a

// block 0044ba0d
0044ba0d  mov        edi, eax
0044ba0f  add        esp, 4
0044ba12  test       edi, edi
0044ba14  je         0x44ba90

// block 0044ba16
0044ba16  mov        ecx, dword ptr [esi + 0xc]
0044ba19  mov        edx, dword ptr [ecx]
0044ba1b  mov        eax, dword ptr [edx + 8]
0044ba1e  push       ebx
0044ba1f  push       edi
0044ba20  call       eax

// block 0044ba22
0044ba22  test       eax, eax
0044ba24  je         0x44ba76

// block 0044ba26
0044ba26  push       ebx
0044ba27  push       0x80
0044ba2c  push       0x9b
0044ba31  push       ebx
0044ba32  push       edi
0044ba33  mov        al, 0x3e
0044ba35  call       0x4639d0

// block 0044ba3a
0044ba3a  mov        eax, dword ptr [esp + 0x14]
0044ba3e  push       0
0044ba40  push       ebx
0044ba41  push       edi
0044ba42  call       0x44c710

// block 0044ba47
0044ba47  mov        ebx, eax
0044ba49  test       ebx, ebx
0044ba4b  je         0x44ba7a

// block 0044ba4d
0044ba4d  mov        ecx, dword ptr [esi + 4]
0044ba50  push       ebp
0044ba51  push       ecx
0044ba52  push       ebx
0044ba53  call       0x44bab0

// block 0044ba58
0044ba58  mov        dword ptr [esi], eax
0044ba5a  test       eax, eax
0044ba5c  je         0x44ba7a

// block 0044ba5e
0044ba5e  push       edi
0044ba5f  call       0x46c941

// block 0044ba64
0044ba64  push       ebx
0044ba65  call       0x46c941

// block 0044ba6a
0044ba6a  add        esp, 8
0044ba6d  pop        edi
0044ba6e  pop        ebp
0044ba6f  mov        al, 1
0044ba71  pop        ebx
0044ba72  add        esp, 0x14
0044ba75  ret        

// block 0044ba76
0044ba76  mov        ebx, dword ptr [esp + 0xc]

// block 0044ba7a
0044ba7a  push       edi
0044ba7b  call       0x46c941

// block 0044ba80
0044ba80  add        esp, 4
0044ba83  test       ebx, ebx
0044ba85  je         0x44ba90

// block 0044ba87
0044ba87  push       ebx
0044ba88  call       0x46c941

// block 0044ba8d
0044ba8d  add        esp, 4

// block 0044ba90
0044ba90  mov        ecx, dword ptr [esi + 0xc]
0044ba93  test       ecx, ecx
0044ba95  je         0x44baa0

// block 0044ba97
0044ba97  mov        edx, dword ptr [ecx]
0044ba99  mov        eax, dword ptr [edx + 0x1c]
0044ba9c  push       1
0044ba9e  call       eax

// block 0044baa0
0044baa0  pop        edi
0044baa1  pop        ebp
0044baa2  mov        dword ptr [esi + 0xc], 0
0044baa9  xor        al, al
0044baab  pop        ebx
0044baac  add        esp, 0x14
0044baaf  ret        
