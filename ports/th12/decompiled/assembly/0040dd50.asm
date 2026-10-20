
// block 0040dd50
0040dd50  sub        esp, 0x10
0040dd53  push       ebx
0040dd54  push       esi
0040dd55  mov        esi, dword ptr [0x4b43cc]
0040dd5b  mov        eax, dword ptr [esi + 0x7c]
0040dd5e  push       edi
0040dd5f  test       al, 1
0040dd61  je         0x40dd81

// block 0040dd63
0040dd63  test       al, 0x40
0040dd65  jne        0x40dec6

// block 0040dd6b
0040dd6b  call       0x4508b0

// block 0040dd70
0040dd70  fstp       qword ptr [esi + 0x98]
0040dd76  or         dword ptr [esi + 0x7c], 0x40
0040dd7a  pop        edi
0040dd7b  pop        esi
0040dd7c  pop        ebx
0040dd7d  add        esp, 0x10
0040dd80  ret        

// block 0040dd81
0040dd81  test       al, 0x40
0040dd83  je         0x40dec6

// block 0040dd89
0040dd89  mov        eax, dword ptr [esi + 0x90]
0040dd8f  mov        dword ptr [esi + 0x94], eax
0040dd95  call       0x4508b0

// block 0040dd9a
0040dd9a  fsub       qword ptr [esi + 0x98]
0040dda0  fst        qword ptr [esp + 0x10]
0040dda4  fst        qword ptr [esi + 0xa0]
0040ddaa  fld        qword ptr [0x4a3e60]
0040ddb0  call       0x4933ca

// block 0040ddb5
0040ddb5  fld        qword ptr [esp + 0x10]
0040ddb9  fsub       st(1)
0040ddbb  fst        qword ptr [esi + 0xa0]
0040ddc1  fld        qword ptr [0x4a3e58]
0040ddc7  fcomp      st(2)
0040ddc9  fnstsw     ax
0040ddcb  fstp       st(1)
0040ddcd  test       ah, 0x41
0040ddd0  jp         0x40dde0

// block 0040ddd2
0040ddd2  fadd       qword ptr [0x4a3e60]
0040ddd8  fstp       qword ptr [esi + 0xa0]
0040ddde  jmp        0x40dde2

// block 0040dde0
0040dde0  fstp       st(0)

// block 0040dde2
0040dde2  fld        qword ptr [esi + 0xa0]
0040dde8  sub        esp, 8
0040ddeb  fstp       qword ptr [esp]
0040ddee  call       0x493290

// block 0040ddf3
0040ddf3  add        esp, 8
0040ddf6  fld        st(0)
0040ddf8  call       0x4931e0

// block 0040ddfd
0040ddfd  mov        edi, eax
0040ddff  cmp        edi, 0x3e8
0040de05  jl         0x40de0c

// block 0040de07
0040de07  mov        edi, 0x3e7

// block 0040de0c
0040de0c  fsubr      qword ptr [esi + 0xa0]
0040de12  fmul       qword ptr [0x4a3ce8]
0040de18  call       0x4931e0

// block 0040de1d
0040de1d  fldz       
0040de1f  mov        ecx, eax
0040de21  fstp       qword ptr [esi + 0xa0]
0040de27  lea        ebx, [ecx + edi + 0x16]
0040de2b  lea        eax, [edi + 0x42]
0040de2e  imul       ebx, ebx, 0x3e8
0040de34  cdq        
0040de35  mov        edi, 0x3e8
0040de3a  idiv       edi
0040de3c  lea        eax, [ecx + 0x21]
0040de3f  mov        ecx, 0x64
0040de44  and        dword ptr [esi + 0x7c], 0xffffffbf
0040de48  add        ebx, edx
0040de4a  cdq        
0040de4b  imul       ebx, ebx, 0x64
0040de4e  idiv       ecx
0040de50  mov        ecx, dword ptr [esi + 0x8c]
0040de56  lea        eax, [ebx + edx]
0040de59  mov        edx, dword ptr [0x4b44e8]
0040de5f  mov        dword ptr [esi + 0xa8], eax
0040de65  cmp        dword ptr [edx + 0x74], 0
0040de69  mov        edx, dword ptr [0x4b4518]
0040de6f  jne        0x40de8c

// block 0040de71
0040de71  mov        edi, dword ptr [0x4b0cb0]
0040de77  mov        edx, dword ptr [edx + edi*4 + 0x20]
0040de7b  mov        dword ptr [edx + ecx*4 + 0x4c], eax
0040de7f  inc        dword ptr [esi + 0x8c]
0040de85  pop        edi
0040de86  pop        esi
0040de87  pop        ebx
0040de88  add        esp, 0x10
0040de8b  ret        

// block 0040de8c
0040de8c  mov        eax, dword ptr [0x4b0cb0]
0040de91  lea        eax, [eax + eax*8]
0040de94  mov        eax, dword ptr [edx + eax*4 + 0xb8]
0040de9b  mov        ecx, dword ptr [eax + ecx*4 + 0x4c]
0040de9f  mov        eax, esi
0040dea1  mov        dword ptr [esi + 0xa8], ecx
0040dea7  call       0x40d710

// block 0040deac
0040deac  test       eax, eax
0040deae  je         0x40dec0

// block 0040deb0
0040deb0  push       esi
0040deb1  mov        ecx, 0x63
0040deb6  mov        eax, 0x3e7
0040debb  call       0x40d770

// block 0040dec0
0040dec0  inc        dword ptr [esi + 0x8c]

// block 0040dec6
0040dec6  pop        edi
0040dec7  pop        esi
0040dec8  pop        ebx
0040dec9  add        esp, 0x10
0040decc  ret        
