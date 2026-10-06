
// block 0040d880
0040d880  push       ebp
0040d881  push       esi
0040d882  push       0x24
0040d884  call       0x46c9ea

// block 0040d889
0040d889  xor        ebp, ebp
0040d88b  add        esp, 4
0040d88e  cmp        eax, ebp
0040d890  je         0x40d8ae

// block 0040d892
0040d892  and        dword ptr [eax + 4], 0xfffffffe
0040d896  mov        dword ptr [eax + 8], ebp
0040d899  mov        dword ptr [eax + 0xc], ebp
0040d89c  mov        dword ptr [eax + 0x10], ebp
0040d89f  mov        dword ptr [eax], ebp
0040d8a1  mov        dword ptr [eax + 0x14], eax
0040d8a4  mov        dword ptr [eax + 0x18], ebp
0040d8a7  mov        dword ptr [eax + 0x1c], ebp
0040d8aa  mov        esi, eax
0040d8ac  jmp        0x40d8b0

// block 0040d8ae
0040d8ae  xor        esi, esi

// block 0040d8b0
0040d8b0  mov        eax, dword ptr [esi + 4]
0040d8b3  and        eax, 0xfffffffd
0040d8b6  push       ebx
0040d8b7  or         eax, 1
0040d8ba  mov        ebx, 0x17
0040d8bf  mov        dword ptr [esi + 8], 0x40e040
0040d8c6  mov        dword ptr [esi + 0xc], ebp
0040d8c9  mov        dword ptr [esi + 0x10], ebp
0040d8cc  mov        dword ptr [esi + 0x20], edi
0040d8cf  mov        dword ptr [esi + 4], eax
0040d8d2  call       0x462380

// block 0040d8d7
0040d8d7  push       0x24
0040d8d9  mov        dword ptr [edi + 8], esi
0040d8dc  call       0x46c9ea

// block 0040d8e1
0040d8e1  add        esp, 4
0040d8e4  cmp        eax, ebp
0040d8e6  je         0x40d904

// block 0040d8e8
0040d8e8  and        dword ptr [eax + 4], 0xfffffffe
0040d8ec  mov        dword ptr [eax + 8], ebp
0040d8ef  mov        dword ptr [eax + 0xc], ebp
0040d8f2  mov        dword ptr [eax + 0x10], ebp
0040d8f5  mov        dword ptr [eax], ebp
0040d8f7  mov        dword ptr [eax + 0x14], eax
0040d8fa  mov        dword ptr [eax + 0x18], ebp
0040d8fd  mov        dword ptr [eax + 0x1c], ebp
0040d900  mov        esi, eax
0040d902  jmp        0x40d906

// block 0040d904
0040d904  xor        esi, esi

// block 0040d906
0040d906  mov        ecx, dword ptr [esi + 4]
0040d909  and        ecx, 0xfffffffd
0040d90c  or         ecx, 1
0040d90f  mov        ebx, 0xa
0040d914  mov        dword ptr [esi + 8], 0x40e050
0040d91b  mov        dword ptr [esi + 0xc], ebp
0040d91e  mov        dword ptr [esi + 0x10], ebp
0040d921  mov        dword ptr [esi + 0x20], edi
0040d924  mov        dword ptr [esi + 4], ecx
0040d927  call       0x462420

// block 0040d92c
0040d92c  fldz       
0040d92e  mov        dword ptr [edi + 0xc], esi
0040d931  mov        eax, dword ptr [edi + 0x34]
0040d934  pop        ebx
0040d935  test       al, 1
0040d937  jne        0x40d953

// block 0040d939
0040d939  or         eax, 1
0040d93c  fst        dword ptr [edi + 0x2c]
0040d93f  mov        dword ptr [edi + 0x28], ebp
0040d942  mov        dword ptr [edi + 0x24], 0xfff0bdc1
0040d949  mov        dword ptr [edi + 0x30], 0x4b2ed0
0040d950  mov        dword ptr [edi + 0x34], eax

// block 0040d953
0040d953  pop        esi
0040d954  fstp       dword ptr [edi + 0x2c]
0040d957  mov        dword ptr [edi + 0x28], ebp
0040d95a  mov        dword ptr [edi + 0x24], 0xffffffff
0040d961  xor        eax, eax
0040d963  pop        ebp
0040d964  ret        
