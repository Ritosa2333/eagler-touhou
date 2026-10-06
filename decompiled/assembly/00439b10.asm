
// block 00439b10
00439b10  sub        esp, 0x40
00439b13  mov        edx, dword ptr [esp + 0x44]
00439b17  push       ebx
00439b18  push       ebp
00439b19  add        edx, 0xa58
00439b1f  push       esi
00439b20  push       edi
00439b21  mov        dword ptr [esp + 0x14], edx
00439b25  lea        edi, [edx + 0x20]
00439b28  mov        dword ptr [esp + 0x18], 0x100
00439b30  xor        ebp, ebp

// block 00439b32
00439b32  cmp        dword ptr [edi + 0x28], ebp
00439b35  je         0x439d77

// block 00439b3b
00439b3b  mov        eax, dword ptr [edi + 0x54]
00439b3e  mov        dword ptr [edi + 0x38], ebp
00439b41  mov        eax, dword ptr [eax + 0x28]
00439b44  cmp        eax, ebp
00439b46  je         0x439b4e

// block 00439b48
00439b48  mov        ecx, dword ptr [esp + 0x54]
00439b4c  call       eax

// block 00439b4e
00439b4e  test       byte ptr [edi + 0x24], 1
00439b52  jne        0x439b72

// block 00439b54
00439b54  fld        dword ptr [edi + 0xc]
00439b57  sub        esp, 8
00439b5a  fstp       dword ptr [esp + 4]
00439b5e  mov        ecx, edi
00439b60  fld        dword ptr [edi + 0x10]
00439b63  fstp       dword ptr [esp]
00439b66  call       0x465390

// block 00439b6b
00439b6b  fldz       
00439b6d  fstp       dword ptr [edi + 8]
00439b70  jmp        0x439b9e

// block 00439b72
00439b72  fld        dword ptr [edi + 0x18]
00439b75  push       ecx
00439b76  fadd       dword ptr [edi + 0x14]
00439b79  fstp       dword ptr [edi + 0x14]
00439b7c  fld        dword ptr [edi + 0xc]
00439b7f  fadd       dword ptr [edi + 0x10]
00439b82  fstp       dword ptr [esp + 0x20]
00439b86  fld        dword ptr [esp + 0x20]
00439b8a  fstp       dword ptr [esp]
00439b8d  call       0x4646e0

// block 00439b92
00439b92  push       ecx
00439b93  fstp       dword ptr [esp]
00439b96  call       0x4646e0

// block 00439b9b
00439b9b  fstp       dword ptr [edi + 0x10]

// block 00439b9e
00439b9e  lea        ebx, [edi - 0xc]
00439ba1  call       0x464db0

// block 00439ba6
00439ba6  mov        ecx, dword ptr [edi + 0x2c]
00439ba9  mov        edx, dword ptr [0x4ce8cc]
00439baf  cmp        ecx, ebp
00439bb1  je         0x439bea

// block 00439bb3
00439bb3  mov        eax, dword ptr [edx + 0x8856b8]
00439bb9  cmp        eax, ebp
00439bbb  je         0x439bcd

// block 00439bbd
00439bbd  lea        ecx, [ecx]

// block 00439bc0
00439bc0  mov        esi, dword ptr [eax]
00439bc2  cmp        dword ptr [esi], ecx
00439bc4  je         0x439be6

// block 00439bc6
00439bc6  mov        eax, dword ptr [eax + 4]
00439bc9  cmp        eax, ebp
00439bcb  jne        0x439bc0

// block 00439bcd
00439bcd  mov        eax, dword ptr [edx + 0x8856c0]
00439bd3  cmp        eax, ebp
00439bd5  je         0x439bea

// block 00439bd7
00439bd7  mov        esi, dword ptr [eax]
00439bd9  cmp        dword ptr [esi], ecx
00439bdb  je         0x439be6

// block 00439bdd
00439bdd  mov        eax, dword ptr [eax + 4]
00439be0  cmp        eax, ebp
00439be2  jne        0x439bd7

// block 00439be4
00439be4  jmp        0x439bea

// block 00439be6
00439be6  cmp        esi, ebp
00439be8  jne        0x439c2b

// block 00439bea
00439bea  mov        dword ptr [edi + 0x28], ebp
00439bed  mov        eax, dword ptr [edi + 0x30]
00439bf0  push       eax
00439bf1  call       0x461920

// block 00439bf6
00439bf6  cmp        eax, ebp
00439bf8  je         0x439c20

// block 00439bfa
00439bfa  mov        edx, 0x10000000
00439bff  or         dword ptr [eax + 0x47c], edx
00439c05  cmp        dword ptr [eax + 0x18], ebp
00439c08  jne        0x439c20

// block 00439c0a
00439c0a  mov        eax, dword ptr [eax + 0x14]
00439c0d  cmp        eax, ebp
00439c0f  je         0x439c20

// block 00439c11
00439c11  mov        ecx, dword ptr [eax]
00439c13  or         dword ptr [ecx + 0x47c], edx
00439c19  mov        eax, dword ptr [eax + 4]
00439c1c  cmp        eax, ebp
00439c1e  jne        0x439c11

// block 00439c20
00439c20  mov        dword ptr [edi + 0x2c], ebp
00439c23  mov        dword ptr [edi + 0x30], ebp
00439c26  jmp        0x439d77

// block 00439c2b
00439c2b  mov        ecx, dword ptr [edi + 0x54]
00439c2e  cmp        byte ptr [ecx + 0x1d], 2
00439c32  je         0x439cfd

// block 00439c38
00439c38  lea        ecx, [esp + 0x20]
00439c3c  mov        eax, esi
00439c3e  call       0x45cdf0

// block 00439c43
00439c43  cmp        dword ptr [edi - 0x1c], 0xa
00439c47  jl         0x439cf7

// block 00439c4d
00439c4d  fld        dword ptr [0x4a3d78]
00439c53  fld        dword ptr [esp + 0x20]
00439c57  fcom       st(1)
00439c59  fnstsw     ax
00439c5b  test       ah, 0x41
00439c5e  jnp        0x439c9b

// block 00439c60
00439c60  fld        qword ptr [0x4a3e80]
00439c66  fcom       st(1)
00439c68  fnstsw     ax
00439c6a  fstp       st(1)
00439c6c  fld        dword ptr [0x4a3e68]
00439c72  test       ah, 0x41
00439c75  jnp        0x439cab

// block 00439c77
00439c77  fld        dword ptr [esp + 0x24]
00439c7b  fcom       st(1)
00439c7d  fnstsw     ax
00439c7f  test       ah, 0x41
00439c82  jnp        0x439d9c

// block 00439c88
00439c88  fcomp      qword ptr [0x4a3e90]
00439c8e  fnstsw     ax
00439c90  test       ah, 1
00439c93  je         0x439cab

// block 00439c95
00439c95  fstp       st(0)
00439c97  fstp       st(0)
00439c99  jmp        0x439cf5

// block 00439c9b
00439c9b  fstp       st(0)
00439c9d  fld        dword ptr [0x4a3e68]
00439ca3  fld        qword ptr [0x4a3e80]
00439ca9  fxch       st(1)

// block 00439cab
00439cab  fld        dword ptr [esp + 0x2c]
00439caf  fcom       st(3)
00439cb1  fnstsw     ax
00439cb3  test       ah, 0x41
00439cb6  jnp        0x439da3

// block 00439cbc
00439cbc  fcomp      st(2)
00439cbe  fnstsw     ax
00439cc0  test       ah, 1
00439cc3  je         0x439da5

// block 00439cc9
00439cc9  fld        dword ptr [esp + 0x30]
00439ccd  fcom       st(1)
00439ccf  fnstsw     ax
00439cd1  test       ah, 0x41
00439cd4  jnp        0x439da3

// block 00439cda
00439cda  fld        qword ptr [0x4a3e90]
00439ce0  fcom       st(1)
00439ce2  fnstsw     ax
00439ce4  fstp       st(1)
00439ce6  test       ah, 0x41
00439ce9  jnp        0x439dab

// block 00439cef
00439cef  fstp       st(1)
00439cf1  fstp       st(1)
00439cf3  fstp       st(1)

// block 00439cf5
00439cf5  fstp       st(0)

// block 00439cf7
00439cf7  mov        edx, dword ptr [0x4ce8cc]

// block 00439cfd
00439cfd  fld        dword ptr [ebx]
00439cff  fld        qword ptr [0x4a3d70]
00439d05  fadd       st(1), st(0)
00439d07  fld        qword ptr [0x4a3d28]
00439d0d  fadd       st(2), st(0)
00439d0f  fxch       st(2)
00439d11  fstp       dword ptr [esi + 0x430]
00439d17  fld        dword ptr [edi - 8]
00439d1a  fld        qword ptr [0x4a3d68]
00439d20  fadd       st(1), st(0)
00439d22  fxch       st(1)
00439d24  fstp       dword ptr [esi + 0x434]
00439d2a  fld        dword ptr [edi - 4]
00439d2d  fstp       dword ptr [esi + 0x438]
00439d33  cmp        dword ptr [edi + 0x30], ebp
00439d36  je         0x439d4c

// block 00439d38
00439d38  mov        ecx, dword ptr [edi + 0x30]
00439d3b  push       ecx
00439d3c  call       0x461920

// block 00439d41
00439d41  cmp        eax, ebp
00439d43  jne        0x439e70

// block 00439d49
00439d49  mov        dword ptr [edi + 0x30], ebp

// block 00439d4c
00439d4c  fstp       st(1)
00439d4e  fstp       st(1)
00439d50  fstp       st(0)

// block 00439d52
00439d52  mov        eax, dword ptr [esi + 0x47c]
00439d58  test       eax, 0x20000000
00439d5d  je         0x439d6e

// block 00439d5f
00439d5f  fld        dword ptr [edi + 0x10]
00439d62  or         eax, 4
00439d65  fstp       dword ptr [esi + 0x2c]
00439d68  mov        dword ptr [esi + 0x47c], eax

// block 00439d6e
00439d6e  mov        esi, dword ptr [esp + 0x14]
00439d72  call       0x464a80

// block 00439d77
00439d77  mov        edx, dword ptr [esp + 0x14]
00439d7b  add        edx, 0x78
00439d7e  add        edi, 0x78
00439d81  sub        dword ptr [esp + 0x18], 1
00439d86  mov        dword ptr [esp + 0x14], edx
00439d8a  jne        0x439b32

// block 00439d90
00439d90  pop        edi
00439d91  pop        esi
00439d92  pop        ebp
00439d93  xor        eax, eax
00439d95  pop        ebx
00439d96  add        esp, 0x40
00439d99  ret        4

// block 00439d9c
00439d9c  fstp       st(0)
00439d9e  jmp        0x439cab

// block 00439da3
00439da3  fstp       st(0)

// block 00439da5
00439da5  fld        qword ptr [0x4a3e90]

// block 00439dab
00439dab  fld        dword ptr [esp + 0x38]
00439daf  fcom       st(4)
00439db1  fnstsw     ax
00439db3  test       ah, 0x41
00439db6  jnp        0x439ddd

// block 00439db8
00439db8  fcomp      st(3)
00439dba  fnstsw     ax
00439dbc  test       ah, 1
00439dbf  je         0x439ddf

// block 00439dc1
00439dc1  fld        dword ptr [esp + 0x3c]
00439dc5  fcom       st(2)
00439dc7  fnstsw     ax
00439dc9  test       ah, 0x41
00439dcc  jnp        0x439ddd

// block 00439dce
00439dce  fcomp      st(1)
00439dd0  fnstsw     ax
00439dd2  test       ah, 1
00439dd5  jne        0x439cef

// block 00439ddb
00439ddb  jmp        0x439ddf

// block 00439ddd
00439ddd  fstp       st(0)

// block 00439ddf
00439ddf  fld        dword ptr [esp + 0x44]
00439de3  fcom       st(4)
00439de5  fnstsw     ax
00439de7  fstp       st(4)
00439de9  test       ah, 0x41
00439dec  jnp        0x439e19

// block 00439dee
00439dee  fxch       st(3)
00439df0  fcomp      st(2)
00439df2  fnstsw     ax
00439df4  fstp       st(1)
00439df6  test       ah, 1
00439df9  je         0x439e1d

// block 00439dfb
00439dfb  fld        dword ptr [esp + 0x48]
00439dff  fcom       st(1)
00439e01  fnstsw     ax
00439e03  fstp       st(1)
00439e05  test       ah, 0x41
00439e08  jnp        0x439e1d

// block 00439e0a
00439e0a  fcompp     
00439e0c  fnstsw     ax
00439e0e  test       ah, 1
00439e11  jne        0x439cf7

// block 00439e17
00439e17  jmp        0x439e21

// block 00439e19
00439e19  fstp       st(3)
00439e1b  fstp       st(0)

// block 00439e1d
00439e1d  fstp       st(0)
00439e1f  fstp       st(0)

// block 00439e21
00439e21  mov        edx, dword ptr [edi + 0x54]
00439e24  cmp        byte ptr [edx + 0x1d], 3
00439e28  jne        0x439e5c

// block 00439e2a
00439e2a  fld        dword ptr [ebx]
00439e2c  fcomp      qword ptr [0x4a40b8]
00439e32  fnstsw     ax
00439e34  test       ah, 0x41
00439e37  jnp        0x439e5c

// block 00439e39
00439e39  fld        dword ptr [ebx]
00439e3b  fcomp      qword ptr [0x4a40b0]
00439e41  fnstsw     ax
00439e43  test       ah, 1
00439e46  je         0x439e5c

// block 00439e48
00439e48  fld        dword ptr [0x4a4050]
00439e4e  fcomp      dword ptr [edi - 8]
00439e51  fnstsw     ax
00439e53  test       ah, 0x41
00439e56  jne        0x439cf7

// block 00439e5c
00439e5c  mov        eax, dword ptr [edi + 0x2c]
00439e5f  push       eax
00439e60  call       0x461a70

// block 00439e65
00439e65  mov        dword ptr [edi + 0x2c], ebp
00439e68  mov        dword ptr [edi + 0x28], ebp
00439e6b  jmp        0x439d77

// block 00439e70
00439e70  fld        dword ptr [ebx]
00439e72  faddp      st(2)
00439e74  fxch       st(1)
00439e76  faddp      st(2)
00439e78  fxch       st(1)
00439e7a  fstp       dword ptr [eax + 0x430]
00439e80  fadd       dword ptr [edi - 8]
00439e83  fstp       dword ptr [eax + 0x434]
00439e89  fld        dword ptr [edi - 4]
00439e8c  fstp       dword ptr [eax + 0x438]
00439e92  jmp        0x439d52
