
// block 0043c590
0043c590  push       ecx
0043c591  push       ebx
0043c592  mov        ebx, dword ptr [0x4b4518]
0043c598  mov        eax, dword ptr [ebx + 8]
0043c59b  push       ebp
0043c59c  xor        ebp, ebp
0043c59e  push       esi
0043c59f  push       edi
0043c5a0  mov        ecx, 2
0043c5a5  cmp        eax, ebp
0043c5a7  je         0x43c5ac

// block 0043c5a9
0043c5a9  or         dword ptr [eax + 4], ecx

// block 0043c5ac
0043c5ac  mov        eax, dword ptr [ebx + 0x1d4]
0043c5b2  cmp        eax, ebp
0043c5b4  je         0x43c5b9

// block 0043c5b6
0043c5b6  or         dword ptr [eax + 4], ecx

// block 0043c5b9
0043c5b9  mov        eax, dword ptr [ebx + 0xc]
0043c5bc  cmp        eax, ebp
0043c5be  je         0x43c5c3

// block 0043c5c0
0043c5c0  or         dword ptr [eax + 4], ecx

// block 0043c5c3
0043c5c3  mov        eax, dword ptr [ebx + 0x10]
0043c5c6  cmp        eax, ebp
0043c5c8  jne        0x43c6c0

// block 0043c5ce
0043c5ce  mov        eax, dword ptr [0x4b0cb0]
0043c5d3  mov        esi, dword ptr [ebx + eax*4 + 0x20]
0043c5d7  mov        ecx, ebx
0043c5d9  call       0x43ccd0

// block 0043c5de
0043c5de  mov        edi, dword ptr [0x4b0cb0]
0043c5e4  call       0x43cbb0

// block 0043c5e9
0043c5e9  mov        dword ptr [ebx + 0xa0], eax
0043c5ef  cmp        dword ptr [0x4cee4c], ebp
0043c5f5  jne        0x43c673

// block 0043c5f7
0043c5f7  mov        eax, dword ptr [0x4b0c44]
0043c5fc  mov        dword ptr [esi + 0xc], eax
0043c5ff  movzx      ecx, word ptr [0x4b0c48]
0043c606  mov        word ptr [esi + 0x10], cx
0043c60a  mov        edx, dword ptr [0x4b0c78]
0043c610  mov        dword ptr [esi + 0x14], edx
0043c613  movzx      eax, word ptr [0x4b0c98]
0043c61a  mov        word ptr [esi + 0x18], ax
0043c61e  movzx      ecx, word ptr [0x4b0c9c]
0043c625  mov        word ptr [esi + 0x1a], cx
0043c629  mov        edx, dword ptr [0x4b0ccc]
0043c62f  mov        dword ptr [esi + 0x2c], edx
0043c632  mov        eax, dword ptr [0x4b0cc4]
0043c637  mov        dword ptr [esi + 0x38], eax
0043c63a  mov        ecx, dword ptr [0x4b0cdc]
0043c640  mov        dword ptr [esi + 0x44], ecx
0043c643  mov        dx, word ptr [0x4b0ca0]
0043c64a  mov        word ptr [esi + 0x1c], dx
0043c64e  movzx      eax, word ptr [0x4b0ca4]
0043c655  mov        word ptr [esi + 0x1e], ax
0043c659  mov        ecx, dword ptr [0x4b0c4c]
0043c65f  mov        dword ptr [esi + 0x20], ecx
0043c662  mov        edx, dword ptr [0x4b0c50]
0043c668  mov        dword ptr [esi + 0x24], edx
0043c66b  mov        eax, dword ptr [0x4b0c54]
0043c670  mov        dword ptr [esi + 0x28], eax

// block 0043c673
0043c673  mov        eax, dword ptr [0x4b4514]
0043c678  mov        ecx, dword ptr [eax + 0x988]
0043c67e  mov        dword ptr [esi + 0x30], ecx
0043c681  mov        edx, dword ptr [eax + 0x98c]
0043c687  mov        dword ptr [esi + 0x34], edx
0043c68a  mov        ecx, dword ptr [0x4b0cd8]
0043c690  mov        dword ptr [esi + 0x3c], ecx
0043c693  mov        edx, dword ptr [0x4b0cb0]
0043c699  mov        dword ptr [ebx + 0x1d0], ebp
0043c69f  mov        dword ptr [ebx + 0x1d8], edx
0043c6a5  mov        eax, dword ptr [eax + 0xc598]
0043c6ab  mov        ecx, edx
0043c6ad  mov        edx, dword ptr [ebx + ecx*4 + 0x20]
0043c6b1  mov        dword ptr [edx + 0x40], eax
0043c6b4  mov        dword ptr [ebx + 0x1d0], ebp
0043c6ba  pop        edi
0043c6bb  pop        esi
0043c6bc  pop        ebp
0043c6bd  pop        ebx
0043c6be  pop        ecx
0043c6bf  ret        

// block 0043c6c0
0043c6c0  cmp        eax, 1
0043c6c3  jne        0x43c723

// block 0043c6c5
0043c6c5  mov        eax, dword ptr [0x4b0cb0]
0043c6ca  lea        ecx, [eax + eax*8]
0043c6cd  mov        esi, dword ptr [ebx + ecx*4 + 0xb8]
0043c6d4  lea        ecx, [esi + 0x30]
0043c6d7  mov        dword ptr [ebx + 0x1d8], eax
0043c6dd  call       0x43add0

// block 0043c6e2
0043c6e2  mov        eax, dword ptr [0x4b4514]
0043c6e7  mov        edx, dword ptr [esi + 0x40]
0043c6ea  mov        dword ptr [eax + 0xc598], edx
0043c6f0  mov        eax, dword ptr [0x4b0cb0]
0043c6f5  cmp        eax, 3
0043c6f8  jne        0x43c706

// block 0043c6fa
0043c6fa  mov        dword ptr [ebx + 0x14], 1
0043c701  mov        eax, dword ptr [0x4b0cb0]

// block 0043c706
0043c706  lea        ecx, [eax + eax*8]
0043c709  mov        edx, dword ptr [ebx + ecx*4 + 0xa8]
0043c710  lea        eax, [ebx + ecx*4 + 0xa8]
0043c717  mov        ecx, dword ptr [eax + 8]
0043c71a  mov        dword ptr [eax + 4], edx
0043c71d  mov        dword ptr [eax + 0xc], ecx
0043c720  mov        dword ptr [eax + 0x14], ebp

// block 0043c723
0043c723  pop        edi
0043c724  pop        esi
0043c725  mov        dword ptr [ebx + 0x1d0], ebp
0043c72b  pop        ebp
0043c72c  pop        ebx
0043c72d  pop        ecx
0043c72e  ret        
