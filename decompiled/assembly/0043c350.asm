
// block 0043c350
0043c350  sub        esp, 0x108
0043c356  mov        eax, dword ptr [0x4ad138]
0043c35b  xor        eax, esp
0043c35d  mov        dword ptr [esp + 0x104], eax
0043c364  mov        eax, dword ptr [esp + 0x110]
0043c36b  push       ebp
0043c36c  mov        ebp, dword ptr [esp + 0x110]
0043c373  mov        edx, ebp
0043c375  push       esi
0043c376  sub        edx, eax
0043c378  push       edi
0043c379  mov        ecx, eax
0043c37b  lea        esi, [edx + 0x1e0]

// block 0043c381
0043c381  mov        dl, byte ptr [ecx]
0043c383  mov        byte ptr [esi + ecx], dl
0043c386  inc        ecx
0043c387  test       dl, dl
0043c389  jne        0x43c381

// block 0043c38b
0043c38b  test       byte ptr [0x4b0ce0], 0x20
0043c392  jne        0x43c3fa

// block 0043c394
0043c394  push       eax
0043c395  lea        eax, [esp + 0x14]
0043c399  push       0x4a12a4
0043c39e  push       eax
0043c39f  call       0x46cbbb

// block 0043c3a4
0043c3a4  add        esp, 0xc
0043c3a7  lea        ecx, [esp + 0x10]
0043c3ab  push       ecx
0043c3ac  call       0x463df0

// block 0043c3b1
0043c3b1  test       eax, eax
0043c3b3  je         0x43c3da

// block 0043c3b5
0043c3b5  lea        esi, [esp + 0x10]
0043c3b9  call       0x464070

// block 0043c3be
0043c3be  test       eax, eax
0043c3c0  jne        0x43c3da

// block 0043c3c2
0043c3c2  lea        edi, [eax + 0x24]
0043c3c5  call       0x464190

// block 0043c3ca
0043c3ca  mov        dword ptr [ebp + 0x18], eax
0043c3cd  cmp        dword ptr [eax], 0x72323174
0043c3d3  je         0x43c3e2

// block 0043c3d5
0043c3d5  call       0x4641e0

// block 0043c3da
0043c3da  or         eax, 0xffffffff
0043c3dd  jmp        0x43c4ef

// block 0043c3e2
0043c3e2  cmp        word ptr [eax + 4], 4
0043c3e7  jne        0x43c3d5

// block 0043c3e9
0043c3e9  mov        edi, dword ptr [eax + 0x1c]
0043c3ec  call       0x464190

// block 0043c3f1
0043c3f1  mov        edi, eax
0043c3f3  call       0x4641e0

// block 0043c3f8
0043c3f8  jmp        0x43c40c

// block 0043c3fa
0043c3fa  push       0
0043c3fc  lea        edx, [esp + 0x10]
0043c400  push       edx
0043c401  call       0x463c10

// block 0043c406
0043c406  mov        dword ptr [ebp + 0x18], eax
0043c409  lea        edi, [eax + 0x24]

// block 0043c40c
0043c40c  mov        eax, dword ptr [ebp + 0x18]
0043c40f  mov        eax, dword ptr [eax + 0x20]
0043c412  push       eax
0043c413  call       0x46d04a

// block 0043c418
0043c418  mov        ecx, dword ptr [ebp + 0x18]
0043c41b  add        esp, 4
0043c41e  mov        dword ptr [ebp + 0x1c8], eax
0043c424  mov        eax, dword ptr [ecx + 0x1c]
0043c427  push       eax
0043c428  push       0x800
0043c42d  push       0xe1
0043c432  push       eax
0043c433  push       edi
0043c434  mov        al, 0x5e
0043c436  call       0x4639d0

// block 0043c43b
0043c43b  mov        edx, dword ptr [ebp + 0x18]
0043c43e  mov        eax, dword ptr [edx + 0x1c]
0043c441  push       eax
0043c442  push       0x40
0043c444  push       0x3a
0043c446  push       eax
0043c447  push       edi
0043c448  mov        al, 0x7d
0043c44a  call       0x4639d0

// block 0043c44f
0043c44f  mov        eax, dword ptr [ebp + 0x18]
0043c452  mov        ecx, dword ptr [ebp + 0x1c8]
0043c458  mov        edx, dword ptr [eax + 0x1c]
0043c45b  mov        eax, dword ptr [eax + 0x20]
0043c45e  push       ecx
0043c45f  push       edx
0043c460  push       edi
0043c461  call       0x44c710

// block 0043c466
0043c466  mov        eax, dword ptr [ebp + 0x1c8]
0043c46c  mov        dword ptr [ebp + 0x1c], eax
0043c46f  add        eax, 0x70
0043c472  xor        esi, esi
0043c474  push       ebx

// block 0043c475
0043c475  mov        ecx, dword ptr [ebp + 0x1c]
0043c478  mov        ecx, dword ptr [ecx + 0x58]
0043c47b  cmp        ecx, 8
0043c47e  jl         0x43c485

// block 0043c480
0043c480  mov        ecx, 6

// block 0043c485
0043c485  cmp        esi, ecx
0043c487  jge        0x43c4d6

// block 0043c489
0043c489  movsx      ecx, word ptr [eax]
0043c48c  lea        edx, [ecx + ecx*8]
0043c48f  mov        dword ptr [ebp + edx*4 + 0xb8], eax
0043c496  movsx      ecx, word ptr [eax]
0043c499  lea        ecx, [ecx + ecx*8]
0043c49c  lea        edx, [eax + 0xa0]
0043c4a2  mov        dword ptr [ebp + ecx*4 + 0xa8], edx
0043c4a9  movsx      ecx, word ptr [eax]
0043c4ac  lea        edx, [ecx + ecx*8]
0043c4af  mov        ebx, dword ptr [ebp + edx*4 + 0xa8]
0043c4b6  lea        ecx, [ebp + edx*4]
0043c4ba  mov        edx, dword ptr [eax + 4]
0043c4bd  lea        edx, [edx + edx*2]
0043c4c0  lea        edx, [ebx + edx*2]
0043c4c3  mov        dword ptr [ecx + 0xb0], edx
0043c4c9  mov        ecx, dword ptr [eax + 8]
0043c4cc  lea        eax, [eax + ecx + 0xa0]
0043c4d3  inc        esi
0043c4d4  jmp        0x43c475

// block 0043c4d6
0043c4d6  test       byte ptr [0x4b0ce0], 0x20
0043c4dd  pop        ebx
0043c4de  jne        0x43c4ed

// block 0043c4e0
0043c4e0  test       edi, edi
0043c4e2  je         0x43c4ed

// block 0043c4e4
0043c4e4  push       edi
0043c4e5  call       0x46c941

// block 0043c4ea
0043c4ea  add        esp, 4

// block 0043c4ed
0043c4ed  xor        eax, eax

// block 0043c4ef
0043c4ef  mov        ecx, dword ptr [esp + 0x110]
0043c4f6  pop        edi
0043c4f7  pop        esi
0043c4f8  pop        ebp
0043c4f9  xor        ecx, esp
0043c4fb  call       0x46c932

// block 0043c500
0043c500  add        esp, 0x108
0043c506  ret        8
