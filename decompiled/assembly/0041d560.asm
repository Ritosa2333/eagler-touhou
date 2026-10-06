
// block 0041d560
0041d560  sub        esp, 0x14
0041d563  push       ebx
0041d564  push       ebp
0041d565  push       esi
0041d566  mov        esi, dword ptr [0x4b43e4]
0041d56c  mov        eax, dword ptr [esi + 8]
0041d56f  push       edi
0041d570  mov        ebp, 2
0041d575  test       eax, eax
0041d577  je         0x41d57c

// block 0041d579
0041d579  or         dword ptr [eax + 4], ebp

// block 0041d57c
0041d57c  mov        eax, dword ptr [esi + 0xc]
0041d57f  test       eax, eax
0041d581  je         0x41d586

// block 0041d583
0041d583  or         dword ptr [eax + 4], ebp

// block 0041d586
0041d586  mov        eax, dword ptr [esi + 0x6cd4]
0041d58c  test       eax, eax
0041d58e  je         0x41d593

// block 0041d590
0041d590  or         dword ptr [eax + 4], ebp

// block 0041d593
0041d593  cmp        dword ptr [esi + 0x6cbc], 0
0041d59a  jne        0x41d5c2

// block 0041d59c
0041d59c  mov        ecx, dword ptr [esi + 0x6d44]
0041d5a2  push       0
0041d5a4  push       0
0041d5a6  lea        eax, [esp + 0x1c]
0041d5aa  push       eax
0041d5ab  push       ecx
0041d5ac  mov        eax, 0x17
0041d5b1  xor        ecx, ecx
0041d5b3  call       0x4615a0

// block 0041d5b8
0041d5b8  mov        edx, dword ptr [esp + 0x14]
0041d5bc  mov        dword ptr [esi + 0x6cbc], edx

// block 0041d5c2
0041d5c2  test       byte ptr [esi + 0x48c], 1
0041d5c9  jne        0x41d7d5

// block 0041d5cf
0041d5cf  xor        edi, edi
0041d5d1  lea        ebx, [esi + 0x10]

// block 0041d5d4
0041d5d4  mov        ecx, dword ptr [esi + 0x6d44]
0041d5da  lea        eax, [edi + 0xd]
0041d5dd  push       eax
0041d5de  push       ebx
0041d5df  call       0x454d10

// block 0041d5e4
0041d5e4  inc        edi
0041d5e5  add        ebx, 0x4b4
0041d5eb  cmp        edi, 8
0041d5ee  jb         0x41d5d4

// block 0041d5f0
0041d5f0  xor        edi, edi
0041d5f2  lea        ebx, [esi + 0x25b0]
0041d5f8  jmp        0x41d600

// block 0041d600
0041d600  lea        ecx, [edi + 0x15]
0041d603  push       ecx
0041d604  mov        ecx, dword ptr [esi + 0x6d44]
0041d60a  push       ebx
0041d60b  call       0x454d10

// block 0041d610
0041d610  inc        edi
0041d611  add        ebx, 0x4b4
0041d617  cmp        edi, 8
0041d61a  jb         0x41d600

// block 0041d61c
0041d61c  xor        edi, edi
0041d61e  lea        ebx, [esi + 0x4b50]
0041d624  jmp        0x41d630

// block 0041d630
0041d630  mov        eax, dword ptr [0x4b43b8]
0041d635  mov        ecx, dword ptr [eax + 0x18fb4]
0041d63b  lea        edx, [edi + 3]
0041d63e  push       edx
0041d63f  push       ebx
0041d640  call       0x454d10

// block 0041d645
0041d645  inc        edi
0041d646  add        ebx, 0x4b4
0041d64c  cmp        edi, ebp
0041d64e  jl         0x41d630

// block 0041d650
0041d650  push       0x4f
0041d652  lea        ecx, [esi + 0x54b8]
0041d658  push       ecx
0041d659  mov        ecx, dword ptr [esi + 0x6d44]
0041d65f  call       0x454d10

// block 0041d664
0041d664  mov        ecx, dword ptr [esi + 0x6d44]
0041d66a  push       0x50
0041d66c  lea        ebp, [esi + 0x596c]
0041d672  push       ebp
0041d673  call       0x454d10

// block 0041d678
0041d678  mov        ecx, dword ptr [esi + 0x6d44]
0041d67e  push       0x50
0041d680  lea        ebx, [esi + 0x5e20]
0041d686  push       ebx
0041d687  call       0x454d10

// block 0041d68c
0041d68c  mov        ecx, dword ptr [esi + 0x6d44]
0041d692  push       0x50
0041d694  lea        edi, [esi + 0x62d4]
0041d69a  push       edi
0041d69b  call       0x454d10

// block 0041d6a0
0041d6a0  mov        eax, dword ptr [ebp + 0x494]
0041d6a6  test       eax, eax
0041d6a8  je         0x41d6b3

// block 0041d6aa
0041d6aa  mov        edx, 7
0041d6af  mov        ecx, ebp
0041d6b1  call       eax

// block 0041d6b3
0041d6b3  mov        edx, 7
0041d6b8  push       ebp
0041d6b9  mov        word ptr [ebp + 0x3c4], dx
0041d6c0  call       0x455630

// block 0041d6c5
0041d6c5  mov        eax, dword ptr [ebx + 0x494]
0041d6cb  test       eax, eax
0041d6cd  je         0x41d6d8

// block 0041d6cf
0041d6cf  mov        edx, 8
0041d6d4  mov        ecx, ebx
0041d6d6  call       eax

// block 0041d6d8
0041d6d8  mov        eax, 8
0041d6dd  push       ebx
0041d6de  mov        word ptr [ebx + 0x3c4], ax
0041d6e5  call       0x455630

// block 0041d6ea
0041d6ea  mov        eax, dword ptr [edi + 0x494]
0041d6f0  test       eax, eax
0041d6f2  je         0x41d6fd

// block 0041d6f4
0041d6f4  mov        edx, 9
0041d6f9  mov        ecx, edi
0041d6fb  call       eax

// block 0041d6fd
0041d6fd  mov        ecx, 9
0041d702  push       edi
0041d703  mov        word ptr [edi + 0x3c4], cx
0041d70a  call       0x455630

// block 0041d70f
0041d70f  mov        edi, dword ptr [esi + 0x6788]
0041d715  mov        edx, dword ptr [0x4b0c4c]
0041d71b  inc        edi
0041d71c  mov        eax, edi
0041d71e  imul       eax, eax, 0x4b4
0041d724  lea        ebx, [eax + esi + 0x54b8]
0041d72b  mov        eax, dword ptr [ebx + 0x494]
0041d731  add        edx, 0xa
0041d734  movzx      ebp, dx
0041d737  test       eax, eax
0041d739  je         0x41d742

// block 0041d73b
0041d73b  movsx      edx, bp
0041d73e  mov        ecx, ebx
0041d740  call       eax

// block 0041d742
0041d742  push       ebx
0041d743  mov        word ptr [ebx + 0x3c4], bp
0041d74a  call       0x455630

// block 0041d74f
0041d74f  inc        edi
0041d750  cmp        edi, 4
0041d753  jl         0x41d75a

// block 0041d755
0041d755  mov        edi, 1

// block 0041d75a
0041d75a  mov        ecx, dword ptr [0x4b0c50]
0041d760  mov        edx, edi
0041d762  imul       edx, edx, 0x4b4
0041d768  mov        eax, dword ptr [edx + esi + 0x594c]
0041d76f  lea        ebx, [edx + esi + 0x54b8]
0041d776  add        ecx, 0xa
0041d779  movzx      ebp, cx
0041d77c  test       eax, eax
0041d77e  je         0x41d787

// block 0041d780
0041d780  movsx      edx, bp
0041d783  mov        ecx, ebx
0041d785  call       eax

// block 0041d787
0041d787  push       ebx
0041d788  mov        word ptr [ebx + 0x3c4], bp
0041d78f  call       0x455630

// block 0041d794
0041d794  inc        edi
0041d795  cmp        edi, 4
0041d798  jl         0x41d79f

// block 0041d79a
0041d79a  mov        edi, 1

// block 0041d79f
0041d79f  mov        eax, dword ptr [0x4b0c54]
0041d7a4  imul       edi, edi, 0x4b4
0041d7aa  add        eax, 0xa
0041d7ad  lea        edi, [edi + esi + 0x54b8]
0041d7b4  movzx      ebx, ax
0041d7b7  mov        eax, dword ptr [edi + 0x494]
0041d7bd  test       eax, eax
0041d7bf  je         0x41d7c8

// block 0041d7c1
0041d7c1  movsx      edx, bx
0041d7c4  mov        ecx, edi
0041d7c6  call       eax

// block 0041d7c8
0041d7c8  push       edi
0041d7c9  mov        word ptr [edi + 0x3c4], bx
0041d7d0  call       0x455630

// block 0041d7d5
0041d7d5  mov        ecx, dword ptr [0x4b0c9c]
0041d7db  mov        edx, dword ptr [0x4b0c98]
0041d7e1  push       ecx
0041d7e2  push       edx
0041d7e3  push       esi
0041d7e4  call       0x41ce60

// block 0041d7e9
0041d7e9  mov        eax, dword ptr [0x4b0ca4]
0041d7ee  mov        ecx, dword ptr [0x4b0ca0]
0041d7f4  push       eax
0041d7f5  push       ecx
0041d7f6  push       esi
0041d7f7  call       0x41cf40

// block 0041d7fc
0041d7fc  cmp        dword ptr [0x4cee40], 8
0041d803  mov        bl, 0x20
0041d805  je         0x41d82b

// block 0041d807
0041d807  test       byte ptr [0x4b0ce0], bl
0041d80d  jne        0x41d837

// block 0041d80f
0041d80f  mov        eax, dword ptr [esi + 0x6ce4]
0041d815  push       0
0041d817  push       1
0041d819  lea        edx, [esp + 0x1c]
0041d81d  push       edx
0041d81e  push       eax
0041d81f  mov        eax, 0x17
0041d824  xor        ecx, ecx
0041d826  call       0x4615a0

// block 0041d82b
0041d82b  test       byte ptr [0x4b0ce0], bl
0041d831  je         0x41d8d0

// block 0041d837
0041d837  mov        ebx, dword ptr [esi + 0x6d44]
0041d83d  mov        ebp, 0x8000
0041d842  test       dword ptr [0x4cee78], ebp
0041d848  je         0x41d85b

// block 0041d84a
0041d84a  push       0x4cf1d0
0041d84f  call       dword ptr [0x498088]

// block 0041d855
0041d855  inc        byte ptr [0x4cf221]

// block 0041d85b
0041d85b  inc        dword ptr [ebx + 0x130]
0041d861  call       0x4621c0

// block 0041d866
0041d866  fldz       
0041d868  mov        edi, eax
0041d86a  fst        dword ptr [esp + 0x18]
0041d86e  mov        ecx, dword ptr [esp + 0x18]
0041d872  fst        dword ptr [esp + 0x1c]
0041d876  mov        edx, dword ptr [esp + 0x1c]
0041d87a  fstp       dword ptr [esp + 0x20]
0041d87e  mov        eax, dword ptr [esp + 0x20]
0041d882  or         dword ptr [edi + 0x480], 1
0041d889  mov        dword ptr [edi + 0x430], ecx
0041d88f  push       0x4d
0041d891  mov        dword ptr [edi + 0x434], edx
0041d897  push       edi
0041d898  mov        ecx, ebx
0041d89a  mov        dword ptr [edi + 0x20], 0x17
0041d8a1  mov        dword ptr [edi + 0x438], eax
0041d8a7  call       0x454d10

// block 0041d8ac
0041d8ac  mov        ebx, edi
0041d8ae  lea        eax, [esp + 0x14]
0041d8b2  call       0x461250

// block 0041d8b7
0041d8b7  test       dword ptr [0x4cee78], ebp
0041d8bd  je         0x41d8d0

// block 0041d8bf
0041d8bf  push       0x4cf1d0
0041d8c4  call       dword ptr [0x49808c]

// block 0041d8ca
0041d8ca  dec        byte ptr [0x4cf221]

// block 0041d8d0
0041d8d0  mov        edx, dword ptr [0x4b43b8]
0041d8d6  push       0
0041d8d8  lea        ecx, [esi + 0x678c]
0041d8de  push       ecx
0041d8df  mov        ecx, dword ptr [edx + 0x18fb4]
0041d8e5  call       0x454d10

// block 0041d8ea
0041d8ea  cmp        dword ptr [0x4b0cb0], 1
0041d8f1  jne        0x41d923

// block 0041d8f3
0041d8f3  mov        eax, dword ptr [0x4b44e8]
0041d8f8  cmp        dword ptr [eax + 0x74], 0
0041d8fc  jne        0x41d923

// block 0041d8fe
0041d8fe  cmp        dword ptr [0x4b0cc4], 0
0041d905  jne        0x41d923

// block 0041d907
0041d907  mov        edx, dword ptr [esi + 0x6d44]
0041d90d  push       0
0041d90f  push       0x33
0041d911  lea        ecx, [esp + 0x1c]
0041d915  push       ecx
0041d916  push       edx
0041d917  mov        eax, 0x17
0041d91c  xor        ecx, ecx
0041d91e  call       0x4615a0

// block 0041d923
0041d923  cmp        dword ptr [0x4cee4c], 0
0041d92a  je         0x41d964

// block 0041d92c
0041d92c  mov        eax, dword ptr [0x4b0ca8]
0041d931  mov        edx, dword ptr [esi + 0x6d44]
0041d937  push       0
0041d939  add        eax, 0x40
0041d93c  push       eax
0041d93d  lea        ecx, [esp + 0x1c]
0041d941  push       ecx
0041d942  push       edx
0041d943  mov        eax, 0x17
0041d948  xor        ecx, ecx
0041d94a  call       0x4615a0

// block 0041d94f
0041d94f  mov        eax, dword ptr [esp + 0x14]
0041d953  push       eax
0041d954  mov        ebx, 3
0041d959  mov        dword ptr [esi + 0x6ca4], eax
0041d95f  call       0x461970

// block 0041d964
0041d964  mov        eax, dword ptr [0x4b0ca8]
0041d969  mov        edx, dword ptr [esi + 0x6d44]
0041d96f  push       0
0041d971  add        eax, 0x45
0041d974  push       eax
0041d975  lea        ecx, [esp + 0x1c]
0041d979  push       ecx
0041d97a  push       edx
0041d97b  mov        eax, 0x17
0041d980  xor        ecx, ecx
0041d982  call       0x4615a0

// block 0041d987
0041d987  mov        ecx, dword ptr [esi + 0x6ca4]
0041d98d  mov        eax, dword ptr [esp + 0x14]
0041d991  push       ecx
0041d992  mov        ebx, 3
0041d997  mov        dword ptr [esi + 0x6ca8], eax
0041d99d  call       0x461970

// block 0041d9a2
0041d9a2  pop        edi
0041d9a3  mov        dword ptr [esi + 0x6cf4], 0
0041d9ad  pop        esi
0041d9ae  pop        ebp
0041d9af  pop        ebx
0041d9b0  add        esp, 0x14
0041d9b3  ret        
