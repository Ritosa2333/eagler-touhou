
// block 00413840
00413840  push       ebp
00413841  mov        ebp, esp
00413843  and        esp, 0xfffffff8
00413846  sub        esp, 0x5c
00413849  push       ebx
0041384a  push       esi
0041384b  mov        esi, dword ptr [ebp + 8]
0041384e  mov        eax, dword ptr [esi + 0x16b8]
00413854  push       edi
00413855  test       eax, 0x20000
0041385a  jne        0x414ada

// block 00413860
00413860  or         eax, 0x20000
00413865  mov        dword ptr [esi + 0x16b8], eax
0041386b  mov        ecx, dword ptr [esi + 0x34]
0041386e  lea        eax, [esi + 0x34]
00413871  mov        dword ptr [esi], ecx
00413873  mov        edx, dword ptr [eax + 4]
00413876  mov        dword ptr [esi + 4], edx
00413879  mov        ecx, dword ptr [eax + 8]
0041387c  mov        dword ptr [esi + 8], ecx
0041387f  mov        edx, dword ptr [eax + 0xc]
00413882  mov        dword ptr [esi + 0xc], edx
00413885  mov        ecx, dword ptr [eax + 0x10]
00413888  mov        dword ptr [esi + 0x10], ecx
0041388b  mov        edx, dword ptr [eax + 0x14]
0041388e  mov        dword ptr [esi + 0x14], edx
00413891  fld        dword ptr [eax + 0x18]
00413894  fstp       dword ptr [esi + 0x18]
00413897  mov        dword ptr [esp + 0x10], eax
0041389b  fld        dword ptr [eax + 0x1c]
0041389e  fstp       dword ptr [esi + 0x1c]
004138a1  cmp        dword ptr [esi + 0x358], 0
004138a8  fld        dword ptr [eax + 0x20]
004138ab  fstp       dword ptr [esi + 0x20]
004138ae  fld        dword ptr [eax + 0x24]
004138b1  fstp       dword ptr [esi + 0x24]
004138b4  fld        dword ptr [eax + 0x28]
004138b7  fstp       dword ptr [esi + 0x28]
004138ba  fld        dword ptr [eax + 0x2c]
004138bd  fstp       dword ptr [esi + 0x2c]
004138c0  mov        eax, dword ptr [eax + 0x30]
004138c3  mov        dword ptr [esi + 0x30], eax
004138c6  je         0x413906

// block 004138c8
004138c8  lea        edi, [esi + 0x324]
004138ce  lea        ebx, [esp + 0x30]
004138d2  call       0x41c020

// block 004138d7
004138d7  test       byte ptr [esi + 0x98], 3
004138de  jne        0x4138fc

// block 004138e0
004138e0  fld        dword ptr [esp + 0x30]
004138e4  push       ecx
004138e5  fstp       dword ptr [esp]
004138e8  call       0x4646e0

// block 004138ed
004138ed  push       ecx
004138ee  fstp       dword ptr [esp]
004138f1  call       0x4646e0

// block 004138f6
004138f6  fstp       dword ptr [esi + 0x84]

// block 004138fc
004138fc  fld        dword ptr [esp + 0x34]
00413900  fstp       dword ptr [esi + 0x80]

// block 00413906
00413906  cmp        dword ptr [esi + 0x3d0], 0
0041390d  je         0x413932

// block 0041390f
0041390f  lea        edi, [esi + 0x39c]
00413915  lea        ebx, [esp + 0x30]
00413919  call       0x41c020

// block 0041391e
0041391e  fld        dword ptr [esp + 0x30]
00413922  fstp       dword ptr [esi + 0x88]
00413928  fld        dword ptr [esp + 0x34]
0041392c  fstp       dword ptr [esi + 0x8c]

// block 00413932
00413932  cmp        dword ptr [esi + 0x394], 0
00413939  je         0x413979

// block 0041393b
0041393b  lea        edi, [esi + 0x360]
00413941  lea        ebx, [esp + 0x30]
00413945  call       0x41c020

// block 0041394a
0041394a  test       byte ptr [esi + 0xcc], 1
00413951  jne        0x41396f

// block 00413953
00413953  fld        dword ptr [esp + 0x30]
00413957  push       ecx
00413958  fstp       dword ptr [esp]
0041395b  call       0x4646e0

// block 00413960
00413960  push       ecx
00413961  fstp       dword ptr [esp]
00413964  call       0x4646e0

// block 00413969
00413969  fstp       dword ptr [esi + 0xb8]

// block 0041396f
0041396f  fld        dword ptr [esp + 0x34]
00413973  fstp       dword ptr [esi + 0xb4]

// block 00413979
00413979  cmp        dword ptr [esi + 0x40c], 0
00413980  je         0x4139a5

// block 00413982
00413982  lea        edi, [esi + 0x3d8]
00413988  lea        ebx, [esp + 0x30]
0041398c  call       0x41c020

// block 00413991
00413991  fld        dword ptr [esp + 0x30]
00413995  fstp       dword ptr [esi + 0xbc]
0041399b  fld        dword ptr [esp + 0x34]
0041399f  fstp       dword ptr [esi + 0xc0]

// block 004139a5
004139a5  cmp        dword ptr [esi + 0x2d0], 0
004139ac  je         0x4139fb

// block 004139ae
004139ae  lea        ecx, [esi + 0x68]
004139b1  lea        edi, [esi + 0x28c]
004139b7  lea        ebx, [esp + 0x50]
004139bb  mov        dword ptr [esp + 0x14], ecx
004139bf  call       0x405900

// block 004139c4
004139c4  fld        dword ptr [eax]
004139c6  mov        ecx, dword ptr [esp + 0x14]
004139ca  fsub       dword ptr [ecx]
004139cc  fstp       dword ptr [esp + 0x5c]
004139d0  mov        edx, dword ptr [esp + 0x5c]
004139d4  fld        dword ptr [eax + 4]
004139d7  fsub       dword ptr [ecx + 4]
004139da  fstp       dword ptr [esp + 0x60]
004139de  fld        dword ptr [eax + 8]
004139e1  mov        eax, dword ptr [esp + 0x60]
004139e5  fsub       dword ptr [ecx + 8]
004139e8  mov        dword ptr [esi + 0x74], edx
004139eb  mov        dword ptr [esi + 0x78], eax
004139ee  fstp       dword ptr [esp + 0x64]
004139f2  mov        ecx, dword ptr [esp + 0x64]
004139f6  mov        dword ptr [esi + 0x7c], ecx
004139f9  jmp        0x413a0a

// block 004139fb
004139fb  add        esi, 0x68
004139fe  mov        dword ptr [esp + 0x14], esi
00413a02  call       0x464d50

// block 00413a07
00413a07  mov        esi, dword ptr [ebp + 8]

// block 00413a0a
00413a0a  cmp        dword ptr [esi + 0x31c], 0
00413a11  je         0x413a6c

// block 00413a13
00413a13  lea        edx, [esi + 0x9c]
00413a19  lea        edi, [esi + 0x2d8]
00413a1f  lea        ebx, [esp + 0x50]
00413a23  mov        dword ptr [esp + 0x18], edx
00413a27  call       0x405900

// block 00413a2c
00413a2c  fld        dword ptr [eax]
00413a2e  mov        edi, dword ptr [esp + 0x18]
00413a32  fsub       dword ptr [edi]
00413a34  fstp       dword ptr [esp + 0x5c]
00413a38  fld        dword ptr [eax + 4]
00413a3b  fsub       dword ptr [edi + 4]
00413a3e  fstp       dword ptr [esp + 0x60]
00413a42  mov        ecx, dword ptr [esp + 0x60]
00413a46  fld        dword ptr [eax + 8]
00413a49  mov        eax, dword ptr [esp + 0x5c]
00413a4d  fsub       dword ptr [edi + 8]
00413a50  mov        dword ptr [esi + 0xa8], eax
00413a56  mov        dword ptr [esi + 0xac], ecx
00413a5c  fstp       dword ptr [esp + 0x64]
00413a60  mov        edx, dword ptr [esp + 0x64]
00413a64  mov        dword ptr [esi + 0xb0], edx
00413a6a  jmp        0x413a82

// block 00413a6c
00413a6c  add        esi, 0x9c
00413a72  mov        dword ptr [esp + 0x18], esi
00413a76  call       0x464d50

// block 00413a7b
00413a7b  mov        esi, dword ptr [ebp + 8]
00413a7e  mov        edi, dword ptr [esp + 0x18]

// block 00413a82
00413a82  mov        ebx, dword ptr [esp + 0x14]
00413a86  call       0x464db0

// block 00413a8b
00413a8b  test       dword ptr [esi + 0x16b8], 0x2000000
00413a95  je         0x413ab9

// block 00413a97
00413a97  fld        dword ptr [edi]
00413a99  fadd       dword ptr [0x4cebdc]
00413a9f  fstp       dword ptr [edi]
00413aa1  fld        dword ptr [edi + 4]
00413aa4  fadd       dword ptr [0x4cebe0]
00413aaa  fstp       dword ptr [edi + 4]
00413aad  fld        dword ptr [edi + 8]
00413ab0  fadd       dword ptr [0x4cebe4]
00413ab6  fstp       dword ptr [edi + 8]

// block 00413ab9
00413ab9  mov        ebx, edi
00413abb  call       0x464db0

// block 00413ac0
00413ac0  call       0x413700

// block 00413ac5
00413ac5  mov        ebx, dword ptr [ebp + 8]
00413ac8  mov        eax, dword ptr [ebx + 0xe0]
00413ace  mov        edx, dword ptr [0x4ce8cc]
00413ad4  lea        esi, [ebx + 0xe0]
00413ada  push       eax
00413adb  mov        dword ptr [esp + 0x10], esi
00413adf  call       0x461920

// block 00413ae4
00413ae4  test       eax, eax
00413ae6  jne        0x413aec

// block 00413ae8
00413ae8  mov        dword ptr [esi], eax
00413aea  jmp        0x413b28

// block 00413aec
00413aec  fld        dword ptr [eax + 0x5c]
00413aef  fmul       dword ptr [eax + 0x44]
00413af2  fstp       dword ptr [esp + 0x2c]
00413af6  fld        dword ptr [esp + 0x2c]
00413afa  fabs       
00413afc  fstp       dword ptr [esp + 0x2c]
00413b00  fld        dword ptr [esp + 0x2c]
00413b04  fstp       dword ptr [ebx + 0x15ec]
00413b0a  fld        dword ptr [eax + 0x58]
00413b0d  fmul       dword ptr [eax + 0x40]
00413b10  fstp       dword ptr [esp + 0x2c]
00413b14  fld        dword ptr [esp + 0x2c]
00413b18  fabs       
00413b1a  fstp       dword ptr [esp + 0x2c]
00413b1e  fld        dword ptr [esp + 0x2c]
00413b22  fstp       dword ptr [ebx + 0x15f0]

// block 00413b28
00413b28  fld        dword ptr [ebx + 0x15ec]
00413b2e  mov        ecx, dword ptr [esp + 0x10]
00413b32  fld        qword ptr [0x4a3cf8]
00413b38  fmul       st(1), st(0)
00413b3a  fld        dword ptr [ecx]
00413b3c  fadd       st(2)
00413b3e  fcomp      qword ptr [0x4a3d60]
00413b44  fnstsw     ax
00413b46  test       ah, 5
00413b49  jnp        0x413bfe

// block 00413b4f
00413b4f  fld        dword ptr [ecx]
00413b51  fsubrp     st(2)
00413b53  fxch       st(1)
00413b55  fcomp      qword ptr [0x4a3d28]
00413b5b  fnstsw     ax
00413b5d  test       ah, 0x41
00413b60  je         0x413c00

// block 00413b66
00413b66  fmul       dword ptr [ebx + 0x15f0]
00413b6c  fld        dword ptr [ebx + 0x38]
00413b6f  fadd       st(1)
00413b71  fcomp      qword ptr [0x4a3d58]
00413b77  fnstsw     ax
00413b79  test       ah, 5
00413b7c  jnp        0x413bdf

// block 00413b7e
00413b7e  fsubr      dword ptr [ebx + 0x38]
00413b81  fcomp      qword ptr [0x4a3d50]
00413b87  fnstsw     ax
00413b89  test       ah, 0x41
00413b8c  je         0x413be1

// block 00413b8e
00413b8e  or         dword ptr [ebx + 0x16b8], 0x8000

// block 00413b98
00413b98  mov        ecx, dword ptr [ebx + 0x16b8]
00413b9e  test       ecx, 0x8000000
00413ba4  je         0x413c49

// block 00413baa
00413baa  mov        edx, dword ptr [0x4b43c4]
00413bb0  mov        eax, dword ptr [edx + 0x3c]
00413bb3  test       eax, eax
00413bb5  je         0x413c23

// block 00413bb7
00413bb7  test       ecx, 0x10000000
00413bbd  jne        0x413c1f

// block 00413bbf
00413bbf  mov        eax, dword ptr [ebx + 0x16bc]
00413bc5  mov        dword ptr [ebx + 0x22c], eax
00413bcb  push       eax
00413bcc  mov        eax, esi
00413bce  call       0x461f40

// block 00413bd3
00413bd3  or         dword ptr [ebx + 0x16b8], 0x10000001
00413bdd  jmp        0x413c49

// block 00413bdf
00413bdf  fstp       st(0)

// block 00413be1
00413be1  mov        eax, dword ptr [ebx + 0x16b8]
00413be7  test       eax, 0x8000
00413bec  je         0x413b98

// block 00413bee
00413bee  test       al, 8
00413bf0  jne        0x413b98

// block 00413bf2
00413bf2  or         eax, 0xffffffff
00413bf5  pop        edi
00413bf6  pop        esi
00413bf7  pop        ebx
00413bf8  mov        esp, ebp
00413bfa  pop        ebp
00413bfb  ret        4

// block 00413bfe
00413bfe  fstp       st(1)

// block 00413c00
00413c00  mov        eax, dword ptr [ebx + 0x16b8]
00413c06  fstp       st(0)
00413c08  test       eax, 0x8000
00413c0d  je         0x413b98

// block 00413c0f
00413c0f  test       al, 4
00413c11  jne        0x413b98

// block 00413c13
00413c13  or         eax, 0xffffffff
00413c16  pop        edi
00413c17  pop        esi
00413c18  pop        ebx
00413c19  mov        esp, ebp
00413c1b  pop        ebp
00413c1c  ret        4

// block 00413c1f
00413c1f  test       eax, eax
00413c21  jne        0x413c49

// block 00413c23
00413c23  test       ecx, 0x10000000
00413c29  je         0x413c49

// block 00413c2b
00413c2b  mov        eax, dword ptr [ebx + 0x16c0]
00413c31  mov        dword ptr [ebx + 0x22c], eax
00413c37  push       eax
00413c38  mov        eax, esi
00413c3a  call       0x461f40

// block 00413c3f
00413c3f  and        dword ptr [ebx + 0x16b8], 0xeffffffe

// block 00413c49
00413c49  mov        eax, dword ptr [ebx + 0x278]
00413c4f  fld        dword ptr [eax]
00413c51  mov        edi, dword ptr [ebx + 0x174c]
00413c57  fstp       dword ptr [esp + 0x2c]
00413c5b  push       ecx
00413c5c  fld        dword ptr [esp + 0x30]
00413c60  fstp       dword ptr [esp]
00413c63  call       0x468d90

// block 00413c68
00413c68  test       eax, eax
00413c6a  jne        0x413c13

// block 00413c6c
00413c6c  mov        eax, dword ptr [ebx + 0x1768]
00413c72  test       eax, eax
00413c74  je         0x413c7e

// block 00413c76
00413c76  mov        ecx, ebx
00413c78  call       eax

// block 00413c7a
00413c7a  test       eax, eax
00413c7c  jne        0x413c13

// block 00413c7e
00413c7e  mov        eax, dword ptr [ebx + 0x16b8]
00413c84  test       eax, 0x400000
00413c89  je         0x413d57

// block 00413c8f
00413c8f  test       al, 0x20
00413c91  jne        0x413d57

// block 00413c97
00413c97  mov        eax, dword ptr [0x4b43cc]
00413c9c  test       byte ptr [eax + 0x7c], 1
00413ca0  je         0x413d1e

// block 00413ca2
00413ca2  mov        eax, dword ptr [eax + 0x78]
00413ca5  cmp        eax, 0x67
00413ca8  jl         0x413d1e

// block 00413caa
00413caa  cmp        eax, 0x70
00413cad  jg         0x413d1e

// block 00413caf
00413caf  mov        ecx, dword ptr [0x4b43c4]
00413cb5  cmp        dword ptr [ecx + 0x3c], 0
00413cb9  je         0x413d1e

// block 00413cbb
00413cbb  mov        edx, dword ptr [0x4b4514]
00413cc1  test       byte ptr [edx + 0xc414], 4
00413cc8  jne        0x413d1e

// block 00413cca
00413cca  lea        edi, [ebx + 0x11c]
00413cd0  mov        esi, edi
00413cd2  call       0x41c890

// block 00413cd7
00413cd7  test       eax, eax
00413cd9  jne        0x413cff

// block 00413cdb
00413cdb  mov        ecx, dword ptr [0x4b43dc]
00413ce1  mov        edx, dword ptr [ecx + 0x48]
00413ce4  push       eax
00413ce5  push       0x32
00413ce7  lea        eax, [esp + 0x34]
00413ceb  push       eax
00413cec  push       edx
00413ced  mov        eax, 0xa
00413cf2  xor        ecx, ecx
00413cf4  call       0x4615a0

// block 00413cf9
00413cf9  mov        eax, dword ptr [esp + 0x2c]
00413cfd  mov        dword ptr [edi], eax

// block 00413cff
00413cff  mov        esi, dword ptr [esp + 0x10]
00413d03  mov        eax, edi
00413d05  call       0x461e30

// block 00413d0a
00413d0a  fld        dword ptr [0x4a3d88]
00413d10  fst        dword ptr [ebx + 0xd0]
00413d16  fstp       dword ptr [ebx + 0xd4]
00413d1c  jmp        0x413d57

// block 00413d1e
00413d1e  lea        esi, [ebx + 0x11c]
00413d24  call       0x41c890

// block 00413d29
00413d29  test       eax, eax
00413d2b  je         0x413d57

// block 00413d2d
00413d2d  mov        ecx, dword ptr [esi]
00413d2f  push       ecx
00413d30  mov        ebx, 1
00413d35  call       0x461970

// block 00413d3a
00413d3a  fld        dword ptr [0x4a3f50]
00413d40  mov        eax, dword ptr [ebp + 8]
00413d43  fst        dword ptr [eax + 0xd0]
00413d49  mov        dword ptr [esi], 0
00413d4f  fstp       dword ptr [eax + 0xd4]
00413d55  mov        ebx, eax

// block 00413d57
00413d57  and        dword ptr [ebx + 0x16b8], 0xffefffff
00413d61  mov        eax, dword ptr [ebx + 0x16b8]
00413d67  test       al, 0x21
00413d69  jne        0x413f5d

// block 00413d6f
00413d6f  mov        eax, dword ptr [ebx + 0x176c]
00413d75  test       eax, eax
00413d77  je         0x413d7f

// block 00413d79
00413d79  mov        ecx, ebx
00413d7b  call       eax

// block 00413d7d
00413d7d  jmp        0x413d90

// block 00413d7f
00413d7f  mov        eax, dword ptr [esp + 0x10]
00413d83  lea        edx, [ebx + 0xd0]
00413d89  push       edx
00413d8a  push       eax
00413d8b  call       0x439ed0

// block 00413d90
00413d90  mov        ecx, dword ptr [0x4b4514]
00413d96  mov        esi, eax
00413d98  mov        eax, dword ptr [ecx + 0xa28]
00413d9e  cmp        eax, 2
00413da1  je         0x413da7

// block 00413da3
00413da3  test       eax, eax
00413da5  jne        0x413db9

// block 00413da7
00413da7  mov        eax, 0x66666667
00413dac  imul       esi
00413dae  sar        edx, 1
00413db0  mov        eax, edx
00413db2  shr        eax, 0x1f
00413db5  add        eax, edx
00413db7  mov        esi, eax

// block 00413db9
00413db9  mov        ecx, dword ptr [0x4b43e4]
00413dbf  cmp        dword ptr [ecx + 0x6d30], 0
00413dc6  je         0x413dca

// block 00413dc8
00413dc8  xor        esi, esi

// block 00413dca
00413dca  mov        eax, dword ptr [0x4b43cc]
00413dcf  mov        edi, dword ptr [eax + 0x7c]
00413dd2  and        edi, 1
00413dd5  je         0x413e19

// block 00413dd7
00413dd7  mov        eax, dword ptr [eax + 0x78]
00413dda  cmp        eax, 0x5d
00413ddd  jl         0x413e19

// block 00413ddf
00413ddf  cmp        eax, 0x63
00413de2  jg         0x413e19

// block 00413de4
00413de4  mov        edx, dword ptr [0x4b43c4]
00413dea  cmp        dword ptr [edx + 0x3c], 0
00413dee  jne        0x413df9

// block 00413df0
00413df0  call       0x412690

// block 00413df5
00413df5  test       eax, eax
00413df7  je         0x413e19

// block 00413df9
00413df9  mov        eax, dword ptr [0x4b4514]
00413dfe  test       byte ptr [eax + 0xc414], 4
00413e05  jne        0x413e19

// block 00413e07
00413e07  mov        eax, 0x66666667
00413e0c  imul       esi
00413e0e  sar        edx, 1
00413e10  mov        ecx, edx
00413e12  shr        ecx, 0x1f
00413e15  add        ecx, edx
00413e17  mov        esi, ecx

// block 00413e19
00413e19  test       edi, edi
00413e1b  je         0x413e5a

// block 00413e1d
00413e1d  mov        edx, dword ptr [0x4b43cc]
00413e23  mov        eax, dword ptr [edx + 0x78]
00413e26  cmp        eax, 0x67
00413e29  jl         0x413e5a

// block 00413e2b
00413e2b  cmp        eax, 0x70
00413e2e  jg         0x413e5a

// block 00413e30
00413e30  mov        eax, dword ptr [0x4b43c4]
00413e35  cmp        dword ptr [eax + 0x3c], 0
00413e39  je         0x413e5a

// block 00413e3b
00413e3b  test       dword ptr [ebx + 0x16b8], 0x400000
00413e45  je         0x413e5a

// block 00413e47
00413e47  mov        ecx, dword ptr [0x4b4514]
00413e4d  test       byte ptr [ecx + 0xc414], 4
00413e54  je         0x413f5d

// block 00413e5a
00413e5a  test       esi, esi
00413e5c  je         0x413f5d

// block 00413e62
00413e62  call       0x412780

// block 00413e67
00413e67  test       eax, eax
00413e69  je         0x413e80

// block 00413e6b
00413e6b  mov        eax, 0x88888889
00413e70  imul       esi
00413e72  add        edx, esi
00413e74  sar        edx, 4
00413e77  mov        eax, edx
00413e79  shr        eax, 0x1f
00413e7c  add        eax, edx
00413e7e  mov        esi, eax

// block 00413e80
00413e80  test       byte ptr [ebx + 0x16b8], 0x10
00413e87  jne        0x413e9f

// block 00413e89
00413e89  cmp        dword ptr [ebx + 0x1694], 0
00413e90  jg         0x413e9f

// block 00413e92
00413e92  lea        ecx, [ebx + 0x1608]
00413e98  mov        eax, esi
00413e9a  call       0x412050

// block 00413e9f
00413e9f  mov        ecx, dword ptr [ebx + 0x174c]
00413ea5  call       0x41a6f0

// block 00413eaa
00413eaa  mov        edi, eax
00413eac  test       edi, edi
00413eae  je         0x413f19

// block 00413eb0
00413eb0  push       0
00413eb2  lea        eax, [ebx + 0x26c]
00413eb8  call       0x4067e0

// block 00413ebd
00413ebd  mov        eax, dword ptr [ebx + 0x174c]
00413ec3  call       0x411df0

// block 00413ec8
00413ec8  mov        eax, dword ptr [ebx + 0x174c]
00413ece  call       0x411e40

// block 00413ed3
00413ed3  mov        esi, dword ptr [ebx + 0x174c]
00413ed9  mov        eax, dword ptr [esi + 0x102c]
00413edf  push       edi
00413ee0  call       0x4694f0

// block 00413ee5
00413ee5  fldz       
00413ee7  mov        ecx, dword ptr [esi + 4]
00413eea  mov        dword ptr [ecx + 4], eax
00413eed  mov        edx, dword ptr [esi + 4]
00413ef0  fstp       dword ptr [edx]
00413ef2  mov        eax, dword ptr [ebx + 0x278]
00413ef8  fld        dword ptr [eax]
00413efa  mov        edi, dword ptr [ebx + 0x174c]
00413f00  fstp       dword ptr [esp + 0x2c]
00413f04  push       ecx
00413f05  fld        dword ptr [esp + 0x30]
00413f09  fstp       dword ptr [esp]
00413f0c  call       0x468d90

// block 00413f11
00413f11  test       eax, eax
00413f13  jne        0x413c13

// block 00413f19
00413f19  mov        edx, dword ptr [ebx + 0x16b8]
00413f1f  xor        ecx, ecx
00413f21  cmp        dword ptr [ebx + 0x1608], ecx
00413f27  setle      cl
00413f2a  shr        edx, 7
00413f2d  not        edx
00413f2f  and        ecx, edx
00413f31  test       cl, 1
00413f34  je         0x413f53

// block 00413f36
00413f36  mov        esi, dword ptr [ebx + 0x174c]
00413f3c  call       0x414af0

// block 00413f41
00413f41  test       eax, eax
00413f43  je         0x413f53

// block 00413f45
00413f45  mov        eax, 1
00413f4a  pop        edi
00413f4b  pop        esi
00413f4c  pop        ebx
00413f4d  mov        esp, ebp
00413f4f  pop        ebp
00413f50  ret        4

// block 00413f53
00413f53  or         dword ptr [ebx + 0x16b8], 0x100000

// block 00413f5d
00413f5d  test       byte ptr [ebx + 0x161c], 2
00413f64  je         0x413f75

// block 00413f66
00413f66  mov        esi, dword ptr [ebx + 0x174c]
00413f6c  call       0x414af0

// block 00413f71
00413f71  test       eax, eax
00413f73  jne        0x413f45

// block 00413f75
00413f75  mov        ecx, dword ptr [ebx + 0x174c]
00413f7b  call       0x41a6f0

// block 00413f80
00413f80  mov        edi, eax
00413f82  test       edi, edi
00413f84  je         0x413fc8

// block 00413f86
00413f86  push       0
00413f88  lea        eax, [ebx + 0x26c]
00413f8e  call       0x4067e0

// block 00413f93
00413f93  mov        eax, dword ptr [ebx + 0x174c]
00413f99  call       0x411df0

// block 00413f9e
00413f9e  mov        eax, dword ptr [ebx + 0x174c]
00413fa4  call       0x411e40

// block 00413fa9
00413fa9  mov        esi, dword ptr [ebx + 0x174c]
00413faf  mov        eax, dword ptr [esi + 0x102c]
00413fb5  push       edi
00413fb6  call       0x4694f0

// block 00413fbb
00413fbb  fldz       
00413fbd  mov        ecx, dword ptr [esi + 4]
00413fc0  mov        dword ptr [ecx + 4], eax
00413fc3  mov        edx, dword ptr [esi + 4]
00413fc6  fstp       dword ptr [edx]

// block 00413fc8
00413fc8  mov        eax, dword ptr [ebx + 0x16b8]
00413fce  test       al, 0x22
00413fd0  jne        0x414048

// block 00413fd2
00413fd2  cmp        dword ptr [ebx + 0x16a8], 0
00413fd9  jg         0x414048

// block 00413fdb
00413fdb  test       eax, 0x2000000
00413fe0  jne        0x414048

// block 00413fe2
00413fe2  mov        eax, dword ptr [ebx + 0x1770]
00413fe8  test       eax, eax
00413fea  je         0x413ff2

// block 00413fec
00413fec  mov        ecx, ebx
00413fee  call       eax

// block 00413ff0
00413ff0  jmp        0x414048

// block 00413ff2
00413ff2  fld        dword ptr [ebx + 0xd8]
00413ff8  mov        eax, dword ptr [esp + 0x10]
00413ffc  fmul       qword ptr [0x4a3cf8]
00414002  push       ecx
00414003  fstp       dword ptr [esp + 0x30]
00414007  fld        dword ptr [esp + 0x30]
0041400b  fstp       dword ptr [esp]
0041400e  call       0x437980

// block 00414013
00414013  test       dword ptr [ebx + 0x16b8], 0x200
0041401d  je         0x414048

// block 0041401f
0041401f  cmp        eax, 2
00414022  jne        0x414048

// block 00414024
00414024  mov        eax, dword ptr [ebx + 0x270]
0041402a  cdq        
0041402b  mov        ecx, 3
00414030  idiv       ecx
00414032  test       edx, edx
00414034  jne        0x414048

// block 00414036
00414036  mov        edx, dword ptr [0x4b4514]
0041403c  add        edx, 0x97c
00414042  push       edx
00414043  call       0x4391c0

// block 00414048
00414048  test       dword ptr [ebx + 0x16b8], 0x80000
00414052  je         0x4141bb

// block 00414058
00414058  fld        dword ptr [ebx + 0x40]
0041405b  xor        edi, edi
0041405d  fcomp      qword ptr [0x4a4380]
00414063  fnstsw     ax
00414065  test       ah, 5
00414068  jp         0x41406f

// block 0041406a
0041406a  lea        eax, [edi - 1]
0041406d  jmp        0x414088

// block 0041406f
0041406f  fld        dword ptr [ebx + 0x40]
00414072  fcomp      qword ptr [0x4a4378]
00414078  fnstsw     ax
0041407a  test       ah, 0x41
0041407d  jne        0x414086

// block 0041407f
0041407f  mov        eax, 1
00414084  jmp        0x414088

// block 00414086
00414086  xor        eax, eax

// block 00414088
00414088  mov        ecx, dword ptr [ebx + 0x230]
0041408e  mov        dword ptr [esp + 0x18], eax
00414092  cmp        ecx, eax
00414094  je         0x4141bb

// block 0041409a
0041409a  cmp        ecx, -1
0041409d  je         0x4140c3

// block 0041409f
0041409f  test       ecx, ecx
004140a1  je         0x4140b6

// block 004140a3
004140a3  cmp        ecx, 1
004140a6  jne        0x4140cc

// block 004140a8
004140a8  mov        edi, eax
004140aa  neg        edi
004140ac  sbb        edi, edi
004140ae  and        edi, 0xfffffffd
004140b1  add        edi, 4
004140b4  jmp        0x4140cc

// block 004140b6
004140b6  xor        ecx, ecx
004140b8  cmp        eax, -1
004140bb  setne      cl
004140be  inc        ecx
004140bf  mov        edi, ecx
004140c1  jmp        0x4140cc

// block 004140c3
004140c3  mov        edi, eax
004140c5  neg        edi
004140c7  sbb        edi, edi
004140c9  add        edi, 3

// block 004140cc
004140cc  mov        esi, dword ptr [esp + 0xc]
004140d0  call       0x461c50

// block 004140d5
004140d5  mov        edx, dword ptr [eax + 0x3f8]
004140db  mov        dword ptr [esp + 0x2c], edx
004140df  call       0x461c50

// block 004140e4
004140e4  mov        ecx, dword ptr [eax + 0x424]
004140ea  mov        edx, dword ptr [eax + 0x428]
004140f0  mov        eax, dword ptr [eax + 0x42c]
004140f6  mov        dword ptr [esp + 0x5c], ecx
004140fa  mov        ecx, dword ptr [esp + 0x18]
004140fe  mov        dword ptr [esp + 0x60], edx
00414102  mov        dword ptr [ebx + 0x230], ecx
00414108  mov        edx, dword ptr [esi]
0041410a  push       edx
0041410b  mov        dword ptr [esp + 0x68], eax
0041410f  call       0x461a70

// block 00414114
00414114  mov        dword ptr [esi], 0
0041411a  mov        ebx, dword ptr [ebx + 0x22c]
00414120  add        ebx, edi
00414122  test       dword ptr [0x4cee78], 0x8000
0041412c  je         0x41413f

// block 0041412e
0041412e  push       0x4cf1d0
00414133  call       dword ptr [0x498088]

// block 00414139
00414139  inc        byte ptr [0x4cf221]

// block 0041413f
0041413f  mov        edi, dword ptr [esp + 0x2c]
00414143  inc        dword ptr [edi + 0x130]
00414149  call       0x4621c0

// block 0041414e
0041414e  mov        ecx, dword ptr [esp + 0x60]
00414152  mov        edx, dword ptr [esp + 0x64]
00414156  mov        esi, eax
00414158  mov        eax, dword ptr [esp + 0x5c]
0041415c  or         dword ptr [esi + 0x480], 1
00414163  mov        dword ptr [esi + 0x430], eax
00414169  push       ebx
0041416a  mov        dword ptr [esi + 0x434], ecx
00414170  lea        ebx, [esp + 0x60]
00414174  mov        eax, esi
00414176  mov        dword ptr [esi + 0x20], 8
0041417d  mov        dword ptr [esi + 0x438], edx
00414183  call       0x454df0

// block 00414188
00414188  mov        ebx, esi
0041418a  lea        eax, [esp + 0x30]
0041418e  call       0x461250

// block 00414193
00414193  test       dword ptr [0x4cee78], 0x8000
0041419d  mov        esi, dword ptr [eax]
0041419f  je         0x4141b2

// block 004141a1
004141a1  push       0x4cf1d0
004141a6  call       dword ptr [0x49808c]

// block 004141ac
004141ac  dec        byte ptr [0x4cf221]

// block 004141b2
004141b2  mov        eax, dword ptr [esp + 0xc]
004141b6  mov        ebx, dword ptr [ebp + 8]
004141b9  mov        dword ptr [eax], esi

// block 004141bb
004141bb  test       dword ptr [ebx + 0x16b8], 0x2000000
004141c5  fld        qword ptr [0x4a3d70]
004141cb  fld        qword ptr [0x4a3d68]
004141d1  jne        0x4142f3

// block 004141d7
004141d7  fld        qword ptr [0x4a3d28]
004141dd  lea        ecx, [ebx + 0x1e0]
004141e3  lea        edx, [ebx + 0x128]
004141e9  mov        dword ptr [esp + 0x14], ecx
004141ed  mov        dword ptr [esp + 0x18], edx
004141f1  mov        dword ptr [esp + 0x24], 0xe

// block 004141f9
004141f9  mov        esi, dword ptr [esp + 0x14]
004141fd  mov        eax, dword ptr [esi - 0x100]
00414203  mov        edx, dword ptr [0x4ce8cc]
00414209  push       eax
0041420a  call       0x461920

// block 0041420f
0041420f  mov        edi, eax
00414211  test       edi, edi
00414213  jne        0x414220

// block 00414215
00414215  mov        dword ptr [esi - 0x100], eax
0041421b  jmp        0x4142d6

// block 00414220
00414220  mov        ecx, dword ptr [esp + 0x18]
00414224  mov        eax, dword ptr [esp + 0x10]
00414228  fld        dword ptr [ecx - 8]
0041422b  fadd       dword ptr [eax]
0041422d  fstp       dword ptr [esp + 0x44]
00414231  fld        dword ptr [eax + 4]
00414234  fadd       dword ptr [ecx - 4]
00414237  fstp       dword ptr [esp + 0x48]
0041423b  fld        dword ptr [eax + 8]
0041423e  fadd       dword ptr [ecx]
00414240  mov        ecx, dword ptr [esp + 0x14]
00414244  mov        esi, dword ptr [ecx]
00414246  fstp       dword ptr [esp + 0x4c]
0041424a  test       esi, esi
0041424c  jl         0x414296

// block 0041424e
0041424e  mov        edx, dword ptr [ebx + esi*4 + 0xe0]
00414255  push       edx
00414256  mov        edx, dword ptr [0x4ce8cc]
0041425c  call       0x461920

// block 00414261
00414261  test       eax, eax
00414263  jne        0x41426c

// block 00414265
00414265  mov        dword ptr [ebx + esi*4 + 0xe0], eax

// block 0041426c
0041426c  fld        dword ptr [esp + 0x44]
00414270  fadd       dword ptr [eax + 0x424]
00414276  fstp       dword ptr [esp + 0x44]
0041427a  fld        dword ptr [eax + 0x428]
00414280  fadd       dword ptr [esp + 0x48]
00414284  fstp       dword ptr [esp + 0x48]
00414288  fld        dword ptr [eax + 0x42c]
0041428e  fadd       dword ptr [esp + 0x4c]
00414292  fstp       dword ptr [esp + 0x4c]

// block 00414296
00414296  fld        dword ptr [esp + 0x44]
0041429a  fadd       st(3)
0041429c  fadd       st(1)
0041429e  fstp       dword ptr [edi + 0x430]
004142a4  fld        dword ptr [esp + 0x48]
004142a8  fadd       st(2)
004142aa  fstp       dword ptr [edi + 0x434]
004142b0  fld        dword ptr [esp + 0x4c]
004142b4  fstp       dword ptr [edi + 0x438]
004142ba  mov        eax, dword ptr [edi + 0x47c]
004142c0  test       eax, 0x20000000
004142c5  je         0x4142d6

// block 004142c7
004142c7  fld        dword ptr [ebx + 0x50]
004142ca  or         eax, 4
004142cd  fstp       dword ptr [edi + 0x2c]
004142d0  mov        dword ptr [edi + 0x47c], eax

// block 004142d6
004142d6  add        dword ptr [esp + 0x18], 0xc
004142db  add        dword ptr [esp + 0x14], 4
004142e0  sub        dword ptr [esp + 0x24], 1
004142e5  jne        0x4141f9

// block 004142eb
004142eb  fstp       st(0)
004142ed  fstp       st(0)
004142ef  fstp       st(0)
004142f1  jmp        0x414338

// block 004142f3
004142f3  mov        edi, dword ptr [esp + 0xc]
004142f7  fstp       st(0)
004142f9  fstp       st(0)
004142fb  mov        esi, 0xe

// block 00414300
00414300  mov        eax, dword ptr [edi]
00414302  mov        edx, dword ptr [0x4ce8cc]
00414308  push       eax
00414309  call       0x461920

// block 0041430e
0041430e  test       eax, eax
00414310  je         0x414330

// block 00414312
00414312  mov        ecx, dword ptr [esp + 0x10]
00414316  mov        edx, dword ptr [ecx]
00414318  mov        dword ptr [eax + 0x430], edx
0041431e  mov        edx, dword ptr [ecx + 4]
00414321  mov        dword ptr [eax + 0x434], edx
00414327  mov        ecx, dword ptr [ecx + 8]
0041432a  mov        dword ptr [eax + 0x438], ecx

// block 00414330
00414330  add        edi, 4
00414333  sub        esi, 1
00414336  jne        0x414300

// block 00414338
00414338  mov        eax, dword ptr [ebx + 0x16b8]
0041433e  test       al, 0x21
00414340  jne        0x4143b0

// block 00414342
00414342  test       eax, 0x6000000
00414347  jne        0x4143b0

// block 00414349
00414349  mov        edi, dword ptr [0x4b4514]
0041434f  mov        eax, dword ptr [edi + 0x8980]
00414355  test       eax, eax
00414357  je         0x41439e

// block 00414359
00414359  fld        dword ptr [eax + 0x1074]
0041435f  mov        edx, dword ptr [esp + 0x10]
00414363  fsub       dword ptr [edi + 0x97c]
00414369  fstp       dword ptr [esp + 0x2c]
0041436d  fld        dword ptr [esp + 0x2c]
00414371  fabs       
00414373  fstp       dword ptr [esp + 0x2c]
00414377  fld        dword ptr [esp + 0x2c]
0041437b  fld        dword ptr [edx]
0041437d  fsub       dword ptr [edi + 0x97c]
00414383  fstp       dword ptr [esp + 0x2c]
00414387  fld        dword ptr [esp + 0x2c]
0041438b  fabs       
0041438d  fstp       dword ptr [esp + 0x2c]
00414391  fld        dword ptr [esp + 0x2c]
00414395  fcompp     
00414397  fnstsw     ax
00414399  test       ah, 0x41
0041439c  jne        0x4143b0

// block 0041439e
0041439e  mov        ecx, dword ptr [ebx + 0x174c]
004143a4  call       0x412660

// block 004143a9
004143a9  mov        byte ptr [edi + 0x8984], 1

// block 004143b0
004143b0  mov        esi, dword ptr [esp + 0xc]
004143b4  mov        eax, dword ptr [esi]
004143b6  mov        edx, dword ptr [0x4ce8cc]
004143bc  push       eax
004143bd  call       0x461920

// block 004143c2
004143c2  test       eax, eax
004143c4  jne        0x4143cd

// block 004143c6
004143c6  mov        dword ptr [esi], eax
004143c8  jmp        0x41457e

// block 004143cd
004143cd  cmp        dword ptr [ebx + 0x1684], 0
004143d4  mov        edx, 0xfffeffff
004143d9  jne        0x41455b

// block 004143df
004143df  test       dword ptr [ebx + 0x16b8], 0x40000000
004143e9  mov        edi, 0x10000
004143ee  je         0x41441d

// block 004143f0
004143f0  mov        ecx, dword ptr [ebx + 0x270]
004143f6  and        ecx, 0x80000003
004143fc  jns        0x414403

// block 004143fe
004143fe  dec        ecx
004143ff  or         ecx, 0xfffffffc
00414402  inc        ecx

// block 00414403
00414403  jne        0x414417

// block 00414405
00414405  or         dword ptr [eax + 0x47c], edi
0041440b  mov        dword ptr [eax + 0x3c0], 0xffff00ff
00414415  jmp        0x41441d

// block 00414417
00414417  and        dword ptr [eax + 0x47c], edx

// block 0041441d
0041441d  mov        ecx, dword ptr [ebx + 0x16b8]
00414423  test       ecx, 0x100000
00414429  je         0x4144e6

// block 0041442f
0041442f  or         dword ptr [eax + 0x47c], edi
00414435  mov        dword ptr [eax + 0x3c0], 0xff0000ff
0041443f  mov        eax, dword ptr [ebx + 0x168c]
00414445  mov        dword ptr [ebx + 0x1684], 4
0041444f  test       eax, eax
00414451  jge        0x4144d2

// block 00414453
00414453  test       dword ptr [ebx + 0x16b8], 0x20400000
0041445d  je         0x4144b9

// block 0041445f
0041445f  mov        edx, dword ptr [0x4b43cc]
00414465  mov        ecx, dword ptr [edx + 0x7c]
00414468  mov        eax, ecx
0041446a  and        eax, 1
0041446d  je         0x414474

// block 0041446f
0041446f  test       cl, 8
00414472  jne        0x4144b9

// block 00414474
00414474  test       eax, eax
00414476  je         0x41448e

// block 00414478
00414478  mov        ecx, dword ptr [ebx + 0x174c]
0041447e  cmp        dword ptr [ecx + 0x2650], 0xc8
00414488  jl         0x4144a0

// block 0041448a
0041448a  test       eax, eax
0041448c  jne        0x4144b9

// block 0041448e
0041448e  mov        edx, dword ptr [ebx + 0x174c]
00414494  cmp        dword ptr [edx + 0x2650], 0x384
0041449e  jge        0x4144b9

// block 004144a0
004144a0  mov        eax, dword ptr [esp + 0x10]
004144a4  fld        dword ptr [eax]
004144a6  push       ecx
004144a7  mov        eax, 0x24
004144ac  fstp       dword ptr [esp]
004144af  call       0x453e20

// block 004144b4
004144b4  jmp        0x41457e

// block 004144b9
004144b9  push       ecx
004144ba  mov        ecx, dword ptr [esp + 0x14]
004144be  fld        dword ptr [ecx]
004144c0  mov        eax, 0x23
004144c5  fstp       dword ptr [esp]
004144c8  call       0x453e20

// block 004144cd
004144cd  jmp        0x41457e

// block 004144d2
004144d2  mov        edx, dword ptr [esp + 0x10]
004144d6  fld        dword ptr [edx]
004144d8  push       ecx
004144d9  fstp       dword ptr [esp]
004144dc  call       0x453e20

// block 004144e1
004144e1  jmp        0x41457e

// block 004144e6
004144e6  mov        esi, dword ptr [ebx + 0x270]
004144ec  and        esi, 0x80000003
004144f2  jns        0x4144f9

// block 004144f4
004144f4  dec        esi
004144f5  or         esi, 0xfffffffc
004144f8  inc        esi

// block 004144f9
004144f9  jne        0x414553

// block 004144fb
004144fb  test       ecx, 0x20400000
00414501  je         0x41457e

// block 00414503
00414503  mov        ecx, dword ptr [0x4b43cc]
00414509  mov        ecx, dword ptr [ecx + 0x7c]
0041450c  mov        esi, ecx
0041450e  and        esi, 1
00414511  je         0x414518

// block 00414513
00414513  test       cl, 8
00414516  jne        0x41457e

// block 00414518
00414518  test       esi, esi
0041451a  je         0x41452f

// block 0041451c
0041451c  mov        edx, dword ptr [ebx + 0x174c]
00414522  cmp        dword ptr [edx + 0x2650], 0x64
00414529  jl         0x414541

// block 0041452b
0041452b  test       esi, esi
0041452d  jne        0x41457e

// block 0041452f
0041452f  mov        ecx, dword ptr [ebx + 0x174c]
00414535  cmp        dword ptr [ecx + 0x2650], 0x1f4
0041453f  jge        0x41457e

// block 00414541
00414541  or         dword ptr [eax + 0x47c], edi
00414547  mov        dword ptr [eax + 0x3c0], 0xff0000ff
00414551  jmp        0x41457e

// block 00414553
00414553  and        dword ptr [eax + 0x47c], edx
00414559  jmp        0x41457e

// block 0041455b
0041455b  and        dword ptr [eax + 0x47c], edx
00414561  test       dword ptr [ebx + 0x16b8], 0x400000
0041456b  je         0x414578

// block 0041456d
0041456d  mov        eax, dword ptr [0x4b43e4]
00414572  and        dword ptr [eax + 0x6c08], edx

// block 00414578
00414578  dec        dword ptr [ebx + 0x1684]

// block 0041457e
0041457e  mov        ecx, dword ptr [ebx + 0x1750]
00414584  test       ecx, ecx
00414586  je         0x414a29

// block 0041458c
0041458c  mov        eax, dword ptr [ebx + 0x1760]
00414592  fld        dword ptr [ebx + 0x1758]
00414598  mov        edx, dword ptr [ecx + 0x14]
0041459b  fstp       dword ptr [esp + 0x28]
0041459f  mov        dword ptr [esp + 0x20], eax
004145a3  mov        eax, dword ptr [0x4b43cc]
004145a8  cmp        dword ptr [eax + 0x78], 0x6f
004145ac  mov        dword ptr [esp + 0x18], edx
004145b0  mov        edx, dword ptr [ebx + 0x1764]
004145b6  mov        dword ptr [esp + 0x24], edx
004145ba  jne        0x4145c0

// block 004145bc
004145bc  fld1       
004145be  jmp        0x4145c6

// block 004145c0
004145c0  fld        dword ptr [0x4a3e54]

// block 004145c6
004145c6  fstp       dword ptr [esp + 0x1c]
004145ca  fld        dword ptr [ebx + 0x1758]
004145d0  fld        dword ptr [ebx + 0x1754]
004145d6  fcompp     
004145d8  fnstsw     ax
004145da  fld        qword ptr [0x4a3d00]
004145e0  test       ah, 0x41
004145e3  jne        0x4145f3

// block 004145e5
004145e5  fld        dword ptr [ebx + 0x1758]
004145eb  fadd       st(1)
004145ed  fstp       dword ptr [ebx + 0x1758]

// block 004145f3
004145f3  fld        dword ptr [0x4a3d78]
004145f9  mov        byte ptr [esp + 0xc], 0xff
004145fe  fcomp      dword ptr [ebx + 0x1758]
00414604  fnstsw     ax
00414606  test       ah, 0x41
00414609  jne        0x414612

// block 0041460b
0041460b  mov        byte ptr [esp + 0xc], 0
00414610  jmp        0x414663

// block 00414612
00414612  fld        dword ptr [0x4a3e04]
00414618  fcomp      dword ptr [ebx + 0x1758]
0041461e  fnstsw     ax
00414620  test       ah, 0x41
00414623  jne        0x414663

// block 00414625
00414625  fld        dword ptr [ebx + 0x1758]
0041462b  fsub       qword ptr [0x4a3d70]
00414631  fnstcw     word ptr [esp + 0xc]
00414635  movzx      eax, word ptr [esp + 0xc]
0041463a  fmul       qword ptr [0x4a3ed0]
00414640  or         eax, 0xc00
00414645  mov        dword ptr [esp + 0x2c], eax
00414649  fmul       qword ptr [0x4a4188]
0041464f  fldcw      word ptr [esp + 0x2c]
00414653  fistp      dword ptr [esp + 0x2c]
00414657  mov        dl, byte ptr [esp + 0x2c]
0041465b  fldcw      word ptr [esp + 0xc]
0041465f  mov        byte ptr [esp + 0xc], dl

// block 00414663
00414663  fld        dword ptr [esp + 0x28]
00414667  mov        esi, dword ptr [esp + 0x10]
0041466b  mov        edx, dword ptr [esi + 4]
0041466e  fld        st(0)
00414670  fmulp      st(2)
00414672  mov        eax, dword ptr [esi]
00414674  fxch       st(1)
00414676  sub        esp, 0x10
00414679  mov        dword ptr [esp + 0x64], edx
0041467d  fadd       qword ptr [0x4a41c0]
00414683  mov        dword ptr [esp + 0x60], eax
00414687  mov        eax, dword ptr [esi + 8]
0041468a  mov        dword ptr [esp + 0x68], eax
0041468e  fstp       dword ptr [esp + 0x3c]
00414692  mov        eax, ecx
00414694  fld        dword ptr [esp + 0x3c]
00414698  fst        dword ptr [esp + 0xc]
0041469c  fstp       dword ptr [esp + 8]
004146a0  fld        dword ptr [esp + 0x64]
004146a4  fsub       st(1)
004146a6  fld        qword ptr [0x4a4170]
004146ac  fsub       st(1), st(0)
004146ae  fxch       st(1)
004146b0  fstp       dword ptr [esp + 0x3c]
004146b4  fld        dword ptr [esp + 0x3c]
004146b8  fstp       dword ptr [esp + 4]
004146bc  fld        dword ptr [esp + 0x60]
004146c0  fsubrp     st(2)
004146c2  fsubp      st(1)
004146c4  fstp       dword ptr [esp + 0x3c]
004146c8  fld        dword ptr [esp + 0x3c]
004146cc  fstp       dword ptr [esp]
004146cf  call       0x410020

// block 004146d4
004146d4  fld        dword ptr [esi]
004146d6  mov        eax, dword ptr [ebx + 0x1750]
004146dc  cmp        dword ptr [eax], 0
004146df  fadd       qword ptr [0x4a3d70]
004146e5  mov        dword ptr [esp + 0x2c], 0
004146ed  fadd       qword ptr [0x4a3d28]
004146f3  fstp       dword ptr [esp + 0x50]
004146f7  fld        dword ptr [esi + 4]
004146fa  fadd       qword ptr [0x4a3d68]
00414700  fstp       dword ptr [esp + 0x54]
00414704  fld        dword ptr [esi + 8]
00414707  mov        esi, dword ptr [eax + 0x10]
0041470a  fstp       dword ptr [esp + 0x58]
0041470e  jle        0x4149e3

// block 00414714
00414714  mov        ecx, dword ptr [ebx + 0x1750]
0041471a  xor        edi, edi
0041471c  cmp        dword ptr [ecx + 4], edi
0041471f  jle        0x4149cc

// block 00414725
00414725  fld        dword ptr [esp + 0x28]
00414729  fmul       st(0), st(0)
0041472b  fst        qword ptr [esp + 0x38]
0041472f  jmp        0x414735

// block 00414731
00414731  fld        qword ptr [esp + 0x38]

// block 00414735
00414735  mov        eax, dword ptr [esp + 0x18]
00414739  fld        dword ptr [eax]
0041473b  fsub       dword ptr [esp + 0x50]
0041473f  fstp       dword ptr [esp + 0x5c]
00414743  mov        edx, dword ptr [esp + 0x5c]
00414747  fld        dword ptr [eax + 4]
0041474a  fsub       dword ptr [esp + 0x54]
0041474e  fstp       dword ptr [esp + 0x60]
00414752  fld        dword ptr [eax + 8]
00414755  mov        eax, dword ptr [esp + 0x60]
00414759  fsub       dword ptr [esp + 0x58]
0041475d  mov        dword ptr [esp + 0x48], eax
00414761  mov        dword ptr [esp + 0x44], edx
00414765  fstp       dword ptr [esp + 0x64]
00414769  mov        ecx, dword ptr [esp + 0x64]
0041476d  fld        dword ptr [esp + 0x60]
00414771  mov        dword ptr [esp + 0x4c], ecx
00414775  fld        dword ptr [esp + 0x5c]
00414779  fmul       st(0), st(0)
0041477b  fld        st(1)
0041477d  fmulp      st(2)
0041477f  faddp      st(1)
00414781  fstp       dword ptr [esp + 0x14]
00414785  fld        dword ptr [esp + 0x14]
00414789  fsubr      st(1)
0041478b  fstp       dword ptr [esp + 0x14]
0041478f  fldz       
00414791  fld        dword ptr [esp + 0x14]
00414795  fcom       st(1)
00414797  fnstsw     ax
00414799  fstp       st(1)
0041479b  test       ah, 1
0041479e  jne        0x41496e

// block 004147a4
004147a4  fdivrp     st(1)
004147a6  mov        edx, dword ptr [ebx + 0x175c]
004147ac  mov        dword ptr [esi + 0x10], edx
004147af  movzx      eax, byte ptr [esi + 0x12]
004147b3  mov        ecx, 0xff
004147b8  sub        ecx, eax
004147ba  mov        dword ptr [esp + 0x10], ecx
004147be  mov        ecx, 0xff
004147c3  fstp       dword ptr [esp + 0x14]
004147c7  fild       dword ptr [esp + 0x10]
004147cb  fld        dword ptr [esp + 0x14]
004147cf  fld        st(0)
004147d1  fmulp      st(2)
004147d3  fld        qword ptr [0x4a3ed0]
004147d9  fld        st(0)
004147db  fnstcw     word ptr [esp + 0x10]
004147df  fsubrp     st(3)
004147e1  movzx      eax, word ptr [esp + 0x10]
004147e6  fxch       st(2)
004147e8  or         eax, 0xc00
004147ed  mov        dword ptr [esp + 0x30], eax
004147f1  movzx      eax, byte ptr [esi + 0x11]
004147f5  fldcw      word ptr [esp + 0x30]
004147f9  sub        ecx, eax
004147fb  fistp      dword ptr [esp + 0x30]
004147ff  movzx      edx, byte ptr [esp + 0x30]
00414804  mov        dword ptr [esp + 0x30], ecx
00414808  mov        byte ptr [esi + 0x12], dl
0041480b  mov        ecx, 0xff
00414810  fldcw      word ptr [esp + 0x10]
00414814  fild       dword ptr [esp + 0x30]
00414818  fnstcw     word ptr [esp + 0x10]
0041481c  fmul       st(1)
0041481e  movzx      eax, word ptr [esp + 0x10]
00414823  or         eax, 0xc00
00414828  mov        dword ptr [esp + 0x30], eax
0041482c  movzx      eax, byte ptr [esi + 0x10]
00414830  fsubr      st(2)
00414832  fldcw      word ptr [esp + 0x30]
00414836  sub        ecx, eax
00414838  fistp      dword ptr [esp + 0x30]
0041483c  movzx      edx, byte ptr [esp + 0x30]
00414841  mov        dword ptr [esp + 0x30], ecx
00414845  mov        byte ptr [esi + 0x11], dl
00414848  fldcw      word ptr [esp + 0x10]
0041484c  fild       dword ptr [esp + 0x30]
00414850  fnstcw     word ptr [esp + 0x10]
00414854  fmul       st(1)
00414856  movzx      eax, word ptr [esp + 0x10]
0041485b  or         eax, 0xc00
00414860  mov        dword ptr [esp + 0x30], eax
00414864  mov        eax, dword ptr [0x4b43cc]
00414869  fsubp      st(2)
0041486b  fldcw      word ptr [esp + 0x30]
0041486f  fxch       st(1)
00414871  fistp      dword ptr [esp + 0x30]
00414875  movzx      edx, byte ptr [esp + 0x30]
0041487a  mov        byte ptr [esi + 0x10], dl
0041487d  cmp        dword ptr [eax + 0x78], 0x6f
00414881  fldcw      word ptr [esp + 0x10]
00414885  jne        0x4148cd

// block 00414887
00414887  movzx      ecx, byte ptr [esp + 0xc]
0041488c  fnstcw     word ptr [esp + 0x10]
00414890  movzx      eax, word ptr [esp + 0x10]
00414895  fld        st(0)
00414897  mov        dword ptr [esp + 0x30], ecx
0041489b  or         eax, 0xc00
004148a0  fild       dword ptr [esp + 0x30]
004148a4  mov        dword ptr [esp + 0x30], eax
004148a8  lea        eax, [esp + 0x44]
004148ac  push       eax
004148ad  fmulp      st(1)
004148af  mov        ecx, eax
004148b1  fldcw      word ptr [esp + 0x34]
004148b5  push       ecx
004148b6  fistp      dword ptr [esp + 0x38]
004148ba  mov        dl, byte ptr [esp + 0x38]
004148be  mov        byte ptr [esi + 0x13], dl
004148c1  fldcw      word ptr [esp + 0x18]
004148c5  fmul       qword ptr [0x4a4108]
004148cb  jmp        0x4148df

// block 004148cd
004148cd  fmul       qword ptr [0x4a3d70]
004148d3  lea        edx, [esp + 0x44]
004148d7  push       edx
004148d8  mov        eax, edx
004148da  mov        byte ptr [esi + 0x13], 0xff
004148de  push       eax

// block 004148df
004148df  fstp       dword ptr [esp + 0x38]
004148e3  call       0x46c6bc

// block 004148e8
004148e8  fld        dword ptr [esp + 0x44]
004148ec  fld        dword ptr [esp + 0x30]
004148f0  fld        st(0)
004148f2  fmulp      st(2)
004148f4  fxch       st(1)
004148f6  fstp       dword ptr [esp + 0x44]
004148fa  fld        dword ptr [esp + 0x48]
004148fe  fmul       st(1)
00414900  fstp       dword ptr [esp + 0x48]
00414904  fmul       dword ptr [esp + 0x4c]
00414908  fstp       dword ptr [esp + 0x4c]
0041490c  fld        dword ptr [esp + 0x20]
00414910  call       0x4938c0

// block 00414915
00414915  fstp       dword ptr [esp + 0x30]
00414919  fld        dword ptr [esp + 0x30]
0041491d  fmul       dword ptr [esp + 0x14]
00414921  fmul       dword ptr [esp + 0x1c]
00414925  fadd       dword ptr [esp + 0x44]
00414929  fstp       dword ptr [esp + 0x44]
0041492d  fld        dword ptr [esp + 0x24]
00414931  call       0x4938c0

// block 00414936
00414936  fstp       dword ptr [esp + 0x30]
0041493a  fld        dword ptr [esp + 0x30]
0041493e  mov        ecx, dword ptr [esp + 0x18]
00414942  fmul       dword ptr [esp + 0x14]
00414946  fmul       dword ptr [esp + 0x1c]
0041494a  fadd       dword ptr [esp + 0x48]
0041494e  fstp       dword ptr [esp + 0x48]
00414952  fld        dword ptr [esp + 0x44]
00414956  fadd       dword ptr [esi]
00414958  fstp       dword ptr [esi]
0041495a  fld        dword ptr [esi + 4]
0041495d  fadd       dword ptr [esp + 0x48]
00414961  fstp       dword ptr [esi + 4]
00414964  fldz       
00414966  fst        dword ptr [esi + 8]
00414969  fstp       dword ptr [ecx + 8]
0041496c  jmp        0x414976

// block 0041496e
0041496e  fstp       st(0)
00414970  mov        byte ptr [esi + 0x13], 0
00414974  fstp       st(0)

// block 00414976
00414976  fld        dword ptr [esp + 0x20]
0041497a  push       ecx
0041497b  fadd       qword ptr [0x4a4370]
00414981  fstp       dword ptr [esp + 0x34]
00414985  fld        dword ptr [esp + 0x34]
00414989  fstp       dword ptr [esp]
0041498c  call       0x4646e0

// block 00414991
00414991  fstp       dword ptr [esp + 0x20]
00414995  push       ecx
00414996  fld        dword ptr [esp + 0x28]
0041499a  fsub       qword ptr [0x4a3e30]
004149a0  fstp       dword ptr [esp + 0x34]
004149a4  fld        dword ptr [esp + 0x34]
004149a8  fstp       dword ptr [esp]
004149ab  call       0x4646e0

// block 004149b0
004149b0  mov        edx, dword ptr [ebx + 0x1750]
004149b6  fstp       dword ptr [esp + 0x24]
004149ba  add        dword ptr [esp + 0x18], 0xc
004149bf  inc        edi
004149c0  add        esi, 0x1c
004149c3  cmp        edi, dword ptr [edx + 4]
004149c6  jl         0x414731

// block 004149cc
004149cc  mov        eax, dword ptr [esp + 0x2c]
004149d0  mov        ecx, dword ptr [ebx + 0x1750]
004149d6  inc        eax
004149d7  cmp        eax, dword ptr [ecx]
004149d9  mov        dword ptr [esp + 0x2c], eax
004149dd  jl         0x414714

// block 004149e3
004149e3  fld        dword ptr [ebx + 0x1760]
004149e9  push       ecx
004149ea  fadd       qword ptr [0x4a3fe0]
004149f0  fstp       dword ptr [esp + 0x34]
004149f4  fld        dword ptr [esp + 0x34]
004149f8  fstp       dword ptr [esp]
004149fb  call       0x4646e0

// block 00414a00
00414a00  fstp       dword ptr [ebx + 0x1760]
00414a06  push       ecx
00414a07  fld        dword ptr [ebx + 0x1764]
00414a0d  fadd       qword ptr [0x4a4370]
00414a13  fstp       dword ptr [esp + 0x34]
00414a17  fld        dword ptr [esp + 0x34]
00414a1b  fstp       dword ptr [esp]
00414a1e  call       0x4646e0

// block 00414a23
00414a23  fstp       dword ptr [ebx + 0x1764]

// block 00414a29
00414a29  cmp        dword ptr [ebx + 0x1694], 0
00414a30  jle        0x414a47

// block 00414a32
00414a32  fld        dword ptr [0x4a3da8]
00414a38  push       ecx
00414a39  lea        esi, [ebx + 0x1690]
00414a3f  fstp       dword ptr [esp]
00414a42  call       0x464a20

// block 00414a47
00414a47  cmp        dword ptr [ebx + 0x16a8], 0
00414a4e  jle        0x414a65

// block 00414a50
00414a50  fld        dword ptr [0x4a3da8]
00414a56  push       ecx
00414a57  lea        esi, [ebx + 0x16a4]
00414a5d  fstp       dword ptr [esp]
00414a60  call       0x464a20

// block 00414a65
00414a65  mov        edx, dword ptr [ebx + 0x270]
00414a6b  mov        ecx, dword ptr [ebx + 0x278]
00414a71  mov        dword ptr [ebx + 0x26c], edx
00414a77  fld        dword ptr [ecx]
00414a79  fcomp      qword ptr [0x4a3db0]
00414a7f  fnstsw     ax
00414a81  test       ah, 0x41
00414a84  jne        0x414ab9

// block 00414a86
00414a86  fld        dword ptr [0x4a3dac]
00414a8c  fcomp      dword ptr [ecx]
00414a8e  fnstsw     ax
00414a90  test       ah, 0x41
00414a93  jne        0x414ab9

// block 00414a95
00414a95  fld        dword ptr [ebx + 0x274]
00414a9b  inc        edx
00414a9c  fadd       qword ptr [0x4a3ce0]
00414aa2  mov        dword ptr [ebx + 0x270], edx
00414aa8  xor        eax, eax
00414aaa  fstp       dword ptr [ebx + 0x274]
00414ab0  pop        edi
00414ab1  pop        esi
00414ab2  pop        ebx
00414ab3  mov        esp, ebp
00414ab5  pop        ebp
00414ab6  ret        4

// block 00414ab9
00414ab9  fld        dword ptr [ecx]
00414abb  fadd       dword ptr [ebx + 0x274]
00414ac1  fstp       dword ptr [esp + 0x30]
00414ac5  fld        dword ptr [esp + 0x30]
00414ac9  fst        dword ptr [ebx + 0x274]
00414acf  call       0x4931e0

// block 00414ad4
00414ad4  mov        dword ptr [ebx + 0x270], eax

// block 00414ada
00414ada  pop        edi
00414adb  pop        esi
00414adc  xor        eax, eax
00414ade  pop        ebx
00414adf  mov        esp, ebp
00414ae1  pop        ebp
00414ae2  ret        4
