
// block 00423ed0
00423ed0  push       ebp
00423ed1  mov        ebp, esp
00423ed3  and        esp, 0xfffffff8
00423ed6  mov        eax, dword ptr [edi + 0xd8]
00423edc  sub        esp, 0x18
00423edf  push       ebx
00423ee0  push       esi
00423ee1  cmp        eax, dword ptr [0x4b0cb8]
00423ee7  je         0x423ef0

// block 00423ee9
00423ee9  mov        dword ptr [edi + 0x14], 0

// block 00423ef0
00423ef0  mov        eax, dword ptr [0x4b0cb0]
00423ef5  lea        ecx, [eax + eax*2]
00423ef8  mov        eax, dword ptr [edi + ecx*4 + 0x1c]
00423efc  push       edi
00423efd  call       0x4237e0

// block 00423f02
00423f02  mov        eax, dword ptr [0x4b4514]
00423f07  fld        dword ptr [eax + 0x97c]
00423f0d  lea        esi, [edi + 0x17c]
00423f13  fadd       qword ptr [0x4a3d70]
00423f19  lea        ebx, [edi + 0x108]
00423f1f  mov        dword ptr [esp + 0xc], 0xa
00423f27  fadd       qword ptr [0x4a3d28]
00423f2d  fstp       dword ptr [esp + 0x14]
00423f31  fld        dword ptr [eax + 0x980]
00423f37  fadd       qword ptr [0x4a3d68]
00423f3d  fstp       dword ptr [esp + 0x18]
00423f41  fld        qword ptr [0x4a3cf8]

// block 00423f47
00423f47  mov        ecx, dword ptr [esi - 0xa0]
00423f4d  test       ecx, ecx
00423f4f  jne        0x423f55

// block 00423f51
00423f51  xor        ecx, ecx
00423f53  jmp        0x423f98

// block 00423f55
00423f55  mov        edx, dword ptr [0x4ce8cc]
00423f5b  mov        eax, dword ptr [edx + 0x8856b8]
00423f61  test       eax, eax
00423f63  je         0x423f72

// block 00423f65
00423f65  mov        edx, dword ptr [eax]
00423f67  cmp        dword ptr [edx], ecx
00423f69  je         0x423f92

// block 00423f6b
00423f6b  mov        eax, dword ptr [eax + 4]
00423f6e  test       eax, eax
00423f70  jne        0x423f65

// block 00423f72
00423f72  mov        eax, dword ptr [0x4ce8cc]
00423f77  mov        eax, dword ptr [eax + 0x8856c0]
00423f7d  test       eax, eax
00423f7f  je         0x423f51

// block 00423f81
00423f81  mov        edx, dword ptr [eax]
00423f83  cmp        dword ptr [edx], ecx
00423f85  je         0x423f92

// block 00423f87
00423f87  mov        eax, dword ptr [eax + 4]
00423f8a  test       eax, eax
00423f8c  jne        0x423f81

// block 00423f8e
00423f8e  xor        ecx, ecx
00423f90  jmp        0x423f98

// block 00423f92
00423f92  mov        ecx, edx
00423f94  test       ecx, ecx
00423f96  jne        0x423faa

// block 00423f98
00423f98  mov        dword ptr [esi - 0xa0], 0
00423fa2  test       ecx, ecx
00423fa4  je         0x424155

// block 00423faa
00423faa  cmp        dword ptr [ecx + 0x158], 0
00423fb1  jne        0x424155

// block 00423fb7
00423fb7  fld        dword ptr [esp + 0x14]
00423fbb  fsub       dword ptr [ebx - 4]
00423fbe  fstp       dword ptr [esp + 8]
00423fc2  fld        dword ptr [esp + 8]
00423fc6  fabs       
00423fc8  fstp       dword ptr [esp + 8]
00423fcc  fld        dword ptr [esp + 8]
00423fd0  fstp       dword ptr [esp + 0x10]
00423fd4  fld        dword ptr [esp + 0x18]
00423fd8  fsub       dword ptr [ebx]
00423fda  fstp       dword ptr [esp + 8]
00423fde  fld        dword ptr [esp + 8]
00423fe2  fabs       
00423fe4  fstp       dword ptr [esp + 8]
00423fe8  fld        dword ptr [esp + 8]
00423fec  fstp       dword ptr [esp + 8]
00423ff0  fld        dword ptr [esi]
00423ff2  fmul       st(1)
00423ff4  fld        dword ptr [esp + 0x10]
00423ff8  fcom       st(1)
00423ffa  fnstsw     ax
00423ffc  fld        dword ptr [esp + 8]
00424000  test       ah, 5
00424003  jp         0x424038

// block 00424005
00424005  fld        dword ptr [ecx + 0x5c]
00424008  fmul       dword ptr [ecx + 0x44]
0042400b  fstp       dword ptr [esp + 0x10]
0042400f  fld        dword ptr [esp + 0x10]
00424013  fmul       st(4)
00424015  fcomp      st(1)
00424017  fnstsw     ax
00424019  test       ah, 0x41
0042401c  jne        0x424038

// block 0042401e
0042401e  mov        al, byte ptr [ecx + 0x400]
00424024  fstp       st(2)
00424026  fstp       st(0)
00424028  shr        al, 2
0042402b  fstp       st(0)
0042402d  mov        byte ptr [ecx + 0x3bf], al
00424033  jmp        0x424155

// block 00424038
00424038  fld        st(2)
0042403a  fadd       qword ptr [0x4a3d70]
00424040  fcom       st(2)
00424042  fnstsw     ax
00424044  test       ah, 0x41
00424047  jne        0x4240bb

// block 00424049
00424049  fld        dword ptr [ecx + 0x5c]
0042404c  fmul       dword ptr [ecx + 0x44]
0042404f  fstp       dword ptr [esp + 0x10]
00424053  fld        dword ptr [esp + 0x10]
00424057  fmul       st(5)
00424059  fcomp      st(2)
0042405b  fnstsw     ax
0042405d  test       ah, 0x41
00424060  jne        0x4240bb

// block 00424062
00424062  mov        eax, dword ptr [ecx + 0x400]
00424068  fstp       st(3)
0042406a  mov        dword ptr [esp + 8], eax
0042406e  fstp       st(0)
00424070  fild       dword ptr [esp + 8]
00424074  lea        eax, [eax + eax*2]
00424077  cdq        
00424078  fxch       st(2)
0042407a  and        edx, 3
0042407d  fsubrp     st(1)
0042407f  add        eax, edx
00424081  fnstcw     word ptr [esp + 8]
00424085  sar        eax, 2
00424088  mov        dword ptr [esp + 0x10], eax
0042408c  movzx      eax, word ptr [esp + 8]
00424091  fimul      dword ptr [esp + 0x10]
00424095  or         eax, 0xc00
0042409a  mov        dword ptr [esp + 0x10], eax
0042409e  fmul       qword ptr [0x4a4188]
004240a4  fsubp      st(1)
004240a6  fldcw      word ptr [esp + 0x10]
004240aa  fistp      dword ptr [esp + 0x10]
004240ae  mov        dl, byte ptr [esp + 0x10]
004240b2  fldcw      word ptr [esp + 8]
004240b6  jmp        0x42414f

// block 004240bb
004240bb  fstp       st(0)
004240bd  fxch       st(1)
004240bf  fcomp      st(2)
004240c1  fnstsw     ax
004240c3  fstp       st(1)
004240c5  test       ah, 5
004240c8  jp         0x424147

// block 004240ca
004240ca  fld        dword ptr [ecx + 0x5c]
004240cd  fmul       dword ptr [ecx + 0x44]
004240d0  fstp       dword ptr [esp + 0x10]
004240d4  fld        dword ptr [esp + 0x10]
004240d8  fmul       st(2)
004240da  fadd       qword ptr [0x4a3d70]
004240e0  fcom       st(1)
004240e2  fnstsw     ax
004240e4  test       ah, 0x41
004240e7  jne        0x424145

// block 004240e9
004240e9  mov        eax, dword ptr [ecx + 0x400]
004240ef  mov        dword ptr [esp + 8], eax
004240f3  fild       dword ptr [esp + 8]
004240f7  lea        eax, [eax + eax*2]
004240fa  cdq        
004240fb  fxch       st(1)
004240fd  and        edx, 3
00424100  fsubrp     st(2)
00424102  add        eax, edx
00424104  fnstcw     word ptr [esp + 8]
00424108  sar        eax, 2
0042410b  mov        dword ptr [esp + 0x10], eax
0042410f  movzx      eax, word ptr [esp + 8]
00424114  fild       dword ptr [esp + 0x10]
00424118  or         eax, 0xc00
0042411d  mov        dword ptr [esp + 0x10], eax
00424121  fmulp      st(2)
00424123  fxch       st(1)
00424125  fmul       qword ptr [0x4a4188]
0042412b  fsubp      st(1)
0042412d  fldcw      word ptr [esp + 0x10]
00424131  fistp      dword ptr [esp + 0x10]
00424135  mov        al, byte ptr [esp + 0x10]
00424139  mov        byte ptr [ecx + 0x3bf], al
0042413f  fldcw      word ptr [esp + 8]
00424143  jmp        0x424155

// block 00424145
00424145  fstp       st(0)

// block 00424147
00424147  mov        dl, byte ptr [ecx + 0x400]
0042414d  fstp       st(0)

// block 0042414f
0042414f  mov        byte ptr [ecx + 0x3bf], dl

// block 00424155
00424155  add        esi, 4
00424158  add        ebx, 0xc
0042415b  sub        dword ptr [esp + 0xc], 1
00424160  jne        0x423f47

// block 00424166
00424166  mov        eax, 1
0042416b  fstp       st(0)
0042416d  add        dword ptr [edi + 0x10], eax
00424170  add        dword ptr [edi + 0x14], eax
00424173  pop        esi
00424174  pop        ebx
00424175  mov        esp, ebp
00424177  pop        ebp
00424178  ret        
