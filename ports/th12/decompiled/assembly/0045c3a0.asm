
// block 0045c3a0
0045c3a0  push       esi
0045c3a1  push       edi
0045c3a2  mov        edi, eax
0045c3a4  mov        eax, dword ptr [edi + 0x47c]
0045c3aa  mov        esi, ecx
0045c3ac  test       al, 1
0045c3ae  jne        0x45c3b8

// block 0045c3b0
0045c3b0  pop        edi
0045c3b1  or         eax, 0xffffffff
0045c3b4  pop        esi
0045c3b5  ret        8

// block 0045c3b8
0045c3b8  test       al, 2
0045c3ba  je         0x45c3b0

// block 0045c3bc
0045c3bc  cmp        byte ptr [edi + 0x3bf], 0
0045c3c3  je         0x45c3b0

// block 0045c3c5
0045c3c5  cmp        dword ptr [esi + 0x4b56a0], 0
0045c3cc  je         0x45c3d3

// block 0045c3ce
0045c3ce  call       0x45a3c0

// block 0045c3d3
0045c3d3  mov        eax, dword ptr [edi + 0x3f4]
0045c3d9  mov        eax, dword ptr [eax + 8]
0045c3dc  cmp        dword ptr [esi + 0x4b563c], eax
0045c3e2  je         0x45c400

// block 0045c3e4
0045c3e4  mov        dword ptr [esi + 0x4b563c], eax
0045c3ea  mov        eax, dword ptr [eax]
0045c3ec  mov        ecx, dword ptr [0x4ce8f0]
0045c3f2  mov        edx, dword ptr [ecx]
0045c3f4  push       eax
0045c3f5  push       0
0045c3f7  push       ecx
0045c3f8  mov        ecx, dword ptr [edx + 0x104]
0045c3fe  call       ecx

// block 0045c400
0045c400  cmp        byte ptr [esi + 0x4b5642], 3
0045c407  je         0x45c451

// block 0045c409
0045c409  mov        eax, dword ptr [0x4ce8f0]
0045c40e  mov        edx, dword ptr [eax]
0045c410  push       0x144
0045c415  push       eax
0045c416  mov        eax, dword ptr [edx + 0x164]
0045c41c  call       eax

// block 0045c41e
0045c41e  mov        eax, dword ptr [0x4ce8f0]
0045c423  mov        ecx, dword ptr [eax]
0045c425  mov        edx, dword ptr [ecx + 0x10c]
0045c42b  push       0
0045c42d  push       6
0045c42f  push       0
0045c431  push       eax
0045c432  call       edx

// block 0045c434
0045c434  mov        eax, dword ptr [0x4ce8f0]
0045c439  mov        ecx, dword ptr [eax]
0045c43b  mov        edx, dword ptr [ecx + 0x10c]
0045c441  push       0
0045c443  push       3
0045c445  push       0
0045c447  push       eax
0045c448  call       edx

// block 0045c44a
0045c44a  mov        byte ptr [esi + 0x4b5642], 3

// block 0045c451
0045c451  mov        eax, esi
0045c453  call       0x459cf0

// block 0045c458
0045c458  mov        eax, dword ptr [0x4ce8cc]
0045c45d  cmp        byte ptr [eax + 0x4b5647], 1
0045c464  je         0x45c49e

// block 0045c466
0045c466  mov        eax, dword ptr [0x4ce8f0]
0045c46b  mov        ecx, dword ptr [eax]
0045c46d  mov        edx, dword ptr [ecx + 0x10c]
0045c473  push       4
0045c475  push       4
0045c477  push       0
0045c479  push       eax
0045c47a  call       edx

// block 0045c47c
0045c47c  mov        eax, dword ptr [0x4ce8f0]
0045c481  mov        ecx, dword ptr [eax]
0045c483  mov        edx, dword ptr [ecx + 0x10c]
0045c489  push       4
0045c48b  push       1
0045c48d  push       0
0045c48f  push       eax
0045c490  call       edx

// block 0045c492
0045c492  mov        eax, dword ptr [0x4ce8cc]
0045c497  mov        byte ptr [eax + 0x4b5647], 1

// block 0045c49e
0045c49e  mov        edx, dword ptr [esp + 0xc]
0045c4a2  mov        eax, dword ptr [0x4ce8f0]
0045c4a7  mov        ecx, dword ptr [eax]
0045c4a9  push       0x1c
0045c4ab  push       edx
0045c4ac  mov        edx, dword ptr [esp + 0x18]
0045c4b0  add        edx, -2
0045c4b3  push       edx
0045c4b4  push       5
0045c4b6  push       eax
0045c4b7  mov        eax, dword ptr [ecx + 0x14c]
0045c4bd  call       eax

// block 0045c4bf
0045c4bf  pop        edi
0045c4c0  xor        eax, eax
0045c4c2  pop        esi
0045c4c3  ret        8
