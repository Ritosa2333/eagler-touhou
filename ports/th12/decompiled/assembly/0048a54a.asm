
// block 0048a54a
0048a54a  mov        edi, edi
0048a54c  push       ebp
0048a54d  mov        ebp, esp
0048a54f  sub        esp, 0x24
0048a552  push       esi
0048a553  push       edi
0048a554  push       dword ptr [ebp + 0x1c]
0048a557  lea        ecx, [ebp - 0x24]
0048a55a  mov        dword ptr [ebp - 0x14], 0x3ff
0048a561  xor        edi, edi
0048a563  mov        dword ptr [ebp - 4], 0x30
0048a56a  call       0x46d3b4

// block 0048a56f
0048a56f  cmp        dword ptr [ebp + 0x14], edi
0048a572  jge        0x48a577

// block 0048a574
0048a574  mov        dword ptr [ebp + 0x14], edi

// block 0048a577
0048a577  mov        esi, dword ptr [ebp + 0xc]
0048a57a  cmp        esi, edi
0048a57c  jne        0x48a5a9

// block 0048a57e
0048a57e  call       0x46e96f

// block 0048a583
0048a583  push       0x16

// block 0048a585
0048a585  pop        esi
0048a586  push       edi
0048a587  push       edi
0048a588  push       edi
0048a589  push       edi
0048a58a  push       edi
0048a58b  mov        dword ptr [eax], esi
0048a58d  call       0x470f2d

// block 0048a592
0048a592  add        esp, 0x14
0048a595  cmp        byte ptr [ebp - 0x18], 0
0048a599  je         0x48a5a2

// block 0048a59b
0048a59b  mov        eax, dword ptr [ebp - 0x1c]
0048a59e  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a5a2
0048a5a2  mov        eax, esi
0048a5a4  jmp        0x48a8b9

// block 0048a5a9
0048a5a9  cmp        dword ptr [ebp + 0x10], edi
0048a5ac  jbe        0x48a57e

// block 0048a5ae
0048a5ae  mov        eax, dword ptr [ebp + 0x14]
0048a5b1  add        eax, 0xb
0048a5b4  mov        byte ptr [esi], 0
0048a5b7  cmp        dword ptr [ebp + 0x10], eax
0048a5ba  ja         0x48a5c5

// block 0048a5bc
0048a5bc  call       0x46e96f

// block 0048a5c1
0048a5c1  push       0x22
0048a5c3  jmp        0x48a585

// block 0048a5c5
0048a5c5  mov        edi, dword ptr [ebp + 8]
0048a5c8  mov        eax, dword ptr [edi]
0048a5ca  mov        dword ptr [ebp - 0xc], eax
0048a5cd  mov        eax, dword ptr [edi + 4]
0048a5d0  mov        ecx, eax
0048a5d2  shr        ecx, 0x14
0048a5d5  mov        edx, 0x7ff
0048a5da  push       ebx
0048a5db  and        ecx, edx
0048a5dd  xor        ebx, ebx
0048a5df  cmp        ecx, edx
0048a5e1  jne        0x48a677

// block 0048a5e7
0048a5e7  test       ebx, ebx
0048a5e9  jne        0x48a677

// block 0048a5ef
0048a5ef  mov        eax, dword ptr [ebp + 0x10]
0048a5f2  cmp        eax, -1
0048a5f5  jne        0x48a5fb

// block 0048a5f7
0048a5f7  or         eax, eax
0048a5f9  jmp        0x48a5fe

// block 0048a5fb
0048a5fb  add        eax, -2

// block 0048a5fe
0048a5fe  push       0
0048a600  push       dword ptr [ebp + 0x14]
0048a603  lea        ebx, [esi + 2]
0048a606  push       eax
0048a607  push       ebx
0048a608  push       edi
0048a609  call       0x48a52a

// block 0048a60e
0048a60e  add        esp, 0x14
0048a611  test       eax, eax
0048a613  je         0x48a62e

// block 0048a615
0048a615  cmp        byte ptr [ebp - 0x18], 0
0048a619  mov        byte ptr [esi], 0
0048a61c  je         0x48a8b8

// block 0048a622
0048a622  mov        ecx, dword ptr [ebp - 0x1c]
0048a625  and        dword ptr [ecx + 0x70], 0xfffffffd
0048a629  jmp        0x48a8b8

// block 0048a62e
0048a62e  cmp        byte ptr [ebx], 0x2d
0048a631  jne        0x48a637

// block 0048a633
0048a633  mov        byte ptr [esi], 0x2d
0048a636  inc        esi

// block 0048a637
0048a637  mov        byte ptr [esi], 0x30
0048a63a  inc        esi
0048a63b  cmp        dword ptr [ebp + 0x18], 0
0048a63f  push       0x65
0048a641  sete       al
0048a644  dec        al
0048a646  and        al, 0xe0
0048a648  add        al, 0x78
0048a64a  mov        byte ptr [esi], al
0048a64c  inc        esi
0048a64d  push       esi
0048a64e  call       0x46d260

// block 0048a653
0048a653  pop        ecx
0048a654  pop        ecx
0048a655  test       eax, eax
0048a657  je         0x48a8a9

// block 0048a65d
0048a65d  cmp        dword ptr [ebp + 0x18], 0
0048a661  sete       cl
0048a664  dec        cl
0048a666  and        cl, 0xe0
0048a669  add        cl, 0x70
0048a66c  mov        byte ptr [eax], cl
0048a66e  mov        byte ptr [eax + 3], 0
0048a672  jmp        0x48a8a9

// block 0048a677
0048a677  and        eax, 0x80000000
0048a67c  xor        ecx, ecx
0048a67e  or         ecx, eax
0048a680  je         0x48a686

// block 0048a682
0048a682  mov        byte ptr [esi], 0x2d
0048a685  inc        esi

// block 0048a686
0048a686  mov        ebx, dword ptr [ebp + 0x18]
0048a689  mov        byte ptr [esi], 0x30
0048a68c  inc        esi
0048a68d  test       ebx, ebx
0048a68f  sete       al
0048a692  dec        al
0048a694  and        al, 0xe0
0048a696  add        al, 0x78
0048a698  mov        byte ptr [esi], al
0048a69a  mov        ecx, dword ptr [edi + 4]
0048a69d  inc        esi
0048a69e  neg        ebx
0048a6a0  sbb        ebx, ebx
0048a6a2  and        ebx, 0xffffffe0
0048a6a5  and        ecx, 0x7ff00000
0048a6ab  xor        eax, eax
0048a6ad  add        ebx, 0x27
0048a6b0  xor        edx, edx
0048a6b2  or         eax, ecx
0048a6b4  jne        0x48a6d7

// block 0048a6b6
0048a6b6  mov        byte ptr [esi], 0x30
0048a6b9  mov        ecx, dword ptr [edi + 4]
0048a6bc  mov        eax, dword ptr [edi]
0048a6be  and        ecx, 0xfffff
0048a6c4  inc        esi
0048a6c5  or         eax, ecx
0048a6c7  jne        0x48a6ce

// block 0048a6c9
0048a6c9  mov        dword ptr [ebp - 0x14], edx
0048a6cc  jmp        0x48a6db

// block 0048a6ce
0048a6ce  mov        dword ptr [ebp - 0x14], 0x3fe
0048a6d5  jmp        0x48a6db

// block 0048a6d7
0048a6d7  mov        byte ptr [esi], 0x31
0048a6da  inc        esi

// block 0048a6db
0048a6db  mov        eax, esi
0048a6dd  inc        esi
0048a6de  mov        dword ptr [ebp + 0xc], eax
0048a6e1  cmp        dword ptr [ebp + 0x14], edx
0048a6e4  jne        0x48a6ea

// block 0048a6e6
0048a6e6  mov        byte ptr [eax], dl
0048a6e8  jmp        0x48a6f9

// block 0048a6ea
0048a6ea  mov        ecx, dword ptr [ebp - 0x24]
0048a6ed  mov        ecx, dword ptr [ecx + 0xbc]
0048a6f3  mov        ecx, dword ptr [ecx]
0048a6f5  mov        cl, byte ptr [ecx]
0048a6f7  mov        byte ptr [eax], cl

// block 0048a6f9
0048a6f9  mov        ecx, dword ptr [edi + 4]
0048a6fc  mov        eax, dword ptr [edi]
0048a6fe  and        ecx, 0xfffff
0048a704  mov        dword ptr [ebp - 8], ecx
0048a707  ja         0x48a711

// block 0048a709
0048a709  cmp        eax, edx
0048a70b  jbe        0x48a7c6

// block 0048a711
0048a711  mov        dword ptr [ebp - 0xc], edx
0048a714  mov        dword ptr [ebp - 8], 0xf0000

// block 0048a71b
0048a71b  cmp        dword ptr [ebp + 0x14], 0
0048a71f  jle        0x48a76e

// block 0048a721
0048a721  mov        edx, dword ptr [edi + 4]
0048a724  and        edx, dword ptr [ebp - 8]
0048a727  mov        eax, dword ptr [edi]
0048a729  movsx      ecx, word ptr [ebp - 4]
0048a72d  and        eax, dword ptr [ebp - 0xc]
0048a730  and        edx, 0xfffff
0048a736  call       0x48e1b0

// block 0048a73b
0048a73b  add        ax, 0x30
0048a73f  movzx      eax, ax
0048a742  cmp        ax, 0x39
0048a746  jbe        0x48a74a

// block 0048a748
0048a748  add        eax, ebx

// block 0048a74a
0048a74a  mov        ecx, dword ptr [ebp - 8]
0048a74d  sub        dword ptr [ebp - 4], 4
0048a751  mov        byte ptr [esi], al
0048a753  mov        eax, dword ptr [ebp - 0xc]
0048a756  shrd       eax, ecx, 4
0048a75a  shr        ecx, 4
0048a75d  inc        esi
0048a75e  dec        dword ptr [ebp + 0x14]
0048a761  cmp        word ptr [ebp - 4], 0
0048a766  mov        dword ptr [ebp - 0xc], eax
0048a769  mov        dword ptr [ebp - 8], ecx
0048a76c  jge        0x48a71b

// block 0048a76e
0048a76e  cmp        word ptr [ebp - 4], 0
0048a773  jl         0x48a7c6

// block 0048a775
0048a775  mov        edx, dword ptr [edi + 4]
0048a778  and        edx, dword ptr [ebp - 8]
0048a77b  mov        eax, dword ptr [edi]
0048a77d  movsx      ecx, word ptr [ebp - 4]
0048a781  and        eax, dword ptr [ebp - 0xc]
0048a784  and        edx, 0xfffff
0048a78a  call       0x48e1b0

// block 0048a78f
0048a78f  cmp        ax, 8
0048a793  jbe        0x48a7c6

// block 0048a795
0048a795  lea        eax, [esi - 1]

// block 0048a798
0048a798  mov        cl, byte ptr [eax]
0048a79a  cmp        cl, 0x66
0048a79d  je         0x48a7a4

// block 0048a79f
0048a79f  cmp        cl, 0x46
0048a7a2  jne        0x48a7aa

// block 0048a7a4
0048a7a4  mov        byte ptr [eax], 0x30
0048a7a7  dec        eax
0048a7a8  jmp        0x48a798

// block 0048a7aa
0048a7aa  cmp        eax, dword ptr [ebp + 0xc]
0048a7ad  je         0x48a7c3

// block 0048a7af
0048a7af  mov        cl, byte ptr [eax]
0048a7b1  cmp        cl, 0x39
0048a7b4  jne        0x48a7bd

// block 0048a7b6
0048a7b6  add        bl, 0x3a
0048a7b9  mov        byte ptr [eax], bl
0048a7bb  jmp        0x48a7c6

// block 0048a7bd
0048a7bd  inc        cl
0048a7bf  mov        byte ptr [eax], cl
0048a7c1  jmp        0x48a7c6

// block 0048a7c3
0048a7c3  inc        byte ptr [eax - 1]

// block 0048a7c6
0048a7c6  cmp        dword ptr [ebp + 0x14], 0
0048a7ca  jle        0x48a7dd

// block 0048a7cc
0048a7cc  push       dword ptr [ebp + 0x14]
0048a7cf  push       0x30
0048a7d1  push       esi
0048a7d2  call       0x477420

// block 0048a7d7
0048a7d7  add        esp, 0xc
0048a7da  add        esi, dword ptr [ebp + 0x14]

// block 0048a7dd
0048a7dd  mov        eax, dword ptr [ebp + 0xc]
0048a7e0  cmp        byte ptr [eax], 0
0048a7e3  jne        0x48a7e7

// block 0048a7e5
0048a7e5  mov        esi, eax

// block 0048a7e7
0048a7e7  cmp        dword ptr [ebp + 0x18], 0
0048a7eb  mov        cl, 0x34
0048a7ed  sete       al
0048a7f0  dec        al
0048a7f2  and        al, 0xe0
0048a7f4  add        al, 0x70
0048a7f6  mov        byte ptr [esi], al
0048a7f8  mov        eax, dword ptr [edi]
0048a7fa  mov        edx, dword ptr [edi + 4]
0048a7fd  inc        esi
0048a7fe  call       0x48e1b0

// block 0048a803
0048a803  xor        ebx, ebx
0048a805  and        eax, 0x7ff
0048a80a  and        edx, ebx
0048a80c  sub        eax, dword ptr [ebp - 0x14]
0048a80f  push       ebx
0048a810  pop        ecx
0048a811  sbb        edx, ecx
0048a813  js         0x48a821

// block 0048a815
0048a815  jg         0x48a81b

// block 0048a817
0048a817  cmp        eax, ebx
0048a819  jb         0x48a821

// block 0048a81b
0048a81b  mov        byte ptr [esi], 0x2b
0048a81e  inc        esi
0048a81f  jmp        0x48a82b

// block 0048a821
0048a821  mov        byte ptr [esi], 0x2d
0048a824  inc        esi
0048a825  neg        eax
0048a827  adc        edx, ebx
0048a829  neg        edx

// block 0048a82b
0048a82b  cmp        edx, ebx
0048a82d  mov        edi, esi
0048a82f  mov        byte ptr [esi], 0x30
0048a832  jl         0x48a858

// block 0048a834
0048a834  mov        ecx, 0x3e8
0048a839  jg         0x48a83f

// block 0048a83b
0048a83b  cmp        eax, ecx
0048a83d  jb         0x48a858

// block 0048a83f
0048a83f  push       ebx
0048a840  push       ecx
0048a841  push       edx
0048a842  push       eax
0048a843  call       0x48e0d0

// block 0048a848
0048a848  add        al, 0x30
0048a84a  mov        byte ptr [esi], al
0048a84c  inc        esi
0048a84d  mov        dword ptr [ebp - 0x10], edx
0048a850  mov        eax, ecx
0048a852  mov        edx, ebx
0048a854  cmp        esi, edi
0048a856  jne        0x48a863

// block 0048a858
0048a858  test       edx, edx
0048a85a  jl         0x48a87a

// block 0048a85c
0048a85c  jg         0x48a863

// block 0048a85e
0048a85e  cmp        eax, 0x64
0048a861  jb         0x48a87a

// block 0048a863
0048a863  push       0
0048a865  push       0x64
0048a867  push       edx
0048a868  push       eax
0048a869  call       0x48e0d0

// block 0048a86e
0048a86e  add        al, 0x30
0048a870  mov        byte ptr [esi], al
0048a872  mov        dword ptr [ebp - 0x10], edx
0048a875  inc        esi
0048a876  mov        eax, ecx
0048a878  mov        edx, ebx

// block 0048a87a
0048a87a  cmp        esi, edi
0048a87c  jne        0x48a889

// block 0048a87e
0048a87e  test       edx, edx
0048a880  jl         0x48a8a1

// block 0048a882
0048a882  jg         0x48a889

// block 0048a884
0048a884  cmp        eax, 0xa
0048a887  jb         0x48a8a1

// block 0048a889
0048a889  push       0
0048a88b  push       0xa
0048a88d  push       edx
0048a88e  push       eax
0048a88f  call       0x48e0d0

// block 0048a894
0048a894  add        al, 0x30
0048a896  mov        byte ptr [esi], al
0048a898  mov        dword ptr [ebp - 0x10], edx
0048a89b  inc        esi
0048a89c  mov        eax, ecx
0048a89e  mov        dword ptr [ebp - 0x10], ebx

// block 0048a8a1
0048a8a1  add        al, 0x30
0048a8a3  mov        byte ptr [esi], al
0048a8a5  mov        byte ptr [esi + 1], 0

// block 0048a8a9
0048a8a9  cmp        byte ptr [ebp - 0x18], 0
0048a8ad  je         0x48a8b6

// block 0048a8af
0048a8af  mov        eax, dword ptr [ebp - 0x1c]
0048a8b2  and        dword ptr [eax + 0x70], 0xfffffffd

// block 0048a8b6
0048a8b6  xor        eax, eax

// block 0048a8b8
0048a8b8  pop        ebx

// block 0048a8b9
0048a8b9  pop        edi
0048a8ba  pop        esi
0048a8bb  leave      
0048a8bc  ret        
