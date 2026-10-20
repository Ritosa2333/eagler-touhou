
// block 0044e4d0
0044e4d0  push       0x1a
0044e4d2  call       0x44d640

// block 0044e4d7
0044e4d7  test       al, al
0044e4d9  jne        0x44e4e2

// block 0044e4db
0044e4db  push       0x15
0044e4dd  call       0x44d640

// block 0044e4e2
0044e4e2  push       ebx
0044e4e3  mov        ebx, dword ptr [0x498088]
0044e4e9  push       ebp
0044e4ea  push       esi
0044e4eb  push       edi
0044e4ec  mov        edi, dword ptr [0x49808c]
0044e4f2  xor        esi, esi
0044e4f4  mov        ebp, 0x8000
0044e4f9  lea        esp, [esp]

// block 0044e500
0044e500  test       dword ptr [0x4cee78], ebp
0044e506  je         0x44e515

// block 0044e508
0044e508  push       0x4cf1e8
0044e50d  call       ebx

// block 0044e50f
0044e50f  inc        byte ptr [0x4cf222]

// block 0044e515
0044e515  mov        eax, dword ptr [0x4ce568]
0044e51a  inc        dword ptr [0x4ce56c]
0044e520  xor        eax, 0x9630
0044e525  sub        eax, 0x6553
0044e52a  movzx      ecx, ax
0044e52d  mov        ax, cx
0044e530  add        ecx, ecx
0044e532  shr        ax, 0xe
0044e536  add        ecx, ecx
0044e538  add        ax, cx
0044e53b  mov        word ptr [0x4ce568], ax
0044e541  test       dword ptr [0x4cee78], ebp
0044e547  je         0x44e55c

// block 0044e549
0044e549  push       0x4cf1e8
0044e54e  call       edi

// block 0044e550
0044e550  dec        byte ptr [0x4cf222]
0044e556  mov        ax, word ptr [0x4ce568]

// block 0044e55c
0044e55c  shr        ax, 9
0044e560  mov        byte ptr [esi + 0x4b0d48], al
0044e566  inc        esi
0044e567  cmp        esi, 0x100
0044e56d  jb         0x44e500

// block 0044e56f
0044e56f  push       0x4a245c
0044e574  push       0x11
0044e576  push       4
0044e578  mov        esi, dword ptr [0x498034]
0044e57e  push       0
0044e580  push       0
0044e582  push       0x80
0044e587  push       0
0044e589  push       0
0044e58b  push       0
0044e58d  push       0x190
0044e592  push       0
0044e594  push       0
0044e596  push       0
0044e598  push       0x20
0044e59a  call       esi

// block 0044e59c
0044e59c  push       0x4a246c
0044e5a1  push       0x11
0044e5a3  push       4
0044e5a5  push       0
0044e5a7  push       0
0044e5a9  push       0x80
0044e5ae  push       0
0044e5b0  push       0
0044e5b2  push       0
0044e5b4  push       0x258
0044e5b9  push       0
0044e5bb  push       0
0044e5bd  push       0
0044e5bf  push       0x20
0044e5c1  mov        dword ptr [0x4ce554], eax
0044e5c6  call       esi

// block 0044e5c8
0044e5c8  push       0x4a245c
0044e5cd  push       0x11
0044e5cf  push       4
0044e5d1  push       0
0044e5d3  push       0
0044e5d5  push       0x80
0044e5da  push       0
0044e5dc  push       0
0044e5de  push       0
0044e5e0  push       0x2bc
0044e5e5  push       0
0044e5e7  push       0
0044e5e9  push       0
0044e5eb  push       0xf
0044e5ed  mov        dword ptr [0x4ce550], eax
0044e5f2  call       esi

// block 0044e5f4
0044e5f4  push       0x4a246c
0044e5f9  push       0x11
0044e5fb  push       4
0044e5fd  push       0
0044e5ff  push       0
0044e601  push       0x80
0044e606  push       0
0044e608  push       0
0044e60a  push       0
0044e60c  push       0x2bc
0044e611  push       0
0044e613  push       0
0044e615  push       0
0044e617  push       0xf
0044e619  mov        dword ptr [0x4cc54c], eax
0044e61e  call       esi

// block 0044e620
0044e620  pop        edi
0044e621  pop        esi
0044e622  pop        ebp
0044e623  mov        dword ptr [0x4b453c], eax
0044e628  pop        ebx
0044e629  ret        
