
// block 0043f3d0
0043f3d0  push       ebx
0043f3d1  push       ebp
0043f3d2  push       esi
0043f3d3  push       edi
0043f3d4  mov        edi, dword ptr [0x4b4530]
0043f3da  push       0x24
0043f3dc  call       0x46c9ea

// block 0043f3e1
0043f3e1  xor        ebp, ebp
0043f3e3  add        esp, 4
0043f3e6  cmp        eax, ebp
0043f3e8  je         0x43f406

// block 0043f3ea
0043f3ea  and        dword ptr [eax + 4], 0xfffffffe
0043f3ee  mov        dword ptr [eax + 8], ebp
0043f3f1  mov        dword ptr [eax + 0xc], ebp
0043f3f4  mov        dword ptr [eax + 0x10], ebp
0043f3f7  mov        dword ptr [eax], ebp
0043f3f9  mov        dword ptr [eax + 0x14], eax
0043f3fc  mov        dword ptr [eax + 0x18], ebp
0043f3ff  mov        dword ptr [eax + 0x1c], ebp
0043f402  mov        esi, eax
0043f404  jmp        0x43f408

// block 0043f406
0043f406  xor        esi, esi

// block 0043f408
0043f408  mov        eax, dword ptr [esi + 4]
0043f40b  and        eax, 0xfffffffd
0043f40e  or         eax, 1
0043f411  mov        ebx, 6
0043f416  mov        dword ptr [esi + 8], 0x43fc90
0043f41d  mov        dword ptr [esi + 0xc], ebp
0043f420  mov        dword ptr [esi + 0x10], ebp
0043f423  mov        dword ptr [esi + 0x20], edi
0043f426  mov        dword ptr [esi + 4], eax
0043f429  call       0x462380

// block 0043f42e
0043f42e  push       0x24
0043f430  mov        dword ptr [edi + 0xc], esi
0043f433  call       0x46c9ea

// block 0043f438
0043f438  add        esp, 4
0043f43b  cmp        eax, ebp
0043f43d  je         0x43f45b

// block 0043f43f
0043f43f  and        dword ptr [eax + 4], 0xfffffffe
0043f443  mov        dword ptr [eax + 8], ebp
0043f446  mov        dword ptr [eax + 0xc], ebp
0043f449  mov        dword ptr [eax + 0x10], ebp
0043f44c  mov        dword ptr [eax], ebp
0043f44e  mov        dword ptr [eax + 0x14], eax
0043f451  mov        dword ptr [eax + 0x18], ebp
0043f454  mov        dword ptr [eax + 0x1c], ebp
0043f457  mov        esi, eax
0043f459  jmp        0x43f45d

// block 0043f45b
0043f45b  xor        esi, esi

// block 0043f45d
0043f45d  mov        ecx, dword ptr [esi + 4]
0043f460  and        ecx, 0xfffffffd
0043f463  or         ecx, 1
0043f466  mov        ebx, 0x38
0043f46b  mov        dword ptr [esi + 8], 0x43fca0
0043f472  mov        dword ptr [esi + 0xc], ebp
0043f475  mov        dword ptr [esi + 0x10], ebp
0043f478  mov        dword ptr [esi + 0x20], edi
0043f47b  mov        dword ptr [esi + 4], ecx
0043f47e  call       0x462420

// block 0043f483
0043f483  mov        ebx, 0x4a1ef8
0043f488  mov        ecx, 0x18
0043f48d  mov        dword ptr [edi + 0x10], esi
0043f490  call       0x45fe60

// block 0043f495
0043f495  mov        dword ptr [edi + 0x14], eax
0043f498  cmp        eax, ebp
0043f49a  jne        0x43f4b6

// block 0043f49c
0043f49c  push       0x49f434
0043f4a1  mov        ecx, 0x4b0ec8
0043f4a6  call       0x464220

// block 0043f4ab
0043f4ab  add        esp, 4
0043f4ae  pop        edi
0043f4af  pop        esi
0043f4b0  pop        ebp
0043f4b1  or         eax, 0xffffffff
0043f4b4  pop        ebx
0043f4b5  ret        

// block 0043f4b6
0043f4b6  mov        ebx, 0x4a1f04
0043f4bb  mov        ecx, 0x19
0043f4c0  call       0x45fe60

// block 0043f4c5
0043f4c5  mov        dword ptr [edi + 0x18], eax
0043f4c8  cmp        eax, ebp
0043f4ca  je         0x43f49c

// block 0043f4cc
0043f4cc  mov        dword ptr [edi + 0xf8], 1
0043f4d6  pop        edi
0043f4d7  pop        esi
0043f4d8  mov        dword ptr [0x4ce55c], ebp
0043f4de  pop        ebp
0043f4df  xor        eax, eax
0043f4e1  pop        ebx
0043f4e2  ret        
