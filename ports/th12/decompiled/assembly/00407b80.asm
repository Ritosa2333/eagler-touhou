
// block 00407b80
00407b80  push       ebp
00407b81  mov        ebp, esp
00407b83  and        esp, 0xfffffff8
00407b86  sub        esp, 0x24
00407b89  push       ebx
00407b8a  push       esi
00407b8b  push       edi
00407b8c  mov        edi, dword ptr [ebp + 8]
00407b8f  mov        ecx, dword ptr [edi + 0x8c]
00407b95  lea        eax, [edi + 0x88]
00407b9b  mov        dword ptr [esp + 0x14], eax
00407b9f  cmp        ecx, dword ptr [eax]
00407ba1  je         0x407dec

// block 00407ba7
00407ba7  mov        ecx, dword ptr [edi + 0x8c]
00407bad  cmp        ecx, 0x5a
00407bb0  jge        0x407bff

// block 00407bb2
00407bb2  mov        eax, dword ptr [0x4b4514]
00407bb7  mov        edx, dword ptr [eax + 0x97c]
00407bbd  add        eax, 0x97c
00407bc2  mov        dword ptr [edi + 0x10], edx
00407bc5  mov        ecx, dword ptr [eax + 4]
00407bc8  mov        dword ptr [edi + 0x14], ecx
00407bcb  mov        edx, dword ptr [eax + 8]
00407bce  mov        dword ptr [edi + 0x18], edx
00407bd1  fld        dword ptr [edi + 0x24]
00407bd4  fadd       qword ptr [0x4a3ce0]
00407bda  push       ecx
00407bdb  fstp       dword ptr [edi + 0x24]
00407bde  fld        dword ptr [edi + 0x20]
00407be1  fadd       qword ptr [0x4a3df0]
00407be7  fstp       dword ptr [esp + 0x10]
00407beb  fld        dword ptr [esp + 0x10]
00407bef  fstp       dword ptr [esp]
00407bf2  call       0x4646e0

// block 00407bf7
00407bf7  fstp       dword ptr [edi + 0x20]
00407bfa  jmp        0x407dec

// block 00407bff
00407bff  mov        eax, dword ptr [edi + 0xac]
00407c05  add        eax, 9
00407c08  lea        eax, [eax + eax*4]
00407c0b  add        eax, eax
00407c0d  cmp        ecx, eax
00407c0f  jge        0x407c52

// block 00407c11
00407c11  mov        eax, dword ptr [0x4b4514]
00407c16  mov        ecx, dword ptr [eax + 0x97c]
00407c1c  add        eax, 0x97c
00407c21  mov        dword ptr [edi + 0x10], ecx
00407c24  mov        edx, dword ptr [eax + 4]
00407c27  mov        dword ptr [edi + 0x14], edx
00407c2a  mov        eax, dword ptr [eax + 8]
00407c2d  mov        dword ptr [edi + 0x18], eax
00407c30  fld        dword ptr [edi + 0x20]
00407c33  fadd       qword ptr [0x4a3df0]
00407c39  push       ecx
00407c3a  fstp       dword ptr [esp + 0x10]
00407c3e  fld        dword ptr [esp + 0x10]
00407c42  fstp       dword ptr [esp]
00407c45  call       0x4646e0

// block 00407c4a
00407c4a  fstp       dword ptr [edi + 0x20]
00407c4d  jmp        0x407dec

// block 00407c52
00407c52  jne        0x407c98

// block 00407c54
00407c54  and        dword ptr [edi + 0x34], 0xfffffffc
00407c58  fld        dword ptr [edi + 0xa0]
00407c5e  fld        dword ptr [edi + 0x9c]
00407c64  lea        esi, [edi + 0x9c]
00407c6a  call       0x4937aa

// block 00407c6f
00407c6f  fstp       dword ptr [esp + 0xc]
00407c73  fld        dword ptr [esp + 0xc]
00407c77  push       ecx
00407c78  fstp       dword ptr [esp]
00407c7b  call       0x4646e0

// block 00407c80
00407c80  push       ecx
00407c81  fstp       dword ptr [esp]
00407c84  call       0x4646e0

// block 00407c89
00407c89  mov        eax, esi
00407c8b  fstp       dword ptr [edi + 0x20]
00407c8e  call       0x408880

// block 00407c93
00407c93  jmp        0x407de9

// block 00407c98
00407c98  cmp        dword ptr [0x4b43dc], 0
00407c9f  je         0x407cbe

// block 00407ca1
00407ca1  fld        dword ptr [0x4a4100]
00407ca7  push       ecx
00407ca8  add        edi, 4
00407cab  fstp       dword ptr [esp]
00407cae  call       0x41a9f0

// block 00407cb3
00407cb3  mov        ecx, dword ptr [ebp + 8]
00407cb6  mov        dword ptr [ecx + 0xa8], eax
00407cbc  mov        edi, ecx

// block 00407cbe
00407cbe  mov        esi, dword ptr [edi + 0xa8]
00407cc4  test       esi, esi
00407cc6  je         0x407dae

// block 00407ccc
00407ccc  mov        eax, esi
00407cce  call       0x407b60

// block 00407cd3
00407cd3  test       eax, eax
00407cd5  jne        0x407dec

// block 00407cdb
00407cdb  fld        dword ptr [edi + 0x20]
00407cde  lea        ebx, [edi + 4]
00407ce1  sub        esp, 8
00407ce4  lea        eax, [esi + 0x1074]
00407cea  fstp       dword ptr [esp + 4]
00407cee  mov        ecx, ebx
00407cf0  call       0x408820

// block 00407cf5
00407cf5  fstp       dword ptr [esp]
00407cf8  call       0x4087b0

// block 00407cfd
00407cfd  fstp       dword ptr [esp + 0x10]
00407d01  fld        dword ptr [edi + 0x1c]
00407d04  fstp       dword ptr [esp + 0xc]
00407d08  fld        dword ptr [esp + 0x10]
00407d0c  fld        st(0)
00407d0e  fabs       
00407d10  fstp       dword ptr [esp + 0x10]
00407d14  fld        dword ptr [esp + 0x10]
00407d18  fcom       qword ptr [0x4a3fc0]
00407d1e  fnstsw     ax
00407d20  test       ah, 1
00407d23  jne        0x407d48

// block 00407d25
00407d25  fstp       st(0)
00407d27  fld        dword ptr [esp + 0xc]
00407d2b  fsub       qword ptr [0x4a40f8]
00407d31  fstp       dword ptr [esp + 0xc]
00407d35  fld1       
00407d37  fcom       dword ptr [esp + 0xc]
00407d3b  fnstsw     ax
00407d3d  test       ah, 0x41
00407d40  jne        0x407d7a

// block 00407d42
00407d42  fstp       dword ptr [esp + 0xc]
00407d46  jmp        0x407d7c

// block 00407d48
00407d48  fcomp      dword ptr [0x4a3ee8]
00407d4e  fnstsw     ax
00407d50  test       ah, 5
00407d53  jp         0x407d7c

// block 00407d55
00407d55  fld        dword ptr [esp + 0xc]
00407d59  fadd       qword ptr [0x4a3fb8]
00407d5f  fstp       dword ptr [esp + 0xc]
00407d63  fld        dword ptr [0x4a3e54]
00407d69  fcom       dword ptr [esp + 0xc]
00407d6d  fnstsw     ax
00407d6f  test       ah, 5
00407d72  jp         0x407d7a

// block 00407d74
00407d74  fstp       dword ptr [esp + 0xc]
00407d78  jmp        0x407d7c

// block 00407d7a
00407d7a  fstp       st(0)

// block 00407d7c
00407d7c  fmul       qword ptr [0x4a3e28]
00407d82  push       ecx
00407d83  fstp       dword ptr [esp + 0x14]
00407d87  fld        dword ptr [esp + 0x14]
00407d8b  fadd       dword ptr [edi + 0x20]
00407d8e  fstp       dword ptr [esp + 0x14]
00407d92  fld        dword ptr [esp + 0x14]
00407d96  fstp       dword ptr [esp]
00407d99  call       0x4646e0

// block 00407d9e
00407d9e  push       ecx
00407d9f  fstp       dword ptr [esp]
00407da2  push       ebx
00407da3  call       0x408910

// block 00407da8
00407da8  fld        dword ptr [esp + 0xc]
00407dac  jmp        0x407de9

// block 00407dae
00407dae  fld        dword ptr [0x4a40f0]
00407db4  sub        esp, 0x10
00407db7  fstp       dword ptr [esp + 0xc]
00407dbb  lea        ecx, [edi + 4]
00407dbe  fld        dword ptr [0x4a3ef0]
00407dc4  fstp       dword ptr [esp + 8]
00407dc8  fld        dword ptr [0x4a40d4]
00407dce  fstp       dword ptr [esp + 4]
00407dd2  fldz       
00407dd4  fstp       dword ptr [esp]
00407dd7  call       0x4086b0

// block 00407ddc
00407ddc  test       eax, eax
00407dde  je         0x407dec

// block 00407de0
00407de0  fld        dword ptr [edi + 0x1c]
00407de3  fmul       qword ptr [0x4a40e8]

// block 00407de9
00407de9  fstp       dword ptr [edi + 0x1c]

// block 00407dec
00407dec  mov        edx, dword ptr [edi + 4]
00407def  mov        eax, dword ptr [edi + 8]
00407df2  mov        ecx, dword ptr [edi + 0xc]
00407df5  lea        esi, [edi + 4]
00407df8  mov        dword ptr [esp + 0x18], edx
00407dfc  mov        dword ptr [esp + 0x1c], eax
00407e00  mov        dword ptr [esp + 0x20], ecx
00407e04  call       0x464d50

// block 00407e09
00407e09  mov        ebx, esi
00407e0b  call       0x464db0

// block 00407e10
00407e10  mov        eax, dword ptr [edi]
00407e12  mov        edx, dword ptr [0x4ce8cc]
00407e18  push       eax
00407e19  call       0x461920

// block 00407e1e
00407e1e  test       eax, eax
00407e20  je         0x407e4e

// block 00407e22
00407e22  fld        dword ptr [esi]
00407e24  fadd       qword ptr [0x4a3d70]
00407e2a  fadd       qword ptr [0x4a3d28]
00407e30  fstp       dword ptr [eax + 0x430]
00407e36  fld        dword ptr [esi + 4]
00407e39  fadd       qword ptr [0x4a3d68]
00407e3f  fstp       dword ptr [eax + 0x434]
00407e45  fld        dword ptr [esi + 8]
00407e48  fstp       dword ptr [eax + 0x438]

// block 00407e4e
00407e4e  fld        dword ptr [esi]
00407e50  fsub       dword ptr [esp + 0x18]
00407e54  fstp       dword ptr [esp + 0x24]
00407e58  mov        edx, dword ptr [esp + 0x24]
00407e5c  fld        dword ptr [esi + 4]
00407e5f  fsub       dword ptr [esp + 0x1c]
00407e63  fstp       dword ptr [esp + 0x28]
00407e67  mov        eax, dword ptr [esp + 0x28]
00407e6b  fld        dword ptr [esi + 8]
00407e6e  mov        esi, dword ptr [esp + 0x14]
00407e72  fsub       dword ptr [esp + 0x20]
00407e76  mov        dword ptr [edi + 0x9c], edx
00407e7c  mov        dword ptr [edi + 0xa0], eax
00407e82  fstp       dword ptr [esp + 0x2c]
00407e86  mov        ecx, dword ptr [esp + 0x2c]
00407e8a  mov        dword ptr [edi + 0xa4], ecx
00407e90  call       0x464a80

// block 00407e95
00407e95  pop        edi
00407e96  pop        esi
00407e97  pop        ebx
00407e98  mov        esp, ebp
00407e9a  pop        ebp
00407e9b  ret        4
