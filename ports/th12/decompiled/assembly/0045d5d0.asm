
// block 0045d5d0
0045d5d0  push       ecx
0045d5d1  push       ebx
0045d5d2  push       ebp
0045d5d3  mov        ebp, dword ptr [esp + 0x10]
0045d5d7  push       esi
0045d5d8  push       edi
0045d5d9  mov        edi, dword ptr [0x4ce8cc]
0045d5df  mov        eax, dword ptr [edi + 0x8856b0]
0045d5e5  lea        ebp, [ebp + ebp*4]
0045d5e9  lea        esi, [edi + 0x8856b0]
0045d5ef  add        ebp, ebp
0045d5f1  add        ebp, ebp
0045d5f3  lea        ebx, [eax + ebp + 0x14]
0045d5f7  cmp        ebx, esi
0045d5f9  jae        0x45d6ff

// block 0045d5ff
0045d5ff  mov        ebx, dword ptr [esp + 0x18]
0045d603  test       ebx, ebx
0045d605  jle        0x45d63d

// block 0045d607
0045d607  fldz       
0045d609  mov        dword ptr [esp + 0x10], ebx
0045d60d  fld1       

// block 0045d60f
0045d60f  fld        dword ptr [ecx]
0045d611  add        ecx, 8
0045d614  fstp       dword ptr [eax]
0045d616  add        edx, 4
0045d619  fld        dword ptr [ecx - 4]
0045d61c  add        eax, 0x14
0045d61f  sub        dword ptr [esp + 0x10], 1
0045d624  fstp       dword ptr [eax - 0x10]
0045d627  fxch       st(1)
0045d629  fst        dword ptr [eax - 0xc]
0045d62c  fxch       st(1)
0045d62e  fst        dword ptr [eax - 8]
0045d631  mov        ebx, dword ptr [edx - 4]
0045d634  mov        dword ptr [eax - 4], ebx
0045d637  jne        0x45d60f

// block 0045d639
0045d639  fstp       st(1)
0045d63b  fstp       st(0)

// block 0045d63d
0045d63d  mov        eax, dword ptr [0x4ce8cc]
0045d642  cmp        byte ptr [eax + 0x4b5647], 0
0045d649  je         0x45d689

// block 0045d64b
0045d64b  mov        eax, dword ptr [0x4ce8f0]
0045d650  mov        ecx, dword ptr [eax]
0045d652  mov        edx, dword ptr [ecx + 0x10c]
0045d658  push       3
0045d65a  push       4
0045d65c  push       0
0045d65e  push       eax
0045d65f  call       edx

// block 0045d661
0045d661  mov        eax, dword ptr [0x4ce8f0]
0045d666  mov        ecx, dword ptr [eax]
0045d668  mov        edx, dword ptr [ecx + 0x10c]
0045d66e  push       3
0045d670  mov        ebx, 1
0045d675  push       ebx
0045d676  push       0
0045d678  push       eax
0045d679  call       edx

// block 0045d67b
0045d67b  mov        eax, dword ptr [0x4ce8cc]
0045d680  mov        byte ptr [eax + 0x4b5647], 0
0045d687  jmp        0x45d68e

// block 0045d689
0045d689  mov        ebx, 1

// block 0045d68e
0045d68e  cmp        byte ptr [edi + 0x4b5642], bl
0045d694  je         0x45d6c8

// block 0045d696
0045d696  mov        eax, dword ptr [0x4ce8f0]
0045d69b  mov        ecx, dword ptr [eax]
0045d69d  mov        edx, dword ptr [ecx + 0x10c]
0045d6a3  push       0
0045d6a5  push       6
0045d6a7  push       0
0045d6a9  push       eax
0045d6aa  call       edx

// block 0045d6ac
0045d6ac  mov        eax, dword ptr [0x4ce8f0]
0045d6b1  mov        ecx, dword ptr [eax]
0045d6b3  mov        edx, dword ptr [ecx + 0x10c]
0045d6b9  push       0
0045d6bb  push       3
0045d6bd  push       0
0045d6bf  push       eax
0045d6c0  call       edx

// block 0045d6c2
0045d6c2  mov        byte ptr [edi + 0x4b5642], bl

// block 0045d6c8
0045d6c8  mov        eax, dword ptr [0x4ce8f0]
0045d6cd  mov        ecx, dword ptr [eax]
0045d6cf  mov        edx, dword ptr [ecx + 0x164]
0045d6d5  push       0x44
0045d6d7  push       eax
0045d6d8  call       edx

// block 0045d6da
0045d6da  mov        edx, dword ptr [esi]
0045d6dc  mov        eax, dword ptr [0x4ce8f0]
0045d6e1  mov        ecx, dword ptr [eax]
0045d6e3  push       0x14
0045d6e5  push       edx
0045d6e6  mov        edx, dword ptr [esp + 0x20]
0045d6ea  dec        edx
0045d6eb  push       edx
0045d6ec  push       3
0045d6ee  push       eax
0045d6ef  mov        eax, dword ptr [ecx + 0x14c]
0045d6f5  call       eax

// block 0045d6f7
0045d6f7  add        dword ptr [esi], ebp
0045d6f9  add        dword ptr [edi + 0xac], ebx

// block 0045d6ff
0045d6ff  pop        edi
0045d700  pop        esi
0045d701  pop        ebp
0045d702  xor        eax, eax
0045d704  pop        ebx
0045d705  pop        ecx
0045d706  ret        4
