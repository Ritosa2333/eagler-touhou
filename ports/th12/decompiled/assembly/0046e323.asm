
// block 0046e323
0046e323  mov        edi, edi
0046e325  push       ebp
0046e326  mov        ebp, esp
0046e328  sub        esp, 0x10
0046e32b  push       ebx
0046e32c  push       dword ptr [ebp + 0x10]
0046e32f  lea        ecx, [ebp - 0x10]
0046e332  call       0x46d3b4

// block 0046e337
0046e337  xor        ebx, ebx
0046e339  cmp        dword ptr [ebp + 8], ebx
0046e33c  jne        0x46e36c

// block 0046e33e
0046e33e  call       0x46e96f

// block 0046e343
0046e343  push       ebx
0046e344  push       ebx
0046e345  push       ebx
0046e346  push       ebx
0046e347  push       ebx
0046e348  mov        dword ptr [eax], 0x16
0046e34e  call       0x470f2d

// block 0046e353
0046e353  add        esp, 0x14
0046e356  cmp        byte ptr [ebp - 4], bl
0046e359  je         0x46e362

// block 0046e35b
0046e35b  mov        eax, dword ptr [ebp - 8]
0046e35e  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0046e362
0046e362  mov        eax, 0x7fffffff
0046e367  jmp        0x46e3f5

// block 0046e36c
0046e36c  push       edi
0046e36d  mov        edi, dword ptr [ebp + 0xc]
0046e370  cmp        edi, ebx
0046e372  jne        0x46e39f

// block 0046e374
0046e374  call       0x46e96f

// block 0046e379
0046e379  push       ebx
0046e37a  push       ebx
0046e37b  push       ebx
0046e37c  push       ebx
0046e37d  push       ebx
0046e37e  mov        dword ptr [eax], 0x16
0046e384  call       0x470f2d

// block 0046e389
0046e389  add        esp, 0x14
0046e38c  cmp        byte ptr [ebp - 4], bl
0046e38f  je         0x46e398

// block 0046e391
0046e391  mov        eax, dword ptr [ebp - 8]
0046e394  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0046e398
0046e398  mov        eax, 0x7fffffff
0046e39d  jmp        0x46e3f4

// block 0046e39f
0046e39f  mov        eax, dword ptr [ebp - 0x10]
0046e3a2  cmp        dword ptr [eax + 0x14], ebx
0046e3a5  jne        0x46e3b4

// block 0046e3a7
0046e3a7  push       edi
0046e3a8  push       dword ptr [ebp + 8]
0046e3ab  call       0x46e2ea

// block 0046e3b0
0046e3b0  pop        ecx
0046e3b1  pop        ecx
0046e3b2  jmp        0x46e3e8

// block 0046e3b4
0046e3b4  push       esi

// block 0046e3b5
0046e3b5  mov        eax, dword ptr [ebp + 8]
0046e3b8  movzx      eax, byte ptr [eax]
0046e3bb  lea        ecx, [ebp - 0x10]
0046e3be  push       ecx
0046e3bf  push       eax
0046e3c0  call       0x47aa12

// block 0046e3c5
0046e3c5  inc        dword ptr [ebp + 8]
0046e3c8  mov        esi, eax
0046e3ca  movzx      eax, byte ptr [edi]
0046e3cd  lea        ecx, [ebp - 0x10]
0046e3d0  push       ecx
0046e3d1  push       eax
0046e3d2  call       0x47aa12

// block 0046e3d7
0046e3d7  add        esp, 0x10
0046e3da  inc        edi
0046e3db  cmp        esi, ebx
0046e3dd  je         0x46e3e3

// block 0046e3df
0046e3df  cmp        esi, eax
0046e3e1  je         0x46e3b5

// block 0046e3e3
0046e3e3  sub        esi, eax
0046e3e5  mov        eax, esi
0046e3e7  pop        esi

// block 0046e3e8
0046e3e8  cmp        byte ptr [ebp - 4], bl
0046e3eb  je         0x46e3f4

// block 0046e3ed
0046e3ed  mov        ecx, dword ptr [ebp - 8]
0046e3f0  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 0046e3f4
0046e3f4  pop        edi

// block 0046e3f5
0046e3f5  pop        ebx
0046e3f6  leave      
0046e3f7  ret        
