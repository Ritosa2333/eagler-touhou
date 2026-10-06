
// block 0042c3f0
0042c3f0  push       ebp
0042c3f1  mov        ebp, esp
0042c3f3  and        esp, 0xfffffff8
0042c3f6  sub        esp, 0x14
0042c3f9  push       ebx
0042c3fa  mov        ebx, ecx
0042c3fc  push       esi
0042c3fd  mov        esi, dword ptr [ebp + 8]
0042c400  push       edi
0042c401  lea        edi, [ebx + 0x454]
0042c407  mov        ecx, 0x78

// block 0042c40c
0042c40c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042c40e
0042c40e  mov        ax, word ptr [ebx + 0x46c]
0042c415  mov        cx, word ptr [ebx + 0x46e]
0042c41c  mov        edi, 2
0042c421  lea        esi, [ebx + 0x634]
0042c427  mov        dword ptr [ebx + 0xc], edi
0042c42a  mov        dword ptr [ebx + 0x10], edi
0042c42d  mov        word ptr [ebx + 0x450], ax
0042c434  mov        word ptr [ebx + 0x452], cx
0042c43b  call       0x402520

// block 0042c440
0042c440  movsx      edx, word ptr [ebx + 0x450]
0042c447  mov        ecx, dword ptr [0x4b44f4]
0042c44d  mov        dword ptr [ebx + 0xae0], 0x42e6d0
0042c457  mov        dword ptr [ebx + 0xae4], ebx
0042c45d  mov        ecx, dword ptr [ecx + 0x488]
0042c463  add        edx, 0xd9
0042c469  mov        eax, esi
0042c46b  call       0x454ee0

// block 0042c470
0042c470  mov        eax, dword ptr [esi + 0x494]
0042c476  test       eax, eax
0042c478  je         0x42c480

// block 0042c47a
0042c47a  mov        edx, edi
0042c47c  mov        ecx, esi
0042c47e  call       eax

// block 0042c480
0042c480  push       esi
0042c481  mov        word ptr [esi + 0x3c4], di
0042c488  call       0x455630

// block 0042c48d
0042c48d  mov        edx, dword ptr [esi + 0x47c]
0042c493  mov        ecx, dword ptr [0x4b44f4]
0042c499  and        edx, 0xffffff3f
0042c49f  or         edx, 0x20
0042c4a2  mov        dword ptr [esi + 0x47c], edx
0042c4a8  mov        eax, dword ptr [ebx + 0xab0]
0042c4ae  movsx      edi, word ptr [ebx + 0x46e]
0042c4b5  and        eax, 0xf0c7ffff
0042c4ba  or         eax, 0xc00000
0042c4bf  mov        dword ptr [ebx + 0xab0], eax
0042c4c5  mov        edx, dword ptr [ecx + 0x488]
0042c4cb  lea        esi, [ebx + 0xae8]
0042c4d1  add        edi, 0x35
0042c4d4  mov        dword ptr [esp + 0x10], edx
0042c4d8  call       0x402520

// block 0042c4dd
0042c4dd  mov        ecx, dword ptr [esp + 0x10]
0042c4e1  mov        al, 0x10
0042c4e3  push       edi
0042c4e4  push       esi
0042c4e5  mov        byte ptr [esi + 0x49d], al
0042c4eb  mov        byte ptr [esi + 0x49c], al
0042c4f1  call       0x454d10

// block 0042c4f6
0042c4f6  mov        eax, dword ptr [esi + 0x494]
0042c4fc  test       eax, eax
0042c4fe  je         0x42c509

// block 0042c500
0042c500  mov        edx, 2
0042c505  mov        ecx, esi
0042c507  call       eax

// block 0042c509
0042c509  mov        eax, 2
0042c50e  push       esi
0042c50f  mov        word ptr [esi + 0x3c4], ax
0042c516  call       0x455630

// block 0042c51b
0042c51b  mov        ecx, dword ptr [esi + 0x47c]
0042c521  and        ecx, 0xffffff3f
0042c527  or         ecx, 0x20
0042c52a  mov        dword ptr [esi + 0x47c], ecx
0042c530  mov        ecx, dword ptr [ebx + 0x470]
0042c536  mov        edx, dword ptr [ebx + 0xf64]
0042c53c  lea        eax, [ecx*8]
0042c543  sub        eax, ecx
0042c545  add        eax, eax
0042c547  add        eax, eax
0042c549  and        edx, 0xf0ffffff
0042c54f  add        eax, eax
0042c551  or         edx, 0x800000
0042c557  push       eax
0042c558  mov        dword ptr [ebx + 0xf64], edx
0042c55e  call       0x46d04a

// block 0042c563
0042c563  mov        dword ptr [ebx + 0xfa0], eax
0042c569  mov        eax, dword ptr [ebx + 0x470]
0042c56f  lea        eax, [eax + eax*4]
0042c572  add        eax, eax
0042c574  add        eax, eax
0042c576  add        esp, 4
0042c579  push       eax
0042c57a  call       0x46d04a

// block 0042c57f
0042c57f  fldz       
0042c581  fcom       dword ptr [ebx + 0x474]
0042c587  mov        ecx, dword ptr [ebx + 0x458]
0042c58d  mov        edx, dword ptr [ebx + 0x45c]
0042c593  mov        dword ptr [ebx + 0xf9c], eax
0042c599  mov        eax, dword ptr [ebx + 0x454]
0042c59f  mov        dword ptr [ebx + 0x50], eax
0042c5a2  fnstsw     ax
0042c5a4  add        esp, 4
0042c5a7  mov        dword ptr [ebx + 0x54], ecx
0042c5aa  mov        dword ptr [ebx + 0x58], edx
0042c5ad  test       ah, 0x44
0042c5b0  jnp        0x42c5e9

// block 0042c5b2
0042c5b2  fstp       st(0)
0042c5b4  sub        esp, 8
0042c5b7  fld        dword ptr [ebx + 0x474]
0042c5bd  lea        ecx, [esp + 0x1c]
0042c5c1  fstp       dword ptr [esp + 4]
0042c5c5  fld        dword ptr [ebx + 0x460]
0042c5cb  fstp       dword ptr [esp]
0042c5ce  call       0x42e880

// block 0042c5d3
0042c5d3  fld        dword ptr [ebx + 0x50]
0042c5d6  fadd       dword ptr [esp + 0x14]
0042c5da  fstp       dword ptr [ebx + 0x50]
0042c5dd  fld        dword ptr [ebx + 0x54]
0042c5e0  fadd       dword ptr [esp + 0x18]
0042c5e4  fstp       dword ptr [ebx + 0x54]
0042c5e7  fldz       

// block 0042c5e9
0042c5e9  xor        edx, edx
0042c5eb  cmp        dword ptr [ebx + 0x470], edx
0042c5f1  jle        0x42c63b

// block 0042c5f3
0042c5f3  xor        ecx, ecx

// block 0042c5f5
0042c5f5  mov        eax, dword ptr [ebx + 0xf9c]
0042c5fb  mov        esi, dword ptr [ebx + 0x50]
0042c5fe  mov        dword ptr [eax + ecx], esi
0042c601  mov        esi, dword ptr [ebx + 0x54]
0042c604  add        eax, ecx
0042c606  mov        dword ptr [eax + 4], esi
0042c609  mov        esi, dword ptr [ebx + 0x58]
0042c60c  mov        dword ptr [eax + 8], esi
0042c60f  fld        dword ptr [ebx + 0x460]
0042c615  mov        eax, dword ptr [ebx + 0xf9c]
0042c61b  fstp       dword ptr [ecx + eax + 0xc]
0042c61f  mov        eax, dword ptr [ebx + 0xf9c]
0042c625  fld        dword ptr [ebx + 0x468]
0042c62b  inc        edx
0042c62c  fstp       dword ptr [ecx + eax + 0x10]
0042c630  add        ecx, 0x14
0042c633  cmp        edx, dword ptr [ebx + 0x470]
0042c639  jl         0x42c5f5

// block 0042c63b
0042c63b  mov        eax, dword ptr [ebx + 0x448]
0042c641  mov        edi, 0xfff0bdc1
0042c646  mov        esi, 0x4b2ed0
0042c64b  test       al, 1
0042c64d  jne        0x42c674

// block 0042c64f
0042c64f  or         eax, 1
0042c652  fst        dword ptr [ebx + 0x440]
0042c658  mov        dword ptr [ebx + 0x43c], 0
0042c662  mov        dword ptr [ebx + 0x438], edi
0042c668  mov        dword ptr [ebx + 0x444], esi
0042c66e  mov        dword ptr [ebx + 0x448], eax

// block 0042c674
0042c674  fld        dword ptr [0x4a3e14]
0042c67a  mov        dword ptr [ebx + 0x43c], 0x1e
0042c684  fstp       dword ptr [ebx + 0x440]
0042c68a  mov        dword ptr [ebx + 0x438], 0x1d
0042c694  mov        eax, dword ptr [ebx + 0x62c]
0042c69a  test       eax, eax
0042c69c  jl         0x42c6a9

// block 0042c69e
0042c69e  push       ecx
0042c69f  fstp       dword ptr [esp]
0042c6a2  call       0x453e20

// block 0042c6a7
0042c6a7  fldz       

// block 0042c6a9
0042c6a9  mov        eax, dword ptr [ebx + 0x38]
0042c6ac  test       al, 1
0042c6ae  jne        0x42c6c6

// block 0042c6b0
0042c6b0  or         eax, 1
0042c6b3  fst        dword ptr [ebx + 0x30]
0042c6b6  mov        dword ptr [ebx + 0x2c], 0
0042c6bd  mov        dword ptr [ebx + 0x28], edi
0042c6c0  mov        dword ptr [ebx + 0x34], esi
0042c6c3  mov        dword ptr [ebx + 0x38], eax

// block 0042c6c6
0042c6c6  fstp       dword ptr [ebx + 0x30]
0042c6c9  mov        dword ptr [ebx + 0x2c], 0
0042c6d0  mov        dword ptr [ebx + 0x28], 0xffffffff
0042c6d7  fld        dword ptr [ebx + 0x464]
0042c6dd  fstp       dword ptr [ebx + 0x70]
0042c6e0  sub        esp, 8
0042c6e3  fld        dword ptr [ebx + 0x460]
0042c6e9  lea        ecx, [ebx + 0x5c]
0042c6ec  fstp       dword ptr [esp + 0x18]
0042c6f0  fld        dword ptr [esp + 0x18]
0042c6f4  fst        dword ptr [ebx + 0x68]
0042c6f7  fld        dword ptr [ebx + 0x468]
0042c6fd  fstp       dword ptr [esp + 0x18]
0042c701  fld        dword ptr [esp + 0x18]
0042c705  fst        dword ptr [ebx + 0x74]
0042c708  fstp       dword ptr [esp + 4]
0042c70c  fstp       dword ptr [esp]
0042c70f  call       0x42e880

// block 0042c714
0042c714  pop        edi
0042c715  pop        esi
0042c716  xor        eax, eax
0042c718  pop        ebx
0042c719  mov        esp, ebp
0042c71b  pop        ebp
0042c71c  ret        4
