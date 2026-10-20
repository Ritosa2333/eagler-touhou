
// block 00403020
00403020  push       ebp
00403021  mov        ebp, esp
00403023  and        esp, 0xfffffff8
00403026  mov        eax, dword ptr [ebx + 0x35bc]
0040302c  sub        esp, 0x40
0040302f  push       esi
00403030  push       edi
00403031  test       al, 8
00403033  je         0x403040

// block 00403035
00403035  mov        eax, 1
0040303a  pop        edi
0040303b  pop        esi
0040303c  mov        esp, ebp
0040303e  pop        ebp
0040303f  ret        

// block 00403040
00403040  test       al, 4
00403042  je         0x40304d

// block 00403044
00403044  cmp        dword ptr [ebx + 0x35c4], 0x3c
0040304b  jge        0x403035

// block 0040304d
0040304d  fldz       
0040304f  fst        dword ptr [ebx + 0x36f4]
00403055  fst        dword ptr [ebx + 0x36f8]
0040305b  fst        dword ptr [esp + 0x3c]
0040305f  mov        eax, dword ptr [esp + 0x3c]
00403063  fst        dword ptr [esp + 0x40]
00403067  mov        ecx, dword ptr [esp + 0x40]
0040306b  fstp       dword ptr [esp + 0x44]
0040306f  mov        edx, dword ptr [esp + 0x44]
00403073  mov        dword ptr [ebx + 0x36fc], eax
00403079  mov        dword ptr [ebx + 0x3700], ecx
0040307f  lea        eax, [ebx + 0x3618]
00403085  push       eax
00403086  lea        ecx, [ebx + 0x3630]
0040308c  push       ecx
0040308d  mov        dword ptr [ebx + 0x3704], edx
00403093  call       0x46c6bc

// block 00403098
00403098  test       byte ptr [ebx + 0x35bc], 4
0040309f  mov        dword ptr [ebx + 0x2774], 0x808080
004030a9  je         0x4030b4

// block 004030ab
004030ab  cmp        dword ptr [ebx + 0x35c4], 0x1e
004030b2  jge        0x403129

// block 004030b4
004030b4  push       ebx
004030b5  call       0x404620

// block 004030ba
004030ba  push       ebx
004030bb  call       0x4049a0

// block 004030c0
004030c0  lea        esi, [ebx + 0x1cc]
004030c6  mov        edi, 8
004030cb  jmp        0x4030d0

// block 004030d0
004030d0  push       esi
004030d1  call       0x455630

// block 004030d6
004030d6  add        esi, 0x4b4
004030dc  sub        edi, 1
004030df  jne        0x4030d0

// block 004030e1
004030e1  cmp        dword ptr [ebx + 0x2778], edi
004030e7  je         0x403129

// block 004030e9
004030e9  fld        dword ptr [0x4b2ed0]
004030ef  lea        edx, [ebx + 0x2794]
004030f5  fstp       dword ptr [esp + 0x20]
004030f9  push       edx
004030fa  fld1       
004030fc  fstp       dword ptr [0x4b2ed0]
00403102  call       0x455630

// block 00403107
00403107  lea        eax, [ebx + 0x2c48]
0040310d  push       eax
0040310e  call       0x455630

// block 00403113
00403113  lea        ecx, [ebx + 0x30fc]
00403119  push       ecx
0040311a  call       0x455630

// block 0040311f
0040311f  fld        dword ptr [esp + 0x20]
00403123  fstp       dword ptr [0x4b2ed0]

// block 00403129
00403129  lea        esi, [ebx + 0x360c]
0040312f  mov        ecx, 0x46
00403134  mov        edi, 0x4ced1c
00403139  mov        dword ptr [ebx + 0x2778], 0

// block 00403143
00403143  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00403145
00403145  mov        ecx, dword ptr [ebx + 0x35dc]
0040314b  xor        edi, edi
0040314d  cmp        ecx, edi
0040314f  je         0x4036dd

// block 00403155
00403155  mov        eax, dword ptr [ebx + 0x35ec]
0040315b  mov        edx, dword ptr [ecx + 0x14]
0040315e  mov        dword ptr [esp + 0x10], eax
00403162  mov        eax, dword ptr [ebx + 0x35f4]
00403168  mov        dword ptr [esp + 0xc], edx
0040316c  mov        edx, dword ptr [ebx + 0x35f0]
00403172  mov        dword ptr [esp + 0x14], edx
00403176  cmp        eax, 1
00403179  jne        0x403341

// block 0040317f
0040317f  mov        eax, dword ptr [0x4b43cc]
00403184  cmp        eax, edi
00403186  je         0x403192

// block 00403188
00403188  test       byte ptr [eax + 0x7c], 1
0040318c  jne        0x4036dd

// block 00403192
00403192  fld        dword ptr [0x4a3db8]
00403198  sub        esp, 0x10
0040319b  fstp       dword ptr [esp + 0xc]
0040319f  mov        eax, ecx
004031a1  fld        dword ptr [0x4a40f0]
004031a7  fstp       dword ptr [esp + 8]
004031ab  fldz       
004031ad  fstp       dword ptr [esp + 4]
004031b1  fld        dword ptr [0x4a3d98]
004031b7  fstp       dword ptr [esp]
004031ba  call       0x410020

// block 004031bf
004031bf  mov        eax, dword ptr [ebx + 0x35dc]
004031c5  cmp        dword ptr [eax], edi
004031c7  mov        esi, dword ptr [eax + 0x10]
004031ca  mov        dword ptr [esp + 8], edi
004031ce  jle        0x403305

// block 004031d4
004031d4  jmp        0x4031e2

// block 004031e0
004031e0  xor        edi, edi

// block 004031e2
004031e2  mov        eax, dword ptr [ebx + 0x35dc]
004031e8  cmp        dword ptr [eax + 4], edi
004031eb  mov        dword ptr [esp + 0x18], edi
004031ef  jle        0x4032cf

// block 004031f5
004031f5  fld        dword ptr [esp + 0x14]
004031f9  call       0x4938c0

// block 004031fe
004031fe  fstp       dword ptr [esp + 0x24]

// block 00403202
00403202  fild       dword ptr [esp + 0x18]
00403206  mov        byte ptr [esi + 0x13], 0xc0
0040320a  mov        edi, dword ptr [ebx + 0x35dc]
00403210  fld        qword ptr [0x4a4258]
00403216  mov        ecx, dword ptr [edi + 4]
00403219  fmul       st(1), st(0)
0040321b  dec        ecx
0040321c  mov        dword ptr [esp + 0x20], ecx
00403220  fild       dword ptr [esp + 0x20]
00403224  fdivp      st(2)
00403226  fsubrp     st(1)
00403228  fstp       dword ptr [esp + 0x1c]
0040322c  fld        dword ptr [esp + 0x10]
00403230  call       0x4938c0

// block 00403235
00403235  fstp       dword ptr [esp + 0x20]
00403239  fld        dword ptr [esp + 0x20]
0040323d  mov        eax, dword ptr [esp + 8]
00403241  fld        dword ptr [esp + 0x1c]
00403245  fld        st(0)
00403247  fmulp      st(2)
00403249  fxch       st(1)
0040324b  fstp       dword ptr [esp + 0x30]
0040324f  fmul       dword ptr [esp + 0x24]
00403253  fstp       dword ptr [esp + 0x34]
00403257  test       eax, eax
00403259  je         0x403290

// block 0040325b
0040325b  mov        ecx, dword ptr [esp + 0x18]
0040325f  test       ecx, ecx
00403261  je         0x403290

// block 00403263
00403263  mov        edx, dword ptr [edi]
00403265  dec        edx
00403266  cmp        eax, edx
00403268  je         0x403290

// block 0040326a
0040326a  mov        eax, dword ptr [edi + 4]
0040326d  dec        eax
0040326e  cmp        ecx, eax
00403270  je         0x403290

// block 00403272
00403272  fld        dword ptr [esi]
00403274  mov        ecx, dword ptr [esp + 0xc]
00403278  fadd       dword ptr [esp + 0x30]
0040327c  fstp       dword ptr [esi]
0040327e  fld        dword ptr [esi + 4]
00403281  fadd       dword ptr [esp + 0x34]
00403285  fstp       dword ptr [esi + 4]
00403288  fldz       
0040328a  fst        dword ptr [esi + 8]
0040328d  fstp       dword ptr [ecx + 8]

// block 00403290
00403290  fld        dword ptr [esp + 0x10]
00403294  push       ecx
00403295  fadd       qword ptr [0x4a4338]
0040329b  fstp       dword ptr [esp + 0x24]
0040329f  fld        dword ptr [esp + 0x24]
004032a3  fstp       dword ptr [esp]
004032a6  call       0x4646e0

// block 004032ab
004032ab  mov        eax, dword ptr [esp + 0x18]
004032af  fstp       dword ptr [esp + 0x10]
004032b3  mov        edx, dword ptr [ebx + 0x35dc]
004032b9  add        dword ptr [esp + 0xc], 0xc
004032be  inc        eax
004032bf  add        esi, 0x1c
004032c2  cmp        eax, dword ptr [edx + 4]
004032c5  mov        dword ptr [esp + 0x18], eax
004032c9  jl         0x403202

// block 004032cf
004032cf  fld        dword ptr [esp + 0x14]
004032d3  push       ecx
004032d4  fsub       qword ptr [0x4a4330]
004032da  fstp       dword ptr [esp + 0x28]
004032de  fld        dword ptr [esp + 0x28]
004032e2  fstp       dword ptr [esp]
004032e5  call       0x4646e0

// block 004032ea
004032ea  mov        eax, dword ptr [esp + 8]
004032ee  fstp       dword ptr [esp + 0x14]
004032f2  mov        ecx, dword ptr [ebx + 0x35dc]
004032f8  inc        eax
004032f9  cmp        eax, dword ptr [ecx]
004032fb  mov        dword ptr [esp + 8], eax
004032ff  jl         0x4031e0

// block 00403305
00403305  fld        dword ptr [ebx + 0x35ec]
0040330b  push       ecx
0040330c  fadd       qword ptr [0x4a3e30]
00403312  fstp       dword ptr [esp + 0x28]
00403316  fld        dword ptr [esp + 0x28]
0040331a  fstp       dword ptr [esp]
0040331d  call       0x4646e0

// block 00403322
00403322  fstp       dword ptr [ebx + 0x35ec]
00403328  fld        dword ptr [ebx + 0x35f0]
0040332e  fadd       qword ptr [0x4a4328]
00403334  fstp       dword ptr [esp + 0x24]
00403338  fld        dword ptr [esp + 0x24]
0040333c  jmp        0x4036ce

// block 00403341
00403341  cmp        eax, 2
00403344  jne        0x4036dd

// block 0040334a
0040334a  fld        dword ptr [ebx + 0x35e4]
00403350  fstp       dword ptr [esp + 0x1c]
00403354  fld        dword ptr [ebx + 0x35e4]
0040335a  fld        dword ptr [ebx + 0x35e0]
00403360  fcompp     
00403362  fnstsw     ax
00403364  fld        qword ptr [0x4a3d00]
0040336a  test       ah, 5
0040336d  jp         0x40337d

// block 0040336f
0040336f  fld        dword ptr [ebx + 0x35e4]
00403375  fsub       st(1)
00403377  fstp       dword ptr [ebx + 0x35e4]

// block 0040337d
0040337d  fld        dword ptr [esp + 0x1c]
00403381  sub        esp, 0x10
00403384  fld        st(0)
00403386  mov        eax, ecx
00403388  fmulp      st(2)
0040338a  fxch       st(1)
0040338c  fstp       dword ptr [esp + 0x34]
00403390  fld        dword ptr [esp + 0x34]
00403394  fst        dword ptr [esp + 0xc]
00403398  fstp       dword ptr [esp + 8]
0040339c  fld        st(0)
0040339e  fsubr      qword ptr [0x4a40b0]
004033a4  fstp       dword ptr [esp + 0x34]
004033a8  fld        dword ptr [esp + 0x34]
004033ac  fstp       dword ptr [esp + 4]
004033b0  fchs       
004033b2  fstp       dword ptr [esp]
004033b5  call       0x410020

// block 004033ba
004033ba  mov        eax, dword ptr [ebx + 0x35dc]
004033c0  cmp        dword ptr [eax], edi
004033c2  mov        esi, dword ptr [eax + 0x10]
004033c5  mov        dword ptr [esp + 0x20], edi
004033c9  jle        0x403659

// block 004033cf
004033cf  fld        qword ptr [0x4a3ed0]

// block 004033d5
004033d5  mov        edx, dword ptr [ebx + 0x35dc]
004033db  xor        edi, edi
004033dd  cmp        dword ptr [edx + 4], edi
004033e0  jle        0x403640

// block 004033e6
004033e6  fld        dword ptr [esp + 0x1c]
004033ea  fmul       st(0), st(0)
004033ec  fst        qword ptr [esp + 0x28]
004033f0  jmp        0x4033f6

// block 004033f2
004033f2  fld        qword ptr [esp + 0x28]

// block 004033f6
004033f6  mov        eax, dword ptr [esp + 0xc]
004033fa  fld        dword ptr [eax]
004033fc  fsub       qword ptr [0x4a40b0]
00403402  fstp       dword ptr [esp + 0x3c]
00403406  fld        dword ptr [eax + 4]
00403409  fsub       qword ptr [0x4a3ee0]
0040340f  fstp       dword ptr [esp + 0x40]
00403413  mov        ecx, dword ptr [esp + 0x40]
00403417  fld        dword ptr [eax + 8]
0040341a  mov        eax, dword ptr [esp + 0x3c]
0040341e  fsub       dword ptr [esp + 0x44]
00403422  mov        dword ptr [esp + 0x30], eax
00403426  mov        dword ptr [esp + 0x34], ecx
0040342a  fstp       dword ptr [esp + 0x44]
0040342e  mov        edx, dword ptr [esp + 0x44]
00403432  fld        dword ptr [esp + 0x40]
00403436  mov        dword ptr [esp + 0x38], edx
0040343a  fld        dword ptr [esp + 0x3c]
0040343e  fmul       st(0), st(0)
00403440  fld        st(1)
00403442  fmulp      st(2)
00403444  faddp      st(1)
00403446  fstp       dword ptr [esp + 0x24]
0040344a  fld        dword ptr [esp + 0x24]
0040344e  fsubr      st(1)
00403450  fstp       dword ptr [esp + 0x18]
00403454  fldz       
00403456  fld        dword ptr [esp + 0x18]
0040345a  fcom       st(1)
0040345c  fnstsw     ax
0040345e  fstp       st(1)
00403460  test       ah, 1
00403463  jne        0x4035da

// block 00403469
00403469  fdivrp     st(1)
0040346b  mov        dword ptr [esi + 0x10], 0xffffffff
00403472  movzx      eax, byte ptr [esi + 0x12]
00403476  mov        ecx, 0xff
0040347b  sub        ecx, eax
0040347d  mov        dword ptr [esp + 0x24], ecx
00403481  mov        ecx, 0xff
00403486  mov        byte ptr [esi + 0x13], 0x60
0040348a  fstp       dword ptr [esp + 0x18]
0040348e  fild       dword ptr [esp + 0x24]
00403492  fld        dword ptr [esp + 0x18]
00403496  fld        st(0)
00403498  fmulp      st(2)
0040349a  fnstcw     word ptr [esp + 8]
0040349e  fld        st(2)
004034a0  movzx      eax, word ptr [esp + 8]
004034a5  fsubrp     st(2)
004034a7  or         eax, 0xc00
004034ac  fxch       st(1)
004034ae  mov        dword ptr [esp + 0x24], eax
004034b2  movzx      eax, byte ptr [esi + 0x11]
004034b6  fldcw      word ptr [esp + 0x24]
004034ba  sub        ecx, eax
004034bc  fistp      dword ptr [esp + 0x24]
004034c0  movzx      edx, byte ptr [esp + 0x24]
004034c5  mov        dword ptr [esp + 0x24], ecx
004034c9  mov        byte ptr [esi + 0x12], dl
004034cc  fldcw      word ptr [esp + 8]
004034d0  mov        ecx, 0xff
004034d5  fild       dword ptr [esp + 0x24]
004034d9  fnstcw     word ptr [esp + 8]
004034dd  fmul       st(1)
004034df  movzx      eax, word ptr [esp + 8]
004034e4  or         eax, 0xc00
004034e9  mov        dword ptr [esp + 0x24], eax
004034ed  movzx      eax, byte ptr [esi + 0x10]
004034f1  fsubr      st(2)
004034f3  fldcw      word ptr [esp + 0x24]
004034f7  sub        ecx, eax
004034f9  fistp      dword ptr [esp + 0x24]
004034fd  movzx      edx, byte ptr [esp + 0x24]
00403502  mov        dword ptr [esp + 0x24], ecx
00403506  mov        byte ptr [esi + 0x11], dl
00403509  fldcw      word ptr [esp + 8]
0040350d  fild       dword ptr [esp + 0x24]
00403511  fnstcw     word ptr [esp + 8]
00403515  fmul       st(1)
00403517  movzx      eax, word ptr [esp + 8]
0040351c  or         eax, 0xc00
00403521  mov        dword ptr [esp + 0x24], eax
00403525  lea        eax, [esp + 0x30]
00403529  fsubp      st(2)
0040352b  push       eax
0040352c  fldcw      word ptr [esp + 0x28]
00403530  mov        ecx, eax
00403532  push       ecx
00403533  fxch       st(1)
00403535  fistp      dword ptr [esp + 0x2c]
00403539  movzx      edx, byte ptr [esp + 0x2c]
0040353e  mov        byte ptr [esi + 0x10], dl
00403541  fldcw      word ptr [esp + 0x10]
00403545  fmul       qword ptr [0x4a3d70]
0040354b  fstp       dword ptr [esp + 0x2c]
0040354f  call       0x46c6bc

// block 00403554
00403554  fld        dword ptr [esp + 0x24]
00403558  fld        st(0)
0040355a  fmul       dword ptr [esp + 0x30]
0040355e  fstp       dword ptr [esp + 0x30]
00403562  fld        st(0)
00403564  fmul       dword ptr [esp + 0x34]
00403568  fstp       dword ptr [esp + 0x34]
0040356c  fmul       dword ptr [esp + 0x38]
00403570  fstp       dword ptr [esp + 0x38]
00403574  fld        dword ptr [esp + 0x10]
00403578  call       0x4938c0

// block 0040357d
0040357d  fstp       dword ptr [esp + 0x24]
00403581  fld        dword ptr [esp + 0x24]
00403585  fmul       dword ptr [esp + 0x18]
00403589  fmul       qword ptr [0x4a4078]
0040358f  fadd       dword ptr [esp + 0x30]
00403593  fstp       dword ptr [esp + 0x30]
00403597  fld        dword ptr [esp + 0x14]
0040359b  call       0x4938c0

// block 004035a0
004035a0  fstp       dword ptr [esp + 0x24]
004035a4  fld        dword ptr [esp + 0x24]
004035a8  mov        edx, dword ptr [esp + 0xc]
004035ac  fmul       dword ptr [esp + 0x18]
004035b0  fmul       qword ptr [0x4a4078]
004035b6  fadd       dword ptr [esp + 0x34]
004035ba  fstp       dword ptr [esp + 0x34]
004035be  fld        dword ptr [esi]
004035c0  fadd       dword ptr [esp + 0x30]
004035c4  fstp       dword ptr [esi]
004035c6  fld        dword ptr [esi + 4]
004035c9  fadd       dword ptr [esp + 0x34]
004035cd  fstp       dword ptr [esi + 4]
004035d0  fldz       
004035d2  fst        dword ptr [esi + 8]
004035d5  fstp       dword ptr [edx + 8]
004035d8  jmp        0x4035e4

// block 004035da
004035da  fstp       st(0)
004035dc  mov        byte ptr [esi + 0x13], 0
004035e0  fstp       st(1)
004035e2  fstp       st(0)

// block 004035e4
004035e4  fld        dword ptr [esp + 0x10]
004035e8  push       ecx
004035e9  fadd       qword ptr [0x4a4058]
004035ef  fstp       dword ptr [esp + 0x28]
004035f3  fld        dword ptr [esp + 0x28]
004035f7  fstp       dword ptr [esp]
004035fa  call       0x4646e0

// block 004035ff
004035ff  fstp       dword ptr [esp + 0x10]
00403603  push       ecx
00403604  fld        dword ptr [esp + 0x18]
00403608  fsub       qword ptr [0x4a4320]
0040360e  fstp       dword ptr [esp + 0x28]
00403612  fld        dword ptr [esp + 0x28]
00403616  fstp       dword ptr [esp]
00403619  call       0x4646e0

// block 0040361e
0040361e  mov        eax, dword ptr [ebx + 0x35dc]
00403624  fstp       dword ptr [esp + 0x14]
00403628  add        dword ptr [esp + 0xc], 0xc
0040362d  fld        qword ptr [0x4a3ed0]
00403633  inc        edi
00403634  add        esi, 0x1c
00403637  cmp        edi, dword ptr [eax + 4]
0040363a  jl         0x4033f2

// block 00403640
00403640  mov        eax, dword ptr [esp + 0x20]
00403644  mov        ecx, dword ptr [ebx + 0x35dc]
0040364a  inc        eax
0040364b  cmp        eax, dword ptr [ecx]
0040364d  mov        dword ptr [esp + 0x20], eax
00403651  jl         0x4033d5

// block 00403657
00403657  fstp       st(0)

// block 00403659
00403659  fld        dword ptr [ebx + 0x35ec]
0040365f  push       ecx
00403660  fadd       qword ptr [0x4a3e30]
00403666  fstp       dword ptr [esp + 0x28]
0040366a  fld        dword ptr [esp + 0x28]
0040366e  fstp       dword ptr [esp]
00403671  call       0x4646e0

// block 00403676
00403676  mov        esi, 0x4ce568
0040367b  fstp       dword ptr [ebx + 0x35ec]
00403681  call       0x464440

// block 00403686
00403686  mov        dword ptr [esp + 0x24], eax
0040368a  fild       dword ptr [esp + 0x24]
0040368e  test       eax, eax
00403690  jge        0x403698

// block 00403692
00403692  fadd       dword ptr [0x4a3e10]

// block 00403698
00403698  fmul       qword ptr [0x4a3eb0]
0040369e  fstp       dword ptr [esp + 0x24]
004036a2  fld        dword ptr [esp + 0x24]
004036a6  fmul       qword ptr [0x4a3cd8]
004036ac  fdiv       qword ptr [0x4a41c0]
004036b2  fadd       qword ptr [0x4a4328]
004036b8  fstp       dword ptr [esp + 0x24]
004036bc  fld        dword ptr [esp + 0x24]
004036c0  fadd       dword ptr [ebx + 0x35f0]
004036c6  fstp       dword ptr [esp + 0x24]
004036ca  fld        dword ptr [esp + 0x24]

// block 004036ce
004036ce  push       ecx
004036cf  fstp       dword ptr [esp]
004036d2  call       0x4646e0

// block 004036d7
004036d7  fstp       dword ptr [ebx + 0x35f0]

// block 004036dd
004036dd  mov        eax, 1
004036e2  add        dword ptr [ebx + 0x35d8], eax
004036e8  pop        edi
004036e9  pop        esi
004036ea  mov        esp, ebp
004036ec  pop        ebp
004036ed  ret        
