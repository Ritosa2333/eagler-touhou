
// block 00405c90
00405c90  sub        esp, 0xd4
00405c96  mov        eax, dword ptr [ebx + 0x84]
00405c9c  push       esi
00405c9d  push       edi
00405c9e  xor        edi, edi
00405ca0  cmp        eax, edi
00405ca2  mov        dword ptr [esp + 8], eax
00405ca6  jle        0x405d31

// block 00405cac
00405cac  lea        esi, [ebx + 0x70]
00405caf  call       0x464a80

// block 00405cb4
00405cb4  mov        ecx, dword ptr [ebx + 0x84]
00405cba  cmp        dword ptr [ebx + 0x74], ecx
00405cbd  mov        dword ptr [esp + 8], ecx
00405cc1  jl         0x405d31

// block 00405cc3
00405cc3  mov        eax, dword ptr [esi + 0x10]
00405cc6  mov        dword ptr [esp + 8], ecx
00405cca  test       al, 1
00405ccc  jne        0x405ce9

// block 00405cce
00405cce  fldz       
00405cd0  or         eax, 1
00405cd3  fstp       dword ptr [esi + 8]
00405cd6  mov        dword ptr [esi + 4], edi
00405cd9  mov        dword ptr [esi], 0xfff0bdc1
00405cdf  mov        dword ptr [esi + 0xc], 0x4b2ed0
00405ce6  mov        dword ptr [esi + 0x10], eax

// block 00405ce9
00405ce9  fild       dword ptr [esp + 8]
00405ced  mov        eax, dword ptr [esp + 0xe0]
00405cf4  mov        dword ptr [esi + 4], ecx
00405cf7  dec        ecx
00405cf8  mov        dword ptr [esi], ecx
00405cfa  fstp       dword ptr [esi + 8]
00405cfd  mov        ecx, 7
00405d02  mov        dword ptr [ebx + 0x84], edi
00405d08  mov        edi, eax
00405d0a  cmp        dword ptr [ebx + 0x88], ecx
00405d10  je         0x405d22

// block 00405d12
00405d12  lea        esi, [ebx + 0x1c]

// block 00405d15
00405d15  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405d17
00405d17  pop        edi
00405d18  pop        esi
00405d19  add        esp, 0xd4
00405d1f  ret        4

// block 00405d22
00405d22  mov        esi, ebx

// block 00405d24
00405d24  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405d26
00405d26  pop        edi
00405d27  pop        esi
00405d28  add        esp, 0xd4
00405d2e  ret        4

// block 00405d31
00405d31  mov        eax, dword ptr [ebx + 0x88]
00405d37  cmp        eax, 7
00405d3a  jne        0x405da7

// block 00405d3c
00405d3c  fld        dword ptr [ebx + 0x1c]
00405d3f  mov        ecx, eax
00405d41  mov        esi, ebx
00405d43  lea        edi, [esp + 0x2c]

// block 00405d47
00405d47  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405d49
00405d49  fadd       dword ptr [esp + 0x2c]
00405d4d  fstp       dword ptr [esp + 0x10]
00405d51  fld        dword ptr [ebx + 0x20]
00405d54  fadd       dword ptr [esp + 0x30]
00405d58  fstp       dword ptr [esp + 0x14]
00405d5c  fld        dword ptr [ebx + 0x24]
00405d5f  fadd       dword ptr [esp + 0x34]
00405d63  lea        ecx, [esp + 0x10]
00405d67  fstp       dword ptr [esp + 0x18]
00405d6b  fld        dword ptr [ebx + 0x28]
00405d6e  fadd       dword ptr [esp + 0x38]
00405d72  fstp       dword ptr [esp + 0x1c]
00405d76  fld        dword ptr [ebx + 0x2c]
00405d79  fadd       dword ptr [esp + 0x3c]
00405d7d  fstp       dword ptr [esp + 0x20]
00405d81  fld        dword ptr [ebx + 0x30]
00405d84  fadd       dword ptr [esp + 0x40]
00405d88  fstp       dword ptr [esp + 0x24]
00405d8c  call       0x406250

// block 00405d91
00405d91  mov        ecx, 7
00405d96  lea        esi, [esp + 0x10]
00405d9a  mov        edi, ebx

// block 00405d9c
00405d9c  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405d9e
00405d9e  lea        esi, [esp + 0x10]
00405da2  jmp        0x405f56

// block 00405da7
00405da7  push       ebp
00405da8  cmp        eax, 0x11
00405dab  jne        0x405e3b

// block 00405db1
00405db1  fld        dword ptr [ebx + 0x54]
00405db4  lea        ebp, [ebx + 0x54]
00405db7  mov        ecx, 7
00405dbc  mov        esi, ebx
00405dbe  lea        edi, [esp + 0x30]

// block 00405dc2
00405dc2  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405dc4
00405dc4  fadd       dword ptr [esp + 0x30]
00405dc8  fstp       dword ptr [esp + 0x14]
00405dcc  fld        dword ptr [ebp + 4]
00405dcf  fadd       dword ptr [esp + 0x34]
00405dd3  fstp       dword ptr [esp + 0x18]
00405dd7  fld        dword ptr [ebp + 8]
00405dda  fadd       dword ptr [esp + 0x38]
00405dde  lea        ecx, [esp + 0x14]
00405de2  fstp       dword ptr [esp + 0x1c]
00405de6  fld        dword ptr [ebp + 0xc]
00405de9  fadd       dword ptr [esp + 0x3c]
00405ded  fstp       dword ptr [esp + 0x20]
00405df1  fld        dword ptr [ebp + 0x10]
00405df4  fadd       dword ptr [esp + 0x40]
00405df8  fstp       dword ptr [esp + 0x24]
00405dfc  fld        dword ptr [ebp + 0x14]
00405dff  fadd       dword ptr [esp + 0x44]
00405e03  fstp       dword ptr [esp + 0x28]
00405e07  call       0x406250

// block 00405e0c
00405e0c  mov        ecx, 7
00405e11  lea        esi, [esp + 0x14]
00405e15  mov        edi, ebx

// block 00405e17
00405e17  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405e19
00405e19  lea        eax, [ebx + 0x1c]
00405e1c  lea        esi, [esp + 0x30]
00405e20  mov        ecx, ebp
00405e22  call       0x406110

// block 00405e27
00405e27  mov        ecx, 7
00405e2c  mov        esi, eax
00405e2e  mov        edi, ebp

// block 00405e30
00405e30  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405e32
00405e32  lea        esi, [esp + 0x14]
00405e36  jmp        0x405f55

// block 00405e3b
00405e3b  cmp        eax, 8
00405e3e  jne        0x405f19

// block 00405e44
00405e44  fld        dword ptr [ebx + 0x78]
00405e47  push       ecx
00405e48  fidiv      dword ptr [esp + 0x10]
00405e4c  lea        eax, [ebx + 0x54]
00405e4f  lea        esi, [esp + 0x34]
00405e53  fstp       dword ptr [esp + 0x10]
00405e57  fld        dword ptr [esp + 0x10]
00405e5b  fld        st(0)
00405e5d  fld1       
00405e5f  fsub       st(1), st(0)
00405e61  fld        st(1)
00405e63  fld        st(3)
00405e65  fadd       st(0), st(0)
00405e67  fld        st(1)
00405e69  fmulp      st(2)
00405e6b  fld        st(0)
00405e6d  fadd       st(3)
00405e6f  fmulp      st(2)
00405e71  fxch       st(1)
00405e73  fstp       dword ptr [esp + 0x14]
00405e77  fld        st(3)
00405e79  fmul       st(0), st(0)
00405e7b  fld        qword ptr [0x4a3fd8]
00405e81  fsubrp     st(2)
00405e83  fmulp      st(1)
00405e85  fstp       dword ptr [esp + 0x50]
00405e89  fsub       st(2)
00405e8b  fmul       st(0), st(0)
00405e8d  fmul       st(2)
00405e8f  fstp       dword ptr [esp + 0x54]
00405e93  fmul       st(1)
00405e95  fmulp      st(1)
00405e97  fstp       dword ptr [esp + 0x10]
00405e9b  fld        dword ptr [esp + 0x10]
00405e9f  fstp       dword ptr [esp]
00405ea2  call       0x4060d0

// block 00405ea7
00405ea7  fld        dword ptr [esp + 0x50]
00405eab  mov        edi, eax
00405ead  push       ecx
00405eae  lea        eax, [ebx + 0x38]
00405eb1  fstp       dword ptr [esp]
00405eb4  lea        esi, [esp + 0x18]
00405eb8  call       0x4060d0

// block 00405ebd
00405ebd  fld        dword ptr [esp + 0x4c]
00405ec1  mov        ebp, eax
00405ec3  push       ecx
00405ec4  lea        eax, [ebx + 0x1c]
00405ec7  fstp       dword ptr [esp]
00405eca  lea        esi, [esp + 0xc8]
00405ed1  call       0x4060d0

// block 00405ed6
00405ed6  fld        dword ptr [esp + 0x10]
00405eda  push       ecx
00405edb  mov        dword ptr [esp + 0x14], eax
00405edf  fstp       dword ptr [esp]
00405ee2  lea        esi, [esp + 0xac]
00405ee9  mov        eax, ebx
00405eeb  call       0x4060d0

// block 00405ef0
00405ef0  mov        ecx, eax
00405ef2  mov        eax, dword ptr [esp + 0x10]
00405ef6  lea        esi, [esp + 0x70]
00405efa  call       0x406110

// block 00405eff
00405eff  mov        ecx, eax
00405f01  mov        eax, ebp
00405f03  lea        esi, [esp + 0x8c]
00405f0a  call       0x406110

// block 00405f0f
00405f0f  mov        ecx, eax
00405f11  mov        eax, edi
00405f13  lea        esi, [esp + 0x54]
00405f17  jmp        0x405f4e

// block 00405f19
00405f19  mov        eax, ebx
00405f1b  call       0x406050

// block 00405f20
00405f20  fstp       dword ptr [esp + 0x10]
00405f24  fld        dword ptr [esp + 0x10]
00405f28  push       ecx
00405f29  lea        ecx, [ebx + 0x1c]
00405f2c  fstp       dword ptr [esp]
00405f2f  mov        eax, ebx
00405f31  lea        esi, [esp + 0x58]
00405f35  call       0x406090

// block 00405f3a
00405f3a  lea        esi, [esp + 0x90]
00405f41  call       0x4060d0

// block 00405f46
00405f46  mov        ecx, eax
00405f48  mov        eax, ebx
00405f4a  lea        esi, [esp + 0x70]

// block 00405f4e
00405f4e  call       0x406110

// block 00405f53
00405f53  mov        esi, eax

// block 00405f55
00405f55  pop        ebp

// block 00405f56
00405f56  mov        eax, dword ptr [esp + 0xe0]
00405f5d  mov        edi, eax
00405f5f  mov        ecx, 7

// block 00405f64
00405f64  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00405f66
00405f66  pop        edi
00405f67  pop        esi
00405f68  add        esp, 0xd4
00405f6e  ret        4
