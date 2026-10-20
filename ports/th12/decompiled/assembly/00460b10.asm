
// block 00460b10
00460b10  push       ecx
00460b11  push       ebx
00460b12  push       ebp
00460b13  mov        ebp, dword ptr [esp + 0x10]
00460b17  push       esi
00460b18  push       edi
00460b19  lea        edi, [eax + eax*4]
00460b1c  mov        esi, edx
00460b1e  mov        eax, dword ptr [esi + ebp*4 + 0x4b50c0]
00460b25  add        edi, edi
00460b27  mov        ebx, ecx
00460b29  mov        ecx, dword ptr [eax + 0x120]
00460b2f  add        edi, edi
00460b31  cmp        dword ptr [edi + ecx], 0
00460b35  je         0x460c25

// block 00460b3b
00460b3b  mov        edx, dword ptr [esp + 0x1c]
00460b3f  mov        eax, dword ptr [esi + edx*4 + 0x4b50c0]
00460b46  mov        ecx, dword ptr [eax + 0x120]
00460b4c  lea        ebx, [ebx + ebx*4]
00460b4f  add        ebx, ebx
00460b51  add        ebx, ebx
00460b53  cmp        dword ptr [ebx + ecx], 0
00460b57  je         0x460c25

// block 00460b5d
00460b5d  call       0x45a3c0

// block 00460b62
00460b62  mov        edx, dword ptr [esi + ebp*4 + 0x4b50c0]
00460b69  mov        eax, dword ptr [edx + 0x120]
00460b6f  mov        ecx, dword ptr [eax + edi]
00460b72  mov        edx, dword ptr [ecx]
00460b74  add        eax, edi
00460b76  mov        eax, dword ptr [eax]
00460b78  lea        ecx, [esp + 0x18]
00460b7c  push       ecx
00460b7d  mov        ecx, dword ptr [edx + 0x48]
00460b80  push       0
00460b82  push       eax
00460b83  call       ecx

// block 00460b85
00460b85  test       eax, eax
00460b87  jne        0x460c25

// block 00460b8d
00460b8d  mov        edx, dword ptr [esp + 0x1c]
00460b91  mov        eax, dword ptr [esi + edx*4 + 0x4b50c0]
00460b98  mov        ecx, dword ptr [eax + 0x120]
00460b9e  mov        edx, dword ptr [ecx + ebx]
00460ba1  lea        eax, [ecx + ebx]
00460ba4  mov        ecx, dword ptr [edx]
00460ba6  mov        eax, dword ptr [eax]
00460ba8  mov        ecx, dword ptr [ecx + 0x48]
00460bab  lea        edx, [esp + 0x10]
00460baf  push       edx
00460bb0  push       0
00460bb2  push       eax
00460bb3  call       ecx

// block 00460bb5
00460bb5  test       eax, eax
00460bb7  je         0x460bcd

// block 00460bb9
00460bb9  mov        eax, dword ptr [esp + 0x18]
00460bbd  mov        edx, dword ptr [eax]
00460bbf  push       eax
00460bc0  mov        eax, dword ptr [edx + 8]
00460bc3  call       eax

// block 00460bc5
00460bc5  pop        edi
00460bc6  pop        esi
00460bc7  pop        ebp
00460bc8  pop        ebx
00460bc9  pop        ecx
00460bca  ret        0x10

// block 00460bcd
00460bcd  mov        ecx, dword ptr [esp + 0x24]
00460bd1  mov        edx, dword ptr [esp + 0x10]
00460bd5  mov        eax, dword ptr [esp + 0x20]
00460bd9  push       0
00460bdb  push       -1
00460bdd  push       ecx
00460bde  mov        ecx, dword ptr [esp + 0x24]
00460be2  push       0
00460be4  push       edx
00460be5  push       eax
00460be6  push       0
00460be8  push       ecx
00460be9  call       0x46c71c

// block 00460bee
00460bee  test       eax, eax
00460bf0  mov        eax, dword ptr [esp + 0x18]
00460bf4  push       eax
00460bf5  je         0x460c12

// block 00460bf7
00460bf7  mov        edx, dword ptr [eax]
00460bf9  mov        eax, dword ptr [edx + 8]
00460bfc  call       eax

// block 00460bfe
00460bfe  mov        eax, dword ptr [esp + 0x10]
00460c02  mov        ecx, dword ptr [eax]
00460c04  mov        edx, dword ptr [ecx + 8]
00460c07  push       eax
00460c08  call       edx

// block 00460c0a
00460c0a  pop        edi
00460c0b  pop        esi
00460c0c  pop        ebp
00460c0d  pop        ebx
00460c0e  pop        ecx
00460c0f  ret        0x10

// block 00460c12
00460c12  mov        ecx, dword ptr [eax]
00460c14  mov        edx, dword ptr [ecx + 8]
00460c17  call       edx

// block 00460c19
00460c19  mov        eax, dword ptr [esp + 0x10]
00460c1d  mov        ecx, dword ptr [eax]
00460c1f  mov        edx, dword ptr [ecx + 8]
00460c22  push       eax
00460c23  call       edx

// block 00460c25
00460c25  pop        edi
00460c26  pop        esi
00460c27  pop        ebp
00460c28  pop        ebx
00460c29  pop        ecx
00460c2a  ret        0x10
