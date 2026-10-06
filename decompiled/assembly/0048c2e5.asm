
// block 0048c2e5
0048c2e5  mov        edi, edi
0048c2e7  push       ebp
0048c2e8  mov        ebp, esp
0048c2ea  push       ecx
0048c2eb  push       ecx
0048c2ec  mov        eax, dword ptr [0x4ad138]
0048c2f1  xor        eax, ebp
0048c2f3  mov        dword ptr [ebp - 4], eax
0048c2f6  mov        eax, dword ptr [0x4b43a0]
0048c2fb  push       ebx
0048c2fc  mov        ebx, dword ptr [0x4981a0]
0048c302  push       esi
0048c303  push       edi
0048c304  xor        edi, edi
0048c306  xor        esi, esi
0048c308  inc        edi
0048c309  cmp        eax, esi
0048c30b  jne        0x48c339

// block 0048c30d
0048c30d  push       esi
0048c30e  push       esi
0048c30f  push       edi
0048c310  push       esi
0048c311  call       ebx

// block 0048c313
0048c313  test       eax, eax
0048c315  je         0x48c31f

// block 0048c317
0048c317  mov        dword ptr [0x4b43a0], edi
0048c31d  jmp        0x48c34e

// block 0048c31f
0048c31f  call       dword ptr [0x4980e4]

// block 0048c325
0048c325  cmp        eax, 0x78
0048c328  jne        0x48c334

// block 0048c32a
0048c32a  push       2
0048c32c  pop        eax
0048c32d  mov        dword ptr [0x4b43a0], eax
0048c332  jmp        0x48c339

// block 0048c334
0048c334  mov        eax, dword ptr [0x4b43a0]

// block 0048c339
0048c339  cmp        eax, 2
0048c33c  je         0x48c400

// block 0048c342
0048c342  cmp        eax, esi
0048c344  je         0x48c400

// block 0048c34a
0048c34a  cmp        eax, edi
0048c34c  jne        0x48c371

// block 0048c34e
0048c34e  cmp        dword ptr [ebp + 0x1c], esi
0048c351  jne        0x48c35e

// block 0048c353
0048c353  mov        eax, dword ptr [ebp + 8]
0048c356  mov        eax, dword ptr [eax]
0048c358  mov        eax, dword ptr [eax + 4]
0048c35b  mov        dword ptr [ebp + 0x1c], eax

// block 0048c35e
0048c35e  push       esi
0048c35f  push       esi
0048c360  push       dword ptr [ebp + 0x10]
0048c363  push       dword ptr [ebp + 0xc]
0048c366  call       ebx

// block 0048c368
0048c368  mov        ecx, eax
0048c36a  mov        dword ptr [ebp - 8], ecx
0048c36d  cmp        ecx, esi
0048c36f  jne        0x48c378

// block 0048c371
0048c371  xor        eax, eax
0048c373  jmp        0x48c412

// block 0048c378
0048c378  jle        0x48c3bf

// block 0048c37a
0048c37a  push       -0x20
0048c37c  xor        edx, edx
0048c37e  pop        eax
0048c37f  div        ecx
0048c381  cmp        eax, 2
0048c384  jb         0x48c3bf

// block 0048c386
0048c386  lea        eax, [ecx + ecx + 8]
0048c38a  cmp        eax, 0x400
0048c38f  ja         0x48c3a7

// block 0048c391
0048c391  call       0x48d830

// block 0048c396
0048c396  mov        edi, esp
0048c398  cmp        edi, esi
0048c39a  je         0x48c371

// block 0048c39c
0048c39c  mov        dword ptr [edi], 0xcccc
0048c3a2  add        edi, 8
0048c3a5  jmp        0x48c3c1

// block 0048c3a7
0048c3a7  push       eax
0048c3a8  call       0x46d04a

// block 0048c3ad
0048c3ad  pop        ecx
0048c3ae  cmp        eax, esi
0048c3b0  je         0x48c3bb

// block 0048c3b2
0048c3b2  mov        dword ptr [eax], 0xdddd
0048c3b8  add        eax, 8

// block 0048c3bb
0048c3bb  mov        edi, eax
0048c3bd  jmp        0x48c3c1

// block 0048c3bf
0048c3bf  xor        edi, edi

// block 0048c3c1
0048c3c1  cmp        edi, esi
0048c3c3  je         0x48c371

// block 0048c3c5
0048c3c5  push       dword ptr [ebp - 8]
0048c3c8  push       edi
0048c3c9  push       dword ptr [ebp + 0x10]
0048c3cc  push       dword ptr [ebp + 0xc]
0048c3cf  call       ebx

// block 0048c3d1
0048c3d1  test       eax, eax
0048c3d3  je         0x48c3f5

// block 0048c3d5
0048c3d5  push       esi
0048c3d6  push       esi
0048c3d7  cmp        dword ptr [ebp + 0x18], esi
0048c3da  jne        0x48c3e0

// block 0048c3dc
0048c3dc  push       esi
0048c3dd  push       esi
0048c3de  jmp        0x48c3e6

// block 0048c3e0
0048c3e0  push       dword ptr [ebp + 0x18]
0048c3e3  push       dword ptr [ebp + 0x14]

// block 0048c3e6
0048c3e6  push       -1
0048c3e8  push       edi
0048c3e9  push       esi
0048c3ea  push       dword ptr [ebp + 0x1c]
0048c3ed  call       dword ptr [0x498134]

// block 0048c3f3
0048c3f3  mov        esi, eax

// block 0048c3f5
0048c3f5  push       edi
0048c3f6  call       0x48326a

// block 0048c3fb
0048c3fb  pop        ecx
0048c3fc  mov        eax, esi
0048c3fe  jmp        0x48c412

// block 0048c400
0048c400  push       dword ptr [ebp + 0x18]
0048c403  push       dword ptr [ebp + 0x14]
0048c406  push       dword ptr [ebp + 0x10]
0048c409  push       dword ptr [ebp + 0xc]
0048c40c  call       dword ptr [0x4980e0]

// block 0048c412
0048c412  lea        esp, [ebp - 0x14]
0048c415  pop        edi
0048c416  pop        esi
0048c417  pop        ebx
0048c418  mov        ecx, dword ptr [ebp - 4]
0048c41b  xor        ecx, ebp
0048c41d  call       0x46c932

// block 0048c422
0048c422  leave      
0048c423  ret        
