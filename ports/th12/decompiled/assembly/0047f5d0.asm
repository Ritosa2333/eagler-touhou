
// block 0047f5d0
0047f5d0  mov        edi, edi
0047f5d2  push       ebp
0047f5d3  mov        ebp, esp
0047f5d5  sub        esp, 0xd0
0047f5db  mov        eax, dword ptr [0x4ad138]
0047f5e0  xor        eax, ebp
0047f5e2  mov        dword ptr [ebp - 4], eax
0047f5e5  mov        eax, dword ptr [0x4b4318]
0047f5ea  push       ebx
0047f5eb  mov        bl, byte ptr [eax]
0047f5ed  inc        dword ptr [0x4b4318]
0047f5f3  push       esi
0047f5f4  movsx      esi, bl
0047f5f7  cmp        esi, 0x44
0047f5fa  push       edi
0047f5fb  mov        edi, dword ptr [ebp + 8]
0047f5fe  jg         0x47f712

// block 0047f604
0047f604  je         0x47f761

// block 0047f60a
0047f60a  sub        esi, 0
0047f60d  je         0x47f705

// block 0047f613
0047f613  sub        esi, 0x30
0047f616  je         0x47f6fa

// block 0047f61c
0047f61c  dec        esi
0047f61d  je         0x47f6b8

// block 0047f623
0047f623  dec        esi
0047f624  jne        0x47f660

// block 0047f626
0047f626  lea        eax, [ebp - 0x80]
0047f629  push       eax
0047f62a  call       0x47f57e

// block 0047f62f
0047f62f  lea        eax, [ebp - 0xc]
0047f632  push       eax
0047f633  call       0x47f57e

// block 0047f638
0047f638  cmp        byte ptr [ebp - 0x7c], 1
0047f63c  pop        ecx
0047f63d  pop        ecx
0047f63e  jg         0x47f70b

// block 0047f644
0047f644  cmp        byte ptr [ebp - 8], 1
0047f648  jg         0x47f70b

// block 0047f64e
0047f64e  push       0x64
0047f650  lea        eax, [ebp - 0x77]
0047f653  push       eax
0047f654  lea        ecx, [ebp - 0x80]
0047f657  call       0x47e213

// block 0047f65c
0047f65c  test       eax, eax
0047f65e  jne        0x47f66e

// block 0047f660
0047f660  push       2

// block 0047f662
0047f662  mov        ecx, edi
0047f664  call       0x47e1ce

// block 0047f669
0047f669  jmp        0x47f88c

// block 0047f66e
0047f66e  mov        al, byte ptr [ebp - 0x77]
0047f671  mov        byte ptr [ebp - 0x78], al
0047f674  cmp        al, 0x2d
0047f676  jne        0x47f684

// block 0047f678
0047f678  mov        al, byte ptr [ebp - 0x76]
0047f67b  mov        byte ptr [ebp - 0x77], al
0047f67e  mov        byte ptr [ebp - 0x76], 0x2e
0047f682  jmp        0x47f688

// block 0047f684
0047f684  mov        byte ptr [ebp - 0x77], 0x2e

// block 0047f688
0047f688  lea        eax, [ebp - 0xc]
0047f68b  push       eax
0047f68c  push       edi
0047f68d  push       0x65
0047f68f  lea        eax, [ebp - 0xa8]
0047f695  push       eax
0047f696  lea        eax, [ebp - 0x78]
0047f699  push       eax
0047f69a  lea        ecx, [ebp - 0xd0]
0047f6a0  call       0x47e59a

// block 0047f6a5
0047f6a5  mov        ecx, eax
0047f6a7  call       0x47ec6e

// block 0047f6ac
0047f6ac  mov        ecx, eax
0047f6ae  call       0x47e9ae

// block 0047f6b3
0047f6b3  jmp        0x47f88c

// block 0047f6b8
0047f6b8  mov        eax, dword ptr [0x4b4318]
0047f6bd  cmp        byte ptr [eax], 0x40
0047f6c0  jne        0x47f6d9

// block 0047f6c2
0047f6c2  inc        dword ptr [0x4b4318]
0047f6c8  push       0x49deac

// block 0047f6cd
0047f6cd  mov        ecx, edi
0047f6cf  call       0x47e59a

// block 0047f6d4
0047f6d4  jmp        0x47f88c

// block 0047f6d9
0047f6d9  lea        eax, [ebp - 0x98]
0047f6df  push       eax
0047f6e0  call       0x481298

// block 0047f6e5
0047f6e5  pop        ecx
0047f6e6  push       eax
0047f6e7  push       edi
0047f6e8  push       0x49db6c
0047f6ed  lea        ecx, [ebp - 0xb8]
0047f6f3  call       0x47e59a

// block 0047f6f8
0047f6f8  jmp        0x47f6ac

// block 0047f6fa
0047f6fa  push       edi
0047f6fb  call       0x47f57e

// block 0047f700
0047f700  jmp        0x47f88b

// block 0047f705
0047f705  dec        dword ptr [0x4b4318]

// block 0047f70b
0047f70b  push       1
0047f70d  jmp        0x47f662

// block 0047f712
0047f712  cmp        esi, 0x45
0047f715  je         0x47f885

// block 0047f71b
0047f71b  jle        0x47f660

// block 0047f721
0047f721  cmp        esi, 0x4a
0047f724  jle        0x47f7dd

// block 0047f72a
0047f72a  cmp        esi, 0x51
0047f72d  je         0x47f761

// block 0047f72f
0047f72f  cmp        esi, 0x52
0047f732  jne        0x47f660

// block 0047f738
0047f738  push       0
0047f73a  lea        eax, [ebp - 0xc]
0047f73d  push       0
0047f73f  push       eax
0047f740  call       0x48023a

// block 0047f745
0047f745  lea        eax, [ebp - 0x80]
0047f748  push       eax
0047f749  call       0x47f57e

// block 0047f74e
0047f74e  mov        eax, dword ptr [ebp - 0xc]
0047f751  mov        dword ptr [edi], eax
0047f753  mov        eax, dword ptr [ebp - 8]
0047f756  add        esp, 0x10
0047f759  mov        dword ptr [edi + 4], eax
0047f75c  jmp        0x47f88c

// block 0047f761
0047f761  lea        eax, [ebp - 0x80]
0047f764  push       eax
0047f765  call       0x47f57e

// block 0047f76a
0047f76a  test       dword ptr [0x4b4328], 0x4000
0047f774  pop        ecx
0047f775  je         0x47f7a1

// block 0047f777
0047f777  push       0x10
0047f779  lea        eax, [ebp - 0x14]
0047f77c  push       eax
0047f77d  lea        ecx, [ebp - 0x80]
0047f780  call       0x47e213

// block 0047f785
0047f785  lea        eax, [ebp - 0x14]
0047f788  push       eax
0047f789  call       0x46d114

// block 0047f78e
0047f78e  push       eax
0047f78f  call       dword ptr [0x4b432c]

// block 0047f795
0047f795  pop        ecx
0047f796  pop        ecx
0047f797  test       eax, eax
0047f799  je         0x47f7a1

// block 0047f79b
0047f79b  push       eax
0047f79c  jmp        0x47f6cd

// block 0047f7a1
0047f7a1  push       0x49dea8
0047f7a6  lea        eax, [ebp - 0x80]
0047f7a9  push       edi
0047f7aa  push       eax
0047f7ab  cmp        bl, 0x44
0047f7ae  jne        0x47f7d0

// block 0047f7b0
0047f7b0  push       0x49de94
0047f7b5  lea        eax, [ebp - 0x88]

// block 0047f7bb
0047f7bb  push       eax
0047f7bc  call       0x47ec4a

// block 0047f7c1
0047f7c1  add        esp, 0xc
0047f7c4  mov        ecx, eax
0047f7c6  call       0x47ec92

// block 0047f7cb
0047f7cb  jmp        0x47f88c

// block 0047f7d0
0047f7d0  push       0x49de10
0047f7d5  lea        eax, [ebp - 0xc8]
0047f7db  jmp        0x47f7bb

// block 0047f7dd
0047f7dd  push       0x7b
0047f7df  lea        ecx, [ebp - 0x80]
0047f7e2  call       0x47e56d

// block 0047f7e7
0047f7e7  cmp        bl, 0x48
0047f7ea  jl         0x47f811

// block 0047f7ec
0047f7ec  cmp        bl, 0x4a
0047f7ef  jg         0x47f811

// block 0047f7f1
0047f7f1  lea        eax, [ebp - 0x90]
0047f7f7  push       eax
0047f7f8  call       0x481298

// block 0047f7fd
0047f7fd  pop        ecx
0047f7fe  push       eax
0047f7ff  lea        ecx, [ebp - 0x80]
0047f802  call       0x47e7bf

// block 0047f807
0047f807  push       0x2c
0047f809  lea        ecx, [ebp - 0x80]
0047f80c  call       0x47e9f6

// block 0047f811
0047f811  sub        esi, 0x46
0047f814  je         0x47f842

// block 0047f816
0047f816  dec        esi
0047f817  je         0x47f822

// block 0047f819
0047f819  dec        esi
0047f81a  je         0x47f862

// block 0047f81c
0047f81c  dec        esi
0047f81d  je         0x47f842

// block 0047f81f
0047f81f  dec        esi
0047f820  jne        0x47f878

// block 0047f822
0047f822  lea        eax, [ebp - 0xa0]
0047f828  push       eax
0047f829  call       0x47f57e

// block 0047f82e
0047f82e  pop        ecx
0047f82f  push       eax
0047f830  lea        ecx, [ebp - 0x80]
0047f833  call       0x47e7bf

// block 0047f838
0047f838  push       0x2c
0047f83a  lea        ecx, [ebp - 0x80]
0047f83d  call       0x47e9f6

// block 0047f842
0047f842  lea        eax, [ebp - 0xb0]
0047f848  push       eax
0047f849  call       0x47f57e

// block 0047f84e
0047f84e  pop        ecx
0047f84f  push       eax
0047f850  lea        ecx, [ebp - 0x80]
0047f853  call       0x47e7bf

// block 0047f858
0047f858  push       0x2c
0047f85a  lea        ecx, [ebp - 0x80]
0047f85d  call       0x47e9f6

// block 0047f862
0047f862  lea        eax, [ebp - 0xc0]
0047f868  push       eax
0047f869  call       0x47f57e

// block 0047f86e
0047f86e  pop        ecx
0047f86f  push       eax
0047f870  lea        ecx, [ebp - 0x80]
0047f873  call       0x47e7bf

// block 0047f878
0047f878  push       0x7d
0047f87a  push       edi
0047f87b  lea        ecx, [ebp - 0x80]
0047f87e  call       0x47ec6e

// block 0047f883
0047f883  jmp        0x47f88c

// block 0047f885
0047f885  push       edi
0047f886  call       0x481298

// block 0047f88b
0047f88b  pop        ecx

// block 0047f88c
0047f88c  mov        ecx, dword ptr [ebp - 4]
0047f88f  mov        eax, edi
0047f891  pop        edi
0047f892  pop        esi
0047f893  xor        ecx, ebp
0047f895  pop        ebx
0047f896  call       0x46c932

// block 0047f89b
0047f89b  leave      
0047f89c  ret        
