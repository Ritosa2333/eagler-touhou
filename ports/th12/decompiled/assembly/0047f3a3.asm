
// block 0047f3a3
0047f3a3  mov        edi, edi
0047f3a5  push       ebp
0047f3a6  mov        ebp, esp
0047f3a8  mov        eax, dword ptr [ebp + 0xc]
0047f3ab  mov        ecx, dword ptr [eax]
0047f3ad  sub        esp, 0x18
0047f3b0  push       ebx
0047f3b1  push       esi
0047f3b2  mov        esi, dword ptr [ebp + 8]
0047f3b5  xor        ebx, ebx
0047f3b7  mov        dword ptr [esi], ecx
0047f3b9  mov        eax, dword ptr [eax + 4]
0047f3bc  inc        ebx
0047f3bd  cmp        al, bl
0047f3bf  mov        dword ptr [esi + 4], eax
0047f3c2  jg         0x47f4d4

// block 0047f3c8
0047f3c8  mov        ecx, dword ptr [0x4b4318]
0047f3ce  cmp        byte ptr [ecx], 0
0047f3d1  je         0x47f4ba

// block 0047f3d7
0047f3d7  lea        eax, [ebp - 8]
0047f3da  push       eax
0047f3db  call       0x47e0c2

// block 0047f3e0
0047f3e0  pop        ecx
0047f3e1  push       esi
0047f3e2  lea        eax, [ebp - 0x10]
0047f3e5  push       eax
0047f3e6  push       0x20
0047f3e8  lea        eax, [ebp - 0x18]
0047f3eb  push       eax
0047f3ec  lea        ecx, [ebp - 8]
0047f3ef  call       0x47ec6e

// block 0047f3f4
0047f3f4  mov        ecx, eax
0047f3f6  call       0x47e9ae

// block 0047f3fb
0047f3fb  push       eax
0047f3fc  mov        ecx, esi
0047f3fe  call       0x47dde0

// block 0047f403
0047f403  cmp        byte ptr [esi + 4], bl
0047f406  jg         0x47f4d4

// block 0047f40c
0047f40c  mov        eax, dword ptr [0x4b4318]
0047f411  cmp        byte ptr [eax], 0x40
0047f414  je         0x47f4b2

// block 0047f41a
0047f41a  push       0x49de08
0047f41f  jmp        0x47f47c

// block 0047f421
0047f421  mov        eax, dword ptr [0x4b4318]
0047f426  mov        al, byte ptr [eax]
0047f428  test       al, al
0047f42a  je         0x47f488

// block 0047f42c
0047f42c  cmp        al, 0x40
0047f42e  je         0x47f488

// block 0047f430
0047f430  push       0x27
0047f432  lea        eax, [ebp - 0x18]
0047f435  push       eax
0047f436  lea        eax, [ebp - 0x10]
0047f439  push       eax
0047f43a  call       0x4814b0

// block 0047f43f
0047f43f  push       eax
0047f440  lea        eax, [ebp - 8]
0047f443  push       0x60
0047f445  push       eax
0047f446  call       0x47ec02

// block 0047f44b
0047f44b  add        esp, 0x10
0047f44e  mov        ecx, eax
0047f450  call       0x47ec6e

// block 0047f455
0047f455  push       eax
0047f456  mov        ecx, esi
0047f458  call       0x47e7bf

// block 0047f45d
0047f45d  mov        eax, dword ptr [0x4b4318]
0047f462  cmp        byte ptr [eax], 0x40
0047f465  jne        0x47f46d

// block 0047f467
0047f467  inc        eax
0047f468  mov        dword ptr [0x4b4318], eax

// block 0047f46d
0047f46d  cmp        byte ptr [esi + 4], bl
0047f470  jg         0x47f4a8

// block 0047f472
0047f472  cmp        byte ptr [eax], 0x40
0047f475  je         0x47f483

// block 0047f477
0047f477  push       0x49de90

// block 0047f47c
0047f47c  mov        ecx, esi
0047f47e  call       0x47ea48

// block 0047f483
0047f483  cmp        byte ptr [esi + 4], bl
0047f486  jle        0x47f421

// block 0047f488
0047f488  cmp        byte ptr [esi + 4], bl
0047f48b  jg         0x47f4a8

// block 0047f48d
0047f48d  mov        eax, dword ptr [0x4b4318]
0047f492  cmp        byte ptr [eax], 0
0047f495  jne        0x47f49f

// block 0047f497
0047f497  push       ebx
0047f498  mov        ecx, esi
0047f49a  call       0x47e4af

// block 0047f49f
0047f49f  push       0x7d
0047f4a1  mov        ecx, esi
0047f4a3  call       0x47e9f6

// block 0047f4a8
0047f4a8  mov        eax, dword ptr [0x4b4318]
0047f4ad  cmp        byte ptr [eax], 0x40
0047f4b0  jne        0x47f4d4

// block 0047f4b2
0047f4b2  inc        dword ptr [0x4b4318]
0047f4b8  jmp        0x47f4d4

// block 0047f4ba
0047f4ba  cmp        al, bl
0047f4bc  jg         0x47f4d4

// block 0047f4be
0047f4be  push       esi
0047f4bf  lea        eax, [ebp - 0x18]
0047f4c2  push       ebx
0047f4c3  push       eax
0047f4c4  call       0x47ec26

// block 0047f4c9
0047f4c9  add        esp, 0xc
0047f4cc  push       eax
0047f4cd  mov        ecx, esi
0047f4cf  call       0x47dde0

// block 0047f4d4
0047f4d4  mov        eax, esi
0047f4d6  pop        esi
0047f4d7  pop        ebx
0047f4d8  leave      
0047f4d9  ret        
