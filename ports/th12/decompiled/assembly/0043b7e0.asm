
// block 0043b7e0
0043b7e0  sub        esp, 8
0043b7e3  cmp        dword ptr [0x4b44e8], 0
0043b7ea  push       ebx
0043b7eb  mov        ebx, eax
0043b7ed  jne        0x43b7f9

// block 0043b7ef
0043b7ef  mov        eax, 1
0043b7f4  pop        ebx
0043b7f5  add        esp, 8
0043b7f8  ret        

// block 0043b7f9
0043b7f9  test       dword ptr [0x4ceae8], 0x200
0043b803  mov        eax, dword ptr [0x4d49d0]
0043b808  mov        dword ptr [0x4d49d4], eax
0043b80d  movzx      eax, word ptr [0x4d48b8]
0043b814  mov        dword ptr [0x4d49d0], eax
0043b819  je         0x43b85a

// block 0043b81b
0043b81b  test       al, 8
0043b81d  je         0x43b829

// block 0043b81f
0043b81f  or         eax, 0x400
0043b824  mov        dword ptr [0x4d49d0], eax

// block 0043b829
0043b829  test       al, 1
0043b82b  je         0x43b84e

// block 0043b82d
0043b82d  mov        ecx, dword ptr [0x4d49cc]
0043b833  inc        ecx
0043b834  mov        dword ptr [0x4d49cc], ecx
0043b83a  cmp        ecx, 8
0043b83d  jb         0x43b880

// block 0043b83f
0043b83f  or         eax, 8
0043b842  mov        dword ptr [0x4d49cc], 8
0043b84c  jmp        0x43b87b

// block 0043b84e
0043b84e  mov        dword ptr [0x4d49cc], 0
0043b858  jmp        0x43b880

// block 0043b85a
0043b85a  test       al, 1
0043b85c  je         0x43b880

// block 0043b85e
0043b85e  mov        ecx, dword ptr [0x4d494c]
0043b864  cmp        ecx, 5
0043b867  jae        0x43b880

// block 0043b869
0043b869  test       al, 8
0043b86b  je         0x43b880

// block 0043b86d
0043b86d  cmp        dword ptr [0x4d4958], 5
0043b874  jae        0x43b880

// block 0043b876
0043b876  or         eax, 0x400

// block 0043b87b
0043b87b  mov        dword ptr [0x4d49d0], eax

// block 0043b880
0043b880  mov        ecx, 0x4d48b8
0043b885  call       0x40f690

// block 0043b88a
0043b88a  mov        eax, dword ptr [ebx + 0x1d0]
0043b890  test       eax, eax
0043b892  jl         0x43b7ef

// block 0043b898
0043b898  cdq        
0043b899  mov        ecx, 0x1e
0043b89e  idiv       ecx
0043b8a0  test       edx, edx
0043b8a2  jne        0x43b906

// block 0043b8a4
0043b8a4  mov        edx, dword ptr [0x4b43e0]
0043b8aa  fld        dword ptr [edx + 0x34]
0043b8ad  fadd       qword ptr [0x4a3cf8]
0043b8b3  fld        qword ptr [0x4a3f98]
0043b8b9  fcomp      st(1)
0043b8bb  fnstsw     ax
0043b8bd  test       ah, 0x41
0043b8c0  jp         0x43b8cb

// block 0043b8c2
0043b8c2  fstp       st(0)
0043b8c4  mov        ecx, 0xff
0043b8c9  jmp        0x43b8f0

// block 0043b8cb
0043b8cb  fnstcw     word ptr [esp + 6]
0043b8cf  movzx      eax, word ptr [esp + 6]
0043b8d4  or         eax, 0xc00
0043b8d9  mov        dword ptr [esp + 8], eax
0043b8dd  fldcw      word ptr [esp + 8]
0043b8e1  fistp      dword ptr [esp + 8]
0043b8e5  mov        al, byte ptr [esp + 8]
0043b8e9  movzx      ecx, al
0043b8ec  fldcw      word ptr [esp + 6]

// block 0043b8f0
0043b8f0  mov        edx, dword ptr [ebx + 0xa0]
0043b8f6  mov        eax, dword ptr [edx]
0043b8f8  mov        edx, dword ptr [eax + 0x18a0]
0043b8fe  mov        byte ptr [edx], cl
0043b900  inc        dword ptr [eax + 0x18a0]

// block 0043b906
0043b906  movzx      ecx, word ptr [0x4d49e0]
0043b90d  movzx      edx, word ptr [0x4d49dc]
0043b914  mov        eax, dword ptr [ebx + 0xa0]
0043b91a  mov        eax, dword ptr [eax]
0043b91c  push       ecx
0043b91d  push       edx
0043b91e  mov        edx, dword ptr [0x4d49d0]
0043b924  call       0x43c9b0

// block 0043b929
0043b929  test       eax, eax
0043b92b  je         0x43b940

// block 0043b92d
0043b92d  push       edi
0043b92e  mov        edi, dword ptr [ebx + 0x1d8]
0043b934  call       0x43cbb0

// block 0043b939
0043b939  mov        dword ptr [ebx + 0xa0], eax
0043b93f  pop        edi

// block 0043b940
0043b940  mov        eax, 1
0043b945  add        dword ptr [ebx + 0x1d0], eax
0043b94b  pop        ebx
0043b94c  add        esp, 8
0043b94f  ret        
