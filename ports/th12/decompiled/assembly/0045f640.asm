
// block 0045f640
0045f640  sub        esp, 0x18
0045f643  test       byte ptr [0x4ceae8], 1
0045f64a  mov        dword ptr [esp + 4], eax
0045f64e  je         0x45f677

// block 0045f650
0045f650  mov        eax, dword ptr [eax*4 + 0x4a2338]
0045f657  cmp        eax, 0x15
0045f65a  je         0x45f66f

// block 0045f65c
0045f65c  test       eax, eax
0045f65e  je         0x45f66f

// block 0045f660
0045f660  cmp        eax, 0x14
0045f663  jne        0x45f677

// block 0045f665
0045f665  mov        dword ptr [esp + 4], 3
0045f66d  jmp        0x45f677

// block 0045f66f
0045f66f  mov        dword ptr [esp + 4], 5

// block 0045f677
0045f677  mov        eax, dword ptr [esi]
0045f679  and        dword ptr [esi + 0x10], 0xfffffffe
0045f67d  lea        edx, [esp]
0045f680  push       edx
0045f681  mov        dword ptr [esi + 8], ebx
0045f684  mov        dword ptr [esp + 4], 0
0045f68c  mov        ecx, dword ptr [eax]
0045f68e  push       0
0045f690  push       eax
0045f691  mov        eax, dword ptr [ecx + 0x48]
0045f694  call       eax

// block 0045f696
0045f696  cmp        dword ptr [esp + 0x1c], 0
0045f69b  push       0
0045f69d  jne        0x45f6b7

// block 0045f69f
0045f69f  mov        ecx, dword ptr [esp + 4]
0045f6a3  push       0
0045f6a5  push       1
0045f6a7  push       0
0045f6a9  push       ebx
0045f6aa  push       edi
0045f6ab  push       0
0045f6ad  push       0
0045f6af  push       ecx
0045f6b0  call       0x46c716

// block 0045f6b5
0045f6b5  jmp        0x45f714

// block 0045f6b7
0045f6b7  mov        edx, dword ptr [edi + 0x1c]
0045f6ba  lea        eax, [edx + edi]
0045f6bd  mov        dword ptr [esp + 0xc], 0
0045f6c5  mov        dword ptr [esp + 0x10], 0
0045f6cd  movsx      ecx, word ptr [eax + 8]
0045f6d1  mov        dword ptr [esp + 0x14], ecx
0045f6d5  movsx      edx, word ptr [eax + 0xa]
0045f6d9  push       1
0045f6db  mov        dword ptr [esp + 0x1c], edx
0045f6df  movsx      ecx, word ptr [eax + 6]
0045f6e3  lea        edx, [esp + 0x10]
0045f6e7  push       edx
0045f6e8  movsx      edx, word ptr [eax + 8]
0045f6ec  add        ecx, ecx
0045f6ee  imul       edx, dword ptr [ecx + ecx + 0x4a235c]
0045f6f6  add        ecx, ecx
0045f6f8  mov        ecx, dword ptr [ecx + 0x4a2338]
0045f6fe  push       0
0045f700  push       edx
0045f701  mov        edx, dword ptr [esp + 0x14]
0045f705  push       ecx
0045f706  add        eax, 0x10
0045f709  push       eax
0045f70a  push       0
0045f70c  push       0
0045f70e  push       edx
0045f70f  call       0x46c6ec

// block 0045f714
0045f714  mov        eax, dword ptr [esp]
0045f717  mov        ecx, dword ptr [eax]
0045f719  mov        edx, dword ptr [ecx + 8]
0045f71c  push       eax
0045f71d  call       edx

// block 0045f71f
0045f71f  mov        eax, esi
0045f721  call       0x45ee00

// block 0045f726
0045f726  mov        eax, dword ptr [esp + 4]
0045f72a  mov        ecx, dword ptr [eax*4 + 0x4a235c]
0045f731  mov        dword ptr [esi + 0xc], ecx
0045f734  xor        eax, eax
0045f736  add        esp, 0x18
0045f739  ret        4
