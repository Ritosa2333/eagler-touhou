
// block 004950be
004950be  movlpd     xmm4, qword ptr [0x4a5d80]
004950c6  movlpd     xmm3, qword ptr [0x4a5d90]
004950ce  xorpd      xmm5, xmm5
004950d2  movlpd     xmm2, qword ptr [0x4a5d88]
004950da  movsd      xmm1, xmm0
004950de  psrlq      xmm0, 0x2c
004950e3  movd       edx, xmm0
004950e7  movsd      xmm7, xmm1
004950eb  mov        ecx, 0x2000
004950f0  pinsrw     xmm5, ecx, 2
004950f5  movsd      xmm0, xmm1
004950f9  mov        eax, 0x7ffff
004950fe  and        eax, edx
00495100  sub        eax, 0x3fb00
00495105  cmp        eax, 0x3bb
0049510a  jae        0x4951e6

// block 00495110
00495110  mulsd      xmm1, xmm1
00495114  and        edx, 0xffff
0049511a  subsd      xmm3, xmm1
0049511e  sqrtsd     xmm3, xmm3
00495122  andpd      xmm2, xmm7
00495126  and        edx, 0xfffffffc
00495129  sub        edx, 0xfb00
0049512f  movlpd     xmm1, qword ptr [edx*2 + 0x4a54c0]
00495138  orpd       xmm2, xmm5
0049513c  movapd     xmm4, xmmword ptr [edx*4 + 0x4a45c0]
00495145  movsd      xmm6, xmm7
00495149  addsd      xmm7, xmm2
0049514d  subsd      xmm0, xmm2
00495151  mulsd      xmm7, xmm0
00495155  mulsd      xmm6, xmm1
00495159  mulsd      xmm3, xmm2
0049515d  movsd      xmm1, xmm6
00495161  addsd      xmm6, xmm3
00495165  divsd      xmm7, xmm6
00495169  movlpd     xmm0, qword ptr [0x4a5d38]
00495171  movlpd     xmm5, qword ptr [0x4a5d28]
00495179  subsd      xmm1, xmm3
0049517d  psrlq      xmm2, 0x3f
00495182  movsd      xmm3, xmm1
00495186  psllq      xmm2, 0x3f
0049518b  mulsd      xmm1, xmm1
0049518f  pshufd     xmm2, xmm2, 0x44
00495194  movlpd     xmm6, qword ptr [0x4a5d30]
0049519c  mulsd      xmm3, xmm1
004951a0  mulsd      xmm0, xmm1
004951a4  sub        esp, 0x10
004951a7  xorpd      xmm4, xmm2
004951ab  mulsd      xmm5, xmm3
004951af  subpd      xmm4, xmmword ptr [0x4a5cc0]
004951b7  mulsd      xmm3, xmm1
004951bb  addsd      xmm0, xmm6
004951bf  mulsd      xmm0, xmm3
004951c3  subsd      xmm5, xmm4
004951c7  pshufd     xmm4, xmm4, 0xee
004951cc  addsd      xmm0, xmm5
004951d0  subsd      xmm0, xmm7
004951d4  subsd      xmm0, xmm4
004951d8  movlpd     qword ptr [esp + 4], xmm0
004951de  fld        qword ptr [esp + 4]
004951e2  add        esp, 0x10
004951e5  ret        

// block 004951e6
004951e6  sub        eax, 0x3bb
004951eb  cmp        eax, 0x41
004951ee  jae        0x495321

// block 004951f4
004951f4  psrlq      xmm7, 0x26
004951f9  psllq      xmm7, 0x26
004951fe  pmovmskb   eax, xmm0
00495202  andnpd     xmm4, xmm0
00495206  subsd      xmm1, xmm7
0049520a  movsd      xmm6, xmm7
0049520e  mulsd      xmm7, xmm7
00495212  addsd      xmm0, xmm6
00495216  orpd       xmm5, xmm4
0049521a  subsd      xmm3, xmm7
0049521e  mulsd      xmm0, xmm1
00495222  movsd      xmm4, xmm3
00495226  subsd      xmm3, xmm0
0049522a  sqrtsd     xmm3, xmm3
0049522e  and        eax, 0x80
00495233  shr        eax, 7
00495236  neg        eax
00495238  movsd      xmm7, xmm3
0049523c  andpd      xmm2, xmm3
00495240  psllq      xmm3, 2
00495245  pextrw     edx, xmm3, 3
0049524a  orpd       xmm2, xmm5
0049524e  movd       xmm3, eax
00495252  pshufd     xmm3, xmm3, 0
00495257  sub        edx, 0xfec0
0049525d  add        edx, edx
0049525f  mulsd      xmm7, qword ptr [edx*4 + 0x4a54c0]
00495268  mulsd      xmm6, xmm2
0049526c  mulsd      xmm1, xmm2
00495270  mulsd      xmm2, xmm2
00495274  subsd      xmm6, xmm7
00495278  andpd      xmm3, xmmword ptr [0x4a5cd0]
00495280  addsd      xmm6, xmm1
00495284  subsd      xmm4, xmm2
00495288  addsd      xmm7, xmm7
0049528c  movlpd     xmm5, qword ptr [0x4a5d28]
00495294  subsd      xmm4, xmm0
00495298  addsd      xmm7, xmm6
0049529c  movlpd     xmm0, qword ptr [0x4a5d38]
004952a4  divsd      xmm4, xmm7
004952a8  movlpd     xmm2, qword ptr [0x4a5d30]
004952b0  addpd      xmm3, xmmword ptr [edx*8 + 0x4a45c0]
004952b9  movsd      xmm1, xmm6
004952bd  mulsd      xmm6, xmm6
004952c1  mulsd      xmm0, xmm6
004952c5  mulsd      xmm1, xmm6
004952c9  sub        esp, 0x10
004952cc  mulsd      xmm5, xmm1
004952d0  mulsd      xmm1, xmm6
004952d4  addsd      xmm0, xmm2
004952d8  pxor       xmm6, xmm6
004952dc  mulsd      xmm0, xmm1
004952e0  addsd      xmm5, xmm3
004952e4  addsd      xmm0, xmm5
004952e8  and        eax, 0x8000
004952ed  pinsrw     xmm6, eax, 3
004952f2  movsd      xmm5, xmm4
004952f6  pshufd     xmm3, xmm3, 0xee
004952fb  addsd      xmm4, xmm3
004952ff  subsd      xmm3, xmm4
00495303  addsd      xmm5, xmm3
00495307  addsd      xmm0, xmm5
0049530b  addsd      xmm0, xmm4
0049530f  xorpd      xmm0, xmm6
00495313  movlpd     qword ptr [esp + 4], xmm0
00495319  fld        qword ptr [esp + 4]
0049531d  add        esp, 0x10
00495320  ret        

// block 00495321
00495321  add        eax, 0x3bbb
00495326  cmp        eax, 0x3800
0049532b  jae        0x4953c1

// block 00495331
00495331  unpcklpd   xmm0, xmm0
00495335  movapd     xmm6, xmmword ptr [0x4a5d40]
0049533d  unpcklpd   xmm1, xmm0
00495341  movapd     xmm2, xmmword ptr [0x4a5d50]
00495349  movapd     xmm4, xmmword ptr [0x4a5d60]
00495351  mulpd      xmm0, xmm0
00495355  movapd     xmm5, xmmword ptr [0x4a5cc0]
0049535d  sub        esp, 0x10
00495360  mulpd      xmm1, xmm0
00495364  mulpd      xmm6, xmm0
00495368  mulpd      xmm0, xmm0
0049536c  movsd      xmm3, xmm1
00495370  mulsd      xmm1, xmm1
00495374  addpd      xmm6, xmm2
00495378  mulpd      xmm4, xmm0
0049537c  mulsd      xmm1, xmm3
00495380  addpd      xmm6, xmm4
00495384  pshufd     xmm0, xmm5, 0xee
00495389  mulpd      xmm1, xmm6
0049538d  pshufd     xmm6, xmm5, 0xee
00495392  subsd      xmm0, xmm7
00495396  pshufd     xmm2, xmm1, 0xee
0049539b  subsd      xmm5, xmm1
0049539f  subsd      xmm6, xmm0
004953a3  subsd      xmm5, xmm2
004953a7  subsd      xmm7, xmm6
004953ab  subsd      xmm5, xmm7
004953af  addsd      xmm0, xmm5
004953b3  movlpd     qword ptr [esp + 4], xmm0
004953b9  fld        qword ptr [esp + 4]
004953bd  add        esp, 0x10
004953c0  ret        

// block 004953c1
004953c1  sub        eax, 0x3bfc
004953c6  cmp        eax, 4
004953c9  jae        0x4954c3

// block 004953cf
004953cf  xorpd      xmm6, xmm6
004953d3  andpd      xmm7, xmmword ptr [0x4a5d80]
004953db  movlpd     xmm4, qword ptr [0x4a5d98]
004953e3  movapd     xmm1, xmmword ptr [0x4a5d40]
004953eb  mulsd      xmm7, xmm4
004953ef  movapd     xmm2, xmmword ptr [0x4a5d50]
004953f7  subsd      xmm4, xmm7
004953fb  movapd     xmm3, xmmword ptr [0x4a5d60]
00495403  pshufd     xmm7, xmm4, 0x44
00495408  sqrtsd     xmm4, xmm4
0049540c  mulpd      xmm1, xmm7
00495410  pshufd     xmm5, xmm7, 0x44
00495415  pextrw     eax, xmm0, 3
0049541a  mulpd      xmm7, xmm7
0049541e  addpd      xmm2, xmm1
00495422  movlpd     xmm1, qword ptr [0x4a5d00]
0049542a  mulpd      xmm3, xmm7
0049542e  cmpltsd    xmm0, xmm6
00495433  mulsd      xmm7, xmm5
00495437  addpd      xmm2, xmm3
0049543b  pshufd     xmm0, xmm0, 0x44
00495440  mulsd      xmm2, xmm7
00495444  andpd      xmm0, xmmword ptr [0x4a5cd0]
0049544c  mulpd      xmm2, xmm5
00495450  andpd      xmm1, xmm4
00495454  pshufd     xmm3, xmm4, 0x44
00495459  subsd      xmm4, xmm1
0049545d  addsd      xmm3, xmm3
00495461  mulsd      xmm1, xmm1
00495465  subsd      xmm3, xmm4
00495469  subsd      xmm5, xmm1
0049546d  mulsd      xmm4, xmm3
00495471  pshufd     xmm3, xmm3, 0xee
00495476  subsd      xmm5, xmm4
0049547a  divsd      xmm5, xmm3
0049547e  sub        esp, 0x10
00495481  addpd      xmm3, xmm3
00495485  mulpd      xmm2, xmm3
00495489  pshufd     xmm4, xmm2, 0xee
0049548e  addsd      xmm2, xmm0
00495492  and        eax, 0x8000
00495497  pinsrw     xmm6, eax, 3
0049549c  pshufd     xmm0, xmm0, 0xee
004954a1  addsd      xmm2, xmm4
004954a5  addsd      xmm2, xmm5
004954a9  addsd      xmm2, xmm3
004954ad  addsd      xmm0, xmm2
004954b1  xorpd      xmm0, xmm6
004954b5  movlpd     qword ptr [esp + 4], xmm0
004954bb  fld        qword ptr [esp + 4]
004954bf  add        esp, 0x10
004954c2  ret        

// block 004954c3
004954c3  add        eax, 0x3fefc
004954c8  cmp        eax, 0x3ff00
004954cd  jb         0x4955bd

// block 004954d3
004954d3  movd       ecx, xmm7
004954d7  psrlq      xmm7, 0x20
004954dc  movd       edx, xmm7
004954e0  and        edx, 0x7fffffff
004954e6  mov        eax, 0x3ff00000
004954eb  sub        eax, edx
004954ed  or         eax, ecx
004954ef  cmp        eax, 0
004954f2  je         0x49557d

// block 004954f8
004954f8  movlpd     xmm2, qword ptr [esp + 4]
004954fe  movd       edx, xmm2
00495502  psrlq      xmm2, 0x20
00495507  movd       ecx, xmm2
0049550b  and        ecx, 0x7fffffff
00495511  sub        edx, 1
00495514  sbb        ecx, 0x7ff00000
0049551a  cmp        ecx, 0
0049551d  jge        0x4955e2

// block 00495523
00495523  xorpd      xmm1, xmm1
00495527  xorpd      xmm0, xmm0
0049552b  mov        edx, 0x7ff0
00495530  pinsrw     xmm1, edx, 3
00495535  mulsd      xmm0, xmm1
00495539  mov        edx, 0x3a

// block 0049553e
0049553e  sub        esp, 0x1c
00495541  movlpd     qword ptr [esp + 0x10], xmm0
00495547  mov        dword ptr [esp + 0xc], edx
0049554b  mov        edx, esp
0049554d  add        edx, 0x10
00495550  mov        dword ptr [esp + 8], edx
00495554  add        edx, 0x10
00495557  mov        dword ptr [esp + 4], edx
0049555b  mov        dword ptr [esp], edx
0049555e  call       0x493b07

// block 00495563
00495563  movlpd     xmm0, qword ptr [esp + 0x10]
00495569  add        esp, 0x1c
0049556c  sub        esp, 0x10
0049556f  movlpd     qword ptr [esp + 4], xmm0
00495575  fld        qword ptr [esp + 4]
00495579  add        esp, 0x10
0049557c  ret        

// block 0049557d
0049557d  pextrw     edx, xmm7, 1
00495582  shr        edx, 0xf
00495585  neg        edx
00495587  movd       xmm7, edx
0049558b  pshufd     xmm7, xmm7, 0
00495590  movlpd     xmm2, qword ptr [0x4a5ce0]
00495598  movlpd     xmm0, qword ptr [0x4a5ce8]
004955a0  andpd      xmm2, xmm7
004955a4  andpd      xmm0, xmm7
004955a8  addsd      xmm0, xmm2
004955ac  sub        esp, 0x10
004955af  movlpd     qword ptr [esp + 4], xmm0
004955b5  fld        qword ptr [esp + 4]
004955b9  add        esp, 0x10
004955bc  ret        

// block 004955bd
004955bd  movlpd     xmm2, qword ptr [0x4a5cc0]
004955c5  movlpd     xmm0, qword ptr [0x4a5cc8]
004955cd  addsd      xmm0, xmm2
004955d1  sub        esp, 0x10
004955d4  movlpd     qword ptr [esp + 4], xmm0
004955da  fld        qword ptr [esp + 4]
004955de  add        esp, 0x10
004955e1  ret        

// block 004955e2
004955e2  xorpd      xmm6, xmm6
004955e6  addsd      xmm0, xmm6
004955ea  mov        edx, 0x3f0
004955ef  jmp        0x49553e
