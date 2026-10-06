
// block 00425c20
00425c20  push       ebp
00425c21  mov        ebp, esp
00425c23  and        esp, 0xfffffff8
00425c26  sub        esp, 0x8c
00425c2c  mov        eax, dword ptr [ebp + 8]
00425c2f  push       ebx
00425c30  xor        ecx, ecx
00425c32  push       esi
00425c33  push       edi
00425c34  lea        edi, [eax + 0x14]
00425c37  mov        dword ptr [eax + 0x666fdc], ecx
00425c3d  mov        dword ptr [eax + 0x666fd4], ecx
00425c43  mov        dword ptr [esp + 0x10], ecx

// block 00425c47
00425c47  mov        eax, dword ptr [edi + 0x9b0]
00425c4d  xor        ebx, ebx
00425c4f  cmp        eax, ebx
00425c51  je         0x426f53

// block 00425c57
00425c57  cmp        eax, 5
00425c5a  jne        0x425cb3

// block 00425c5c
00425c5c  add        dword ptr [edi + 0x9c0], -1
00425c63  jns        0x426f53

// block 00425c69
00425c69  mov        eax, dword ptr [0x4b43c8]
00425c6e  mov        ebx, dword ptr [edi + 0x9b4]
00425c74  mov        dword ptr [edi + 0x9b0], 2
00425c7e  mov        ecx, dword ptr [eax + 0x4debdc]
00425c84  mov        esi, edi
00425c86  add        ebx, 0xa5
00425c8c  mov        dword ptr [esp + 0xc], ecx
00425c90  call       0x402520

// block 00425c95
00425c95  mov        ecx, dword ptr [esp + 0xc]
00425c99  mov        al, 0x10
00425c9b  push       ebx
00425c9c  push       edi
00425c9d  mov        byte ptr [edi + 0x49d], al
00425ca3  mov        byte ptr [edi + 0x49c], al
00425ca9  call       0x454d10

// block 00425cae
00425cae  jmp        0x426f53

// block 00425cb3
00425cb3  cmp        eax, 1
00425cb6  jne        0x425e0c

// block 00425cbc
00425cbc  mov        edx, dword ptr [0x4b4534]
00425cc2  cmp        dword ptr [edx + 0x2c], ebx
00425cc5  je         0x425cd6

// block 00425cc7
00425cc7  mov        eax, edi
00425cc9  call       0x4271c0

// block 00425cce
00425cce  cmp        eax, ebx
00425cd0  jne        0x426879

// block 00425cd6
00425cd6  mov        ecx, dword ptr [0x4b4514]
00425cdc  mov        eax, dword ptr [ecx + 0xa28]
00425ce2  cmp        eax, 2
00425ce5  je         0x425d1b

// block 00425ce7
00425ce7  cmp        eax, 4
00425cea  je         0x425d1b

// block 00425cec
00425cec  cmp        dword ptr [edi + 0x98c], 0x28
00425cf3  jge        0x425d08

// block 00425cf5
00425cf5  fld        dword ptr [0x4a3db8]
00425cfb  fcomp      dword ptr [ecx + 0x980]
00425d01  fnstsw     ax
00425d03  test       ah, 0x41
00425d06  jne        0x425d1b

// block 00425d08
00425d08  fld        dword ptr [0x4a3db8]
00425d0e  fcomp      dword ptr [ecx + 0x980]
00425d14  fnstsw     ax
00425d16  test       ah, 0x41
00425d19  je         0x425d28

// block 00425d1b
00425d1b  mov        eax, dword ptr [0x4b43e4]
00425d20  cmp        dword ptr [eax + 0x6d30], ebx
00425d26  je         0x425d36

// block 00425d28
00425d28  mov        ecx, dword ptr [ecx + 0xa2c]
00425d2e  fld        dword ptr [ecx + 8]
00425d31  jmp        0x425eab

// block 00425d36
00425d36  fld        dword ptr [edi + 0x978]
00425d3c  fld        dword ptr [0x4b2ed0]
00425d42  fld        st(0)
00425d44  fmulp      st(2)
00425d46  fxch       st(1)
00425d48  fstp       dword ptr [esp + 0x14]
00425d4c  fld        dword ptr [edi + 0x97c]
00425d52  fmul       st(1)
00425d54  fstp       dword ptr [esp + 0x18]
00425d58  fmul       dword ptr [edi + 0x980]
00425d5e  fstp       dword ptr [esp + 0x1c]
00425d62  fld        dword ptr [edi + 0x96c]
00425d68  fadd       dword ptr [esp + 0x14]
00425d6c  fstp       dword ptr [edi + 0x96c]
00425d72  fld        dword ptr [esp + 0x18]
00425d76  fadd       dword ptr [edi + 0x970]
00425d7c  fstp       dword ptr [edi + 0x970]
00425d82  fld        dword ptr [esp + 0x1c]
00425d86  fadd       dword ptr [edi + 0x974]
00425d8c  fstp       dword ptr [edi + 0x974]
00425d92  fld        dword ptr [0x4b2ed0]
00425d98  fmul       qword ptr [0x4a4378]
00425d9e  fadd       dword ptr [edi + 0x97c]
00425da4  fstp       dword ptr [esp + 0xc]
00425da8  fld        dword ptr [esp + 0xc]
00425dac  fst        dword ptr [edi + 0x97c]
00425db2  fldz       
00425db4  fcom       st(1)
00425db6  fnstsw     ax
00425db8  fstp       st(1)
00425dba  test       ah, 0x41
00425dbd  jp         0x425dc7

// block 00425dbf
00425dbf  fstp       dword ptr [edi + 0x978]
00425dc5  jmp        0x425dc9

// block 00425dc7
00425dc7  fstp       st(0)

// block 00425dc9
00425dc9  fld        dword ptr [0x4a4014]
00425dcf  fcom       dword ptr [edi + 0x97c]
00425dd5  fnstsw     ax
00425dd7  test       ah, 5
00425dda  jp         0x425de4

// block 00425ddc
00425ddc  fstp       dword ptr [edi + 0x97c]
00425de2  jmp        0x425de6

// block 00425de4
00425de4  fstp       st(0)

// block 00425de6
00425de6  fld        dword ptr [edi + 0x970]
00425dec  fcomp      qword ptr [0x4a4418]
00425df2  fnstsw     ax
00425df4  test       ah, 0x41
00425df7  jne        0x4260af

// block 00425dfd
00425dfd  mov        dword ptr [edi + 0x9b0], 0
00425e07  jmp        0x426f53

// block 00425e0c
00425e0c  cmp        eax, 2
00425e0f  jne        0x425f1b

// block 00425e15
00425e15  fld        dword ptr [edi + 0x978]
00425e1b  fld        dword ptr [0x4b2ed0]
00425e21  fld        st(0)
00425e23  fmulp      st(2)
00425e25  fxch       st(1)
00425e27  fstp       dword ptr [esp + 0x20]
00425e2b  fld        dword ptr [edi + 0x97c]
00425e31  fmul       st(1)
00425e33  fstp       dword ptr [esp + 0x24]
00425e37  fmul       dword ptr [edi + 0x980]
00425e3d  fstp       dword ptr [esp + 0x28]
00425e41  fld        dword ptr [esp + 0x20]
00425e45  fadd       dword ptr [edi + 0x96c]
00425e4b  fstp       dword ptr [edi + 0x96c]
00425e51  fld        dword ptr [esp + 0x24]
00425e55  fadd       dword ptr [edi + 0x970]
00425e5b  fstp       dword ptr [edi + 0x970]
00425e61  fld        dword ptr [edi + 0x974]
00425e67  fadd       dword ptr [esp + 0x28]
00425e6b  fstp       dword ptr [edi + 0x974]
00425e71  fld        dword ptr [0x4b2ed0]
00425e77  fmul       qword ptr [0x4a4378]
00425e7d  fadd       dword ptr [edi + 0x97c]
00425e83  fstp       dword ptr [esp + 0xc]
00425e87  fld        dword ptr [esp + 0xc]
00425e8b  fst        dword ptr [edi + 0x97c]
00425e91  fldz       
00425e93  fcompp     
00425e95  fnstsw     ax
00425e97  test       ah, 0x41
00425e9a  jp         0x425ebd

// block 00425e9c
00425e9c  mov        edx, dword ptr [0x4b4514]
00425ea2  mov        eax, dword ptr [edx + 0xa2c]
00425ea8  fld        dword ptr [eax + 8]

// block 00425eab
00425eab  fstp       dword ptr [edi + 0x9bc]
00425eb1  mov        dword ptr [edi + 0x9b0], 3
00425ebb  jmp        0x425f24

// block 00425ebd
00425ebd  fld        dword ptr [edi + 0x970]
00425ec3  fcomp      qword ptr [0x4a4418]
00425ec9  fnstsw     ax
00425ecb  test       ah, 0x41
00425ece  jne        0x4260af

// block 00425ed4
00425ed4  mov        dword ptr [edi + 0x9b0], 0
00425ede  mov        eax, dword ptr [0x4b0ccc]
00425ee3  sub        eax, 4
00425ee6  cmp        eax, 0x400
00425eeb  mov        dword ptr [0x4b0ccc], eax
00425ef0  jle        0x425f01

// block 00425ef2
00425ef2  mov        dword ptr [0x4b0ccc], 0x400
00425efc  jmp        0x426f53

// block 00425f01
00425f01  cmp        eax, 0xfffffc00
00425f06  jge        0x426f53

// block 00425f0c
00425f0c  mov        dword ptr [0x4b0ccc], 0xfffffc00
00425f16  jmp        0x426f53

// block 00425f1b
00425f1b  cmp        eax, 3
00425f1e  jne        0x425fdd

// block 00425f24
00425f24  mov        ecx, dword ptr [0x4b4534]
00425f2a  cmp        dword ptr [ecx + 0x2c], ebx
00425f2d  je         0x425f3e

// block 00425f2f
00425f2f  mov        eax, edi
00425f31  call       0x4271c0

// block 00425f36
00425f36  cmp        eax, ebx
00425f38  jne        0x426879

// block 00425f3e
00425f3e  fld        dword ptr [edi + 0x9bc]
00425f44  lea        esi, [edi + 0x96c]
00425f4a  sub        esp, 8
00425f4d  mov        ecx, esi
00425f4f  fstp       dword ptr [esp + 4]
00425f53  lea        ebx, [edi + 0x978]
00425f59  call       0x4377a0

// block 00425f5e
00425f5e  fstp       dword ptr [esp]
00425f61  mov        ecx, ebx
00425f63  call       0x427d00

// block 00425f68
00425f68  fld        dword ptr [0x4b2ed0]
00425f6e  fld        st(0)
00425f70  fmul       dword ptr [ebx]
00425f72  fstp       dword ptr [esp + 0x2c]
00425f76  fld        dword ptr [ebx + 4]
00425f79  fmul       st(1)
00425f7b  fstp       dword ptr [esp + 0x30]
00425f7f  fmul       dword ptr [ebx + 8]
00425f82  fstp       dword ptr [esp + 0x34]
00425f86  fld        dword ptr [esp + 0x2c]
00425f8a  fadd       dword ptr [esi]
00425f8c  fstp       dword ptr [esi]
00425f8e  fld        dword ptr [esi + 4]
00425f91  fadd       dword ptr [esp + 0x30]
00425f95  fstp       dword ptr [esi + 4]
00425f98  fld        dword ptr [esp + 0x34]
00425f9c  fadd       dword ptr [esi + 8]
00425f9f  fstp       dword ptr [esi + 8]
00425fa2  fld        dword ptr [0x4a404c]
00425fa8  fcomp      dword ptr [edi + 0x9bc]
00425fae  fnstsw     ax
00425fb0  test       ah, 0x41
00425fb3  jne        0x426224

// block 00425fb9
00425fb9  fld        dword ptr [edi + 0x9bc]
00425fbf  mov        edx, dword ptr [0x4b4514]
00425fc5  fadd       qword ptr [0x4a3fb8]
00425fcb  fstp       dword ptr [edi + 0x9bc]
00425fd1  cmp        dword ptr [edx + 0xa28], 4
00425fd8  jmp        0x426099

// block 00425fdd
00425fdd  cmp        eax, 4
00425fe0  jne        0x426189

// block 00425fe6
00425fe6  mov        eax, dword ptr [0x4b4534]
00425feb  cmp        dword ptr [eax + 0x2c], ebx
00425fee  je         0x425fff

// block 00425ff0
00425ff0  mov        eax, edi
00425ff2  call       0x4271c0

// block 00425ff7
00425ff7  cmp        eax, ebx
00425ff9  jne        0x426879

// block 00425fff
00425fff  fld        dword ptr [edi + 0x9bc]
00426005  lea        esi, [edi + 0x96c]
0042600b  sub        esp, 8
0042600e  mov        ecx, esi
00426010  fstp       dword ptr [esp + 4]
00426014  lea        ebx, [edi + 0x978]
0042601a  call       0x4377a0

// block 0042601f
0042601f  fstp       dword ptr [esp]
00426022  mov        ecx, ebx
00426024  call       0x427d00

// block 00426029
00426029  fld        dword ptr [ebx]
0042602b  fld        dword ptr [0x4b2ed0]
00426031  fld        st(0)
00426033  fmulp      st(2)
00426035  fxch       st(1)
00426037  fstp       dword ptr [esp + 0x3c]
0042603b  fld        dword ptr [ebx + 4]
0042603e  fmul       st(1)
00426040  fstp       dword ptr [esp + 0x40]
00426044  fmul       dword ptr [ebx + 8]
00426047  fstp       dword ptr [esp + 0x44]
0042604b  fld        dword ptr [esp + 0x3c]
0042604f  fadd       dword ptr [esi]
00426051  fstp       dword ptr [esi]
00426053  fld        dword ptr [esi + 4]
00426056  fadd       dword ptr [esp + 0x40]
0042605a  fstp       dword ptr [esi + 4]
0042605d  fld        dword ptr [esi + 8]
00426060  fadd       dword ptr [esp + 0x44]
00426064  fstp       dword ptr [esi + 8]
00426067  fld        dword ptr [0x4a404c]
0042606d  fcomp      dword ptr [edi + 0x9bc]
00426073  fnstsw     ax
00426075  test       ah, 0x41
00426078  jne        0x42608c

// block 0042607a
0042607a  fld        dword ptr [edi + 0x9bc]
00426080  fadd       qword ptr [0x4a3fb8]
00426086  fstp       dword ptr [edi + 0x9bc]

// block 0042608c
0042608c  mov        ecx, dword ptr [0x4b4514]
00426092  cmp        dword ptr [ecx + 0xa28], 4

// block 00426099
00426099  jne        0x4260af

// block 0042609b
0042609b  fldz       
0042609d  fst        dword ptr [ebx]

// block 0042609f
0042609f  fstp       dword ptr [edi + 0x97c]
004260a5  mov        dword ptr [edi + 0x9b0], 1

// block 004260af
004260af  mov        ecx, dword ptr [0x4b4514]
004260b5  cmp        dword ptr [ecx + 0xa28], 2
004260bc  je         0x426ecb

// block 004260c2
004260c2  fld        dword ptr [edi + 0x96c]
004260c8  fldz       
004260ca  fsub       st(1), st(0)
004260cc  fxch       st(1)
004260ce  fstp       dword ptr [esp + 0x80]
004260d5  fld        dword ptr [edi + 0x970]
004260db  fsub       st(1)
004260dd  fstp       dword ptr [esp + 0x84]
004260e4  fld        dword ptr [edi + 0x96c]
004260ea  fadd       st(1)
004260ec  fstp       dword ptr [esp + 0x8c]
004260f3  fadd       dword ptr [edi + 0x970]
004260f9  fstp       dword ptr [esp + 0x90]
00426100  fld        dword ptr [ecx + 0xc444]
00426106  fld        dword ptr [esp + 0x8c]
0042610d  fcom       st(1)
0042610f  fnstsw     ax
00426111  fstp       st(1)
00426113  fld        dword ptr [esp + 0x84]
0042611a  fld        dword ptr [esp + 0x80]
00426121  fld        dword ptr [esp + 0x90]
00426128  test       ah, 5
0042612b  jnp        0x426dd8

// block 00426131
00426131  fld        dword ptr [ecx + 0xc448]
00426137  fcomp      st(1)
00426139  fnstsw     ax
0042613b  test       ah, 0x41
0042613e  je         0x426dd8

// block 00426144
00426144  fld        dword ptr [ecx + 0xc450]
0042614a  fcomp      st(2)
0042614c  fnstsw     ax
0042614e  test       ah, 5
00426151  jnp        0x426dd8

// block 00426157
00426157  fld        dword ptr [ecx + 0xc454]
0042615d  fcomp      st(3)
0042615f  fnstsw     ax
00426161  test       ah, 5
00426164  jnp        0x426dd8

// block 0042616a
0042616a  mov        eax, dword ptr [edi + 0x9b4]
00426170  fstp       st(3)
00426172  fstp       st(2)
00426174  dec        eax
00426175  fstp       st(1)
00426177  fstp       st(0)
00426179  cmp        eax, 0x11
0042617c  ja         0x426da4

// block 00426182
00426182  jmp        dword ptr [eax*4 + 0x426f7c]

// block 00426189
00426189  cmp        eax, 8
0042618c  jne        0x426236

// block 00426192
00426192  fld        dword ptr [edi + 0x984]
00426198  lea        esi, [edi + 0x96c]
0042619e  sub        esp, 8
004261a1  mov        ecx, esi
004261a3  fstp       dword ptr [esp + 4]
004261a7  lea        ebx, [edi + 0x978]
004261ad  call       0x4377a0

// block 004261b2
004261b2  fstp       dword ptr [esp]
004261b5  mov        ecx, ebx
004261b7  call       0x427d00

// block 004261bc
004261bc  fld        dword ptr [0x4b2ed0]
004261c2  fld        st(0)
004261c4  fmul       dword ptr [ebx]
004261c6  fstp       dword ptr [esp + 0x4c]
004261ca  fld        dword ptr [ebx + 4]
004261cd  fmul       st(1)
004261cf  fstp       dword ptr [esp + 0x50]
004261d3  fmul       dword ptr [ebx + 8]
004261d6  fstp       dword ptr [esp + 0x54]
004261da  fld        dword ptr [esp + 0x4c]
004261de  fadd       dword ptr [esi]
004261e0  fstp       dword ptr [esi]
004261e2  fld        dword ptr [esp + 0x50]
004261e6  fadd       dword ptr [esi + 4]
004261e9  fstp       dword ptr [esi + 4]
004261ec  fld        dword ptr [esp + 0x54]
004261f0  fadd       dword ptr [esi + 8]
004261f3  fstp       dword ptr [esi + 8]
004261f6  cmp        dword ptr [edi + 0x98c], 0x3c
004261fd  jl         0x426224

// block 004261ff
004261ff  fld        dword ptr [0x4a404c]
00426205  fcomp      dword ptr [edi + 0x984]
0042620b  fnstsw     ax
0042620d  test       ah, 0x41
00426210  jne        0x426224

// block 00426212
00426212  fld        dword ptr [edi + 0x984]
00426218  fadd       qword ptr [0x4a3fb8]
0042621e  fstp       dword ptr [edi + 0x984]

// block 00426224
00426224  mov        edx, dword ptr [0x4b4514]
0042622a  cmp        dword ptr [edx + 0xa28], 4
00426231  jmp        0x426099

// block 00426236
00426236  cmp        eax, 6
00426239  jne        0x426870

// block 0042623f
0042623f  mov        eax, dword ptr [0x4b4534]
00426244  cmp        dword ptr [eax + 0x2c], ebx
00426247  je         0x426258

// block 00426249
00426249  mov        eax, edi
0042624b  call       0x4271c0

// block 00426250
00426250  cmp        eax, ebx
00426252  jne        0x426879

// block 00426258
00426258  mov        ecx, dword ptr [0x4b43e4]
0042625e  cmp        dword ptr [ecx + 0x6d30], ebx
00426264  je         0x426276

// block 00426266
00426266  push       0x2710
0042626b  lea        eax, [edi + 0x988]
00426271  call       0x4067e0

// block 00426276
00426276  mov        eax, dword ptr [edi + 0x9b4]
0042627c  cmp        eax, 0xd
0042627f  je         0x42628f

// block 00426281
00426281  cmp        eax, 0xe
00426284  je         0x42628f

// block 00426286
00426286  cmp        eax, 0xf
00426289  jne        0x426375

// block 0042628f
0042628f  mov        eax, dword ptr [0x4b4514]
00426294  fld        dword ptr [edi + 0x970]
0042629a  fsub       dword ptr [eax + 0x980]
004262a0  fld        dword ptr [edi + 0x96c]
004262a6  fsub       dword ptr [eax + 0x97c]
004262ac  fmul       st(0), st(0)
004262ae  fld        st(1)
004262b0  fmulp      st(2)
004262b2  faddp      st(1)
004262b4  fstp       dword ptr [esp + 0xc]
004262b8  fld        dword ptr [esp + 0xc]
004262bc  fcomp      qword ptr [0x4a4410]
004262c2  fnstsw     ax
004262c4  test       ah, 0x41
004262c7  jp         0x42632d

// block 004262c9
004262c9  fld        dword ptr [0x4a3da8]
004262cf  lea        ebx, [edi + 0x99c]
004262d5  push       ecx
004262d6  mov        esi, ebx
004262d8  fstp       dword ptr [esp]
004262db  call       0x464a20

// block 004262e0
004262e0  fld        dword ptr [0x4a3da8]
004262e6  push       ecx
004262e7  lea        esi, [edi + 0x988]
004262ed  fstp       dword ptr [esp]
004262f0  call       0x464a20

// block 004262f5
004262f5  cmp        dword ptr [edi + 0x9a0], 0x5a
004262fc  jne        0x42630f

// block 004262fe
004262fe  fld        dword ptr [0x4a3da8]
00426304  push       ecx
00426305  mov        esi, ebx
00426307  fstp       dword ptr [esp]
0042630a  call       0x464a20

// block 0042630f
0042630f  cmp        dword ptr [edi + 0x9c4], 0
00426316  jne        0x426352

// block 00426318
00426318  xor        eax, eax
0042631a  mov        esi, edi
0042631c  call       0x421a10

// block 00426321
00426321  mov        dword ptr [edi + 0x9c4], 1
0042632b  jmp        0x426352

// block 0042632d
0042632d  cmp        dword ptr [edi + 0x9c4], ebx
00426333  je         0x426352

// block 00426335
00426335  cmp        dword ptr [edi + 0x9a0], 0x5a
0042633c  mov        esi, edi
0042633e  jl         0x426347

// block 00426340
00426340  call       0x40d5d0

// block 00426345
00426345  jmp        0x42634c

// block 00426347
00426347  call       0x4219e0

// block 0042634c
0042634c  mov        dword ptr [edi + 0x9c4], ebx

// block 00426352
00426352  mov        eax, dword ptr [edi + 0x9a0]
00426358  cmp        eax, 0x5a
0042635b  jne        0x426483

// block 00426361
00426361  mov        esi, edi
00426363  call       0x40d5d0

// block 00426368
00426368  xor        ebx, ebx

// block 0042636a
0042636a  lea        esi, [edi + 0x99c]
00426370  call       0x464a80

// block 00426375
00426375  fld        dword ptr [edi + 0x978]
0042637b  fld        dword ptr [0x4b2ed0]
00426381  fld        st(0)
00426383  fmulp      st(2)
00426385  fxch       st(1)
00426387  fstp       dword ptr [esp + 0x5c]
0042638b  fld        dword ptr [edi + 0x97c]
00426391  fmul       st(1)
00426393  fstp       dword ptr [esp + 0x60]
00426397  fmul       dword ptr [edi + 0x980]
0042639d  fstp       dword ptr [esp + 0x64]
004263a1  fld        dword ptr [edi + 0x96c]
004263a7  fadd       dword ptr [esp + 0x5c]
004263ab  fstp       dword ptr [edi + 0x96c]
004263b1  fld        dword ptr [edi + 0x970]
004263b7  fadd       dword ptr [esp + 0x60]
004263bb  fstp       dword ptr [edi + 0x970]
004263c1  fld        dword ptr [esp + 0x64]
004263c5  fadd       dword ptr [edi + 0x974]
004263cb  fstp       dword ptr [edi + 0x974]
004263d1  cmp        dword ptr [edi + 0x98c], 0x4b0
004263db  jge        0x42650f

// block 004263e1
004263e1  fld        dword ptr [0x4a440c]
004263e7  fcomp      dword ptr [edi + 0x96c]
004263ed  fnstsw     ax
004263ef  fldz       
004263f1  test       ah, 1
004263f4  jne        0x426403

// block 004263f6
004263f6  fcom       dword ptr [edi + 0x978]
004263fc  fnstsw     ax
004263fe  test       ah, 0x41
00426401  je         0x426423

// block 00426403
00426403  fld        dword ptr [0x4a4198]
00426409  fcomp      dword ptr [edi + 0x96c]
0042640f  fnstsw     ax
00426411  test       ah, 0x41
00426414  jp         0x426435

// block 00426416
00426416  fcom       dword ptr [edi + 0x978]
0042641c  fnstsw     ax
0042641e  test       ah, 5
00426421  jp         0x426435

// block 00426423
00426423  fld        dword ptr [edi + 0x978]
00426429  fmul       qword ptr [0x4a4168]
0042642f  fstp       dword ptr [edi + 0x978]

// block 00426435
00426435  fld        dword ptr [0x4a3f4c]
0042643b  fcomp      dword ptr [edi + 0x970]
00426441  fnstsw     ax
00426443  test       ah, 1
00426446  jne        0x426459

// block 00426448
00426448  fcom       dword ptr [edi + 0x97c]
0042644e  fnstsw     ax
00426450  test       ah, 0x41
00426453  je         0x4264f9

// block 00426459
00426459  fld        dword ptr [0x4a4408]
0042645f  fcomp      dword ptr [edi + 0x970]
00426465  fnstsw     ax
00426467  test       ah, 0x41
0042646a  jp         0x426588

// block 00426470
00426470  fcomp      dword ptr [edi + 0x97c]
00426476  fnstsw     ax
00426478  test       ah, 5
0042647b  jp         0x42658a

// block 00426481
00426481  jmp        0x4264fb

// block 00426483
00426483  cmp        eax, 0x96
00426488  jl         0x426368

// block 0042648e
0042648e  xor        ebx, ebx
00426490  push       ebx
00426491  lea        eax, [edi + 0x99c]
00426497  call       0x4067e0

// block 0042649c
0042649c  mov        eax, dword ptr [edi + 0x9b4]
004264a2  sub        eax, 0xc
004264a5  cdq        
004264a6  mov        ecx, 3
004264ab  idiv       ecx
004264ad  lea        eax, [edx + 0xd]
004264b0  mov        edx, dword ptr [0x4b43c8]
004264b6  mov        dword ptr [edi + 0x9b4], eax
004264bc  add        eax, 0xa5
004264c1  push       eax
004264c2  mov        eax, dword ptr [edx + 0x4debdc]
004264c8  push       eax
004264c9  mov        eax, edi
004264cb  call       0x4061e0

// block 004264d0
004264d0  fld        dword ptr [edi + 0x96c]
004264d6  push       ecx
004264d7  lea        eax, [ebx + 0x35]
004264da  fstp       dword ptr [esp]
004264dd  call       0x453e20

// block 004264e2
004264e2  mov        ecx, dword ptr [edi + 0x968]
004264e8  push       ecx
004264e9  call       0x461a70

// block 004264ee
004264ee  mov        dword ptr [edi + 0x968], ebx
004264f4  jmp        0x42636a

// block 004264f9
004264f9  fstp       st(0)

// block 004264fb
004264fb  fld        dword ptr [edi + 0x97c]
00426501  fmul       qword ptr [0x4a4168]
00426507  fstp       dword ptr [edi + 0x97c]
0042650d  jmp        0x42658a

// block 0042650f
0042650f  fld        dword ptr [edi + 0x96c]
00426515  fcomp      qword ptr [0x4a4400]
0042651b  fnstsw     ax
0042651d  test       ah, 0x41
00426520  jnp        0x42655b

// block 00426522
00426522  fld        dword ptr [edi + 0x96c]
00426528  fcomp      qword ptr [0x4a43a0]
0042652e  fnstsw     ax
00426530  test       ah, 1
00426533  je         0x42655b

// block 00426535
00426535  fld        dword ptr [0x4a43f8]
0042653b  fcomp      dword ptr [edi + 0x970]
00426541  fnstsw     ax
00426543  test       ah, 1
00426546  je         0x42655b

// block 00426548
00426548  fld        dword ptr [edi + 0x970]
0042654e  fcomp      qword ptr [0x4a3e90]
00426554  fnstsw     ax
00426556  test       ah, 1
00426559  jne        0x42658a

// block 0042655b
0042655b  mov        dword ptr [edi + 0x9b0], ebx
00426561  mov        eax, dword ptr [0x4b0ccc]
00426566  sub        eax, 4
00426569  cmp        eax, 0x400
0042656e  mov        dword ptr [0x4b0ccc], eax
00426573  jle        0x426848

// block 00426579
00426579  mov        dword ptr [0x4b0ccc], 0x400
00426583  jmp        0x426859

// block 00426588
00426588  fstp       st(0)

// block 0042658a
0042658a  cmp        dword ptr [0x4b0c58], 3
00426591  jl         0x4265aa

// block 00426593
00426593  mov        edx, dword ptr [edi + 0x968]
00426599  push       edx
0042659a  call       0x461a70

// block 0042659f
0042659f  mov        dword ptr [edi + 0x968], ebx
004265a5  jmp        0x4260af

// block 004265aa
004265aa  mov        eax, dword ptr [edi + 0x9b4]
004265b0  add        eax, -0xa
004265b3  cmp        eax, 5
004265b6  ja         0x4260af

// block 004265bc
004265bc  jmp        dword ptr [eax*4 + 0x426fc4]

// block 004265c3
004265c3  mov        eax, dword ptr [0x4b0c4c]
004265c8  cmp        eax, 1
004265cb  jne        0x426621

// block 004265cd
004265cd  cmp        dword ptr [0x4b0c50], eax
004265d3  jne        0x42662f

// block 004265d5
004265d5  cmp        dword ptr [edi + 0x968], 0
004265dc  jne        0x4260af

// block 004265e2
004265e2  mov        ecx, dword ptr [0x4b43c8]
004265e8  mov        edx, dword ptr [ecx + 0x4debdc]
004265ee  push       0
004265f0  push       0xd3
004265f5  lea        eax, [esp + 0x60]
004265f9  push       eax
004265fa  push       edx
004265fb  mov        eax, 0x17
00426600  xor        ecx, ecx
00426602  call       0x4615a0

// block 00426607
00426607  mov        eax, dword ptr [esp + 0x58]
0042660b  mov        ebx, 7
00426610  push       eax
00426611  mov        dword ptr [edi + 0x968], eax
00426617  call       0x461970

// block 0042661c
0042661c  jmp        0x4260af

// block 00426621
00426621  cmp        eax, 2
00426624  jne        0x42664a

// block 00426626
00426626  cmp        dword ptr [0x4b0c50], 3
0042662d  je         0x426658

// block 0042662f
0042662f  mov        eax, dword ptr [edi + 0x968]
00426635  push       eax
00426636  call       0x461a70

// block 0042663b
0042663b  mov        dword ptr [edi + 0x968], 0
00426645  jmp        0x4260af

// block 0042664a
0042664a  cmp        eax, 3
0042664d  jne        0x42662f

// block 0042664f
0042664f  cmp        dword ptr [0x4b0c50], 2
00426656  jne        0x42662f

// block 00426658
00426658  cmp        dword ptr [edi + 0x968], 0
0042665f  jne        0x4260af

// block 00426665
00426665  mov        ecx, dword ptr [0x4b43c8]
0042666b  mov        edx, dword ptr [ecx + 0x4debdc]
00426671  push       0
00426673  push       0xd3
00426678  lea        eax, [esp + 0x80]
0042667f  push       eax
00426680  push       edx
00426681  mov        eax, 0x17
00426686  xor        ecx, ecx
00426688  call       0x4615a0

// block 0042668d
0042668d  mov        eax, dword ptr [esp + 0x78]
00426691  jmp        0x426832

// block 00426696
00426696  mov        eax, dword ptr [0x4b0c4c]
0042669b  cmp        eax, 2
0042669e  jne        0x4266f4

// block 004266a0
004266a0  cmp        dword ptr [0x4b0c50], eax
004266a6  jne        0x426702

// block 004266a8
004266a8  cmp        dword ptr [edi + 0x968], 0
004266af  jne        0x4260af

// block 004266b5
004266b5  mov        edx, dword ptr [0x4b43c8]
004266bb  mov        eax, dword ptr [edx + 0x4debdc]
004266c1  push       0
004266c3  push       0xd3
004266c8  lea        ecx, [esp + 0x40]
004266cc  push       ecx
004266cd  push       eax
004266ce  mov        eax, 0x17
004266d3  xor        ecx, ecx
004266d5  call       0x4615a0

// block 004266da
004266da  mov        eax, dword ptr [esp + 0x38]
004266de  mov        ebx, 8
004266e3  push       eax
004266e4  mov        dword ptr [edi + 0x968], eax
004266ea  call       0x461970

// block 004266ef
004266ef  jmp        0x4260af

// block 004266f4
004266f4  cmp        eax, 1
004266f7  jne        0x42671d

// block 004266f9
004266f9  cmp        dword ptr [0x4b0c50], 3
00426700  je         0x42672b

// block 00426702
00426702  mov        ecx, dword ptr [edi + 0x968]
00426708  push       ecx
00426709  call       0x461a70

// block 0042670e
0042670e  mov        dword ptr [edi + 0x968], 0
00426718  jmp        0x4260af

// block 0042671d
0042671d  cmp        eax, 3
00426720  jne        0x426702

// block 00426722
00426722  cmp        dword ptr [0x4b0c50], 1
00426729  jne        0x426702

// block 0042672b
0042672b  cmp        dword ptr [edi + 0x968], 0
00426732  jne        0x4260af

// block 00426738
00426738  mov        edx, dword ptr [0x4b43c8]
0042673e  mov        eax, dword ptr [edx + 0x4debdc]
00426744  push       0
00426746  push       0xd3
0042674b  lea        ecx, [esp + 0x70]
0042674f  push       ecx
00426750  push       eax
00426751  mov        eax, 0x17
00426756  xor        ecx, ecx
00426758  call       0x4615a0

// block 0042675d
0042675d  mov        eax, dword ptr [esp + 0x68]
00426761  jmp        0x426832

// block 00426766
00426766  mov        eax, dword ptr [0x4b0c4c]
0042676b  cmp        eax, 3
0042676e  jne        0x4267c3

// block 00426770
00426770  cmp        dword ptr [0x4b0c50], eax
00426776  jne        0x4267d1

// block 00426778
00426778  cmp        dword ptr [edi + 0x968], 0
0042677f  jne        0x4260af

// block 00426785
00426785  mov        eax, dword ptr [0x4b43c8]
0042678a  mov        ecx, dword ptr [eax + 0x4debdc]
00426790  push       0
00426792  push       0xd3
00426797  lea        edx, [esp + 0x50]
0042679b  push       edx
0042679c  push       ecx
0042679d  mov        eax, 0x17
004267a2  xor        ecx, ecx
004267a4  call       0x4615a0

// block 004267a9
004267a9  mov        eax, dword ptr [esp + 0x48]
004267ad  push       eax
004267ae  mov        ebx, 9
004267b3  mov        dword ptr [edi + 0x968], eax
004267b9  call       0x461970

// block 004267be
004267be  jmp        0x4260af

// block 004267c3
004267c3  cmp        eax, 1
004267c6  jne        0x4267ec

// block 004267c8
004267c8  cmp        dword ptr [0x4b0c50], 2
004267cf  je         0x4267fa

// block 004267d1
004267d1  mov        edx, dword ptr [edi + 0x968]
004267d7  push       edx
004267d8  call       0x461a70

// block 004267dd
004267dd  mov        dword ptr [edi + 0x968], 0
004267e7  jmp        0x4260af

// block 004267ec
004267ec  cmp        eax, 2
004267ef  jne        0x4267d1

// block 004267f1
004267f1  cmp        dword ptr [0x4b0c50], 1
004267f8  jne        0x4267d1

// block 004267fa
004267fa  cmp        dword ptr [edi + 0x968], 0
00426801  jne        0x4260af

// block 00426807
00426807  mov        eax, dword ptr [0x4b43c8]
0042680c  mov        ecx, dword ptr [eax + 0x4debdc]
00426812  push       0
00426814  push       0xd3
00426819  lea        edx, [esp + 0x84]
00426820  push       edx
00426821  push       ecx
00426822  mov        eax, 0x17
00426827  xor        ecx, ecx
00426829  call       0x4615a0

// block 0042682e
0042682e  mov        eax, dword ptr [esp + 0x7c]

// block 00426832
00426832  mov        ebx, 0xa
00426837  push       eax
00426838  mov        dword ptr [edi + 0x968], eax
0042683e  call       0x461970

// block 00426843
00426843  jmp        0x4260af

// block 00426848
00426848  cmp        eax, 0xfffffc00
0042684d  jge        0x426859

// block 0042684f
0042684f  mov        dword ptr [0x4b0ccc], 0xfffffc00

// block 00426859
00426859  mov        eax, dword ptr [edi + 0x968]
0042685f  push       eax
00426860  call       0x461a70

// block 00426865
00426865  mov        dword ptr [edi + 0x968], ebx
0042686b  jmp        0x426f53

// block 00426870
00426870  cmp        eax, 7
00426873  jne        0x4260af

// block 00426879
00426879  mov        ecx, dword ptr [0x4b4534]
0042687f  cmp        dword ptr [ecx + 0x2c], 0
00426883  jne        0x426892

// block 00426885
00426885  fldz       
00426887  fst        dword ptr [edi + 0x978]
0042688d  jmp        0x42609f

// block 00426892
00426892  fld        dword ptr [edi + 0x9bc]
00426898  sub        esp, 8
0042689b  lea        esi, [edi + 0x96c]
004268a1  fstp       dword ptr [esp + 4]
004268a5  lea        ebx, [edi + 0x978]
004268ab  call       0x44ac00

// block 004268b0
004268b0  fstp       dword ptr [esp]
004268b3  mov        ecx, ebx
004268b5  call       0x427d00

// block 004268ba
004268ba  fld        dword ptr [ebx]
004268bc  fld        dword ptr [0x4b2ed0]
004268c2  fld        st(0)
004268c4  fmulp      st(2)
004268c6  fxch       st(1)
004268c8  fstp       dword ptr [esp + 0x6c]
004268cc  fld        dword ptr [ebx + 4]
004268cf  fmul       st(1)
004268d1  fstp       dword ptr [esp + 0x70]
004268d5  fmul       dword ptr [ebx + 8]
004268d8  fstp       dword ptr [esp + 0x74]
004268dc  fld        dword ptr [esi]
004268de  fadd       dword ptr [esp + 0x6c]
004268e2  fstp       dword ptr [esi]
004268e4  fld        dword ptr [esi + 4]
004268e7  fadd       dword ptr [esp + 0x70]
004268eb  fstp       dword ptr [esi + 4]
004268ee  fld        dword ptr [esi + 8]
004268f1  fadd       dword ptr [esp + 0x74]
004268f5  fstp       dword ptr [esi + 8]
004268f8  fld        dword ptr [0x4a4250]
004268fe  fcomp      dword ptr [edi + 0x9bc]
00426904  fnstsw     ax
00426906  test       ah, 0x41
00426909  jne        0x426932

// block 0042690b
0042690b  mov        edx, dword ptr [0x4b4534]
00426911  fild       dword ptr [edx + 0x14]
00426914  fdiv       qword ptr [0x4a43f0]
0042691a  fmul       qword ptr [0x4a40e0]
00426920  fadd       qword ptr [0x4a43e8]
00426926  fadd       dword ptr [edi + 0x9bc]
0042692c  fstp       dword ptr [edi + 0x9bc]

// block 00426932
00426932  mov        eax, dword ptr [0x4b4534]
00426937  mov        eax, dword ptr [eax + 0x34]
0042693a  fld        dword ptr [eax + 0x1078]
00426940  add        eax, 0x1074
00426945  fsub       dword ptr [esi + 4]
00426948  fld        dword ptr [eax]
0042694a  fsub       dword ptr [esi]
0042694c  fmul       st(0), st(0)
0042694e  fld        st(1)
00426950  fmulp      st(2)
00426952  faddp      st(1)
00426954  fstp       dword ptr [esp + 0xc]
00426958  fld        dword ptr [esp + 0xc]
0042695c  fcomp      dword ptr [0x4a43e0]
00426962  fnstsw     ax
00426964  test       ah, 5
00426967  jp         0x4260af

// block 0042696d
0042696d  fld        dword ptr [esi]
0042696f  push       ecx
00426970  mov        eax, 0x26
00426975  fstp       dword ptr [esp]
00426978  mov        dword ptr [edi + 0x9b0], 0
00426982  call       0x453e20

// block 00426987
00426987  mov        eax, dword ptr [edi + 0x9b4]
0042698d  call       0x44aca0

// block 00426992
00426992  jmp        0x426f53

// block 00426997
00426997  mov        ecx, dword ptr [0x4b0c48]
0042699d  cmp        ecx, dword ptr [0x4b0cd0]
004269a3  jl         0x4269df

// block 004269a5
004269a5  fld        dword ptr [0x4a3edc]
004269ab  push       ecx
004269ac  mov        esi, 0x4b0c40
004269b1  fstp       dword ptr [esp]
004269b4  call       0x427ca0

// block 004269b9
004269b9  push       0xff40ff40
004269be  lea        esi, [edi + 0x96c]
004269c4  push       esi
004269c5  mov        eax, 0x64
004269ca  call       0x43e250

// block 004269cf
004269cf  fld        dword ptr [esi]
004269d1  push       ecx
004269d2  mov        eax, 0xd
004269d7  fstp       dword ptr [esp]
004269da  call       0x453e20

// block 004269df
004269df  mov        ebx, dword ptr [0x4b0cd0]
004269e5  mov        eax, 0x4b0c40
004269ea  call       0x422d70

// block 004269ef
004269ef  test       eax, eax
004269f1  je         0x426da4

// block 004269f7
004269f7  mov        edx, dword ptr [0x4b4514]
004269fd  push       edx
004269fe  call       0x4385b0

// block 00426a03
00426a03  push       0xffffff40
00426a08  lea        esi, [edi + 0x96c]
00426a0e  push       esi
00426a0f  or         eax, 0xffffffff
00426a12  call       0x43e250

// block 00426a17
00426a17  fld        dword ptr [esi]
00426a19  push       ecx
00426a1a  mov        eax, 0xd
00426a1f  fstp       dword ptr [esp]
00426a22  call       0x453e20

// block 00426a27
00426a27  mov        eax, dword ptr [0x4b0ccc]
00426a2c  add        eax, 0xc

// block 00426a2f
00426a2f  cmp        eax, 0x400
00426a34  mov        dword ptr [0x4b0ccc], eax
00426a39  jle        0x426a4a

// block 00426a3b
00426a3b  mov        dword ptr [0x4b0ccc], 0x400
00426a45  jmp        0x426da4

// block 00426a4a
00426a4a  cmp        eax, 0xfffffc00
00426a4f  jge        0x426da4

// block 00426a55
00426a55  mov        dword ptr [0x4b0ccc], 0xfffffc00
00426a5f  jmp        0x426da4

// block 00426a64
00426a64  mov        eax, edi
00426a66  call       0x426ff0

// block 00426a6b
00426a6b  jmp        0x426da4

// block 00426a70
00426a70  mov        eax, dword ptr [0x4b0c48]
00426a75  cmp        eax, dword ptr [0x4b0cd0]
00426a7b  jl         0x426ab7

// block 00426a7d
00426a7d  fld        dword ptr [0x4a41fc]
00426a83  push       ecx
00426a84  mov        esi, 0x4b0c40
00426a89  fstp       dword ptr [esp]
00426a8c  call       0x427ca0

// block 00426a91
00426a91  push       0xff40ff40
00426a96  lea        esi, [edi + 0x96c]
00426a9c  push       esi
00426a9d  mov        eax, 0xc8
00426aa2  call       0x43e250

// block 00426aa7
00426aa7  fld        dword ptr [esi]
00426aa9  push       ecx
00426aaa  mov        eax, 0xd
00426aaf  fstp       dword ptr [esp]
00426ab2  call       0x453e20

// block 00426ab7
00426ab7  mov        ebx, dword ptr [0x4b0cd4]
00426abd  mov        eax, 0x4b0c40
00426ac2  call       0x422d70

// block 00426ac7
00426ac7  test       eax, eax
00426ac9  je         0x426da4

// block 00426acf
00426acf  mov        ecx, dword ptr [0x4b4514]
00426ad5  push       ecx
00426ad6  call       0x4385b0

// block 00426adb
00426adb  fld        dword ptr [edi + 0x96c]
00426ae1  lea        esi, [edi + 0x96c]
00426ae7  push       ecx
00426ae8  mov        eax, 0xd
00426aed  fstp       dword ptr [esp]
00426af0  call       0x453e20

// block 00426af5
00426af5  push       0xffffff40
00426afa  push       esi
00426afb  or         eax, 0xffffffff
00426afe  call       0x43e250

// block 00426b03
00426b03  mov        eax, dword ptr [0x4b0ccc]
00426b08  add        eax, 0x18
00426b0b  jmp        0x426a2f

// block 00426b10
00426b10  mov        edx, dword ptr [0x4b4514]
00426b16  fld        dword ptr [edx + 0x980]
00426b1c  fstp       dword ptr [esp + 0xc]
00426b20  fld        dword ptr [0x4a3db8]
00426b26  fcomp      dword ptr [esp + 0xc]
00426b2a  fnstsw     ax
00426b2c  test       ah, 1
00426b2f  je         0x426b9e

// block 00426b31
00426b31  cmp        dword ptr [edi + 0x9b0], 3
00426b38  je         0x426b9e

// block 00426b3a
00426b3a  mov        eax, 0x4b0c40
00426b3f  call       0x421880

// block 00426b44
00426b44  fld        dword ptr [esp + 0xc]
00426b48  lea        eax, [eax + eax*2]
00426b4b  cdq        
00426b4c  and        edx, 3
00426b4f  add        eax, edx
00426b51  mov        esi, eax
00426b53  sar        esi, 2
00426b56  call       0x4931e0

// block 00426b5b
00426b5b  mov        ecx, eax
00426b5d  add        ecx, -0x80
00426b60  imul       ecx, esi
00426b63  mov        eax, 0x91a2b3c5
00426b68  imul       ecx
00426b6a  add        edx, ecx
00426b6c  sar        edx, 8
00426b6f  mov        eax, edx
00426b71  shr        eax, 0x1f
00426b74  add        eax, edx
00426b76  sub        esi, eax
00426b78  mov        eax, 0x66666667
00426b7d  imul       esi
00426b7f  sar        edx, 2
00426b82  mov        eax, edx
00426b84  shr        eax, 0x1f
00426b87  add        eax, edx
00426b89  lea        esi, [eax + eax*4]
00426b8c  add        esi, esi
00426b8e  test       esi, esi
00426b90  jg         0x426b97

// block 00426b92
00426b92  mov        esi, 0xa

// block 00426b97
00426b97  push       -1
00426b99  jmp        0x426d8b

// block 00426b9e
00426b9e  mov        eax, 0x4b0c40
00426ba3  call       0x421880

// block 00426ba8
00426ba8  jmp        0x426d65

// block 00426bad
00426bad  cmp        dword ptr [0x4b0c9c], 0
00426bb4  mov        eax, 0x4b0c40
00426bb9  mov        ecx, 2
00426bbe  je         0x426bc5

// block 00426bc0
00426bc0  mov        ecx, 1

// block 00426bc5
00426bc5  call       0x422d30

// block 00426bca
00426bca  mov        eax, dword ptr [0x4b0ccc]
00426bcf  add        eax, 0x100
00426bd4  jmp        0x426a2f

// block 00426bd9
00426bd9  mov        eax, 0x4b0c40
00426bde  call       0x422ce0

// block 00426be3
00426be3  mov        eax, dword ptr [0x4b0ccc]
00426be8  add        eax, 0x100
00426bed  jmp        0x426a2f

// block 00426bf2
00426bf2  mov        eax, 0x4b0c40
00426bf7  call       0x422e10

// block 00426bfc
00426bfc  mov        eax, dword ptr [0x4b0ccc]
00426c01  add        eax, 0x100
00426c06  jmp        0x426a2f

// block 00426c0b
00426c0b  mov        eax, 0x4b0c40
00426c10  call       0x422dd0

// block 00426c15
00426c15  mov        eax, dword ptr [0x4b0ccc]
00426c1a  add        eax, 0x100
00426c1f  jmp        0x426a2f

// block 00426c24
00426c24  fld        dword ptr [0x4a4210]
00426c2a  push       ecx
00426c2b  mov        esi, 0x4b0c40
00426c30  fstp       dword ptr [esp]
00426c33  call       0x427ca0

// block 00426c38
00426c38  push       0xa
00426c3a  jmp        0x426d9a

// block 00426c3f
00426c3f  xor        eax, eax
00426c41  mov        ecx, edi
00426c43  call       0x4270b0

// block 00426c48
00426c48  jmp        0x426da4

// block 00426c4d
00426c4d  mov        eax, 1
00426c52  mov        ecx, edi
00426c54  call       0x4270b0

// block 00426c59
00426c59  jmp        0x426da4

// block 00426c5e
00426c5e  mov        eax, 2
00426c63  mov        ecx, edi
00426c65  call       0x4270b0

// block 00426c6a
00426c6a  jmp        0x426da4

// block 00426c6f
00426c6f  fld        dword ptr [edi + 0x96c]
00426c75  push       ecx
00426c76  mov        eax, 0x27
00426c7b  fstp       dword ptr [esp]
00426c7e  call       0x453e20

// block 00426c83
00426c83  mov        eax, dword ptr [edi + 0x9c8]
00426c89  inc        eax
00426c8a  cmp        eax, 4
00426c8d  ja         0x426da4

// block 00426c93
00426c93  jmp        dword ptr [eax*4 + 0x426fdc]

// block 00426c9a
00426c9a  xor        esi, esi
00426c9c  cmp        dword ptr [edi + 0x9cc], esi
00426ca2  jle        0x426cb4

// block 00426ca4
00426ca4  mov        eax, edi
00426ca6  call       0x426ff0

// block 00426cab
00426cab  inc        esi
00426cac  cmp        esi, dword ptr [edi + 0x9cc]
00426cb2  jl         0x426ca4

// block 00426cb4
00426cb4  mov        eax, 0x4b0c40
00426cb9  call       0x421880

// block 00426cbe
00426cbe  imul       eax, dword ptr [edi + 0x9d0]
00426cc5  mov        dword ptr [esp + 0xc], eax
00426cc9  fild       dword ptr [esp + 0xc]
00426ccd  jmp        0x426d5a

// block 00426cd2
00426cd2  xor        esi, esi
00426cd4  cmp        dword ptr [edi + 0x9cc], esi
00426cda  jle        0x426cf0

// block 00426cdc
00426cdc  lea        esp, [esp]

// block 00426ce0
00426ce0  mov        eax, edi
00426ce2  call       0x426ff0

// block 00426ce7
00426ce7  inc        esi
00426ce8  cmp        esi, dword ptr [edi + 0x9cc]
00426cee  jl         0x426ce0

// block 00426cf0
00426cf0  mov        eax, 0x4b0c40
00426cf5  call       0x421880

// block 00426cfa
00426cfa  mov        ecx, dword ptr [edi + 0x9cc]
00426d00  mov        ebx, eax
00426d02  imul       ecx, ebx
00426d05  mov        dword ptr [esp + 0xc], ecx
00426d09  fild       dword ptr [esp + 0xc]
00426d0d  fmul       dword ptr [edi + 0x9d4]
00426d13  call       0x4931e0

// block 00426d18
00426d18  mov        edx, dword ptr [edi + 0x9d0]
00426d1e  imul       edx, ebx
00426d21  mov        esi, eax
00426d23  add        esi, edx
00426d25  jmp        0x426d7d

// block 00426d27
00426d27  xor        esi, esi
00426d29  cmp        dword ptr [edi + 0x9cc], esi
00426d2f  jle        0x426d41

// block 00426d31
00426d31  mov        eax, edi
00426d33  call       0x426ff0

// block 00426d38
00426d38  inc        esi
00426d39  cmp        esi, dword ptr [edi + 0x9cc]
00426d3f  jl         0x426d31

// block 00426d41
00426d41  mov        eax, 0x4b0c40
00426d46  call       0x421880

// block 00426d4b
00426d4b  imul       eax, dword ptr [edi + 0x9d0]
00426d52  mov        dword ptr [esp + 0xc], eax
00426d56  fild       dword ptr [esp + 0xc]

// block 00426d5a
00426d5a  fmul       dword ptr [edi + 0x9d4]
00426d60  call       0x4931e0

// block 00426d65
00426d65  mov        ecx, eax
00426d67  mov        eax, 0x66666667
00426d6c  imul       ecx
00426d6e  sar        edx, 2
00426d71  mov        eax, edx
00426d73  shr        eax, 0x1f
00426d76  add        eax, edx
00426d78  lea        esi, [eax + eax*4]
00426d7b  add        esi, esi

// block 00426d7d
00426d7d  test       esi, esi
00426d7f  jg         0x426d86

// block 00426d81
00426d81  mov        esi, 0xa

// block 00426d86
00426d86  push       0xffffff00

// block 00426d8b
00426d8b  lea        eax, [edi + 0x96c]
00426d91  push       eax
00426d92  mov        eax, esi
00426d94  call       0x43e250

// block 00426d99
00426d99  push       esi

// block 00426d9a
00426d9a  mov        ecx, 0x4b0c40
00426d9f  call       0x40e730

// block 00426da4
00426da4  fld        dword ptr [edi + 0x96c]
00426daa  push       ecx
00426dab  mov        eax, 0x26
00426db0  fstp       dword ptr [esp]
00426db3  mov        dword ptr [edi + 0x9b0], 0
00426dbd  call       0x453e20

// block 00426dc2
00426dc2  mov        eax, dword ptr [edi + 0x968]
00426dc8  push       eax
00426dc9  mov        ebx, 1
00426dce  call       0x461970

// block 00426dd3
00426dd3  jmp        0x426f53

// block 00426dd8
00426dd8  mov        eax, dword ptr [edi + 0x9b0]
00426dde  cmp        eax, 4
00426de1  je         0x426ec3

// block 00426de7
00426de7  cmp        eax, 3
00426dea  je         0x426ec3

// block 00426df0
00426df0  cmp        eax, 6
00426df3  je         0x426ec3

// block 00426df9
00426df9  cmp        eax, 7
00426dfc  je         0x426ec3

// block 00426e02
00426e02  cmp        eax, 8
00426e05  je         0x426ec3

// block 00426e0b
00426e0b  mov        edx, dword ptr [0x4d49d0]
00426e11  and        edx, 8
00426e14  je         0x426e52

// block 00426e16
00426e16  fld        dword ptr [ecx + 0xc45c]
00426e1c  fcomp      st(4)
00426e1e  fnstsw     ax
00426e20  test       ah, 0x41
00426e23  je         0x426e52

// block 00426e25
00426e25  fld        dword ptr [ecx + 0xc460]
00426e2b  fcomp      st(1)
00426e2d  fnstsw     ax
00426e2f  test       ah, 0x41
00426e32  je         0x426e52

// block 00426e34
00426e34  fld        dword ptr [ecx + 0xc468]
00426e3a  fcomp      st(2)
00426e3c  fnstsw     ax
00426e3e  test       ah, 5
00426e41  jnp        0x426e52

// block 00426e43
00426e43  fld        dword ptr [ecx + 0xc46c]
00426e49  fcomp      st(3)
00426e4b  fnstsw     ax
00426e4d  test       ah, 5
00426e50  jp         0x426e9a

// block 00426e52
00426e52  test       edx, edx
00426e54  jne        0x426ec3

// block 00426e56
00426e56  fld        dword ptr [ecx + 0xc474]
00426e5c  fcomp      st(4)
00426e5e  fnstsw     ax
00426e60  fstp       st(3)
00426e62  test       ah, 0x41
00426e65  je         0x426ec5

// block 00426e67
00426e67  fld        dword ptr [ecx + 0xc478]
00426e6d  fcomp      st(3)
00426e6f  fnstsw     ax
00426e71  fstp       st(2)
00426e73  test       ah, 0x41
00426e76  je         0x426ec7

// block 00426e78
00426e78  fld        dword ptr [ecx + 0xc480]
00426e7e  fcomp      st(2)
00426e80  fnstsw     ax
00426e82  fstp       st(1)
00426e84  test       ah, 5
00426e87  jnp        0x426ec9

// block 00426e89
00426e89  fld        dword ptr [ecx + 0xc484]
00426e8f  fcompp     
00426e91  fnstsw     ax
00426e93  test       ah, 5
00426e96  jnp        0x426ecb

// block 00426e98
00426e98  jmp        0x426ea2

// block 00426e9a
00426e9a  fstp       st(3)
00426e9c  fstp       st(2)
00426e9e  fstp       st(1)
00426ea0  fstp       st(0)

// block 00426ea2
00426ea2  mov        ecx, dword ptr [ecx + 0xa2c]
00426ea8  fld        dword ptr [ecx + 8]
00426eab  mov        dword ptr [edi + 0x9b0], 4
00426eb5  fdiv       qword ptr [0x4a3fd8]
00426ebb  fstp       dword ptr [edi + 0x9bc]
00426ec1  jmp        0x426ecb

// block 00426ec3
00426ec3  fstp       st(3)

// block 00426ec5
00426ec5  fstp       st(2)

// block 00426ec7
00426ec7  fstp       st(1)

// block 00426ec9
00426ec9  fstp       st(0)

// block 00426ecb
00426ecb  test       byte ptr [edi + 0x47c], 1
00426ed2  je         0x426eda

// block 00426ed4
00426ed4  push       edi
00426ed5  call       0x455630

// block 00426eda
00426eda  test       byte ptr [edi + 0x930], 1
00426ee1  je         0x426eef

// block 00426ee3
00426ee3  lea        edx, [edi + 0x4b4]
00426ee9  push       edx
00426eea  call       0x455630

// block 00426eef
00426eef  mov        eax, dword ptr [edi + 0x968]
00426ef5  test       eax, eax
00426ef7  je         0x426f3f

// block 00426ef9
00426ef9  mov        edx, dword ptr [0x4ce8cc]
00426eff  push       eax
00426f00  call       0x461920

// block 00426f05
00426f05  test       eax, eax
00426f07  je         0x426f3f

// block 00426f09
00426f09  fld        dword ptr [edi + 0x96c]
00426f0f  fadd       qword ptr [0x4a3d70]
00426f15  fadd       qword ptr [0x4a3d28]
00426f1b  fstp       dword ptr [eax + 0x430]
00426f21  fld        dword ptr [edi + 0x970]
00426f27  fadd       qword ptr [0x4a3d68]
00426f2d  fstp       dword ptr [eax + 0x434]
00426f33  fld        dword ptr [edi + 0x974]
00426f39  fstp       dword ptr [eax + 0x438]

// block 00426f3f
00426f3f  lea        esi, [edi + 0x988]
00426f45  call       0x464a80

// block 00426f4a
00426f4a  mov        eax, dword ptr [ebp + 8]
00426f4d  inc        dword ptr [eax + 0x666fd4]

// block 00426f53
00426f53  mov        eax, dword ptr [esp + 0x10]
00426f57  inc        eax
00426f58  add        edi, 0x9d8
00426f5e  cmp        eax, 0xa68
00426f63  mov        dword ptr [esp + 0x10], eax
00426f67  jl         0x425c47

// block 00426f6d
00426f6d  pop        edi
00426f6e  pop        esi
00426f6f  mov        eax, 1
00426f74  pop        ebx
00426f75  mov        esp, ebp
00426f77  pop        ebp
00426f78  ret        4
