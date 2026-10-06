
// block 00437f30
00437f30  push       ebp
00437f31  mov        ebp, esp
00437f33  and        esp, 0xfffffff8
00437f36  sub        esp, 0x34
00437f39  push       esi
00437f3a  mov        esi, ecx
00437f3c  fld        dword ptr [esi + 0x97c]
00437f42  fsub       dword ptr [eax]
00437f44  fstp       dword ptr [esp + 0x14]
00437f48  fld        dword ptr [esi + 0x980]
00437f4e  fsub       dword ptr [eax + 4]
00437f51  fstp       dword ptr [esp + 0x18]
00437f55  fld        dword ptr [ebp + 8]
00437f58  fchs       
00437f5a  fstp       dword ptr [esp + 8]
00437f5e  fld        dword ptr [esp + 8]
00437f62  call       0x4938c0

// block 00437f67
00437f67  fstp       dword ptr [esp + 0xc]
00437f6b  fld        dword ptr [esp + 0xc]
00437f6f  fstp       dword ptr [esp + 0x10]
00437f73  fld        dword ptr [esp + 8]
00437f77  call       0x4939f0

// block 00437f7c
00437f7c  fstp       dword ptr [esp + 0xc]
00437f80  fld        dword ptr [esp + 0xc]
00437f84  fstp       dword ptr [esp + 0xc]
00437f88  fld        dword ptr [esp + 0xc]
00437f8c  fld        st(0)
00437f8e  fld        dword ptr [esp + 0x14]
00437f92  fld        st(0)
00437f94  fmulp      st(2)
00437f96  fld        dword ptr [esp + 0x18]
00437f9a  fld        st(0)
00437f9c  fld        dword ptr [esp + 0x10]
00437fa0  fld        st(0)
00437fa2  fmulp      st(2)
00437fa4  fxch       st(4)
00437fa6  fsubrp     st(1)
00437fa8  fstp       dword ptr [esp + 0x14]
00437fac  fmulp      st(3)
00437fae  fmulp      st(1)
00437fb0  faddp      st(1)
00437fb2  fstp       dword ptr [esp + 0x18]
00437fb6  fld        dword ptr [esi + 0x9e4]
00437fbc  fld        qword ptr [0x4a3d68]
00437fc2  fmul       st(1), st(0)
00437fc4  fxch       st(1)
00437fc6  fstp       dword ptr [esp + 0x20]
00437fca  fld        dword ptr [esi + 0x9e8]
00437fd0  fmul       st(1)
00437fd2  fstp       dword ptr [esp + 0x24]
00437fd6  fld        dword ptr [esp + 0x14]
00437fda  fld        st(0)
00437fdc  fsub       dword ptr [esp + 0x20]
00437fe0  fstp       dword ptr [esp + 0x2c]
00437fe4  fld        dword ptr [esp + 0x18]
00437fe8  fld        st(0)
00437fea  fsub       dword ptr [esp + 0x24]
00437fee  fstp       dword ptr [esp + 0x30]
00437ff2  fld        dword ptr [esi + 0x9e4]
00437ff8  fmul       st(3)
00437ffa  fstp       dword ptr [esp + 0x20]
00437ffe  fld        dword ptr [esi + 0x9e8]
00438004  fmulp      st(3)
00438006  fxch       st(2)
00438008  fstp       dword ptr [esp + 0x24]
0043800c  fld        dword ptr [esp + 0x20]
00438010  fadd       st(1)
00438012  fstp       dword ptr [esp + 0x14]
00438016  fld        dword ptr [esp + 0x24]
0043801a  fadd       st(2)
0043801c  fstp       dword ptr [esp + 0x18]
00438020  fld        dword ptr [ebp + 0x10]
00438023  fld        st(0)
00438025  fld        qword ptr [0x4a3cf8]
0043802b  fmul       st(1), st(0)
0043802d  fld        dword ptr [esp + 0x2c]
00438031  fcomp      st(2)
00438033  fnstsw     ax
00438035  test       ah, 0x41
00438038  je         0x43817c

// block 0043803e
0043803e  fld        dword ptr [ebp + 0xc]
00438041  fld        st(0)
00438043  fmul       st(2)
00438045  fld        dword ptr [esp + 0x30]
00438049  fcomp      st(1)
0043804b  fnstsw     ax
0043804d  test       ah, 0x41
00438050  je         0x43818f

// block 00438056
00438056  fxch       st(4)
00438058  fchs       
0043805a  fmul       st(2)
0043805c  fld        dword ptr [esp + 0x14]
00438060  fcomp      st(1)
00438062  fnstsw     ax
00438064  test       ah, 5
00438067  jnp        0x4381a6

// block 0043806d
0043806d  fxch       st(1)
0043806f  fchs       
00438071  fmulp      st(2)
00438073  fld        dword ptr [esp + 0x18]
00438077  fcomp      st(2)
00438079  fnstsw     ax
0043807b  test       ah, 5
0043807e  jnp        0x4381bd

// block 00438084
00438084  fld        st(4)
00438086  fsub       dword ptr [esi + 0x9e4]
0043808c  fstp       dword ptr [esp + 0x2c]
00438090  fld        st(5)
00438092  fsub       dword ptr [esi + 0x9e8]
00438098  fstp       dword ptr [esp + 0x30]
0043809c  fld        dword ptr [esi + 0x9e4]
004380a2  faddp      st(5)
004380a4  fxch       st(4)
004380a6  fstp       dword ptr [esp + 0x20]
004380aa  fld        dword ptr [esi + 0x9e8]
004380b0  faddp      st(5)
004380b2  fxch       st(4)
004380b4  fstp       dword ptr [esp + 0x24]
004380b8  fld        dword ptr [esp + 0x2c]
004380bc  fcompp     
004380be  fnstsw     ax
004380c0  test       ah, 0x41
004380c3  je         0x43815a

// block 004380c9
004380c9  fld        dword ptr [esp + 0x30]
004380cd  fcompp     
004380cf  fnstsw     ax
004380d1  test       ah, 0x41
004380d4  je         0x43816c

// block 004380da
004380da  fld        dword ptr [esp + 0x20]
004380de  fcompp     
004380e0  fnstsw     ax
004380e2  test       ah, 5
004380e5  jnp        0x43816e

// block 004380eb
004380eb  fld        dword ptr [esp + 0x24]
004380ef  fcompp     
004380f1  fnstsw     ax
004380f3  test       ah, 5
004380f6  jnp        0x438170

// block 004380f8
004380f8  mov        eax, dword ptr [0x4b43e4]
004380fd  test       eax, eax
004380ff  je         0x43810e

// block 00438101
00438101  cmp        dword ptr [eax + 0x6d30], 0
00438108  jne        0x4381c9

// block 0043810e
0043810e  mov        eax, dword ptr [esi + 0xa28]
00438114  cmp        eax, 2
00438117  je         0x4381c9

// block 0043811d
0043811d  cmp        eax, 4
00438120  je         0x4381c9

// block 00438126
00438126  cmp        eax, 3
00438129  je         0x4381c9

// block 0043812f
0043812f  test       byte ptr [esi + 0xc414], 2
00438136  jne        0x4381c9

// block 0043813c
0043813c  cmp        dword ptr [esi + 0xc404], 0
00438143  jg         0x4381c9

// block 00438149
00438149  call       0x438370

// block 0043814e
0043814e  mov        eax, 1
00438153  pop        esi
00438154  mov        esp, ebp
00438156  pop        ebp
00438157  ret        0xc

// block 0043815a
0043815a  fstp       st(2)
0043815c  mov        eax, 2
00438161  fstp       st(0)
00438163  fstp       st(0)
00438165  pop        esi
00438166  mov        esp, ebp
00438168  pop        ebp
00438169  ret        0xc

// block 0043816c
0043816c  fstp       st(1)

// block 0043816e
0043816e  fstp       st(0)

// block 00438170
00438170  mov        eax, 2
00438175  pop        esi
00438176  mov        esp, ebp
00438178  pop        ebp
00438179  ret        0xc

// block 0043817c
0043817c  fstp       st(1)
0043817e  xor        eax, eax
00438180  fstp       st(2)
00438182  fstp       st(2)
00438184  fstp       st(0)
00438186  fstp       st(0)
00438188  pop        esi
00438189  mov        esp, ebp
0043818b  pop        ebp
0043818c  ret        0xc

// block 0043818f
0043818f  fstp       st(3)
00438191  xor        eax, eax
00438193  fstp       st(4)
00438195  fstp       st(4)
00438197  fstp       st(3)
00438199  fstp       st(0)
0043819b  fstp       st(0)
0043819d  fstp       st(0)
0043819f  pop        esi
004381a0  mov        esp, ebp
004381a2  pop        ebp
004381a3  ret        0xc

// block 004381a6
004381a6  fstp       st(3)
004381a8  xor        eax, eax
004381aa  fstp       st(4)
004381ac  fstp       st(4)
004381ae  fstp       st(3)
004381b0  fstp       st(2)
004381b2  fstp       st(0)
004381b4  fstp       st(0)
004381b6  pop        esi
004381b7  mov        esp, ebp
004381b9  pop        ebp
004381ba  ret        0xc

// block 004381bd
004381bd  fstp       st(2)
004381bf  fstp       st(3)
004381c1  fstp       st(3)
004381c3  fstp       st(1)
004381c5  fstp       st(1)
004381c7  fstp       st(0)

// block 004381c9
004381c9  xor        eax, eax
004381cb  pop        esi
004381cc  mov        esp, ebp
004381ce  pop        ebp
004381cf  ret        0xc
