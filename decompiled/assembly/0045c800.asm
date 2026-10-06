
// block 0045c800
0045c800  push       ebp
0045c801  mov        ebp, dword ptr [esp + 8]
0045c805  cmp        dword ptr [ebp + 0x4b56a0], 0
0045c80c  push       esi
0045c80d  push       edi
0045c80e  mov        edi, eax
0045c810  je         0x45c819

// block 0045c812
0045c812  mov        esi, ebp
0045c814  call       0x45a3c0

// block 0045c819
0045c819  cmp        byte ptr [ebp + 0x4b5642], 3
0045c820  je         0x45c83e

// block 0045c822
0045c822  mov        eax, dword ptr [0x4ce8f0]
0045c827  mov        ecx, dword ptr [eax]
0045c829  mov        edx, dword ptr [ecx + 0x164]
0045c82f  push       0x144
0045c834  push       eax
0045c835  call       edx

// block 0045c837
0045c837  mov        byte ptr [ebp + 0x4b5642], 3

// block 0045c83e
0045c83e  mov        eax, ebp
0045c840  call       0x459cf0

// block 0045c845
0045c845  mov        eax, dword ptr [edi + 0x3f4]
0045c84b  mov        eax, dword ptr [eax + 8]
0045c84e  cmp        dword ptr [ebp + 0x4b563c], eax
0045c854  je         0x45c872

// block 0045c856
0045c856  mov        dword ptr [ebp + 0x4b563c], eax
0045c85c  mov        eax, dword ptr [eax]
0045c85e  mov        ecx, dword ptr [0x4ce8f0]
0045c864  mov        edx, dword ptr [ecx]
0045c866  push       eax
0045c867  push       0
0045c869  push       ecx
0045c86a  mov        ecx, dword ptr [edx + 0x104]
0045c870  call       ecx

// block 0045c872
0045c872  mov        esi, dword ptr [0x4ce8cc]
0045c878  call       0x45a3c0

// block 0045c87d
0045c87d  mov        eax, dword ptr [0x4ce8f0]
0045c882  mov        edx, dword ptr [eax]
0045c884  push       0
0045c886  push       0xe
0045c888  push       eax
0045c889  mov        eax, dword ptr [edx + 0xe4]
0045c88f  call       eax

// block 0045c891
0045c891  cmp        byte ptr [ebp + 0x4b5642], 1
0045c898  je         0x45c8cd

// block 0045c89a
0045c89a  mov        eax, dword ptr [0x4ce8f0]
0045c89f  mov        ecx, dword ptr [eax]
0045c8a1  mov        edx, dword ptr [ecx + 0x10c]
0045c8a7  push       0
0045c8a9  push       6
0045c8ab  push       0
0045c8ad  push       eax
0045c8ae  call       edx

// block 0045c8b0
0045c8b0  mov        eax, dword ptr [0x4ce8f0]
0045c8b5  mov        ecx, dword ptr [eax]
0045c8b7  mov        edx, dword ptr [ecx + 0x10c]
0045c8bd  push       0
0045c8bf  push       3
0045c8c1  push       0
0045c8c3  push       eax
0045c8c4  call       edx

// block 0045c8c6
0045c8c6  mov        byte ptr [ebp + 0x4b5642], 1

// block 0045c8cd
0045c8cd  mov        edx, dword ptr [esp + 0x14]
0045c8d1  mov        eax, dword ptr [0x4ce8f0]
0045c8d6  mov        ecx, dword ptr [eax]
0045c8d8  push       0x1c
0045c8da  push       edx
0045c8db  mov        edx, dword ptr [esp + 0x20]
0045c8df  add        edx, -2
0045c8e2  push       edx
0045c8e3  push       6
0045c8e5  push       eax
0045c8e6  mov        eax, dword ptr [ecx + 0x14c]
0045c8ec  call       eax

// block 0045c8ee
0045c8ee  pop        edi
0045c8ef  pop        esi
0045c8f0  xor        eax, eax
0045c8f2  pop        ebp
0045c8f3  ret        0xc
