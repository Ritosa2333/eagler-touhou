
// block 0045f740
0045f740  sub        esp, 0x38
0045f743  push       ebp
0045f744  xor        ebp, ebp
0045f746  test       byte ptr [0x4ceae8], 1
0045f74d  mov        dword ptr [esp + 8], eax
0045f751  je         0x45f77a

// block 0045f753
0045f753  mov        eax, dword ptr [eax*4 + 0x4a2338]
0045f75a  cmp        eax, 0x15
0045f75d  je         0x45f772

// block 0045f75f
0045f75f  cmp        eax, ebp
0045f761  je         0x45f772

// block 0045f763
0045f763  cmp        eax, 0x14
0045f766  jne        0x45f77a

// block 0045f768
0045f768  mov        dword ptr [esp + 8], 3
0045f770  jmp        0x45f77a

// block 0045f772
0045f772  mov        dword ptr [esp + 8], 5

// block 0045f77a
0045f77a  mov        eax, dword ptr [esp + 0x40]
0045f77e  and        dword ptr [esi + 0x10], 0xfffffffe
0045f782  mov        dword ptr [esi + 8], eax
0045f785  mov        eax, dword ptr [esi]
0045f787  lea        edx, [esp + 4]
0045f78b  push       edx
0045f78c  mov        dword ptr [esp + 8], ebp
0045f790  mov        ecx, dword ptr [eax]
0045f792  push       ebp
0045f793  push       eax
0045f794  mov        eax, dword ptr [ecx + 0x48]
0045f797  call       eax

// block 0045f799
0045f799  cmp        dword ptr [esp + 0x44], ebp
0045f79d  jne        0x45f7e5

// block 0045f79f
0045f79f  mov        eax, dword ptr [esp + 4]
0045f7a3  mov        ecx, dword ptr [eax]
0045f7a5  lea        edx, [esp + 0x1c]
0045f7a9  push       edx
0045f7aa  push       eax
0045f7ab  mov        eax, dword ptr [ecx + 0x30]
0045f7ae  call       eax

// block 0045f7b0
0045f7b0  mov        ecx, dword ptr [esp + 0x34]
0045f7b4  mov        eax, dword ptr [esp + 0x40]
0045f7b8  mov        edx, dword ptr [esp + 0x38]
0045f7bc  push       ebp
0045f7bd  push       ebp
0045f7be  push       1
0045f7c0  push       ebp
0045f7c1  push       eax
0045f7c2  push       ebx
0045f7c3  mov        dword ptr [esp + 0x2c], ecx
0045f7c7  lea        ecx, [esp + 0x24]
0045f7cb  push       ecx
0045f7cc  mov        dword ptr [esp + 0x34], edx
0045f7d0  mov        edx, dword ptr [esp + 0x20]
0045f7d4  push       ebp
0045f7d5  push       edx
0045f7d6  mov        dword ptr [esp + 0x30], ebp
0045f7da  mov        dword ptr [esp + 0x34], edi
0045f7de  call       0x46c716

// block 0045f7e3
0045f7e3  jmp        0x45f855

// block 0045f7e5
0045f7e5  mov        eax, dword ptr [ebx + 0x1c]
0045f7e8  add        eax, ebx
0045f7ea  mov        dword ptr [esp + 0xc], ebp
0045f7ee  mov        dword ptr [esp + 0x10], ebp
0045f7f2  movsx      ecx, word ptr [eax + 8]
0045f7f6  mov        dword ptr [esp + 0x14], ecx
0045f7fa  movsx      edx, word ptr [eax + 0xa]
0045f7fe  mov        dword ptr [esp + 0x18], edx
0045f802  mov        dword ptr [esp + 0x1c], ebp
0045f806  mov        dword ptr [esp + 0x20], edi
0045f80a  movsx      ecx, word ptr [eax + 8]
0045f80e  mov        dword ptr [esp + 0x24], ecx
0045f812  movsx      edx, word ptr [eax + 0xa]
0045f816  push       ebp
0045f817  add        edx, edi
0045f819  push       1
0045f81b  mov        dword ptr [esp + 0x30], edx
0045f81f  movsx      ecx, word ptr [eax + 6]
0045f823  lea        edx, [esp + 0x14]
0045f827  push       edx
0045f828  movsx      edx, word ptr [eax + 8]
0045f82c  add        ecx, ecx
0045f82e  imul       edx, dword ptr [ecx + ecx + 0x4a235c]
0045f836  add        ecx, ecx
0045f838  mov        ecx, dword ptr [ecx + 0x4a2338]
0045f83e  push       ebp
0045f83f  push       edx
0045f840  push       ecx
0045f841  add        eax, 0x10
0045f844  push       eax
0045f845  mov        eax, dword ptr [esp + 0x20]
0045f849  lea        edx, [esp + 0x38]
0045f84d  push       edx
0045f84e  push       ebp
0045f84f  push       eax
0045f850  call       0x46c6ec

// block 0045f855
0045f855  mov        eax, dword ptr [esp + 4]
0045f859  mov        ecx, dword ptr [eax]
0045f85b  mov        edx, dword ptr [ecx + 8]
0045f85e  push       eax
0045f85f  call       edx

// block 0045f861
0045f861  mov        eax, esi
0045f863  call       0x45ee00

// block 0045f868
0045f868  mov        eax, dword ptr [esp + 8]
0045f86c  mov        ecx, dword ptr [eax*4 + 0x4a235c]
0045f873  mov        dword ptr [esi + 0xc], ecx
0045f876  xor        eax, eax
0045f878  pop        ebp
0045f879  add        esp, 0x38
0045f87c  ret        8
