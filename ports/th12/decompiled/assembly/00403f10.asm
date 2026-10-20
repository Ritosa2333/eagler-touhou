
// block 00403f10
00403f10  sub        esp, 0x1f4
00403f16  fld        dword ptr [edx]
00403f18  push       esi
00403f19  fadd       dword ptr [ecx + 4]
00403f1c  mov        esi, eax
00403f1e  fstp       dword ptr [esp + 0x20]
00403f22  fld        dword ptr [ecx + 8]
00403f25  fadd       dword ptr [edx + 4]
00403f28  fstp       dword ptr [esp + 0x24]
00403f2c  fld        dword ptr [ecx + 0xc]
00403f2f  fadd       dword ptr [edx + 8]
00403f32  fstp       dword ptr [esp + 0x28]
00403f36  fld        dword ptr [esi + 0x3c]
00403f39  fadd       dword ptr [esi]
00403f3b  fstp       dword ptr [esp + 0x2c]
00403f3f  fld        dword ptr [esi + 0x40]
00403f42  fadd       dword ptr [esi + 4]
00403f45  fstp       dword ptr [esp + 0x30]
00403f49  fld        dword ptr [esi + 0x44]
00403f4c  fadd       dword ptr [esi + 8]
00403f4f  fstp       dword ptr [esp + 0x34]
00403f53  fld        dword ptr [esp + 0x20]
00403f57  fsub       dword ptr [esp + 0x2c]
00403f5b  fstp       dword ptr [esp + 0x14]
00403f5f  mov        eax, dword ptr [esp + 0x14]
00403f63  fld        dword ptr [esp + 0x24]
00403f67  mov        dword ptr [esp + 0x78], eax
00403f6b  fsub       dword ptr [esp + 0x30]
00403f6f  fstp       dword ptr [esp + 0x18]
00403f73  mov        eax, dword ptr [esp + 0x18]
00403f77  fld        dword ptr [esp + 0x28]
00403f7b  mov        dword ptr [esp + 0x7c], eax
00403f7f  fsub       dword ptr [esp + 0x34]
00403f83  fstp       dword ptr [esp + 0x1c]
00403f87  mov        eax, dword ptr [esp + 0x1c]
00403f8b  fld        dword ptr [esp + 0x18]
00403f8f  mov        dword ptr [esp + 0x80], eax
00403f96  fld        dword ptr [esp + 0x14]
00403f9a  fld        dword ptr [esp + 0x1c]
00403f9e  fld        st(1)
00403fa0  fmulp      st(2)
00403fa2  fld        st(2)
00403fa4  fmulp      st(3)
00403fa6  fxch       st(1)
00403fa8  faddp      st(2)
00403faa  fmul       st(0), st(0)
00403fac  faddp      st(1)
00403fae  fstp       dword ptr [esp + 4]
00403fb2  fld        dword ptr [esp + 4]
00403fb6  fld        dword ptr [esp + 0x1fc]
00403fbd  fcompp     
00403fbf  fnstsw     ax
00403fc1  test       ah, 5
00403fc4  jnp        0x4044e0

// block 00403fca
00403fca  fld        dword ptr [ecx + 0x10]
00403fcd  fld        qword ptr [0x4a3cf8]
00403fd3  fmul       st(1), st(0)
00403fd5  fxch       st(1)
00403fd7  fstp       dword ptr [esp + 0x14]
00403fdb  fld        dword ptr [ecx + 0x14]
00403fde  fmul       st(1)
00403fe0  fstp       dword ptr [esp + 0x18]
00403fe4  fld        dword ptr [ecx + 0x18]
00403fe7  fmul       st(1)
00403fe9  fstp       dword ptr [esp + 0x1c]
00403fed  fld        dword ptr [esp + 0x14]
00403ff1  fadd       dword ptr [ecx + 4]
00403ff4  fstp       dword ptr [esp + 4]
00403ff8  fld        dword ptr [esp + 4]
00403ffc  fst        dword ptr [esp + 0x78]
00404000  fld        dword ptr [esp + 0x18]
00404004  fld        st(0)
00404006  fadd       dword ptr [ecx + 8]
00404009  fstp       dword ptr [esp + 4]
0040400d  fld        dword ptr [esp + 4]
00404011  fst        dword ptr [esp + 0x7c]
00404015  fld        dword ptr [esp + 0x1c]
00404019  fld        st(0)
0040401b  fadd       dword ptr [ecx + 0xc]
0040401e  fstp       dword ptr [esp + 4]
00404022  fld        dword ptr [esp + 4]
00404026  fst        dword ptr [esp + 0x80]
0040402d  fxch       st(4)
0040402f  fst        dword ptr [esp + 0x84]
00404036  fxch       st(2)
00404038  fst        dword ptr [esp + 0x88]
0040403f  fld        dword ptr [ecx + 0xc]
00404042  fsub       st(2)
00404044  fstp       dword ptr [esp + 4]
00404048  fld        dword ptr [esp + 4]
0040404c  fst        dword ptr [esp + 0x8c]
00404053  fxch       st(3)
00404055  fst        dword ptr [esp + 0x90]
0040405c  fld        dword ptr [ecx + 8]
0040405f  fsubrp     st(5)
00404061  fxch       st(4)
00404063  fstp       dword ptr [esp + 4]
00404067  fld        dword ptr [esp + 4]
0040406b  fst        dword ptr [esp + 0x94]
00404072  fst        dword ptr [esp + 0xa0]
00404079  fxch       st(5)
0040407b  fst        dword ptr [esp + 0x98]
00404082  fxch       st(4)
00404084  fstp       dword ptr [esp + 0x9c]
0040408b  fxch       st(2)
0040408d  fst        dword ptr [esp + 0xa4]
00404094  fld        dword ptr [ecx + 4]
00404097  fsub       dword ptr [esp + 0x14]
0040409b  fstp       dword ptr [esp + 4]
0040409f  fld        dword ptr [esp + 4]
004040a3  fst        dword ptr [esp + 0xa8]
004040aa  fst        dword ptr [esp + 0xb4]
004040b1  fst        dword ptr [esp + 0xc0]
004040b8  fstp       dword ptr [esp + 0xcc]
004040bf  fxch       st(2)
004040c1  fst        dword ptr [esp + 0xac]
004040c8  fst        dword ptr [esp + 0xb8]
004040cf  fxch       st(3)
004040d1  fst        dword ptr [esp + 0xb0]
004040d8  fst        dword ptr [esp + 0xc8]
004040df  fxch       st(2)
004040e1  fst        dword ptr [esp + 0xbc]
004040e8  fst        dword ptr [esp + 0xd4]
004040ef  fxch       st(4)
004040f1  fst        dword ptr [esp + 0xc4]
004040f8  fst        dword ptr [esp + 0xd0]
004040ff  fld        dword ptr [ecx + 4]
00404102  fstp       dword ptr [esp + 0xd8]
00404109  fst        dword ptr [esp + 0xdc]
00404110  fxch       st(4)
00404112  fst        dword ptr [esp + 0xe0]
00404119  fld        dword ptr [ecx + 4]
0040411c  fstp       dword ptr [esp + 0xe4]
00404123  fxch       st(3)
00404125  fst        dword ptr [esp + 0xe8]
0040412c  fxch       st(3)
0040412e  sub        esp, 0xc
00404131  fstp       dword ptr [esp + 0xf8]
00404138  fld        dword ptr [ecx + 4]
0040413b  fstp       dword ptr [esp + 0xfc]
00404142  fxch       st(3)
00404144  fst        dword ptr [esp + 0x100]
0040414b  fxch       st(1)
0040414d  fst        dword ptr [esp + 0x104]
00404154  fld        dword ptr [ecx + 4]

// block 00404157
00404157  fstp       dword ptr [esp + 0x108]
0040415e  fxch       st(2)
00404160  fst        dword ptr [esp + 0x10c]
00404167  fxch       st(2)
00404169  fstp       dword ptr [esp + 0x110]
00404170  fld        dword ptr [ecx + 4]
00404173  fstp       dword ptr [esp + 0x114]
0040417a  fst        dword ptr [esp + 0x118]
00404181  fld        dword ptr [ecx + 0xc]
00404184  fstp       dword ptr [esp + 0x11c]
0040418b  fld        dword ptr [ecx + 4]
0040418e  fstp       dword ptr [esp + 0x120]
00404195  fxch       st(1)
00404197  fst        dword ptr [esp + 0x124]
0040419e  fld        dword ptr [ecx + 0xc]
004041a1  fstp       dword ptr [esp + 0x128]
004041a8  fld        dword ptr [ecx + 4]
004041ab  fstp       dword ptr [esp + 0x12c]
004041b2  fxch       st(1)
004041b4  fstp       dword ptr [esp + 0x130]
004041bb  fxch       st(1)
004041bd  fmulp      st(2)
004041bf  fld        dword ptr [ecx + 0xc]
004041c2  fsub       st(2)
004041c4  fstp       dword ptr [esp + 0x134]
004041cb  fld        dword ptr [ecx + 4]
004041ce  fstp       dword ptr [esp + 0x138]
004041d5  fstp       dword ptr [esp + 0x13c]
004041dc  fadd       dword ptr [ecx + 0xc]
004041df  lea        ecx, [esp + 0x44]
004041e3  fstp       dword ptr [esp + 0x140]
004041ea  fldz       
004041ec  fst        dword ptr [esp + 0x7c]
004041f0  fst        dword ptr [esp + 0x78]
004041f4  fst        dword ptr [esp + 0x74]
004041f8  fst        dword ptr [esp + 0x70]
004041fc  fst        dword ptr [esp + 0x68]
00404200  fst        dword ptr [esp + 0x64]
00404204  fst        dword ptr [esp + 0x60]
00404208  fst        dword ptr [esp + 0x5c]
0040420c  fst        dword ptr [esp + 0x54]
00404210  fst        dword ptr [esp + 0x50]
00404214  fst        dword ptr [esp + 0x4c]
00404218  fstp       dword ptr [esp + 0x48]
0040421c  fld1       
0040421e  fst        dword ptr [esp + 0x80]
00404225  fst        dword ptr [esp + 0x6c]
00404229  fst        dword ptr [esp + 0x58]
0040422d  fstp       dword ptr [esp + 0x44]
00404231  fld        dword ptr [edx + 8]
00404234  fstp       dword ptr [esp + 8]
00404238  fld        dword ptr [edx + 4]
0040423b  fstp       dword ptr [esp + 4]
0040423f  fld        dword ptr [edx]
00404241  fstp       dword ptr [esp]
00404244  push       ecx
00404245  call       0x46c6c2

// block 0040424a
0040424a  push       0x10
0040424c  lea        edx, [esp + 0x3c]
00404250  push       edx
00404251  lea        eax, [esi + 0x4c]
00404254  push       eax
00404255  lea        ecx, [esi + 0x8c]
0040425b  push       ecx
0040425c  add        esi, 0xcc
00404262  push       esi
00404263  push       0xc
00404265  lea        edx, [esp + 0x90]
0040426c  push       edx
0040426d  push       0xc
0040426f  lea        eax, [esp + 0x158]
00404276  push       eax
00404277  call       0x46c6c8

// block 0040427c
0040427c  fld        dword ptr [0x4a3e9c]
00404282  fstp       dword ptr [esp + 8]
00404286  lea        ecx, [esp + 0x138]
0040428d  fld        dword ptr [0x4a3e98]
00404293  mov        edx, 4
00404298  fstp       dword ptr [esp + 4]
0040429c  fld        dword ptr [0x4a3e00]
004042a2  fstp       dword ptr [esp + 0x10]
004042a6  fld        dword ptr [0x4a3e54]
004042ac  fstp       dword ptr [esp + 0xc]
004042b0  fld1       
004042b2  fldz       

// block 004042b4
004042b4  fcom       dword ptr [ecx + 8]
004042b7  fnstsw     ax
004042b9  test       ah, 0x41
004042bc  jp         0x404324

// block 004042be
004042be  fxch       st(1)
004042c0  fcom       dword ptr [ecx + 8]
004042c3  fnstsw     ax
004042c5  test       ah, 1
004042c8  jne        0x404322

// block 004042ca
004042ca  fld        dword ptr [ecx]
004042cc  fld        dword ptr [esp + 8]
004042d0  fcompp     
004042d2  fnstsw     ax
004042d4  test       ah, 0x41
004042d7  jne        0x4042df

// block 004042d9
004042d9  fld        dword ptr [ecx]
004042db  fstp       dword ptr [esp + 8]

// block 004042df
004042df  fld        dword ptr [ecx]
004042e1  fld        dword ptr [esp + 0x10]
004042e5  fcompp     
004042e7  fnstsw     ax
004042e9  test       ah, 5
004042ec  jp         0x4042f4

// block 004042ee
004042ee  fld        dword ptr [ecx]
004042f0  fstp       dword ptr [esp + 0x10]

// block 004042f4
004042f4  fld        dword ptr [ecx + 4]
004042f7  fld        dword ptr [esp + 4]
004042fb  fcompp     
004042fd  fnstsw     ax
004042ff  test       ah, 0x41
00404302  jne        0x40430b

// block 00404304
00404304  fld        dword ptr [ecx + 4]
00404307  fstp       dword ptr [esp + 4]

// block 0040430b
0040430b  fld        dword ptr [ecx + 4]
0040430e  fld        dword ptr [esp + 0xc]
00404312  fcompp     
00404314  fnstsw     ax
00404316  test       ah, 5
00404319  jp         0x404322

// block 0040431b
0040431b  fld        dword ptr [ecx + 4]
0040431e  fstp       dword ptr [esp + 0xc]

// block 00404322
00404322  fxch       st(1)

// block 00404324
00404324  fcom       dword ptr [ecx + 0x14]
00404327  fnstsw     ax
00404329  test       ah, 0x41
0040432c  jp         0x404398

// block 0040432e
0040432e  fxch       st(1)
00404330  fcom       dword ptr [ecx + 0x14]
00404333  fnstsw     ax
00404335  test       ah, 1
00404338  jne        0x404396

// block 0040433a
0040433a  fld        dword ptr [ecx + 0xc]
0040433d  fld        dword ptr [esp + 8]
00404341  fcompp     
00404343  fnstsw     ax
00404345  test       ah, 0x41
00404348  jne        0x404351

// block 0040434a
0040434a  fld        dword ptr [ecx + 0xc]
0040434d  fstp       dword ptr [esp + 8]

// block 00404351
00404351  fld        dword ptr [ecx + 0xc]
00404354  fld        dword ptr [esp + 0x10]
00404358  fcompp     
0040435a  fnstsw     ax
0040435c  test       ah, 5
0040435f  jp         0x404368

// block 00404361
00404361  fld        dword ptr [ecx + 0xc]
00404364  fstp       dword ptr [esp + 0x10]

// block 00404368
00404368  fld        dword ptr [ecx + 0x10]
0040436b  fld        dword ptr [esp + 4]
0040436f  fcompp     
00404371  fnstsw     ax
00404373  test       ah, 0x41
00404376  jne        0x40437f

// block 00404378
00404378  fld        dword ptr [ecx + 0x10]
0040437b  fstp       dword ptr [esp + 4]

// block 0040437f
0040437f  fld        dword ptr [ecx + 0x10]
00404382  fld        dword ptr [esp + 0xc]
00404386  fcompp     
00404388  fnstsw     ax
0040438a  test       ah, 5
0040438d  jp         0x404396

// block 0040438f
0040438f  fld        dword ptr [ecx + 0x10]
00404392  fstp       dword ptr [esp + 0xc]

// block 00404396
00404396  fxch       st(1)

// block 00404398
00404398  fcom       dword ptr [ecx + 0x20]
0040439b  fnstsw     ax
0040439d  test       ah, 0x41
004043a0  jp         0x40440c

// block 004043a2
004043a2  fxch       st(1)
004043a4  fcom       dword ptr [ecx + 0x20]
004043a7  fnstsw     ax
004043a9  test       ah, 1
004043ac  jne        0x40440a

// block 004043ae
004043ae  fld        dword ptr [ecx + 0x18]
004043b1  fld        dword ptr [esp + 8]
004043b5  fcompp     
004043b7  fnstsw     ax
004043b9  test       ah, 0x41
004043bc  jne        0x4043c5

// block 004043be
004043be  fld        dword ptr [ecx + 0x18]
004043c1  fstp       dword ptr [esp + 8]

// block 004043c5
004043c5  fld        dword ptr [ecx + 0x18]
004043c8  fld        dword ptr [esp + 0x10]
004043cc  fcompp     
004043ce  fnstsw     ax
004043d0  test       ah, 5
004043d3  jp         0x4043dc

// block 004043d5
004043d5  fld        dword ptr [ecx + 0x18]
004043d8  fstp       dword ptr [esp + 0x10]

// block 004043dc
004043dc  fld        dword ptr [ecx + 0x1c]
004043df  fld        dword ptr [esp + 4]
004043e3  fcompp     
004043e5  fnstsw     ax
004043e7  test       ah, 0x41
004043ea  jne        0x4043f3

// block 004043ec
004043ec  fld        dword ptr [ecx + 0x1c]
004043ef  fstp       dword ptr [esp + 4]

// block 004043f3
004043f3  fld        dword ptr [ecx + 0x1c]
004043f6  fld        dword ptr [esp + 0xc]
004043fa  fcompp     
004043fc  fnstsw     ax
004043fe  test       ah, 5
00404401  jp         0x40440a

// block 00404403
00404403  fld        dword ptr [ecx + 0x1c]
00404406  fstp       dword ptr [esp + 0xc]

// block 0040440a
0040440a  fxch       st(1)

// block 0040440c
0040440c  fcom       dword ptr [ecx + 0x2c]
0040440f  fnstsw     ax
00404411  test       ah, 0x41
00404414  jp         0x404480

// block 00404416
00404416  fxch       st(1)
00404418  fcom       dword ptr [ecx + 0x2c]
0040441b  fnstsw     ax
0040441d  test       ah, 1
00404420  jne        0x40447e

// block 00404422
00404422  fld        dword ptr [ecx + 0x24]
00404425  fld        dword ptr [esp + 8]
00404429  fcompp     
0040442b  fnstsw     ax
0040442d  test       ah, 0x41
00404430  jne        0x404439

// block 00404432
00404432  fld        dword ptr [ecx + 0x24]
00404435  fstp       dword ptr [esp + 8]

// block 00404439
00404439  fld        dword ptr [ecx + 0x24]
0040443c  fld        dword ptr [esp + 0x10]
00404440  fcompp     
00404442  fnstsw     ax
00404444  test       ah, 5
00404447  jp         0x404450

// block 00404449
00404449  fld        dword ptr [ecx + 0x24]
0040444c  fstp       dword ptr [esp + 0x10]

// block 00404450
00404450  fld        dword ptr [ecx + 0x28]
00404453  fld        dword ptr [esp + 4]
00404457  fcompp     
00404459  fnstsw     ax
0040445b  test       ah, 0x41
0040445e  jne        0x404467

// block 00404460
00404460  fld        dword ptr [ecx + 0x28]
00404463  fstp       dword ptr [esp + 4]

// block 00404467
00404467  fld        dword ptr [ecx + 0x28]
0040446a  fld        dword ptr [esp + 0xc]
0040446e  fcompp     
00404470  fnstsw     ax
00404472  test       ah, 5
00404475  jp         0x40447e

// block 00404477
00404477  fld        dword ptr [ecx + 0x28]
0040447a  fstp       dword ptr [esp + 0xc]

// block 0040447e
0040447e  fxch       st(1)

// block 00404480
00404480  add        ecx, 0x30
00404483  sub        edx, 1
00404486  jne        0x4042b4

// block 0040448c
0040448c  fstp       st(0)
0040448e  fstp       st(0)
00404490  fld        dword ptr [0x4a3d78]
00404496  fcomp      dword ptr [esp + 0x10]
0040449a  fnstsw     ax
0040449c  test       ah, 0x41
0040449f  jp         0x4044e0

// block 004044a1
004044a1  fld        dword ptr [esp + 8]
004044a5  fcomp      qword ptr [0x4a3e80]
004044ab  fnstsw     ax
004044ad  test       ah, 0x41
004044b0  jp         0x4044e0

// block 004044b2
004044b2  fld        dword ptr [0x4a3e68]
004044b8  fcomp      dword ptr [esp + 0xc]
004044bc  fnstsw     ax
004044be  test       ah, 0x41
004044c1  jp         0x4044e0

// block 004044c3
004044c3  fld        dword ptr [esp + 4]
004044c7  fcomp      qword ptr [0x4a3e90]
004044cd  fnstsw     ax
004044cf  test       ah, 0x41
004044d2  jp         0x4044e0

// block 004044d4
004044d4  xor        eax, eax
004044d6  pop        esi
004044d7  add        esp, 0x1f4
004044dd  ret        4

// block 004044e0
004044e0  mov        eax, 1
004044e5  pop        esi
004044e6  add        esp, 0x1f4
004044ec  ret        4
