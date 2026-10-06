
// block 0047c397
0047c397  mov        edi, edi
0047c399  push       ebp
0047c39a  mov        ebp, esp
0047c39c  sub        esp, 0x10
0047c39f  push       ebx
0047c3a0  push       esi
0047c3a1  mov        esi, dword ptr [ebp + 0xc]
0047c3a4  xor        ebx, ebx
0047c3a6  push       edi
0047c3a7  mov        edi, dword ptr [ebp + 0x10]
0047c3aa  cmp        esi, ebx
0047c3ac  jne        0x47c3c2

// block 0047c3ae
0047c3ae  cmp        edi, ebx
0047c3b0  jbe        0x47c3c2

// block 0047c3b2
0047c3b2  mov        eax, dword ptr [ebp + 8]
0047c3b5  cmp        eax, ebx
0047c3b7  je         0x47c3bb

// block 0047c3b9
0047c3b9  mov        dword ptr [eax], ebx

// block 0047c3bb
0047c3bb  xor        eax, eax
0047c3bd  jmp        0x47c445

// block 0047c3c2
0047c3c2  mov        eax, dword ptr [ebp + 8]
0047c3c5  cmp        eax, ebx
0047c3c7  je         0x47c3cc

// block 0047c3c9
0047c3c9  or         dword ptr [eax], 0xffffffff

// block 0047c3cc
0047c3cc  cmp        edi, 0x7fffffff
0047c3d2  jbe        0x47c3ef

// block 0047c3d4
0047c3d4  call       0x46e96f

// block 0047c3d9
0047c3d9  push       0x16
0047c3db  pop        esi
0047c3dc  push       ebx
0047c3dd  push       ebx
0047c3de  push       ebx
0047c3df  push       ebx
0047c3e0  push       ebx
0047c3e1  mov        dword ptr [eax], esi
0047c3e3  call       0x470f2d

// block 0047c3e8
0047c3e8  add        esp, 0x14

// block 0047c3eb
0047c3eb  mov        eax, esi
0047c3ed  jmp        0x47c445

// block 0047c3ef
0047c3ef  push       dword ptr [ebp + 0x18]
0047c3f2  lea        ecx, [ebp - 0x10]
0047c3f5  call       0x46d3b4

// block 0047c3fa
0047c3fa  mov        eax, dword ptr [ebp - 0x10]
0047c3fd  cmp        dword ptr [eax + 0x14], ebx
0047c400  jne        0x47c4a2

// block 0047c406
0047c406  mov        ax, word ptr [ebp + 0x14]
0047c40a  mov        ecx, 0xff
0047c40f  cmp        ax, cx
0047c412  jbe        0x47c44a

// block 0047c414
0047c414  cmp        esi, ebx
0047c416  je         0x47c427

// block 0047c418
0047c418  cmp        edi, ebx
0047c41a  jbe        0x47c427

// block 0047c41c
0047c41c  push       edi
0047c41d  push       ebx
0047c41e  push       esi
0047c41f  call       0x477420

// block 0047c424
0047c424  add        esp, 0xc

// block 0047c427
0047c427  call       0x46e96f

// block 0047c42c
0047c42c  mov        dword ptr [eax], 0x2a
0047c432  call       0x46e96f

// block 0047c437
0047c437  mov        eax, dword ptr [eax]
0047c439  cmp        byte ptr [ebp - 4], bl
0047c43c  je         0x47c445

// block 0047c43e
0047c43e  mov        ecx, dword ptr [ebp - 8]
0047c441  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0047c445
0047c445  pop        edi
0047c446  pop        esi
0047c447  pop        ebx
0047c448  leave      
0047c449  ret        

// block 0047c44a
0047c44a  cmp        esi, ebx
0047c44c  je         0x47c480

// block 0047c44e
0047c44e  cmp        edi, ebx
0047c450  ja         0x47c47e

// block 0047c452
0047c452  call       0x46e96f

// block 0047c457
0047c457  push       0x22
0047c459  pop        esi
0047c45a  push       ebx
0047c45b  push       ebx
0047c45c  push       ebx
0047c45d  push       ebx
0047c45e  push       ebx
0047c45f  mov        dword ptr [eax], esi
0047c461  call       0x470f2d

// block 0047c466
0047c466  add        esp, 0x14
0047c469  cmp        byte ptr [ebp - 4], bl
0047c46c  je         0x47c3eb

// block 0047c472
0047c472  mov        eax, dword ptr [ebp - 8]
0047c475  and        dword ptr [eax + 0x70], 0xfffffffd
0047c479  jmp        0x47c3eb

// block 0047c47e
0047c47e  mov        byte ptr [esi], al

// block 0047c480
0047c480  mov        eax, dword ptr [ebp + 8]
0047c483  cmp        eax, ebx
0047c485  je         0x47c48d

// block 0047c487
0047c487  mov        dword ptr [eax], 1

// block 0047c48d
0047c48d  cmp        byte ptr [ebp - 4], bl
0047c490  je         0x47c3bb

// block 0047c496
0047c496  mov        eax, dword ptr [ebp - 8]
0047c499  and        dword ptr [eax + 0x70], 0xfffffffd
0047c49d  jmp        0x47c3bb

// block 0047c4a2
0047c4a2  lea        ecx, [ebp + 0xc]
0047c4a5  push       ecx
0047c4a6  push       ebx
0047c4a7  push       edi
0047c4a8  push       esi
0047c4a9  push       1
0047c4ab  lea        ecx, [ebp + 0x14]
0047c4ae  push       ecx
0047c4af  push       ebx
0047c4b0  mov        dword ptr [ebp + 0xc], ebx
0047c4b3  push       dword ptr [eax + 4]
0047c4b6  call       dword ptr [0x498134]

// block 0047c4bc
0047c4bc  cmp        eax, ebx
0047c4be  je         0x47c4d4

// block 0047c4c0
0047c4c0  cmp        dword ptr [ebp + 0xc], ebx
0047c4c3  jne        0x47c427

// block 0047c4c9
0047c4c9  mov        ecx, dword ptr [ebp + 8]
0047c4cc  cmp        ecx, ebx
0047c4ce  je         0x47c48d

// block 0047c4d0
0047c4d0  mov        dword ptr [ecx], eax
0047c4d2  jmp        0x47c48d

// block 0047c4d4
0047c4d4  call       dword ptr [0x4980e4]

// block 0047c4da
0047c4da  cmp        eax, 0x7a
0047c4dd  jne        0x47c427

// block 0047c4e3
0047c4e3  cmp        esi, ebx
0047c4e5  je         0x47c452

// block 0047c4eb
0047c4eb  cmp        edi, ebx
0047c4ed  jbe        0x47c452

// block 0047c4f3
0047c4f3  push       edi
0047c4f4  push       ebx
0047c4f5  push       esi
0047c4f6  call       0x477420

// block 0047c4fb
0047c4fb  add        esp, 0xc
0047c4fe  jmp        0x47c452
