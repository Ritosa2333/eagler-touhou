
// block 00469d40
00469d40  push       ebp
00469d41  mov        ebp, esp
00469d43  and        esp, 0xfffffff8
00469d46  sub        esp, 0x18
00469d49  push       ebx
00469d4a  push       ebp
00469d4b  push       esi
00469d4c  push       edi
00469d4d  mov        edi, dword ptr [ecx + 0x478]
00469d53  cmp        dword ptr [edi + 0x3f8], 0x7530
00469d5d  jge        0x46a1ef

// block 00469d63
00469d63  mov        esi, edi
00469d65  mov        ecx, 0xb
00469d6a  lea        eax, [edi + 0x324]

// block 00469d70
00469d70  mov        edx, dword ptr [eax - 0xc]
00469d73  mov        dword ptr [eax], edx
00469d75  mov        edx, dword ptr [eax - 8]
00469d78  mov        dword ptr [eax + 4], edx
00469d7b  mov        edx, dword ptr [eax - 4]
00469d7e  mov        dword ptr [eax + 8], edx
00469d81  dec        ecx
00469d82  sub        eax, 0xc
00469d85  test       ecx, ecx
00469d87  jg         0x469d70

// block 00469d89
00469d89  test       byte ptr [edi + 0x360], 1
00469d90  lea        ebx, [edi + 0x330]
00469d96  jne        0x469db7

// block 00469d98
00469d98  fld        dword ptr [ebx + 0x18]
00469d9b  sub        esp, 8
00469d9e  fstp       dword ptr [esp + 4]
00469da2  lea        ecx, [ebx + 0xc]
00469da5  fld        dword ptr [ebx + 0x1c]
00469da8  fstp       dword ptr [esp]
00469dab  call       0x465390

// block 00469db0
00469db0  fldz       
00469db2  fstp       dword ptr [ebx + 0x14]
00469db5  jmp        0x469de3

// block 00469db7
00469db7  fld        dword ptr [ebx + 0x24]
00469dba  push       ecx
00469dbb  fadd       dword ptr [ebx + 0x20]
00469dbe  fstp       dword ptr [ebx + 0x20]
00469dc1  fld        dword ptr [ebx + 0x18]
00469dc4  fadd       dword ptr [ebx + 0x1c]
00469dc7  fstp       dword ptr [esp + 0x1c]
00469dcb  fld        dword ptr [esp + 0x1c]
00469dcf  fstp       dword ptr [esp]
00469dd2  call       0x4646e0

// block 00469dd7
00469dd7  push       ecx
00469dd8  fstp       dword ptr [esp]
00469ddb  call       0x4646e0

// block 00469de0
00469de0  fstp       dword ptr [ebx + 0x1c]

// block 00469de3
00469de3  call       0x464db0

// block 00469de8
00469de8  fld        dword ptr [ebx]
00469dea  fsub       dword ptr [edi + 0x408]
00469df0  mov        eax, dword ptr [ebx]
00469df2  mov        ecx, dword ptr [ebx + 4]
00469df5  mov        edx, dword ptr [ebx + 8]
00469df8  fstp       dword ptr [esp + 0x18]
00469dfc  mov        dword ptr [edi + 0x2a0], eax
00469e02  fld        dword ptr [esp + 0x18]
00469e06  mov        dword ptr [edi + 0x2a4], ecx
00469e0c  fabs       
00469e0e  mov        dword ptr [edi + 0x2a8], edx
00469e14  fstp       dword ptr [esp + 0x18]
00469e18  fld        dword ptr [esp + 0x18]
00469e1c  fcomp      dword ptr [0x4a4010]
00469e22  fnstsw     ax
00469e24  test       ah, 1
00469e27  jne        0x469e61

// block 00469e29
00469e29  fld        dword ptr [edi + 0x34c]
00469e2f  sub        esp, 8
00469e32  fstp       dword ptr [esp + 4]
00469e36  fld        dword ptr [0x4a4060]
00469e3c  fstp       dword ptr [esp]
00469e3f  call       0x46a520

// block 00469e44
00469e44  fsub       qword ptr [0x4a4058]
00469e4a  push       ecx
00469e4b  fstp       dword ptr [esp + 0x1c]
00469e4f  fld        dword ptr [esp + 0x1c]
00469e53  fstp       dword ptr [esp]
00469e56  call       0x4646e0

// block 00469e5b
00469e5b  fstp       dword ptr [edi + 0x34c]

// block 00469e61
00469e61  fldz       
00469e63  fcom       dword ptr [edi + 0x328]
00469e69  fnstsw     ax
00469e6b  test       ah, 1
00469e6e  jne        0x469e7f

// block 00469e70
00469e70  fstp       st(0)
00469e72  mov        eax, 1
00469e77  pop        edi
00469e78  pop        esi
00469e79  pop        ebp
00469e7a  pop        ebx
00469e7b  mov        esp, ebp
00469e7d  pop        ebp
00469e7e  ret        

// block 00469e7f
00469e7f  mov        eax, dword ptr [edi + 0x3f8]
00469e85  neg        eax
00469e87  mov        dword ptr [esp + 0x18], eax
00469e8b  fild       dword ptr [esp + 0x18]
00469e8f  mov        eax, dword ptr [edi + 0x414]
00469e95  xor        ebx, ebx
00469e97  fmul       dword ptr [edi + 0x348]
00469e9d  fmul       qword ptr [0x4a3d40]
00469ea3  fstp       dword ptr [esp + 0x14]
00469ea7  cmp        eax, ebx
00469ea9  je         0x469f2f

// block 00469eaf
00469eaf  mov        ecx, dword ptr [edi + 0x2b8]
00469eb5  fstp       st(0)
00469eb7  mov        dword ptr [eax + 0x18], ecx
00469eba  mov        edx, dword ptr [edi + 0x2bc]
00469ec0  add        eax, 0x18
00469ec3  mov        dword ptr [eax + 4], edx
00469ec6  mov        ecx, dword ptr [edi + 0x2c0]
00469ecc  mov        dword ptr [eax + 8], ecx
00469ecf  fld        dword ptr [edi + 0x2b0]
00469ed5  fsub       dword ptr [edi + 0x2a4]
00469edb  fstp       dword ptr [esp + 0x18]
00469edf  fld        dword ptr [esp + 0x18]
00469ee3  fld        dword ptr [edi + 0x2ac]
00469ee9  fsub       dword ptr [edi + 0x2a0]
00469eef  fstp       dword ptr [esp + 0x18]
00469ef3  fld        dword ptr [esp + 0x18]
00469ef7  call       0x4937aa

// block 00469efc
00469efc  fstp       dword ptr [esp + 0x18]
00469f00  fld        dword ptr [esp + 0x18]
00469f04  mov        edx, dword ptr [edi + 0x414]
00469f0a  fstp       dword ptr [edx + 8]
00469f0d  mov        ecx, dword ptr [edi + 0x414]
00469f13  fld        dword ptr [0x4a4050]
00469f19  fcomp      dword ptr [ecx + 0x1c]
00469f1c  fnstsw     ax
00469f1e  fldz       
00469f20  test       ah, 0x41
00469f23  jne        0x469f2f

// block 00469f25
00469f25  and        dword ptr [ecx + 0x70], 0xfffffffe
00469f29  mov        dword ptr [edi + 0x414], ebx

// block 00469f2f
00469f2f  fld        dword ptr [esp + 0x14]
00469f33  fcom       st(1)
00469f35  fnstsw     ax
00469f37  fld1       
00469f39  test       ah, 5
00469f3c  jnp        0x469f44

// block 00469f3e
00469f3e  fstp       st(1)
00469f40  jmp        0x469f5b

// block 00469f42
00469f42  fxch       st(1)

// block 00469f44
00469f44  fadd       st(1), st(0)
00469f46  fxch       st(1)
00469f48  fstp       dword ptr [esp + 0x14]
00469f4c  fld        dword ptr [esp + 0x14]
00469f50  fcom       st(2)
00469f52  fnstsw     ax
00469f54  test       ah, 5
00469f57  jnp        0x469f42

// block 00469f59
00469f59  fstp       st(0)

// block 00469f5b
00469f5b  fstp       st(1)
00469f5d  lea        ebp, [edi + 0x1c]
00469f60  fstp       st(0)

// block 00469f62
00469f62  test       ebx, ebx
00469f64  jle        0x469fb0

// block 00469f66
00469f66  lea        eax, [ebx + ebx*2 + 0xa8]
00469f6d  fld        dword ptr [edi + eax*4 + 4]
00469f71  lea        eax, [edi + eax*4]
00469f74  lea        ecx, [ebx + ebx*2 + 0xa5]
00469f7b  fsub       dword ptr [edi + ecx*4 + 4]
00469f7f  lea        ecx, [edi + ecx*4]
00469f82  fstp       dword ptr [esp + 0x1c]
00469f86  fld        dword ptr [esp + 0x1c]
00469f8a  fld        dword ptr [eax]
00469f8c  fsub       dword ptr [ecx]
00469f8e  fstp       dword ptr [esp + 0x1c]
00469f92  fld        dword ptr [esp + 0x1c]
00469f96  call       0x4937aa

// block 00469f9b
00469f9b  fstp       dword ptr [esp + 0x1c]
00469f9f  fld        dword ptr [esp + 0x1c]
00469fa3  push       ecx
00469fa4  fstp       dword ptr [esp]
00469fa7  call       0x4646e0

// block 00469fac
00469fac  fstp       dword ptr [esp + 0x10]

// block 00469fb0
00469fb0  cmp        ebx, 0xb
00469fb3  jge        0x46a09a

// block 00469fb9
00469fb9  lea        edx, [ebx + ebx*2 + 0xa8]
00469fc0  fld        dword ptr [edi + edx*4 + 4]
00469fc4  lea        eax, [edi + edx*4]
00469fc7  lea        ecx, [ebx + ebx*2 + 0xab]
00469fce  fsub       dword ptr [edi + ecx*4 + 4]
00469fd2  lea        ecx, [edi + ecx*4]
00469fd5  fstp       dword ptr [esp + 0x1c]
00469fd9  fld        dword ptr [esp + 0x1c]
00469fdd  fld        dword ptr [eax]
00469fdf  fsub       dword ptr [ecx]
00469fe1  fstp       dword ptr [esp + 0x1c]
00469fe5  fld        dword ptr [esp + 0x1c]
00469fe9  call       0x4937aa

// block 00469fee
00469fee  fstp       dword ptr [esp + 0x1c]
00469ff2  fld        dword ptr [esp + 0x1c]
00469ff6  push       ecx
00469ff7  fstp       dword ptr [esp]
00469ffa  call       0x4646e0

// block 00469fff
00469fff  fstp       dword ptr [esp + 0x18]
0046a003  test       ebx, ebx
0046a005  jne        0x46a025

// block 0046a007
0046a007  fld        dword ptr [0x4a3e08]
0046a00d  sub        esp, 8
0046a010  fstp       dword ptr [esp + 4]
0046a014  fld        dword ptr [esp + 0x20]
0046a018  fstp       dword ptr [esp]
0046a01b  call       0x465280

// block 0046a020
0046a020  jmp        0x46a0ac

// block 0046a025
0046a025  fld        dword ptr [esp + 0x18]
0046a029  push       ecx
0046a02a  fadd       dword ptr [esp + 0x14]
0046a02e  fstp       dword ptr [esp + 0x20]
0046a032  fld        dword ptr [esp + 0x20]
0046a036  fstp       dword ptr [esp]
0046a039  call       0x4646e0

// block 0046a03e
0046a03e  fmul       qword ptr [0x4a3cf8]
0046a044  push       ecx
0046a045  fstp       dword ptr [esp + 0x20]
0046a049  fld        dword ptr [esp + 0x20]
0046a04d  fstp       dword ptr [esp]
0046a050  call       0x4646e0

// block 0046a055
0046a055  fstp       dword ptr [esp + 0x10]
0046a059  sub        esp, 8
0046a05c  fld        dword ptr [esp + 0x18]
0046a060  fstp       dword ptr [esp + 4]
0046a064  fld        dword ptr [esp + 0x20]
0046a068  fstp       dword ptr [esp]
0046a06b  call       0x42e770

// block 0046a070
0046a070  push       ecx
0046a071  fstp       dword ptr [esp]
0046a074  call       0x4646e0

// block 0046a079
0046a079  fcomp      dword ptr [0x4a3cf4]
0046a07f  fnstsw     ax
0046a081  test       ah, 0x41
0046a084  jp         0x46a0b9

// block 0046a086
0046a086  fld        dword ptr [esp + 0x10]
0046a08a  fadd       qword ptr [0x4a3cd8]
0046a090  fstp       dword ptr [esp + 0x1c]
0046a094  fld        dword ptr [esp + 0x1c]
0046a098  jmp        0x46a0ac

// block 0046a09a
0046a09a  fld        dword ptr [esp + 0x10]
0046a09e  fadd       qword ptr [0x4a4058]
0046a0a4  fstp       dword ptr [esp + 0x1c]
0046a0a8  fld        dword ptr [esp + 0x1c]

// block 0046a0ac
0046a0ac  push       ecx
0046a0ad  fstp       dword ptr [esp]
0046a0b0  call       0x4646e0

// block 0046a0b5
0046a0b5  fstp       dword ptr [esp + 0x10]

// block 0046a0b9
0046a0b9  lea        edx, [ebx + ebx*2 + 0xa8]
0046a0c0  fld        dword ptr [edi + edx*4]
0046a0c3  lea        eax, [edi + edx*4]
0046a0c6  fadd       qword ptr [0x4a3d70]
0046a0cc  sub        esp, 8
0046a0cf  fadd       qword ptr [0x4a3d28]
0046a0d5  fstp       dword ptr [esi]
0046a0d7  fld        dword ptr [eax + 4]
0046a0da  fadd       qword ptr [0x4a3d68]
0046a0e0  fstp       dword ptr [esi + 4]
0046a0e3  fld        dword ptr [eax + 8]
0046a0e6  fstp       dword ptr [esi + 8]
0046a0e9  mov        eax, dword ptr [esi]
0046a0eb  mov        ecx, dword ptr [esi + 4]
0046a0ee  fld        dword ptr [0x4a404c]
0046a0f4  mov        edx, dword ptr [esi + 8]
0046a0f7  fstp       dword ptr [esp + 4]
0046a0fb  fld        dword ptr [esp + 0x18]
0046a0ff  mov        dword ptr [ebp], eax
0046a102  mov        dword ptr [ebp + 4], ecx
0046a105  fstp       dword ptr [esp]
0046a108  lea        ecx, [esp + 0x28]
0046a10c  mov        dword ptr [ebp + 8], edx
0046a10f  call       0x46a4a0

// block 0046a114
0046a114  fld        dword ptr [esi]
0046a116  fld        dword ptr [esp + 0x20]
0046a11a  add        esi, 0x1c
0046a11d  fld        st(0)
0046a11f  add        ebp, 0x1c
0046a122  faddp      st(2)
0046a124  inc        ebx
0046a125  fxch       st(1)
0046a127  add        esi, 0x1c
0046a12a  add        ebp, 0x1c
0046a12d  cmp        ebx, 0xc
0046a130  fstp       dword ptr [esi - 0x38]
0046a133  fld        dword ptr [esp + 0x24]
0046a137  fld        st(0)
0046a139  fadd       dword ptr [esi - 0x34]
0046a13c  fstp       dword ptr [esi - 0x34]
0046a13f  fld        dword ptr [esp + 0x14]
0046a143  fst        dword ptr [esi - 0x20]
0046a146  fld        dword ptr [esi - 0x1c]
0046a149  fsubrp     st(3)
0046a14b  fxch       st(2)
0046a14d  fstp       dword ptr [esi - 0x1c]
0046a150  fsubr      dword ptr [esi - 0x18]
0046a153  fstp       dword ptr [esi - 0x18]
0046a156  fst        dword ptr [esi - 4]
0046a159  fld        dword ptr [edi + 0x348]
0046a15f  fmul       qword ptr [0x4a3d40]
0046a165  faddp      st(1)
0046a167  fstp       dword ptr [esp + 0x14]
0046a16b  jl         0x469f62

// block 0046a171
0046a171  mov        edx, dword ptr [edi + 0x3f8]
0046a177  mov        ecx, dword ptr [edi + 0x400]
0046a17d  mov        dword ptr [edi + 0x3f4], edx
0046a183  fld        dword ptr [ecx]
0046a185  fcomp      qword ptr [0x4a3db0]
0046a18b  fnstsw     ax
0046a18d  test       ah, 0x41
0046a190  jne        0x46a1c4

// block 0046a192
0046a192  fld        dword ptr [0x4a3dac]
0046a198  fcomp      dword ptr [ecx]
0046a19a  fnstsw     ax
0046a19c  test       ah, 0x41
0046a19f  jne        0x46a1c4

// block 0046a1a1
0046a1a1  fld        dword ptr [edi + 0x3fc]
0046a1a7  inc        edx
0046a1a8  fadd       qword ptr [0x4a3ce0]
0046a1ae  mov        dword ptr [edi + 0x3f8], edx
0046a1b4  xor        eax, eax
0046a1b6  fstp       dword ptr [edi + 0x3fc]
0046a1bc  pop        edi
0046a1bd  pop        esi
0046a1be  pop        ebp
0046a1bf  pop        ebx
0046a1c0  mov        esp, ebp
0046a1c2  pop        ebp
0046a1c3  ret        

// block 0046a1c4
0046a1c4  fld        dword ptr [ecx]
0046a1c6  fadd       dword ptr [edi + 0x3fc]
0046a1cc  fstp       dword ptr [esp + 0x1c]
0046a1d0  fld        dword ptr [esp + 0x1c]
0046a1d4  fst        dword ptr [edi + 0x3fc]
0046a1da  call       0x4931e0

// block 0046a1df
0046a1df  mov        dword ptr [edi + 0x3f8], eax
0046a1e5  xor        eax, eax
0046a1e7  pop        edi
0046a1e8  pop        esi
0046a1e9  pop        ebp
0046a1ea  pop        ebx
0046a1eb  mov        esp, ebp
0046a1ed  pop        ebp
0046a1ee  ret        

// block 0046a1ef
0046a1ef  mov        eax, dword ptr [edi + 0x414]
0046a1f5  test       eax, eax
0046a1f7  je         0x46a207

// block 0046a1f9
0046a1f9  and        dword ptr [eax + 0x70], 0xfffffffe
0046a1fd  mov        dword ptr [edi + 0x414], 0

// block 0046a207
0046a207  pop        edi
0046a208  mov        eax, 1
0046a20d  pop        esi
0046a20e  pop        ebp
0046a20f  pop        ebx
0046a210  mov        esp, ebp
0046a212  pop        ebp
0046a213  ret        
