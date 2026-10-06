
// block 0044c310
0044c310  push       -1
0044c312  push       0x496fa8
0044c317  mov        eax, dword ptr fs:[0]
0044c31d  push       eax
0044c31e  sub        esp, 0xc
0044c321  push       ebx
0044c322  push       ebp
0044c323  push       esi
0044c324  push       edi
0044c325  mov        eax, dword ptr [0x4ad138]
0044c32a  xor        eax, esp
0044c32c  push       eax
0044c32d  lea        eax, [esp + 0x20]
0044c331  mov        dword ptr fs:[0], eax
0044c337  mov        esi, ecx
0044c339  call       0x44c130

// block 0044c33e
0044c33e  mov        ebp, eax
0044c340  xor        edi, edi
0044c342  cmp        ebp, edi
0044c344  je         0x44c375

// block 0044c346
0044c346  push       0x2f
0044c348  push       esi
0044c349  call       0x46d1a0

// block 0044c34e
0044c34e  add        esp, 8
0044c351  cmp        eax, edi
0044c353  je         0x44c358

// block 0044c355
0044c355  lea        esi, [eax + 1]

// block 0044c358
0044c358  mov        ebx, esi
0044c35a  mov        eax, ebp
0044c35c  call       0x44b8e0

// block 0044c361
0044c361  mov        ecx, dword ptr [esp + 0x20]
0044c365  mov        dword ptr fs:[0], ecx
0044c36c  pop        ecx
0044c36d  pop        edi
0044c36e  pop        esi
0044c36f  pop        ebp
0044c370  pop        ebx
0044c371  add        esp, 0x18
0044c374  ret        

// block 0044c375
0044c375  mov        ebx, 0x4a22dc
0044c37a  mov        dword ptr [esp + 0x14], ebx
0044c37e  mov        dword ptr [esp + 0x18], 0xffffffff
0044c386  mov        dword ptr [esp + 0x1c], edi
0044c38a  push       0x4a2334
0044c38f  push       esi
0044c390  lea        ecx, [esp + 0x1c]
0044c394  mov        dword ptr [esp + 0x30], edi
0044c398  call       0x44bda0

// block 0044c39d
0044c39d  mov        eax, dword ptr [esp + 0x18]
0044c3a1  cmp        eax, -1
0044c3a4  je         0x44c3b4

// block 0044c3a6
0044c3a6  push       edi
0044c3a7  push       eax
0044c3a8  call       dword ptr [0x4980b0]

// block 0044c3ae
0044c3ae  mov        edi, eax
0044c3b0  mov        eax, dword ptr [esp + 0x18]

// block 0044c3b4
0044c3b4  mov        dword ptr [esp + 0x14], ebx
0044c3b8  cmp        eax, -1
0044c3bb  je         0x44c3c4

// block 0044c3bd
0044c3bd  push       eax
0044c3be  call       dword ptr [0x4980a4]

// block 0044c3c4
0044c3c4  mov        eax, edi
0044c3c6  mov        ecx, dword ptr [esp + 0x20]
0044c3ca  mov        dword ptr fs:[0], ecx
0044c3d1  pop        ecx
0044c3d2  pop        edi
0044c3d3  pop        esi
0044c3d4  pop        ebp
0044c3d5  pop        ebx
0044c3d6  add        esp, 0x18
0044c3d9  ret        
