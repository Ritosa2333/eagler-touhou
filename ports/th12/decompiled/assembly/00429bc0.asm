
// block 00429bc0
00429bc0  push       ebp
00429bc1  mov        ebp, esp
00429bc3  and        esp, 0xfffffff8
00429bc6  sub        esp, 0x354
00429bcc  mov        eax, dword ptr [0x4ad138]
00429bd1  xor        eax, esp
00429bd3  mov        dword ptr [esp + 0x350], eax
00429bda  mov        eax, dword ptr [ebp + 0xc]
00429bdd  push       ebx
00429bde  push       esi
00429bdf  mov        esi, dword ptr [ebp + 8]
00429be2  push       edi
00429be3  xor        edi, edi
00429be5  mov        ebx, ecx
00429be7  mov        dword ptr [esp + 0x34], ebx
00429beb  mov        dword ptr [esp + 0x2c], eax
00429bef  cmp        dword ptr [ebp + 0x14], edi
00429bf2  je         0x429c15

// block 00429bf4
00429bf4  cmp        dword ptr [ebx + 0x44c], edi
00429bfa  je         0x429c15

// block 00429bfc
00429bfc  xor        eax, eax
00429bfe  pop        edi
00429bff  pop        esi
00429c00  pop        ebx
00429c01  mov        ecx, dword ptr [esp + 0x350]
00429c08  xor        ecx, esp
00429c0a  call       0x46c932

// block 00429c0f
00429c0f  mov        esp, ebp
00429c11  pop        ebp
00429c12  ret        0x10

// block 00429c15
00429c15  mov        ecx, dword ptr [ebx + 0x50]
00429c18  fld        dword ptr [0x4a3e54]
00429c1e  mov        edx, dword ptr [ebx + 0x54]
00429c21  fstp       dword ptr [esp + 0x10]
00429c25  mov        eax, dword ptr [ebx + 0x58]
00429c28  push       0x100
00429c2d  mov        dword ptr [esp + 0x4c], ecx
00429c31  lea        ecx, [esp + 0x25c]
00429c38  push       edi
00429c39  push       ecx
00429c3a  mov        dword ptr [esp + 0x44], edi
00429c3e  mov        dword ptr [esp + 0x58], edx
00429c42  mov        dword ptr [esp + 0x5c], eax
00429c46  call       0x477420

// block 00429c4b
00429c4b  mov        eax, dword ptr [esp + 0x38]
00429c4f  fld        dword ptr [eax]
00429c51  add        esp, 4
00429c54  fld        qword ptr [0x4a3cf8]
00429c5a  lea        ecx, [esp + 0x44]
00429c5e  fmul       st(1), st(0)
00429c60  fxch       st(1)
00429c62  fstp       dword ptr [esp + 0x28]
00429c66  fmul       dword ptr [eax + 4]
00429c69  fstp       dword ptr [esp + 0x2c]
00429c6d  fld        dword ptr [esi]
00429c6f  fld        dword ptr [esp + 0x28]
00429c73  fld        st(0)
00429c75  fsubp      st(2)
00429c77  fxch       st(1)
00429c79  fstp       dword ptr [esp + 0x5c]
00429c7d  fld        dword ptr [esi + 4]
00429c80  fld        dword ptr [esp + 0x2c]
00429c84  fld        st(0)
00429c86  fsubp      st(2)
00429c88  fxch       st(1)
00429c8a  fstp       dword ptr [esp + 0x60]
00429c8e  fld        dword ptr [esi]
00429c90  faddp      st(2)
00429c92  fxch       st(1)
00429c94  fstp       dword ptr [esp + 0x68]
00429c98  fadd       dword ptr [esi + 4]
00429c9b  fstp       dword ptr [esp + 0x6c]
00429c9f  fldz       
00429ca1  fstp       dword ptr [esp + 0x4c]
00429ca5  fld        dword ptr [0x4a3e54]
00429cab  fstp       dword ptr [esp + 4]
00429caf  fld        dword ptr [ebx + 0x68]
00429cb2  fstp       dword ptr [esp]
00429cb5  call       0x42e880

// block 00429cba
00429cba  fld        dword ptr [ebx + 0x50]
00429cbd  fld        dword ptr [esp + 0x3c]
00429cc1  fld        st(0)
00429cc3  faddp      st(2)
00429cc5  fxch       st(1)
00429cc7  fstp       dword ptr [esp + 0x20]
00429ccb  mov        edx, dword ptr [esp + 0x20]
00429ccf  fld        dword ptr [ebx + 0x54]
00429cd2  mov        dword ptr [esp + 0x14], edx
00429cd6  fld        dword ptr [esp + 0x40]
00429cda  fld        st(0)
00429cdc  faddp      st(2)
00429cde  fxch       st(1)
00429ce0  fstp       dword ptr [esp + 0x24]
00429ce4  mov        eax, dword ptr [esp + 0x24]
00429ce8  fld        dword ptr [ebx + 0x58]
00429ceb  mov        dword ptr [esp + 0x18], eax
00429cef  fld        dword ptr [esp + 0x44]
00429cf3  fld        st(0)
00429cf5  faddp      st(2)
00429cf7  fxch       st(1)
00429cf9  fstp       dword ptr [esp + 0x28]
00429cfd  mov        ecx, dword ptr [esp + 0x28]
00429d01  fldz       
00429d03  mov        dword ptr [esp + 0x1c], ecx
00429d07  fstp       dword ptr [esp + 0x1c]
00429d0b  fld        st(2)
00429d0d  faddp      st(3)
00429d0f  fxch       st(2)
00429d11  fstp       dword ptr [esp + 0x3c]
00429d15  fadd       st(0), st(0)
00429d17  fstp       dword ptr [esp + 0x40]
00429d1b  fadd       st(0), st(0)
00429d1d  fstp       dword ptr [esp + 0x44]
00429d21  fld        dword ptr [ebx + 0x6c]
00429d24  fcomp      qword ptr [0x4a3d68]
00429d2a  fnstsw     ax
00429d2c  test       ah, 1
00429d2f  jne        0x429ff0

// block 00429d35
00429d35  jmp        0x429d3f

// block 00429d37
00429d37  fstp       st(3)
00429d39  fstp       st(2)
00429d3b  fstp       st(1)
00429d3d  fstp       st(0)

// block 00429d3f
00429d3f  fld        dword ptr [0x4a3d78]
00429d45  fld        dword ptr [esp + 0x14]
00429d49  fld        dword ptr [esp + 0x54]
00429d4d  fcomp      st(1)
00429d4f  fnstsw     ax
00429d51  fld        dword ptr [esp + 0x18]
00429d55  test       ah, 0x41
00429d58  je         0x429ea3

// block 00429d5e
00429d5e  fld        dword ptr [esp + 0x60]
00429d62  fcomp      st(2)
00429d64  fnstsw     ax
00429d66  fstp       st(1)
00429d68  test       ah, 5
00429d6b  jnp        0x429ea5

// block 00429d71
00429d71  fld        dword ptr [esp + 0x58]
00429d75  fcomp      st(1)
00429d77  fnstsw     ax
00429d79  test       ah, 0x41
00429d7c  je         0x429ea5

// block 00429d82
00429d82  fld        dword ptr [esp + 0x64]
00429d86  fcompp     
00429d88  fnstsw     ax
00429d8a  test       ah, 5
00429d8d  jnp        0x429ea7

// block 00429d93
00429d93  mov        esi, 1
00429d98  add        dword ptr [esp + 0x38], esi
00429d9c  cmp        dword ptr [ebp + 0x10], 0
00429da0  mov        byte ptr [esp + edi + 0x258], 1
00429da8  je         0x429de3

// block 00429daa
00429daa  sub        esp, 8
00429dad  fst        dword ptr [esp + 4]
00429db1  lea        ecx, [esp + 0x1c]
00429db5  fstp       dword ptr [esp]
00429db8  call       0x42e820

// block 00429dbd
00429dbd  test       eax, eax
00429dbf  jne        0x429de5

// block 00429dc1
00429dc1  fld        dword ptr [0x4a41f4]
00429dc7  sub        esp, 8
00429dca  fstp       dword ptr [esp + 4]
00429dce  mov        edx, ecx
00429dd0  fld        dword ptr [0x4a4060]
00429dd6  fstp       dword ptr [esp]
00429dd9  push       edx
00429dda  push       9
00429ddc  call       0x4273f0

// block 00429de1
00429de1  jmp        0x429de5

// block 00429de3
00429de3  fstp       st(0)

// block 00429de5
00429de5  test       dword ptr [0x4cee78], 0x8000
00429def  movsx      eax, word ptr [ebx + 0x47a]
00429df6  mov        edx, dword ptr [0x4b44f4]
00429dfc  mov        ebx, dword ptr [edx + 0x488]
00429e02  lea        ecx, [eax + eax + 4]
00429e06  mov        dword ptr [esp + 0x2c], ecx
00429e0a  je         0x429e1d

// block 00429e0c
00429e0c  push       0x4cf1d0
00429e11  call       dword ptr [0x498088]

// block 00429e17
00429e17  inc        byte ptr [0x4cf221]

// block 00429e1d
00429e1d  add        dword ptr [ebx + 0x130], esi
00429e23  call       0x4621c0

// block 00429e28
00429e28  fld        dword ptr [esp + 0x14]
00429e2c  fadd       qword ptr [0x4a3d70]
00429e32  mov        esi, eax
00429e34  or         dword ptr [esi + 0x480], 1
00429e3b  mov        eax, dword ptr [esp + 0x2c]
00429e3f  fadd       qword ptr [0x4a3d28]
00429e45  mov        dword ptr [esi + 0x20], 0x17
00429e4c  push       eax
00429e4d  push       esi
00429e4e  fstp       dword ptr [esi + 0x430]
00429e54  mov        ecx, ebx
00429e56  fld        dword ptr [esp + 0x20]
00429e5a  fadd       qword ptr [0x4a3d68]
00429e60  fstp       dword ptr [esi + 0x434]
00429e66  fld        dword ptr [esp + 0x24]
00429e6a  fstp       dword ptr [esi + 0x438]
00429e70  call       0x454d10

// block 00429e75
00429e75  mov        ebx, esi
00429e77  lea        eax, [esp + 0x6c]
00429e7b  call       0x461250

// block 00429e80
00429e80  test       dword ptr [0x4cee78], 0x8000
00429e8a  je         0x429e9d

// block 00429e8c
00429e8c  push       0x4cf1d0
00429e91  call       dword ptr [0x49808c]

// block 00429e97
00429e97  dec        byte ptr [0x4cf221]

// block 00429e9d
00429e9d  mov        ebx, dword ptr [esp + 0x34]
00429ea1  jmp        0x429ea9

// block 00429ea3
00429ea3  fstp       st(1)

// block 00429ea5
00429ea5  fstp       st(0)

// block 00429ea7
00429ea7  fstp       st(0)

// block 00429ea9
00429ea9  fld        dword ptr [esp + 0x3c]
00429ead  inc        edi
00429eae  fld        st(0)
00429eb0  fadd       dword ptr [esp + 0x14]
00429eb4  fstp       dword ptr [esp + 0x14]
00429eb8  fld        dword ptr [esp + 0x40]
00429ebc  fld        st(0)
00429ebe  fadd       dword ptr [esp + 0x18]
00429ec2  fstp       dword ptr [esp + 0x18]
00429ec6  fld        dword ptr [esp + 0x1c]
00429eca  fld        dword ptr [esp + 0x44]
00429ece  fld        st(0)
00429ed0  faddp      st(2)
00429ed2  fxch       st(1)
00429ed4  fstp       dword ptr [esp + 0x1c]
00429ed8  fld        dword ptr [esp + 0x10]
00429edc  fld        qword ptr [0x4a3d68]
00429ee2  fadd       st(1), st(0)
00429ee4  fxch       st(1)
00429ee6  fstp       dword ptr [esp + 0x10]
00429eea  fld        dword ptr [esp + 0x10]
00429eee  fadd       qword ptr [0x4a4078]
00429ef4  fld        dword ptr [ebx + 0x6c]
00429ef7  fcompp     
00429ef9  fnstsw     ax
00429efb  test       ah, 1
00429efe  je         0x429d37

// block 00429f04
00429f04  mov        eax, dword ptr [esp + 0x38]
00429f08  mov        dword ptr [esp + 0x2c], edi
00429f0c  test       eax, eax
00429f0e  je         0x429fe8

// block 00429f14
00429f14  cmp        eax, edi
00429f16  jl         0x429f3b

// block 00429f18
00429f18  fstp       st(3)
00429f1a  mov        byte ptr [ebx + 0x7c], 1
00429f1e  fstp       st(2)
00429f20  fstp       st(1)
00429f22  fstp       st(0)
00429f24  pop        edi
00429f25  pop        esi
00429f26  pop        ebx
00429f27  mov        ecx, dword ptr [esp + 0x350]
00429f2e  xor        ecx, esp
00429f30  call       0x46c932

// block 00429f35
00429f35  mov        esp, ebp
00429f37  pop        ebp
00429f38  ret        0x10

// block 00429f3b
00429f3b  fld        dword ptr [0x4a3e00]
00429f41  xor        ebx, ebx
00429f43  test       edi, edi
00429f45  jle        0x42a00b

// block 00429f4b
00429f4b  cmp        byte ptr [esp + ebx + 0x258], 0
00429f53  je         0x429f5a

// block 00429f55
00429f55  inc        ebx
00429f56  cmp        ebx, edi
00429f58  jl         0x429f4b

// block 00429f5a
00429f5a  mov        dword ptr [esp + 0x10], ebx
00429f5e  test       ebx, ebx
00429f60  je         0x42a00b

// block 00429f66
00429f66  fild       dword ptr [esp + 0x10]
00429f6a  mov        ecx, dword ptr [esp + 0x34]
00429f6e  fstp       dword ptr [esp + 0x10]
00429f72  fld        dword ptr [esp + 0x10]
00429f76  fst        dword ptr [esp + 0x10]
00429f7a  fld        dword ptr [esp + 0x10]
00429f7e  fld        st(0)
00429f80  fmulp      st(7)
00429f82  fxch       st(6)
00429f84  fstp       dword ptr [esp + 0x20]
00429f88  fld        st(4)
00429f8a  fmul       st(6)
00429f8c  fstp       dword ptr [esp + 0x24]
00429f90  fld        st(3)
00429f92  fmulp      st(6)
00429f94  fxch       st(5)
00429f96  fstp       dword ptr [esp + 0x28]
00429f9a  fld        dword ptr [ecx + 0x50]
00429f9d  fadd       dword ptr [esp + 0x20]
00429fa1  fstp       dword ptr [ecx + 0x50]
00429fa4  fld        dword ptr [esp + 0x24]
00429fa8  fadd       dword ptr [ecx + 0x54]
00429fab  fstp       dword ptr [ecx + 0x54]
00429fae  fld        dword ptr [ecx + 0x58]
00429fb1  fadd       dword ptr [esp + 0x28]
00429fb5  fstp       dword ptr [ecx + 0x58]
00429fb8  fxch       st(4)
00429fba  fmul       st(1)
00429fbc  fld        dword ptr [ecx + 0x6c]
00429fbf  fsub       st(1)
00429fc1  fstp       dword ptr [esp + 0x10]
00429fc5  fld        dword ptr [esp + 0x10]
00429fc9  fst        dword ptr [ecx + 0x6c]
00429fcc  fcom       st(5)
00429fce  fnstsw     ax
00429fd0  test       ah, 0x41
00429fd3  jne        0x429fe0

// block 00429fd5
00429fd5  fstp       dword ptr [ecx + 0x464]
00429fdb  fstp       dword ptr [ecx + 0x78]
00429fde  jmp        0x42a011

// block 00429fe0
00429fe0  fstp       st(1)
00429fe2  mov        byte ptr [ecx + 0x7c], 1
00429fe6  fstp       st(0)

// block 00429fe8
00429fe8  fstp       st(3)
00429fea  fstp       st(2)
00429fec  fstp       st(1)

// block 00429fee
00429fee  fstp       st(0)

// block 00429ff0
00429ff0  mov        ecx, dword ptr [esp + 0x35c]
00429ff7  mov        eax, dword ptr [esp + 0x38]
00429ffb  pop        edi
00429ffc  pop        esi
00429ffd  pop        ebx
00429ffe  xor        ecx, esp
0042a000  call       0x46c932

// block 0042a005
0042a005  mov        esp, ebp
0042a007  pop        ebp
0042a008  ret        0x10

// block 0042a00b
0042a00b  mov        ecx, dword ptr [esp + 0x34]
0042a00f  fstp       st(4)

// block 0042a011
0042a011  xor        eax, eax
0042a013  cmp        ebx, edi
0042a015  jge        0x429fe8

// block 0042a017
0042a017  cmp        byte ptr [esp + ebx + 0x258], 0
0042a01f  jne        0x42a029

// block 0042a021
0042a021  inc        ebx
0042a022  inc        eax
0042a023  cmp        ebx, edi
0042a025  jl         0x42a017

// block 0042a027
0042a027  jmp        0x429fe8

// block 0042a029
0042a029  cmp        ebx, edi
0042a02b  mov        dword ptr [esp + 0x10], eax
0042a02f  jge        0x429fe8

// block 0042a031
0042a031  fild       dword ptr [esp + 0x10]
0042a035  fmul       st(1)
0042a037  fld        dword ptr [ecx + 0x464]
0042a03d  fld        dword ptr [ecx + 0x6c]
0042a040  fsub       st(2)
0042a042  fsubp      st(1)
0042a044  fstp       dword ptr [ecx + 0x464]
0042a04a  fstp       dword ptr [esp + 0x10]
0042a04e  fld        dword ptr [esp + 0x10]
0042a052  fst        dword ptr [ecx + 0x6c]
0042a055  fcomp      st(4)
0042a057  fnstsw     ax
0042a059  test       ah, 5
0042a05c  jp         0x42a062

// block 0042a05e
0042a05e  mov        byte ptr [ecx + 0x7c], 1

// block 0042a062
0042a062  mov        edx, dword ptr [0x4b44f4]
0042a068  jmp        0x42a084

// block 0042a06a
0042a06a  fld        qword ptr [0x4a3d68]
0042a070  mov        edi, dword ptr [esp + 0x2c]
0042a074  fld        dword ptr [esp + 0x40]
0042a078  mov        ecx, dword ptr [esp + 0x34]
0042a07c  fld        dword ptr [esp + 0x44]
0042a080  fxch       st(1)
0042a082  fxch       st(2)

// block 0042a084
0042a084  cmp        ebx, edi
0042a086  jge        0x429fe8

// block 0042a08c
0042a08c  cmp        byte ptr [esp + ebx + 0x258], 0
0042a094  je         0x42a099

// block 0042a096
0042a096  inc        ebx
0042a097  jmp        0x42a084

// block 0042a099
0042a099  cmp        ebx, edi
0042a09b  jge        0x429fe8

// block 0042a0a1
0042a0a1  xor        eax, eax
0042a0a3  mov        dword ptr [esp + 0x30], ebx

// block 0042a0a7
0042a0a7  cmp        byte ptr [esp + ebx + 0x258], 0
0042a0af  jne        0x42a0b7

// block 0042a0b1
0042a0b1  inc        ebx
0042a0b2  inc        eax
0042a0b3  cmp        ebx, edi
0042a0b5  jl         0x42a0a7

// block 0042a0b7
0042a0b7  mov        dword ptr [esp + 0x10], eax
0042a0bb  fimul      dword ptr [esp + 0x10]
0042a0bf  lea        esi, [ecx + 0x454]
0042a0c5  mov        ecx, 0x7a
0042a0ca  lea        edi, [esp + 0x70]

// block 0042a0ce
0042a0ce  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 0042a0d0
0042a0d0  fstp       dword ptr [esp + 0x84]
0042a0d7  fld        dword ptr [esp + 0x84]
0042a0de  fst        dword ptr [esp + 0x80]
0042a0e5  fcomp      st(3)
0042a0e7  fnstsw     ax
0042a0e9  test       ah, 0x41
0042a0ec  jne        0x42a1dc

// block 0042a0f2
0042a0f2  fild       dword ptr [esp + 0x30]
0042a0f6  lea        edi, [edx + 0x468]
0042a0fc  fstp       dword ptr [esp + 0x30]
0042a100  fld        dword ptr [esp + 0x30]
0042a104  mov        dword ptr [esp + 0x30], edx
0042a108  fld        st(0)
0042a10a  fmul       dword ptr [esp + 0x3c]
0042a10e  fstp       dword ptr [esp + 0x20]
0042a112  fld        st(0)
0042a114  fmulp      st(3)
0042a116  fxch       st(2)
0042a118  fstp       dword ptr [esp + 0x24]
0042a11c  fmulp      st(1)
0042a11e  fstp       dword ptr [esp + 0x28]
0042a122  fld        dword ptr [esp + 0x48]
0042a126  fadd       dword ptr [esp + 0x20]
0042a12a  fstp       dword ptr [esp + 0x14]
0042a12e  mov        ecx, dword ptr [esp + 0x14]
0042a132  fld        dword ptr [esp + 0x4c]
0042a136  mov        dword ptr [esp + 0x70], ecx
0042a13a  fadd       dword ptr [esp + 0x24]
0042a13e  fstp       dword ptr [esp + 0x18]
0042a142  mov        eax, dword ptr [esp + 0x18]
0042a146  fld        dword ptr [esp + 0x50]
0042a14a  mov        dword ptr [esp + 0x74], eax
0042a14e  fadd       dword ptr [esp + 0x28]
0042a152  fstp       dword ptr [esp + 0x1c]
0042a156  mov        ecx, dword ptr [esp + 0x1c]
0042a15a  mov        dword ptr [esp + 0x78], ecx
0042a15e  cmp        dword ptr [edi], 0x100
0042a164  jge        0x42a1e0

// block 0042a166
0042a166  inc        dword ptr [edx + 0x46c]
0042a16c  fstp       st(0)
0042a16e  cmp        dword ptr [edx + 0x46c], 0x10000
0042a178  lea        esi, [edx + 0x46c]
0042a17e  jge        0x42a186

// block 0042a180
0042a180  mov        dword ptr [esi], 0x10000

// block 0042a186
0042a186  push       0xfa4
0042a18b  call       0x46c9ea

// block 0042a190
0042a190  add        esp, 4
0042a193  test       eax, eax
0042a195  je         0x42a19e

// block 0042a197
0042a197  call       0x428520

// block 0042a19c
0042a19c  jmp        0x42a1a0

// block 0042a19e
0042a19e  xor        eax, eax

// block 0042a1a0
0042a1a0  mov        edx, dword ptr [esi]
0042a1a2  mov        dword ptr [eax + 0x80], edx
0042a1a8  mov        edx, dword ptr [esp + 0x30]
0042a1ac  mov        ecx, dword ptr [edx + 0x464]
0042a1b2  mov        dword ptr [eax + 4], ecx
0042a1b5  mov        dword ptr [ecx + 8], eax
0042a1b8  inc        dword ptr [edi]
0042a1ba  mov        dword ptr [edx + 0x464], eax
0042a1c0  mov        edx, dword ptr [eax]
0042a1c2  mov        edx, dword ptr [edx + 4]
0042a1c5  lea        ecx, [esp + 0x70]
0042a1c9  push       ecx
0042a1ca  mov        ecx, eax
0042a1cc  call       edx

// block 0042a1ce
0042a1ce  fld        dword ptr [0x4a3e00]
0042a1d4  mov        edx, dword ptr [0x4b44f4]
0042a1da  jmp        0x42a1e0

// block 0042a1dc
0042a1dc  fstp       st(0)
0042a1de  fstp       st(0)

// block 0042a1e0
0042a1e0  cmp        ebx, dword ptr [esp + 0x2c]
0042a1e4  jl         0x42a06a

// block 0042a1ea
0042a1ea  jmp        0x429fee
