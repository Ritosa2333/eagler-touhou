
// block 0041d250
0041d250  push       ebx
0041d251  push       ebp
0041d252  mov        ebp, dword ptr [esp + 0xc]
0041d256  push       edi
0041d257  mov        ebx, 0x49fce4
0041d25c  mov        ecx, 5
0041d261  call       0x45fe60

// block 0041d266
0041d266  xor        edi, edi
0041d268  mov        dword ptr [ebp + 0x6d44], eax
0041d26e  cmp        eax, edi
0041d270  jne        0x41d28d

// block 0041d272
0041d272  push       0x49f434
0041d277  mov        ecx, 0x4b0ec8
0041d27c  call       0x464220

// block 0041d281
0041d281  add        esp, 4

// block 0041d284
0041d284  pop        edi
0041d285  pop        ebp
0041d286  or         eax, 0xffffffff
0041d289  pop        ebx
0041d28a  ret        4

// block 0041d28d
0041d28d  push       ebp
0041d28e  call       0x41d3b0

// block 0041d293
0041d293  test       eax, eax
0041d295  jne        0x41d284

// block 0041d297
0041d297  push       esi
0041d298  push       0x24
0041d29a  call       0x46c9ea

// block 0041d29f
0041d29f  add        esp, 4
0041d2a2  cmp        eax, edi
0041d2a4  je         0x41d2c2

// block 0041d2a6
0041d2a6  and        dword ptr [eax + 4], 0xfffffffe
0041d2aa  mov        dword ptr [eax + 8], edi
0041d2ad  mov        dword ptr [eax + 0xc], edi
0041d2b0  mov        dword ptr [eax + 0x10], edi
0041d2b3  mov        dword ptr [eax], edi
0041d2b5  mov        dword ptr [eax + 0x14], eax
0041d2b8  mov        dword ptr [eax + 0x18], edi
0041d2bb  mov        dword ptr [eax + 0x1c], edi
0041d2be  mov        esi, eax
0041d2c0  jmp        0x41d2c4

// block 0041d2c2
0041d2c2  xor        esi, esi

// block 0041d2c4
0041d2c4  mov        eax, dword ptr [esi + 4]
0041d2c7  and        eax, 0xfffffffd
0041d2ca  or         eax, 1
0041d2cd  mov        ebx, 0x19
0041d2d2  mov        dword ptr [esi + 8], 0x41f8d0
0041d2d9  mov        dword ptr [esi + 0xc], edi
0041d2dc  mov        dword ptr [esi + 0x10], edi
0041d2df  mov        dword ptr [esi + 0x20], ebp
0041d2e2  mov        dword ptr [esi + 4], eax
0041d2e5  call       0x462380

// block 0041d2ea
0041d2ea  push       0x24
0041d2ec  mov        dword ptr [ebp + 8], esi
0041d2ef  call       0x46c9ea

// block 0041d2f4
0041d2f4  add        esp, 4
0041d2f7  cmp        eax, edi
0041d2f9  je         0x41d317

// block 0041d2fb
0041d2fb  and        dword ptr [eax + 4], 0xfffffffe
0041d2ff  mov        dword ptr [eax + 8], edi
0041d302  mov        dword ptr [eax + 0xc], edi
0041d305  mov        dword ptr [eax + 0x10], edi
0041d308  mov        dword ptr [eax], edi
0041d30a  mov        dword ptr [eax + 0x14], eax
0041d30d  mov        dword ptr [eax + 0x18], edi
0041d310  mov        dword ptr [eax + 0x1c], edi
0041d313  mov        esi, eax
0041d315  jmp        0x41d319

// block 0041d317
0041d317  xor        esi, esi

// block 0041d319
0041d319  mov        ecx, dword ptr [esi + 4]
0041d31c  and        ecx, 0xfffffffd
0041d31f  or         ecx, 1
0041d322  mov        ebx, 0x3b
0041d327  mov        dword ptr [esi + 8], 0x41f8e0
0041d32e  mov        dword ptr [esi + 0xc], edi
0041d331  mov        dword ptr [esi + 0x10], edi
0041d334  mov        dword ptr [esi + 0x20], ebp
0041d337  mov        dword ptr [esi + 4], ecx
0041d33a  call       0x462420

// block 0041d33f
0041d33f  push       0x24
0041d341  mov        dword ptr [ebp + 0xc], esi
0041d344  call       0x46c9ea

// block 0041d349
0041d349  add        esp, 4
0041d34c  cmp        eax, edi
0041d34e  je         0x41d36c

// block 0041d350
0041d350  and        dword ptr [eax + 4], 0xfffffffe
0041d354  mov        dword ptr [eax + 8], edi
0041d357  mov        dword ptr [eax + 0xc], edi
0041d35a  mov        dword ptr [eax + 0x10], edi
0041d35d  mov        dword ptr [eax], edi
0041d35f  mov        dword ptr [eax + 0x14], eax
0041d362  mov        dword ptr [eax + 0x18], edi
0041d365  mov        dword ptr [eax + 0x1c], edi
0041d368  mov        esi, eax
0041d36a  jmp        0x41d36e

// block 0041d36c
0041d36c  xor        esi, esi

// block 0041d36e
0041d36e  mov        edx, dword ptr [esi + 4]
0041d371  and        edx, 0xfffffffd
0041d374  or         edx, 1
0041d377  mov        ebx, 0x2c
0041d37c  mov        dword ptr [esi + 8], 0x41f8f0
0041d383  mov        dword ptr [esi + 0xc], edi
0041d386  mov        dword ptr [esi + 0x10], edi
0041d389  mov        dword ptr [esi + 0x20], ebp
0041d38c  mov        dword ptr [esi + 4], edx
0041d38f  call       0x462420

// block 0041d394
0041d394  mov        dword ptr [ebp + 0x6cd4], esi
0041d39a  pop        esi
0041d39b  pop        edi
0041d39c  pop        ebp
0041d39d  xor        eax, eax
0041d39f  pop        ebx
0041d3a0  ret        4
