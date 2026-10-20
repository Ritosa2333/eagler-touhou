
// block 004099e0
004099e0  sub        esp, 0x18
004099e3  push       ebx
004099e4  push       ebp
004099e5  mov        ebp, dword ptr [esp + 0x24]
004099e9  test       byte ptr [ebp], 8
004099ed  push       esi
004099ee  push       edi
004099ef  je         0x409a05

// block 004099f1
004099f1  mov        eax, ebp
004099f3  call       0x409350

// block 004099f8
004099f8  or         eax, 0xffffffff
004099fb  pop        edi
004099fc  pop        esi
004099fd  pop        ebp
004099fe  pop        ebx
004099ff  add        esp, 0x18
00409a02  ret        4

// block 00409a05
00409a05  movzx      eax, word ptr [ebp + 0x532]
00409a0c  sub        eax, 1
00409a0f  je         0x409c9a

// block 00409a15
00409a15  sub        eax, 1
00409a18  je         0x409aac

// block 00409a1e
00409a1e  sub        eax, 1
00409a21  jne        0x409f6e

// block 00409a27
00409a27  fld        dword ptr [ebp + 0x4c8]
00409a2d  fld        dword ptr [0x4b2ed0]
00409a33  fld        st(0)
00409a35  fmulp      st(2)
00409a37  fxch       st(1)
00409a39  fstp       dword ptr [esp + 0x10]
00409a3d  fld        dword ptr [ebp + 0x4cc]
00409a43  fmul       st(1)
00409a45  fstp       dword ptr [esp + 0x14]
00409a49  fmul       dword ptr [ebp + 0x4d0]
00409a4f  fstp       dword ptr [esp + 0x18]
00409a53  fld        dword ptr [esp + 0x10]
00409a57  fld        qword ptr [0x4a3cf8]
00409a5d  fmul       st(1), st(0)
00409a5f  fxch       st(1)
00409a61  fstp       dword ptr [esp + 0x1c]
00409a65  fld        dword ptr [esp + 0x14]
00409a69  fmul       st(1)
00409a6b  fstp       dword ptr [esp + 0x20]
00409a6f  fmul       dword ptr [esp + 0x18]
00409a73  fstp       dword ptr [esp + 0x24]
00409a77  fld        dword ptr [esp + 0x1c]
00409a7b  fadd       dword ptr [ebp + 0x4bc]
00409a81  fstp       dword ptr [ebp + 0x4bc]
00409a87  fld        dword ptr [esp + 0x20]
00409a8b  fadd       dword ptr [ebp + 0x4c0]
00409a91  fstp       dword ptr [ebp + 0x4c0]
00409a97  fld        dword ptr [esp + 0x24]
00409a9b  fadd       dword ptr [ebp + 0x4c4]
00409aa1  fstp       dword ptr [ebp + 0x4c4]
00409aa7  jmp        0x409f6e

// block 00409aac
00409aac  fld        dword ptr [ebp + 0x4c8]
00409ab2  lea        ebx, [ebp + 0x4bc]
00409ab8  fld        dword ptr [0x4b2ed0]
00409abe  fld        st(0)
00409ac0  fmulp      st(2)
00409ac2  fxch       st(1)
00409ac4  fstp       dword ptr [esp + 0x1c]
00409ac8  fld        dword ptr [ebp + 0x4cc]
00409ace  fmul       st(1)
00409ad0  fstp       dword ptr [esp + 0x20]
00409ad4  fmul       dword ptr [ebp + 0x4d0]
00409ada  fstp       dword ptr [esp + 0x24]
00409ade  fld        dword ptr [esp + 0x1c]
00409ae2  fld        qword ptr [0x4a3cf8]
00409ae8  fmul       st(1), st(0)
00409aea  fxch       st(1)
00409aec  fstp       dword ptr [esp + 0x10]
00409af0  fld        dword ptr [esp + 0x20]
00409af4  fmul       st(1)
00409af6  fstp       dword ptr [esp + 0x14]
00409afa  fmul       dword ptr [esp + 0x24]
00409afe  fstp       dword ptr [esp + 0x18]
00409b02  fld        dword ptr [ebx]
00409b04  fadd       dword ptr [esp + 0x10]
00409b08  fstp       dword ptr [ebx]
00409b0a  fld        dword ptr [esp + 0x14]
00409b0e  fadd       dword ptr [ebx + 4]
00409b11  fstp       dword ptr [ebx + 4]
00409b14  fld        dword ptr [esp + 0x18]
00409b18  fadd       dword ptr [ebx + 8]
00409b1b  fstp       dword ptr [ebx + 8]
00409b1e  cmp        dword ptr [ebp + 0x4e8], 8
00409b25  jl         0x409c81

// block 00409b2b
00409b2b  mov        eax, dword ptr [ebp]
00409b2e  test       al, 2
00409b30  je         0x409c81

// block 00409b36
00409b36  test       al, 0x10
00409b38  jne        0x409b49

// block 00409b3a
00409b3a  lea        eax, [ebp + 0x4dc]
00409b40  mov        ecx, ebx
00409b42  call       0x437810

// block 00409b47
00409b47  jmp        0x409b5a

// block 00409b49
00409b49  fld        dword ptr [ebp + 0x4dc]
00409b4f  push       ecx
00409b50  mov        eax, ebx
00409b52  fstp       dword ptr [esp]
00409b55  call       0x437980

// block 00409b5a
00409b5a  cmp        eax, 1
00409b5d  jne        0x409c6c

// block 00409b63
00409b63  mov        eax, 3
00409b68  lea        esi, [ebp + 8]
00409b6b  lea        edi, [eax - 2]
00409b6e  mov        word ptr [ebp + 0x532], ax
00409b75  call       0x40d6e0

// block 00409b7a
00409b7a  mov        eax, dword ptr [ebp + 0x524]
00409b80  mov        dword ptr [esp + 0x2c], eax
00409b84  test       eax, eax
00409b86  jl         0x409f6e

// block 00409b8c
00409b8c  test       dword ptr [0x4cee78], 0x8000
00409b96  mov        ecx, dword ptr [0x4b43c8]
00409b9c  mov        edi, dword ptr [ecx + 0x4debdc]
00409ba2  je         0x409bb5

// block 00409ba4
00409ba4  push       0x4cf1d0
00409ba9  call       dword ptr [0x498088]

// block 00409baf
00409baf  inc        byte ptr [0x4cf221]

// block 00409bb5
00409bb5  inc        dword ptr [edi + 0x130]
00409bbb  call       0x4621c0

// block 00409bc0
00409bc0  mov        esi, eax
00409bc2  or         dword ptr [esi + 0x480], 1
00409bc9  mov        dword ptr [esi + 0x20], 0x17
00409bd0  test       ebx, ebx
00409bd2  jne        0x409c02

// block 00409bd4
00409bd4  fldz       
00409bd6  fst        dword ptr [esp + 0x1c]
00409bda  mov        edx, dword ptr [esp + 0x1c]
00409bde  fst        dword ptr [esp + 0x20]
00409be2  mov        eax, dword ptr [esp + 0x20]
00409be6  fstp       dword ptr [esp + 0x24]
00409bea  mov        ecx, dword ptr [esp + 0x24]
00409bee  mov        dword ptr [esi + 0x430], edx
00409bf4  mov        dword ptr [esi + 0x434], eax
00409bfa  mov        dword ptr [esi + 0x438], ecx
00409c00  jmp        0x409c2e

// block 00409c02
00409c02  fld        dword ptr [ebx]
00409c04  fadd       qword ptr [0x4a3d70]
00409c0a  fadd       qword ptr [0x4a3d28]
00409c10  fstp       dword ptr [esi + 0x430]
00409c16  fld        dword ptr [ebx + 4]
00409c19  fadd       qword ptr [0x4a3d68]
00409c1f  fstp       dword ptr [esi + 0x434]
00409c25  fld        dword ptr [ebx + 8]
00409c28  fstp       dword ptr [esi + 0x438]

// block 00409c2e
00409c2e  mov        edx, dword ptr [esp + 0x2c]
00409c32  push       edx

// block 00409c33
00409c33  push       esi
00409c34  mov        ecx, edi
00409c36  call       0x454d10

// block 00409c3b
00409c3b  mov        ebx, esi
00409c3d  lea        eax, [esp + 0x2c]
00409c41  call       0x461250

// block 00409c46
00409c46  test       dword ptr [0x4cee78], 0x8000
00409c50  je         0x409f6e

// block 00409c56
00409c56  push       0x4cf1d0
00409c5b  call       dword ptr [0x49808c]

// block 00409c61
00409c61  dec        byte ptr [0x4cf221]
00409c67  jmp        0x409f6e

// block 00409c6c
00409c6c  cmp        eax, 2
00409c6f  jne        0x409c81

// block 00409c71
00409c71  test       byte ptr [ebp], 4
00409c75  jne        0x409c81

// block 00409c77
00409c77  push       ebx
00409c78  call       0x4391c0

// block 00409c7d
00409c7d  or         dword ptr [ebp], 4

// block 00409c81
00409c81  cmp        dword ptr [ebp + 0x404], 0
00409c88  je         0x409f6e

// block 00409c8e
00409c8e  mov        eax, 1
00409c93  mov        word ptr [ebp + 0x532], ax

// block 00409c9a
00409c9a  mov        ecx, ebp
00409c9c  call       0x40aa60

// block 00409ca1
00409ca1  mov        eax, dword ptr [ebp + 0x528]
00409ca7  test       eax, eax
00409ca9  je         0x409de2

// block 00409caf
00409caf  xor        ebx, ebx
00409cb1  test       al, 1
00409cb3  je         0x409cbe

// block 00409cb5
00409cb5  mov        edx, ebp
00409cb7  call       0x40b670

// block 00409cbc
00409cbc  mov        ebx, eax

// block 00409cbe
00409cbe  test       byte ptr [ebp + 0x528], 4
00409cc5  je         0x409cd0

// block 00409cc7
00409cc7  mov        eax, ebp
00409cc9  call       0x40b6f0

// block 00409cce
00409cce  add        ebx, eax

// block 00409cd0
00409cd0  test       dword ptr [ebp + 0x528], 0x20000000
00409cda  je         0x409ce5

// block 00409cdc
00409cdc  mov        eax, ebp
00409cde  call       0x40b800

// block 00409ce3
00409ce3  add        ebx, eax

// block 00409ce5
00409ce5  test       byte ptr [ebp + 0x528], 8
00409cec  je         0x409cf7

// block 00409cee
00409cee  mov        eax, ebp
00409cf0  call       0x40b920

// block 00409cf5
00409cf5  add        ebx, eax

// block 00409cf7
00409cf7  test       byte ptr [ebp + 0x528], 0x10
00409cfe  je         0x409d09

// block 00409d00
00409d00  mov        eax, ebp
00409d02  call       0x40b9c0

// block 00409d07
00409d07  add        ebx, eax

// block 00409d09
00409d09  test       byte ptr [ebp + 0x528], 0x40
00409d10  je         0x409d1b

// block 00409d12
00409d12  mov        eax, ebp
00409d14  call       0x40baf0

// block 00409d19
00409d19  add        ebx, eax

// block 00409d1b
00409d1b  test       byte ptr [ebp + 0x528], 0x20
00409d22  je         0x409d2d

// block 00409d24
00409d24  mov        eax, ebp
00409d26  call       0x40bc20

// block 00409d2b
00409d2b  add        ebx, eax

// block 00409d2d
00409d2d  test       dword ptr [ebp + 0x528], 0x100
00409d37  je         0x409d42

// block 00409d39
00409d39  mov        eax, ebp
00409d3b  call       0x40bef0

// block 00409d40
00409d40  add        ebx, eax

// block 00409d42
00409d42  test       dword ptr [ebp + 0x528], 0x800000
00409d4c  je         0x409d57

// block 00409d4e
00409d4e  mov        eax, ebp
00409d50  call       0x40c170

// block 00409d55
00409d55  add        ebx, eax

// block 00409d57
00409d57  test       dword ptr [ebp + 0x528], 0x2000000
00409d61  je         0x409d6c

// block 00409d63
00409d63  mov        eax, ebp
00409d65  call       0x40c230

// block 00409d6a
00409d6a  add        ebx, eax

// block 00409d6c
00409d6c  test       dword ptr [ebp + 0x528], 0x8000000
00409d76  je         0x409d81

// block 00409d78
00409d78  mov        eax, ebp
00409d7a  call       0x40c3b0

// block 00409d7f
00409d7f  add        ebx, eax

// block 00409d81
00409d81  test       dword ptr [ebp + 0x528], 0x400
00409d8b  je         0x409d96

// block 00409d8d
00409d8d  mov        edi, ebp
00409d8f  call       0x40c480

// block 00409d94
00409d94  add        ebx, eax

// block 00409d96
00409d96  mov        eax, dword ptr [ebp + 0x528]
00409d9c  test       eax, 0x1000
00409da1  je         0x409dcf

// block 00409da3
00409da3  cmp        dword ptr [ebp + 0x808], 0
00409daa  jg         0x409dba

// block 00409dac
00409dac  xor        eax, 0x1000
00409db1  mov        dword ptr [ebp + 0x528], eax
00409db7  inc        ebx
00409db8  jmp        0x409dcf

// block 00409dba
00409dba  fld        dword ptr [0x4a3da8]
00409dc0  push       ecx
00409dc1  lea        esi, [ebp + 0x804]
00409dc7  fstp       dword ptr [esp]
00409dca  call       0x464a20

// block 00409dcf
00409dcf  mov        eax, dword ptr [ebp + 4]
00409dd2  test       eax, eax
00409dd4  je         0x409dda

// block 00409dd6
00409dd6  dec        eax
00409dd7  mov        dword ptr [ebp + 4], eax

// block 00409dda
00409dda  test       ebx, ebx
00409ddc  jne        0x409c9a

// block 00409de2
00409de2  fld        dword ptr [ebp + 0x4c8]
00409de8  lea        ebx, [ebp + 0x4bc]
00409dee  fld        dword ptr [0x4b2ed0]
00409df4  fld        st(0)
00409df6  fmulp      st(2)
00409df8  fxch       st(1)
00409dfa  fstp       dword ptr [esp + 0x1c]
00409dfe  fld        dword ptr [ebp + 0x4cc]
00409e04  fmul       st(1)
00409e06  fstp       dword ptr [esp + 0x20]
00409e0a  fmul       dword ptr [ebp + 0x4d0]
00409e10  fstp       dword ptr [esp + 0x24]
00409e14  fld        dword ptr [ebx]
00409e16  fadd       dword ptr [esp + 0x1c]
00409e1a  fstp       dword ptr [ebx]
00409e1c  fld        dword ptr [esp + 0x20]
00409e20  fadd       dword ptr [ebx + 4]
00409e23  fstp       dword ptr [ebx + 4]
00409e26  fld        dword ptr [ebx + 8]
00409e29  fadd       dword ptr [esp + 0x24]
00409e2d  fstp       dword ptr [ebx + 8]
00409e30  mov        eax, dword ptr [ebp]
00409e33  test       al, 2
00409e35  je         0x409f6e

// block 00409e3b
00409e3b  test       al, 0x10
00409e3d  jne        0x409e4e

// block 00409e3f
00409e3f  lea        eax, [ebp + 0x4dc]
00409e45  mov        ecx, ebx
00409e47  call       0x437810

// block 00409e4c
00409e4c  jmp        0x409e5f

// block 00409e4e
00409e4e  fld        dword ptr [ebp + 0x4dc]
00409e54  push       ecx
00409e55  mov        eax, ebx
00409e57  fstp       dword ptr [esp]
00409e5a  call       0x437980

// block 00409e5f
00409e5f  cmp        eax, 1
00409e62  jne        0x409f59

// block 00409e68
00409e68  mov        ecx, 3
00409e6d  lea        esi, [ebp + 8]
00409e70  mov        word ptr [ebp + 0x532], cx
00409e77  mov        eax, dword ptr [esi + 0x494]
00409e7d  test       eax, eax
00409e7f  je         0x409e88

// block 00409e81
00409e81  lea        edx, [ecx - 2]
00409e84  mov        ecx, esi
00409e86  call       eax

// block 00409e88
00409e88  mov        edx, 1
00409e8d  mov        word ptr [esi + 0x3c4], dx
00409e94  mov        eax, dword ptr [ebp + 0x524]
00409e9a  mov        dword ptr [esp + 0x2c], eax
00409e9e  test       eax, eax
00409ea0  jl         0x409f6e

// block 00409ea6
00409ea6  test       dword ptr [0x4cee78], 0x8000
00409eb0  mov        eax, dword ptr [0x4b43c8]
00409eb5  mov        edi, dword ptr [eax + 0x4debdc]
00409ebb  je         0x409ece

// block 00409ebd
00409ebd  push       0x4cf1d0
00409ec2  call       dword ptr [0x498088]

// block 00409ec8
00409ec8  inc        byte ptr [0x4cf221]

// block 00409ece
00409ece  inc        dword ptr [edi + 0x130]
00409ed4  call       0x4621c0

// block 00409ed9
00409ed9  mov        esi, eax
00409edb  or         dword ptr [esi + 0x480], 1
00409ee2  mov        dword ptr [esi + 0x20], 0x17
00409ee9  test       ebx, ebx
00409eeb  jne        0x409f23

// block 00409eed
00409eed  fldz       
00409eef  fst        dword ptr [esp + 0x1c]
00409ef3  mov        ecx, dword ptr [esp + 0x1c]
00409ef7  fst        dword ptr [esp + 0x20]
00409efb  mov        edx, dword ptr [esp + 0x20]
00409eff  fstp       dword ptr [esp + 0x24]
00409f03  mov        eax, dword ptr [esp + 0x24]
00409f07  mov        dword ptr [esi + 0x430], ecx
00409f0d  mov        ecx, dword ptr [esp + 0x2c]
00409f11  mov        dword ptr [esi + 0x434], edx
00409f17  mov        dword ptr [esi + 0x438], eax
00409f1d  push       ecx
00409f1e  jmp        0x409c33

// block 00409f23
00409f23  fld        dword ptr [ebx]
00409f25  mov        ecx, dword ptr [esp + 0x2c]
00409f29  fadd       qword ptr [0x4a3d70]
00409f2f  push       ecx
00409f30  fadd       qword ptr [0x4a3d28]
00409f36  fstp       dword ptr [esi + 0x430]
00409f3c  fld        dword ptr [ebx + 4]
00409f3f  fadd       qword ptr [0x4a3d68]
00409f45  fstp       dword ptr [esi + 0x434]
00409f4b  fld        dword ptr [ebx + 8]
00409f4e  fstp       dword ptr [esi + 0x438]
00409f54  jmp        0x409c33

// block 00409f59
00409f59  cmp        eax, 2
00409f5c  jne        0x409f6e

// block 00409f5e
00409f5e  test       byte ptr [ebp], 4
00409f62  jne        0x409f6e

// block 00409f64
00409f64  push       ebx
00409f65  call       0x4391c0

// block 00409f6a
00409f6a  or         dword ptr [ebp], 4

// block 00409f6e
00409f6e  cmp        dword ptr [ebp + 0x3fc], 0
00409f75  je         0x409fdb

// block 00409f77
00409f77  test       dword ptr [ebp + 0x528], 0x20000
00409f81  je         0x409f8a

// block 00409f83
00409f83  mov        edi, ebp
00409f85  call       0x40c000

// block 00409f8a
00409f8a  test       dword ptr [ebp + 0x528], 0x40000
00409f94  je         0x409f9d

// block 00409f96
00409f96  mov        edi, ebp
00409f98  call       0x40c0b0

// block 00409f9d
00409f9d  test       dword ptr [ebp + 0x528], 0x400
00409fa7  jne        0x409fdb

// block 00409fa9
00409fa9  cmp        dword ptr [ebp + 0x520], 1
00409fb0  jge        0x409fdb

// block 00409fb2
00409fb2  mov        eax, dword ptr [ebp + 0x3fc]
00409fb8  fld        dword ptr [eax + 0x34]
00409fbb  sub        esp, 8
00409fbe  fstp       dword ptr [esp + 4]
00409fc2  lea        ecx, [ebp + 0x4bc]
00409fc8  fld        dword ptr [eax + 0x38]
00409fcb  fstp       dword ptr [esp]
00409fce  call       0x409900

// block 00409fd3
00409fd3  test       eax, eax
00409fd5  jne        0x4099f1

// block 00409fdb
00409fdb  mov        eax, dword ptr [ebp + 4]
00409fde  test       eax, eax
00409fe0  je         0x409fe6

// block 00409fe2
00409fe2  dec        eax
00409fe3  mov        dword ptr [ebp + 4], eax

// block 00409fe6
00409fe6  mov        eax, dword ptr [ebp + 0x520]
00409fec  test       eax, eax
00409fee  jle        0x409ff7

// block 00409ff0
00409ff0  dec        eax
00409ff1  mov        dword ptr [ebp + 0x520], eax

// block 00409ff7
00409ff7  lea        edx, [ebp + 8]
00409ffa  push       edx
00409ffb  call       0x455630

// block 0040a000
0040a000  test       eax, eax
0040a002  jne        0x4099f1

// block 0040a008
0040a008  pop        edi
0040a009  pop        esi
0040a00a  pop        ebp
0040a00b  pop        ebx
0040a00c  add        esp, 0x18
0040a00f  ret        4
