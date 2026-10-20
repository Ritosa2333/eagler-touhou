
// block 00438e80
00438e80  push       ebp
00438e81  mov        ebp, esp
00438e83  and        esp, 0xfffffff8
00438e86  sub        esp, 0x14
00438e89  cmp        dword ptr [edi + 0xd0], 0
00438e90  push       esi
00438e91  mov        esi, dword ptr [0x4b4514]
00438e97  jne        0x438fe6

// block 00438e9d
00438e9d  cmp        dword ptr [esi + 0xc598], 0
00438ea4  jne        0x438fe6

// block 00438eaa
00438eaa  fld        dword ptr [esi + 0x9a0]
00438eb0  fabs       
00438eb2  fstp       dword ptr [esp + 4]
00438eb6  fld        dword ptr [esp + 4]
00438eba  fld        dword ptr [0x4a3fe8]
00438ec0  fcom       st(1)
00438ec2  fnstsw     ax
00438ec4  fstp       st(1)
00438ec6  test       ah, 5
00438ec9  jnp        0x438ee8

// block 00438ecb
00438ecb  fld        dword ptr [esi + 0x9a4]
00438ed1  fabs       
00438ed3  fstp       dword ptr [esp + 4]
00438ed7  fcomp      dword ptr [esp + 4]
00438edb  fnstsw     ax
00438edd  test       ah, 5
00438ee0  jp         0x438fe6

// block 00438ee6
00438ee6  jmp        0x438eea

// block 00438ee8
00438ee8  fstp       st(0)

// block 00438eea
00438eea  fld        dword ptr [esi + 0x9b0]
00438ef0  fld        dword ptr [esi + 0x9ac]
00438ef6  call       0x4937aa

// block 00438efb
00438efb  fstp       dword ptr [esp + 4]
00438eff  fld        dword ptr [esp + 4]
00438f03  sub        esp, 8
00438f06  fstp       dword ptr [esp + 0xc]
00438f0a  fld        dword ptr [esi + 0xc48c]
00438f10  fstp       dword ptr [esp + 4]
00438f14  fld        dword ptr [esp + 0xc]
00438f18  fstp       dword ptr [esp]
00438f1b  call       0x42e770

// block 00438f20
00438f20  push       ecx
00438f21  fstp       dword ptr [esp]
00438f24  call       0x4646e0

// block 00438f29
00438f29  fabs       
00438f2b  fstp       dword ptr [esp + 8]
00438f2f  fld        dword ptr [esp + 8]
00438f33  fcomp      qword ptr [0x4a3fe0]
00438f39  fnstsw     ax
00438f3b  test       ah, 5
00438f3e  jp         0x438f49

// block 00438f40
00438f40  fld        dword ptr [esp + 4]
00438f44  jmp        0x438fe0

// block 00438f49
00438f49  fld        dword ptr [esi + 0xc48c]
00438f4f  sub        esp, 8
00438f52  fstp       dword ptr [esp + 4]
00438f56  fld        dword ptr [esp + 0xc]
00438f5a  fstp       dword ptr [esp]
00438f5d  call       0x42e770

// block 00438f62
00438f62  push       ecx
00438f63  fstp       dword ptr [esp]
00438f66  call       0x4646e0

// block 00438f6b
00438f6b  fstp       dword ptr [esp + 8]
00438f6f  push       ecx
00438f70  fld        dword ptr [esp + 0xc]
00438f74  fld        st(0)
00438f76  fabs       
00438f78  fstp       dword ptr [esp + 0xc]
00438f7c  fld        dword ptr [esp + 0xc]
00438f80  fcomp      qword ptr [0x4a3fc0]
00438f86  fnstsw     ax
00438f88  test       ah, 5
00438f8b  jp         0x438fb3

// block 00438f8d
00438f8d  fdiv       qword ptr [0x4a3fd8]
00438f93  fstp       dword ptr [esp + 0xc]
00438f97  fld        dword ptr [esp + 0xc]
00438f9b  fstp       dword ptr [esp]
00438f9e  call       0x4646e0

// block 00438fa3
00438fa3  fadd       dword ptr [esi + 0xc48c]
00438fa9  fstp       dword ptr [esp + 8]
00438fad  fld        dword ptr [esp + 8]
00438fb1  jmp        0x438fd7

// block 00438fb3
00438fb3  fdiv       qword ptr [0x4a3fd0]
00438fb9  fstp       dword ptr [esp + 0xc]
00438fbd  fld        dword ptr [esp + 0xc]
00438fc1  fstp       dword ptr [esp]
00438fc4  call       0x4646e0

// block 00438fc9
00438fc9  fadd       dword ptr [esi + 0xc48c]
00438fcf  fstp       dword ptr [esp + 8]
00438fd3  fld        dword ptr [esp + 8]

// block 00438fd7
00438fd7  push       ecx
00438fd8  fstp       dword ptr [esp]
00438fdb  call       0x4646e0

// block 00438fe0
00438fe0  fstp       dword ptr [esi + 0xc48c]

// block 00438fe6
00438fe6  mov        eax, dword ptr [esi + 0xc598]
00438fec  fld        dword ptr [edi + eax*8 + 0x1c]
00438ff0  push       ecx
00438ff1  fadd       dword ptr [esi + 0xc48c]
00438ff7  fstp       dword ptr [esp + 0xc]
00438ffb  fld        dword ptr [esp + 0xc]
00438fff  fstp       dword ptr [esp]
00439002  call       0x4646e0

// block 00439007
00439007  fstp       dword ptr [esp + 8]
0043900b  sub        esp, 8
0043900e  fld        dword ptr [esp + 0x10]
00439012  fst        dword ptr [edi + 0xa8]
00439018  mov        ecx, dword ptr [esi + 0xc598]
0043901e  fld        dword ptr [edi + ecx*8 + 0x20]
00439022  lea        ecx, [esp + 0x14]
00439026  fstp       dword ptr [esp + 4]
0043902a  fstp       dword ptr [esp]
0043902d  call       0x4393d0

// block 00439032
00439032  fld        dword ptr [esp + 0xc]
00439036  fld        qword ptr [0x4a3df8]
0043903c  fmul       st(1), st(0)
0043903e  fxch       st(1)
00439040  call       0x4931e0

// block 00439045
00439045  fmul       dword ptr [esp + 0x10]
00439049  mov        esi, dword ptr [0x4b4514]
0043904f  mov        edx, dword ptr [esi + 0x988]
00439055  sub        edx, eax
00439057  mov        dword ptr [edi + 0x54], edx
0043905a  call       0x4931e0

// block 0043905f
0043905f  mov        ecx, dword ptr [esi + 0x98c]
00439065  sub        ecx, eax
00439067  mov        dword ptr [edi + 0x58], ecx
0043906a  pop        esi
0043906b  mov        esp, ebp
0043906d  pop        ebp
0043906e  ret        
