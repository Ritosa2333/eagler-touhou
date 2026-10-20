
// block 0045c560
0045c560  push       esi
0045c561  mov        esi, eax
0045c563  cmp        dword ptr [esi + 0x4b56a0], 0
0045c56a  je         0x45c571

// block 0045c56c
0045c56c  call       0x45a3c0

// block 0045c571
0045c571  cmp        byte ptr [esi + 0x4b5642], 4
0045c578  je         0x45c593

// block 0045c57a
0045c57a  mov        eax, dword ptr [0x4ce8f0]
0045c57f  mov        ecx, dword ptr [eax]
0045c581  mov        edx, dword ptr [ecx + 0x164]
0045c587  push       0x44
0045c589  push       eax
0045c58a  call       edx

// block 0045c58c
0045c58c  mov        byte ptr [esi + 0x4b5642], 4

// block 0045c593
0045c593  mov        eax, esi
0045c595  call       0x459cf0

// block 0045c59a
0045c59a  mov        eax, dword ptr [0x4ce8f0]
0045c59f  mov        ecx, dword ptr [eax]
0045c5a1  mov        edx, dword ptr [ecx + 0x10c]
0045c5a7  push       2
0045c5a9  push       4
0045c5ab  push       0
0045c5ad  push       eax
0045c5ae  call       edx

// block 0045c5b0
0045c5b0  mov        eax, dword ptr [0x4ce8f0]
0045c5b5  mov        ecx, dword ptr [eax]
0045c5b7  mov        edx, dword ptr [ecx + 0x10c]
0045c5bd  push       2
0045c5bf  push       1
0045c5c1  push       0
0045c5c3  push       eax
0045c5c4  call       edx

// block 0045c5c6
0045c5c6  mov        eax, dword ptr [0x4ce8f0]
0045c5cb  mov        ecx, dword ptr [eax]
0045c5cd  mov        edx, dword ptr [ecx + 0x10c]
0045c5d3  push       0
0045c5d5  push       5
0045c5d7  push       0
0045c5d9  push       eax
0045c5da  call       edx

// block 0045c5dc
0045c5dc  mov        eax, dword ptr [0x4ce8f0]
0045c5e1  mov        ecx, dword ptr [eax]
0045c5e3  mov        edx, dword ptr [ecx + 0x10c]
0045c5e9  push       0
0045c5eb  push       2
0045c5ed  push       0
0045c5ef  push       eax
0045c5f0  call       edx

// block 0045c5f2
0045c5f2  mov        esi, dword ptr [0x4ce8cc]
0045c5f8  call       0x45a3c0

// block 0045c5fd
0045c5fd  mov        eax, dword ptr [0x4ce8f0]
0045c602  mov        ecx, dword ptr [eax]
0045c604  mov        edx, dword ptr [ecx + 0xe4]
0045c60a  push       0
0045c60c  push       0xe
0045c60e  push       eax
0045c60f  call       edx

// block 0045c611
0045c611  mov        edx, dword ptr [esp + 8]
0045c615  mov        eax, dword ptr [0x4ce8f0]
0045c61a  mov        ecx, dword ptr [eax]
0045c61c  push       0x14
0045c61e  push       edx
0045c61f  mov        edx, dword ptr [esp + 0x14]
0045c623  add        edx, -2
0045c626  push       edx
0045c627  push       5
0045c629  push       eax
0045c62a  mov        eax, dword ptr [ecx + 0x14c]
0045c630  call       eax

// block 0045c632
0045c632  mov        eax, dword ptr [0x4ce8cc]
0045c637  mov        cl, 0xff
0045c639  push       4
0045c63b  mov        byte ptr [eax + 0x4b5642], cl
0045c641  mov        byte ptr [eax + 0x4b5641], cl
0045c647  mov        byte ptr [eax + 0x4b5640], 7
0045c64e  mov        byte ptr [eax + 0x4b5643], cl
0045c654  mov        eax, dword ptr [0x4ce8f0]
0045c659  mov        ecx, dword ptr [eax]
0045c65b  mov        edx, dword ptr [ecx + 0x10c]
0045c661  push       4
0045c663  push       0
0045c665  push       eax
0045c666  call       edx

// block 0045c668
0045c668  mov        eax, dword ptr [0x4ce8f0]
0045c66d  mov        ecx, dword ptr [eax]
0045c66f  mov        edx, dword ptr [ecx + 0x10c]
0045c675  push       4
0045c677  push       1
0045c679  push       0
0045c67b  push       eax
0045c67c  call       edx

// block 0045c67e
0045c67e  mov        eax, dword ptr [0x4ce8f0]
0045c683  mov        ecx, dword ptr [eax]
0045c685  push       2
0045c687  push       5
0045c689  mov        edx, dword ptr [ecx + 0x10c]
0045c68f  push       0
0045c691  push       eax
0045c692  call       edx

// block 0045c694
0045c694  mov        eax, dword ptr [0x4ce8f0]
0045c699  mov        ecx, dword ptr [eax]
0045c69b  mov        edx, dword ptr [ecx + 0x10c]
0045c6a1  push       2
0045c6a3  push       2
0045c6a5  push       0
0045c6a7  push       eax
0045c6a8  call       edx

// block 0045c6aa
0045c6aa  xor        eax, eax
0045c6ac  pop        esi
0045c6ad  ret        8
