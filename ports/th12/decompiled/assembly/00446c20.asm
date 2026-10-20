
// block 00446c20
00446c20  mov        eax, dword ptr [edi + 0x24]
00446c23  sub        esp, 0x18
00446c26  sub        eax, 2
00446c29  push       ebx
00446c2a  push       ebp
00446c2b  push       esi
00446c2c  je         0x446ebd

// block 00446c32
00446c32  sub        eax, 2
00446c35  jne        0x4470b0

// block 00446c3b
00446c3b  fld        dword ptr [0x4a3f30]
00446c41  mov        eax, dword ptr [edi + 0x5a78]
00446c47  mov        ecx, dword ptr [edi + eax*4 + 0x5a80]
00446c4e  fstp       dword ptr [esp + 0x18]
00446c52  fld        dword ptr [0x4a3f00]
00446c58  mov        esi, dword ptr [ecx + 0x1c]
00446c5b  fstp       dword ptr [esp + 0x1c]
00446c5f  mov        ebp, 0xa
00446c64  cmp        dword ptr [edi + 0x2b8], ebp
00446c6a  fldz       
00446c6c  fstp       dword ptr [esp + 0x20]
00446c70  mov        dword ptr [esp + 0x14], esi
00446c74  jge        0x446cab

// block 00446c76
00446c76  cdq        
00446c77  fld        dword ptr [edi + 0x2bc]
00446c7d  mov        ecx, 0x19
00446c82  fld        qword ptr [0x4a3e40]
00446c88  idiv       ecx
00446c8a  fld        st(0)
00446c8c  fsubrp     st(2)
00446c8e  mov        eax, edx
00446c90  shl        eax, 4
00446c93  sub        eax, edx
00446c95  mov        dword ptr [esp + 0x10], eax
00446c99  fild       dword ptr [esp + 0x10]
00446c9d  fmulp      st(2)
00446c9f  fdivp      st(1)
00446ca1  fadd       qword ptr [0x4a3f68]
00446ca7  fstp       dword ptr [esp + 0x1c]

// block 00446cab
00446cab  mov        ecx, dword ptr [0x4b43b8]
00446cb1  lea        edx, [esi + 0xc]
00446cb4  push       edx
00446cb5  mov        dword ptr [ecx + 0x18f94], 1
00446cbf  call       0x46d8e4

// block 00446cc4
00446cc4  add        esp, 4
00446cc7  sub        esp, 8
00446cca  cmp        dword ptr [edi + 0x1d8], 0
00446cd1  jne        0x446d2e

// block 00446cd3
00446cd3  mov        ecx, dword ptr [esi + 0x68]
00446cd6  fld        dword ptr [esi + 0x54]
00446cd9  mov        edx, dword ptr [ecx*4 + 0x4aee88]
00446ce0  fstp       qword ptr [esp]
00446ce3  mov        ecx, dword ptr [esi + 0x64]
00446ce6  push       edx
00446ce7  mov        edx, dword ptr [ecx*4 + 0x4aee38]
00446cee  mov        ecx, dword ptr [esi + 0x5c]
00446cf1  push       edx
00446cf2  mov        edx, dword ptr [esi + 0x60]
00446cf5  lea        ecx, [edx + ecx*2]
00446cf8  mov        edx, dword ptr [ecx*4 + 0x4aee20]
00446cff  mov        ecx, dword ptr [eax + 4]
00446d02  push       edx
00446d03  mov        edx, dword ptr [eax + 8]
00446d06  push       ecx
00446d07  mov        ecx, dword ptr [eax + 0xc]
00446d0a  push       edx
00446d0b  mov        edx, dword ptr [eax + 0x10]
00446d0e  mov        eax, dword ptr [eax + 0x14]
00446d11  push       ecx
00446d12  inc        edx
00446d13  push       edx
00446d14  cdq        
00446d15  mov        ecx, 0x64
00446d1a  idiv       ecx
00446d1c  mov        eax, dword ptr [0x4b33b0]
00446d21  push       edx
00446d22  mov        edx, dword ptr [edi + 0x5a78]
00446d28  push       esi
00446d29  inc        edx
00446d2a  push       edx
00446d2b  push       eax
00446d2c  jmp        0x446da5

// block 00446d2e
00446d2e  mov        ecx, dword ptr [edi + 0x5a78]
00446d34  mov        edx, dword ptr [edi + ecx*4 + 0x5a80]
00446d3b  mov        ecx, dword ptr [edx + 0x1e7]
00446d41  mov        dword ptr [0x4d4f30], ecx
00446d47  mov        byte ptr [0x4d4f34], 0
00446d4e  fld        dword ptr [esi + 0x54]
00446d51  mov        edx, dword ptr [esi + 0x68]
00446d54  fstp       qword ptr [esp]
00446d57  mov        ecx, dword ptr [edx*4 + 0x4aee88]
00446d5e  mov        edx, dword ptr [esi + 0x64]
00446d61  push       ecx
00446d62  mov        ecx, dword ptr [edx*4 + 0x4aee38]
00446d69  mov        edx, dword ptr [esi + 0x5c]
00446d6c  push       ecx
00446d6d  mov        ecx, dword ptr [esi + 0x60]
00446d70  lea        edx, [ecx + edx*2]
00446d73  mov        ecx, dword ptr [edx*4 + 0x4aee20]
00446d7a  mov        edx, dword ptr [eax + 4]
00446d7d  push       ecx
00446d7e  mov        ecx, dword ptr [eax + 8]
00446d81  push       edx
00446d82  mov        edx, dword ptr [eax + 0xc]
00446d85  push       ecx
00446d86  mov        ecx, dword ptr [eax + 0x10]
00446d89  mov        eax, dword ptr [eax + 0x14]
00446d8c  push       edx
00446d8d  inc        ecx
00446d8e  push       ecx
00446d8f  cdq        
00446d90  mov        ecx, 0x64
00446d95  idiv       ecx
00446d97  push       edx
00446d98  mov        edx, dword ptr [0x4b33b8]
00446d9e  push       esi
00446d9f  push       0x4d4f30
00446da4  push       edx

// block 00446da5
00446da5  mov        esi, dword ptr [0x4b43b8]
00446dab  lea        ebx, [esp + 0x4c]
00446daf  call       0x4015c0

// block 00446db4
00446db4  add        esp, 0x34
00446db7  cmp        dword ptr [edi + 0x2b8], ebp
00446dbd  jl         0x446e98

// block 00446dc3
00446dc3  fld        dword ptr [0x4a3f64]
00446dc9  mov        ebp, 1
00446dce  fstp       dword ptr [esp + 0x18]
00446dd2  mov        dword ptr [esp + 0xc], 0x24
00446dda  fld        dword ptr [0x4a3db8]
00446de0  fstp       dword ptr [esp + 0x1c]

// block 00446de4
00446de4  mov        ecx, dword ptr [edi + 0x28]
00446de7  mov        esi, dword ptr [0x4b43b8]
00446ded  lea        eax, [ebp - 1]
00446df0  sub        ecx, eax
00446df2  neg        ecx
00446df4  sbb        ecx, ecx
00446df6  and        ecx, 0xff808180
00446dfc  add        ecx, 0xffffff00
00446e02  mov        dword ptr [esi + 0x18f80], ecx
00446e08  mov        edx, dword ptr [edi + 0x5a78]
00446e0e  mov        eax, dword ptr [edi + edx*4 + 0x5a80]
00446e15  mov        ecx, dword ptr [esp + 0xc]
00446e19  add        eax, ecx
00446e1b  cmp        dword ptr [eax + 0xb8], 0
00446e22  jne        0x446e3f

// block 00446e24
00446e24  mov        edx, dword ptr [ebp*4 + 0x4aee60]
00446e2b  push       edx
00446e2c  push       0x4a1f58
00446e31  lea        ebx, [esp + 0x20]
00446e35  call       0x4015c0

// block 00446e3a
00446e3a  add        esp, 8
00446e3d  jmp        0x446e7b

// block 00446e3f
00446e3f  cmp        ebp, 6
00446e42  jge        0x446e56

// block 00446e44
00446e44  mov        eax, dword ptr [eax + 0xdc]
00446e4a  test       eax, eax
00446e4c  je         0x446e56

// block 00446e4e
00446e4e  mov        ecx, dword ptr [eax + 0x38]
00446e51  mov        edx, dword ptr [eax + 0xc]
00446e54  jmp        0x446e60

// block 00446e56
00446e56  mov        eax, dword ptr [esp + 0x14]
00446e5a  mov        ecx, dword ptr [eax + 0x6c]
00446e5d  mov        edx, dword ptr [eax + 0x14]

// block 00446e60
00446e60  mov        eax, dword ptr [ebp*4 + 0x4aee60]
00446e67  push       ecx
00446e68  push       edx
00446e69  push       eax
00446e6a  push       0x4a1f88
00446e6f  lea        ebx, [esp + 0x28]
00446e73  call       0x4015c0

// block 00446e78
00446e78  add        esp, 0x10

// block 00446e7b
00446e7b  fld        dword ptr [esp + 0x1c]
00446e7f  add        dword ptr [esp + 0xc], 0x24
00446e84  fadd       qword ptr [0x4a3f08]
00446e8a  inc        ebp
00446e8b  cmp        ebp, 8
00446e8e  fstp       dword ptr [esp + 0x1c]
00446e92  jl         0x446de4

// block 00446e98
00446e98  mov        eax, dword ptr [0x4b43b8]
00446e9d  mov        dword ptr [eax + 0x18f80], 0xffffffff
00446ea7  mov        dword ptr [eax + 0x18f94], 0
00446eb1  mov        eax, 1
00446eb6  pop        esi
00446eb7  pop        ebp
00446eb8  pop        ebx
00446eb9  add        esp, 0x18
00446ebc  ret        

// block 00446ebd
00446ebd  mov        esi, dword ptr [0x4b43b8]
00446ec3  fld        dword ptr [0x4a3f30]
00446ec9  mov        dword ptr [esi + 0x18f94], 1
00446ed3  fstp       dword ptr [esp + 0x18]
00446ed7  mov        ebp, dword ptr [edi + 0x1d8]
00446edd  fld        dword ptr [0x4a3f00]
00446ee3  imul       ebp, ebp, 0x19
00446ee6  fstp       dword ptr [esp + 0x1c]
00446eea  fldz       
00446eec  fstp       dword ptr [esp + 0x20]
00446ef0  lea        ecx, [ebp + 0x19]
00446ef3  cmp        ebp, ecx
00446ef5  jge        0x44709c

// block 00446efb
00446efb  lea        edx, [ebp + 1]
00446efe  lea        ebx, [edi + ebp*4 + 0x5a80]
00446f05  mov        dword ptr [esp + 0xc], edx
00446f09  mov        dword ptr [esp + 0x10], ebx
00446f0d  jmp        0x446f14

// block 00446f10
00446f10  mov        ebx, dword ptr [esp + 0x10]

// block 00446f14
00446f14  mov        eax, 0x51eb851f
00446f19  imul       ebp
00446f1b  sar        edx, 3
00446f1e  mov        eax, edx
00446f20  shr        eax, 0x1f
00446f23  add        eax, edx
00446f25  mov        edx, dword ptr [edi + 0x28]
00446f28  imul       eax, eax, 0x19
00446f2b  mov        ecx, ebp
00446f2d  sub        ecx, eax
00446f2f  sub        edx, ecx
00446f31  neg        edx
00446f33  sbb        edx, edx
00446f35  and        edx, 0xff808180
00446f3b  add        edx, 0xffffff00
00446f41  mov        dword ptr [esi + 0x18f80], edx
00446f47  mov        eax, dword ptr [ebx]
00446f49  test       eax, eax
00446f4b  je         0x447044

// block 00446f51
00446f51  mov        esi, dword ptr [eax + 0x1c]
00446f54  lea        eax, [esi + 0xc]
00446f57  push       eax
00446f58  call       0x46d8e4

// block 00446f5d
00446f5d  add        esp, 4
00446f60  sub        esp, 8
00446f63  cmp        dword ptr [edi + 0x1d8], 0
00446f6a  jne        0x446fc4

// block 00446f6c
00446f6c  mov        ecx, dword ptr [esi + 0x68]
00446f6f  fld        dword ptr [esi + 0x54]
00446f72  mov        edx, dword ptr [ecx*4 + 0x4aee88]
00446f79  fstp       qword ptr [esp]
00446f7c  mov        ecx, dword ptr [esi + 0x64]
00446f7f  push       edx
00446f80  mov        edx, dword ptr [ecx*4 + 0x4aee38]
00446f87  mov        ecx, dword ptr [esi + 0x5c]
00446f8a  push       edx
00446f8b  mov        edx, dword ptr [esi + 0x60]
00446f8e  lea        ecx, [edx + ecx*2]
00446f91  mov        edx, dword ptr [ecx*4 + 0x4aee20]
00446f98  mov        ecx, dword ptr [eax + 4]
00446f9b  push       edx
00446f9c  mov        edx, dword ptr [eax + 8]
00446f9f  push       ecx
00446fa0  mov        ecx, dword ptr [eax + 0xc]
00446fa3  push       edx
00446fa4  mov        edx, dword ptr [eax + 0x10]
00446fa7  mov        eax, dword ptr [eax + 0x14]
00446faa  push       ecx
00446fab  inc        edx
00446fac  push       edx
00446fad  cdq        
00446fae  mov        ecx, 0x64
00446fb3  idiv       ecx
00446fb5  mov        eax, dword ptr [0x4b33b0]
00446fba  push       edx
00446fbb  mov        edx, dword ptr [esp + 0x34]
00446fbf  push       esi
00446fc0  push       edx
00446fc1  push       eax
00446fc2  jmp        0x447030

// block 00446fc4
00446fc4  mov        ecx, dword ptr [ebx]
00446fc6  mov        edx, dword ptr [ecx + 0x1e7]
00446fcc  mov        dword ptr [0x4d4f30], edx
00446fd2  mov        byte ptr [0x4d4f34], 0
00446fd9  fld        dword ptr [esi + 0x54]
00446fdc  mov        ecx, dword ptr [esi + 0x68]
00446fdf  fstp       qword ptr [esp]
00446fe2  mov        edx, dword ptr [ecx*4 + 0x4aee88]
00446fe9  mov        ecx, dword ptr [esi + 0x64]
00446fec  push       edx
00446fed  mov        edx, dword ptr [ecx*4 + 0x4aee38]
00446ff4  mov        ecx, dword ptr [esi + 0x5c]
00446ff7  push       edx
00446ff8  mov        edx, dword ptr [esi + 0x60]
00446ffb  lea        ecx, [edx + ecx*2]
00446ffe  mov        edx, dword ptr [ecx*4 + 0x4aee20]
00447005  mov        ecx, dword ptr [eax + 4]
00447008  push       edx
00447009  mov        edx, dword ptr [eax + 8]
0044700c  push       ecx
0044700d  mov        ecx, dword ptr [eax + 0xc]
00447010  push       edx
00447011  mov        edx, dword ptr [eax + 0x10]
00447014  mov        eax, dword ptr [eax + 0x14]
00447017  push       ecx
00447018  inc        edx
00447019  push       edx
0044701a  cdq        
0044701b  mov        ecx, 0x64
00447020  idiv       ecx
00447022  push       edx
00447023  mov        edx, dword ptr [0x4b33b8]
00447029  push       esi
0044702a  push       0x4d4f30
0044702f  push       edx

// block 00447030
00447030  mov        esi, dword ptr [0x4b43b8]
00447036  lea        ebx, [esp + 0x4c]
0044703a  call       0x4015c0

// block 0044703f
0044703f  add        esp, 0x34
00447042  jmp        0x44706c

// block 00447044
00447044  mov        eax, dword ptr [esp + 0xc]
00447048  xor        ecx, ecx
0044704a  cmp        dword ptr [edi + 0x1d8], ecx
00447050  push       eax
00447051  setne      cl
00447054  lea        ebx, [esp + 0x1c]
00447058  lea        ecx, [ecx + ecx + 1]
0044705c  mov        edx, dword ptr [ecx*4 + 0x4b33b0]
00447063  push       edx
00447064  call       0x4015c0

// block 00447069
00447069  add        esp, 8

// block 0044706c
0044706c  mov        eax, dword ptr [edi + 0x1d8]
00447072  fld        dword ptr [esp + 0x1c]
00447076  fadd       qword ptr [0x4a3ef8]
0044707c  add        dword ptr [esp + 0x10], 4
00447081  inc        dword ptr [esp + 0xc]
00447085  mov        esi, dword ptr [0x4b43b8]
0044708b  inc        eax
0044708c  fstp       dword ptr [esp + 0x1c]
00447090  imul       eax, eax, 0x19
00447093  inc        ebp
00447094  cmp        ebp, eax
00447096  jl         0x446f10

// block 0044709c
0044709c  mov        dword ptr [esi + 0x18f80], 0xffffffff
004470a6  mov        dword ptr [esi + 0x18f94], 0

// block 004470b0
004470b0  pop        esi
004470b1  pop        ebp
004470b2  mov        eax, 1
004470b7  pop        ebx
004470b8  add        esp, 0x18
004470bb  ret        
