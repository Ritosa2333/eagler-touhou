
// block 00433bb0
00433bb0  sub        esp, 0x58
00433bb3  mov        eax, dword ptr [0x4ad138]
00433bb8  xor        eax, esp
00433bba  mov        dword ptr [esp + 0x54], eax
00433bbe  push       ebp
00433bbf  mov        ebp, dword ptr [esp + 0x60]
00433bc3  mov        eax, dword ptr [ebp + 4]
00433bc6  add        eax, -0xc
00433bc9  cmp        eax, 7
00433bcc  ja         0x434842

// block 00433bd2
00433bd2  push       ebx
00433bd3  push       esi
00433bd4  push       edi
00433bd5  jmp        dword ptr [eax*4 + 0x434854]

// block 00433bdc
00433bdc  cmp        dword ptr [ebp + 0x14], 0xa
00433be0  jl         0x43483f

// block 00433be6
00433be6  test       dword ptr [0x4d48c4], 0x80001
00433bf0  je         0x43483f

// block 00433bf6
00433bf6  mov        edx, 7
00433bfb  call       0x453d90

// block 00433c00
00433c00  mov        eax, dword ptr [ebp + 0x1e8]
00433c06  push       eax
00433c07  mov        ebx, 2
00433c0c  mov        dword ptr [ebp + 4], 0xd
00433c13  call       0x461970

// block 00433c18
00433c18  push       0
00433c1a  lea        eax, [ebp + 0x10]
00433c1d  call       0x4067e0

// block 00433c22
00433c22  pop        edi
00433c23  pop        esi
00433c24  pop        ebx
00433c25  pop        ebp
00433c26  mov        ecx, dword ptr [esp + 0x54]
00433c2a  xor        ecx, esp
00433c2c  call       0x46c932

// block 00433c31
00433c31  add        esp, 0x58
00433c34  ret        4

// block 00433c37
00433c37  cmp        dword ptr [ebp + 0x14], 0xa
00433c3b  jl         0x43483f

// block 00433c41
00433c41  call       0x431b30

// block 00433c46
00433c46  test       byte ptr [0x4b0ce0], 0x10
00433c4d  je         0x433d1a

// block 00433c53
00433c53  mov        edx, dword ptr [0x4b0c90]
00433c59  mov        ecx, dword ptr [0x4b0c94]
00433c5f  mov        eax, dword ptr [0x4b0ca8]
00433c64  lea        ecx, [ecx + edx*2]
00433c67  imul       ecx, ecx, 0x45f4
00433c6d  add        ecx, dword ptr [0x4b451c]
00433c73  lea        edx, [eax + eax*2]
00433c76  mov        eax, dword ptr [0x4b0cb0]
00433c7b  lea        edx, [eax + edx*2]
00433c7e  lea        eax, [ecx + edx*8 + 0x5a4]
00433c85  mov        ecx, dword ptr [0x4b0c44]
00433c8b  cmp        ecx, dword ptr [eax]
00433c8d  jle        0x433c91

// block 00433c8f
00433c8f  mov        dword ptr [eax], ecx

// block 00433c91
00433c91  mov        edi, dword ptr [ebp + 0x2dc]
00433c97  push       0x6f
00433c99  lea        eax, [esp + 0x24]
00433c9d  push       eax
00433c9e  call       0x4357c0

// block 00433ca3
00433ca3  mov        ecx, dword ptr [eax]
00433ca5  mov        dword ptr [ebp + 0x1e8], ecx
00433cab  mov        dword ptr [ebp + 4], 0xe
00433cb2  mov        dword ptr [ebp + 0x40], 4
00433cb9  mov        dword ptr [ebp + 0x108], 1
00433cc3  mov        eax, dword ptr [ebp + 0x40]
00433cc6  test       eax, eax
00433cc8  je         0x433cd9

// block 00433cca
00433cca  jg         0x433cd2

// block 00433ccc
00433ccc  dec        eax
00433ccd  mov        dword ptr [ebp + 0x38], eax
00433cd0  jmp        0x433ce0

// block 00433cd2
00433cd2  xor        eax, eax
00433cd4  mov        dword ptr [ebp + 0x38], eax
00433cd7  jmp        0x433ce0

// block 00433cd9
00433cd9  mov        dword ptr [ebp + 0x38], 0

// block 00433ce0
00433ce0  mov        edx, dword ptr [ebp + 0x1e8]
00433ce6  push       edx
00433ce7  mov        ebx, 3
00433cec  call       0x461970

// block 00433cf1
00433cf1  movzx      ebx, word ptr [ebp + 0x38]
00433cf5  mov        eax, dword ptr [ebp + 0x1e8]
00433cfb  add        bx, 7
00433cff  push       eax
00433d00  call       0x461970

// block 00433d05
00433d05  pop        edi
00433d06  pop        esi
00433d07  pop        ebx
00433d08  pop        ebp
00433d09  mov        ecx, dword ptr [esp + 0x54]
00433d0d  xor        ecx, esp
00433d0f  call       0x46c932

// block 00433d14
00433d14  add        esp, 0x58
00433d17  ret        4

// block 00433d1a
00433d1a  mov        esi, ebp
00433d1c  call       0x433a30

// block 00433d21
00433d21  push       0
00433d23  lea        eax, [ebp + 0x10]
00433d26  call       0x4067e0

// block 00433d2b
00433d2b  cmp        dword ptr [ebp + 0x1f8], 0
00433d32  jne        0x433d5b

// block 00433d34
00433d34  lea        eax, [ebp + 0x1e8]
00433d3a  mov        dword ptr [ebp + 4], 0x12
00433d41  call       0x461d80

// block 00433d46
00433d46  pop        edi
00433d47  pop        esi
00433d48  pop        ebx
00433d49  pop        ebp
00433d4a  mov        ecx, dword ptr [esp + 0x54]
00433d4e  xor        ecx, esp
00433d50  call       0x46c932

// block 00433d55
00433d55  add        esp, 0x58
00433d58  ret        4

// block 00433d5b
00433d5b  mov        edi, dword ptr [ebp + 0x2dc]
00433d61  push       0x6f
00433d63  lea        ecx, [esp + 0x18]
00433d67  push       ecx
00433d68  call       0x4357c0

// block 00433d6d
00433d6d  mov        edx, dword ptr [eax]
00433d6f  mov        dword ptr [ebp + 0x1e8], edx
00433d75  mov        dword ptr [ebp + 4], 0xe
00433d7c  mov        dword ptr [ebp + 0x40], 4
00433d83  mov        dword ptr [ebp + 0x108], 1
00433d8d  mov        eax, dword ptr [ebp + 0x40]
00433d90  test       eax, eax
00433d92  je         0x433da3

// block 00433d94
00433d94  jg         0x433d9c

// block 00433d96
00433d96  dec        eax
00433d97  mov        dword ptr [ebp + 0x38], eax
00433d9a  jmp        0x433daa

// block 00433d9c
00433d9c  xor        eax, eax
00433d9e  mov        dword ptr [ebp + 0x38], eax
00433da1  jmp        0x433daa

// block 00433da3
00433da3  mov        dword ptr [ebp + 0x38], 0

// block 00433daa
00433daa  mov        eax, dword ptr [ebp + 0x1e8]
00433db0  push       eax
00433db1  mov        ebx, 3
00433db6  call       0x461970

// block 00433dbb
00433dbb  movzx      ebx, word ptr [ebp + 0x38]
00433dbf  mov        ecx, dword ptr [ebp + 0x1e8]
00433dc5  add        bx, 7
00433dc9  push       ecx
00433dca  call       0x461970

// block 00433dcf
00433dcf  pop        edi
00433dd0  pop        esi
00433dd1  pop        ebx
00433dd2  pop        ebp
00433dd3  mov        ecx, dword ptr [esp + 0x54]
00433dd7  xor        ecx, esp
00433dd9  call       0x46c932

// block 00433dde
00433dde  add        esp, 0x58
00433de1  ret        4

// block 00433de4
00433de4  mov        edx, dword ptr [ebp + 0x38]
00433de7  lea        esi, [ebp + 0x38]
00433dea  mov        al, 0x10
00433dec  mov        dword ptr [esi + 4], edx
00433def  test       byte ptr [0x4d48c4], al
00433df5  jne        0x433dff

// block 00433df7
00433df7  test       byte ptr [0x4d48c0], al
00433dfd  je         0x433e08

// block 00433dff
00433dff  push       -1
00433e01  mov        eax, esi
00433e03  call       0x464970

// block 00433e08
00433e08  test       byte ptr [0x4d48c4], 0x20
00433e0f  jne        0x433e1a

// block 00433e11
00433e11  test       byte ptr [0x4d48c0], 0x20
00433e18  je         0x433e23

// block 00433e1a
00433e1a  push       1
00433e1c  mov        eax, esi
00433e1e  call       0x464970

// block 00433e23
00433e23  mov        eax, dword ptr [esi + 4]
00433e26  cmp        eax, dword ptr [esi]
00433e28  je         0x433e47

// block 00433e2a
00433e2a  movzx      ebx, word ptr [esi]
00433e2d  mov        ecx, dword ptr [ebp + 0x1e8]
00433e33  add        bx, 7
00433e37  push       ecx
00433e38  call       0x461970

// block 00433e3d
00433e3d  mov        edx, 0xa
00433e42  call       0x453d90

// block 00433e47
00433e47  test       dword ptr [0x4d48c4], 0x80001
00433e51  je         0x433ebe

// block 00433e53
00433e53  mov        eax, dword ptr [ebp + 0x38]
00433e56  lea        esi, [ebp + 0x1e8]
00433e5c  lea        edi, [eax + 0x6b]
00433e5f  lea        ebx, [esp + 0x18]
00433e63  call       0x462060

// block 00433e68
00433e68  mov        edx, dword ptr [eax]
00433e6a  push       edx
00433e6b  mov        ebx, 6
00433e70  call       0x461970

// block 00433e75
00433e75  lea        edx, [ebx + 1]
00433e78  call       0x453d90

// block 00433e7d
00433e7d  lea        edi, [ebp + 0x10]
00433e80  mov        ebx, 0x13
00433e85  push       0
00433e87  mov        eax, edi
00433e89  mov        dword ptr [ebp + 4], ebx
00433e8c  call       0x4067e0

// block 00433e91
00433e91  call       0x431b30

// block 00433e96
00433e96  mov        eax, dword ptr [ebp + 0x38]
00433e99  sub        eax, 1
00433e9c  je         0x433ea8

// block 00433e9e
00433e9e  sub        eax, 1
00433ea1  je         0x433f02

// block 00433ea3
00433ea3  sub        eax, 1
00433ea6  jne        0x433ebe

// block 00433ea8
00433ea8  push       0
00433eaa  mov        eax, edi
00433eac  mov        dword ptr [ebp + 4], ebx
00433eaf  call       0x4067e0

// block 00433eb4
00433eb4  mov        edx, 7
00433eb9  call       0x453d90

// block 00433ebe
00433ebe  test       dword ptr [0x4d48c4], 0x102
00433ec8  je         0x43483f

// block 00433ece
00433ece  mov        edx, 9
00433ed3  call       0x453d90

// block 00433ed8
00433ed8  mov        ecx, 1
00433edd  cmp        dword ptr [ebp + 0x38], ecx
00433ee0  je         0x43483f

// block 00433ee6
00433ee6  mov        eax, dword ptr [ebp + 0x40]
00433ee9  test       eax, eax
00433eeb  je         0x433f94

// block 00433ef1
00433ef1  cmp        eax, ecx
00433ef3  jg         0x433f8d

// block 00433ef9
00433ef9  dec        eax
00433efa  mov        dword ptr [ebp + 0x38], eax
00433efd  jmp        0x433f97

// block 00433f02
00433f02  push       0
00433f04  mov        eax, edi
00433f06  mov        dword ptr [ebp + 4], 0x10
00433f0d  call       0x4067e0

// block 00433f12
00433f12  mov        eax, esi
00433f14  call       0x461d80

// block 00433f19
00433f19  lea        eax, [ebp + 0x38]
00433f1c  call       0x464900

// block 00433f21
00433f21  mov        dword ptr [ebp + 0x40], 0x19
00433f28  mov        dword ptr [ebp + 0x108], 1
00433f32  mov        ecx, dword ptr [eax + 8]
00433f35  test       ecx, ecx
00433f37  je         0x433f46

// block 00433f39
00433f39  jg         0x433f40

// block 00433f3b
00433f3b  dec        ecx
00433f3c  mov        dword ptr [eax], ecx
00433f3e  jmp        0x433f4c

// block 00433f40
00433f40  xor        ecx, ecx
00433f42  mov        dword ptr [eax], ecx
00433f44  jmp        0x433f4c

// block 00433f46
00433f46  mov        dword ptr [eax], 0

// block 00433f4c
00433f4c  mov        esi, 1
00433f51  lea        edi, [ebp + 0x204]
00433f57  jmp        0x433f60

// block 00433f60
00433f60  push       esi
00433f61  lea        eax, [esp + 0x28]
00433f65  push       0x4a1024
00433f6a  push       eax
00433f6b  call       0x46cbbb

// block 00433f70
00433f70  add        esp, 0xc
00433f73  lea        ecx, [esp + 0x24]
00433f77  push       ecx
00433f78  call       0x43b6f0

// block 00433f7d
00433f7d  mov        dword ptr [edi], eax
00433f7f  inc        esi
00433f80  add        edi, 4
00433f83  cmp        esi, 0x19
00433f86  jle        0x433f60

// block 00433f88
00433f88  jmp        0x433ebe

// block 00433f8d
00433f8d  mov        eax, ecx
00433f8f  mov        dword ptr [ebp + 0x38], eax
00433f92  jmp        0x433f97

// block 00433f94
00433f94  mov        dword ptr [ebp + 0x38], ecx

// block 00433f97
00433f97  movzx      ebx, word ptr [ebp + 0x38]
00433f9b  mov        edx, dword ptr [ebp + 0x1e8]
00433fa1  add        bx, 7
00433fa5  push       edx
00433fa6  call       0x461970

// block 00433fab
00433fab  pop        edi
00433fac  pop        esi
00433fad  pop        ebx
00433fae  pop        ebp
00433faf  mov        ecx, dword ptr [esp + 0x54]
00433fb3  xor        ecx, esp
00433fb5  call       0x46c932

// block 00433fba
00433fba  add        esp, 0x58
00433fbd  ret        4

// block 00433fc0
00433fc0  cmp        dword ptr [ebp + 0x14], 0xa
00433fc4  jl         0x43483f

// block 00433fca
00433fca  mov        eax, dword ptr [ebp + 0x38]
00433fcd  lea        esi, [ebp + 0x38]
00433fd0  mov        bl, 0x10
00433fd2  mov        dword ptr [esi + 4], eax
00433fd5  test       byte ptr [0x4d48c4], bl
00433fdb  jne        0x433fe5

// block 00433fdd
00433fdd  test       byte ptr [0x4d48c0], bl
00433fe3  je         0x433fee

// block 00433fe5
00433fe5  push       -1
00433fe7  mov        eax, esi
00433fe9  call       0x464970

// block 00433fee
00433fee  test       byte ptr [0x4d48c4], 0x20
00433ff5  jne        0x434000

// block 00433ff7
00433ff7  test       byte ptr [0x4d48c0], 0x20
00433ffe  je         0x434009

// block 00434000
00434000  push       1
00434002  mov        eax, esi
00434004  call       0x464970

// block 00434009
00434009  mov        ecx, dword ptr [esi + 4]
0043400c  cmp        ecx, dword ptr [esi]
0043400e  je         0x43401a

// block 00434010
00434010  mov        edx, 0xa
00434015  call       0x453d90

// block 0043401a
0043401a  mov        eax, dword ptr [0x4d48c4]
0043401f  test       eax, 0x80001
00434024  je         0x434174

// block 0043402a
0043402a  push       0
0043402c  lea        eax, [ebp + 0x10]
0043402f  mov        dword ptr [ebp + 4], 0x11
00434036  call       0x4067e0

// block 0043403b
0043403b  mov        eax, dword ptr [ebp + 0x118]
00434041  lea        edi, [ebp + 0x110]
00434047  test       eax, eax
00434049  je         0x434058

// block 0043404b
0043404b  jg         0x434052

// block 0043404d
0043404d  dec        eax
0043404e  mov        dword ptr [edi], eax
00434050  jmp        0x43405e

// block 00434052
00434052  xor        eax, eax
00434054  mov        dword ptr [edi], eax
00434056  jmp        0x43405e

// block 00434058
00434058  mov        dword ptr [edi], 0

// block 0043405e
0043405e  cmp        dword ptr [ebp + 0x1f4], 0
00434065  mov        dword ptr [ebp + 0x118], 0x5b
0043406f  mov        dword ptr [ebp + 0x1e0], 1
00434079  je         0x4340a3

// block 0043407b
0043407b  test       byte ptr [0x4b0ce0], bl
00434081  jne        0x4340a3

// block 00434083
00434083  mov        esi, dword ptr [0x4b4518]
00434089  mov        edx, dword ptr [esi + 0x1c]
0043408c  add        esi, 0x1c
0043408f  add        edx, 0xc
00434092  push       edx
00434093  call       0x46d5b7

// block 00434098
00434098  mov        eax, dword ptr [esi]
0043409a  mov        dword ptr [eax + 0x68], 8
004340a1  jmp        0x4340c2

// block 004340a3
004340a3  mov        esi, dword ptr [0x4b4518]
004340a9  mov        ecx, dword ptr [esi + 0x1c]
004340ac  add        esi, 0x1c
004340af  add        ecx, 0xc
004340b2  push       ecx
004340b3  call       0x46d5b7

// block 004340b8
004340b8  mov        edx, dword ptr [esi]
004340ba  mov        eax, dword ptr [0x4b0cb0]
004340bf  mov        dword ptr [edx + 0x68], eax

// block 004340c2
004340c2  mov        eax, dword ptr [0x4b451c]
004340c7  lea        esi, [ebp + 0x2cc]
004340cd  add        eax, 0x1e9c0
004340d2  mov        edx, esi
004340d4  add        esp, 4
004340d7  sub        edx, eax
004340d9  lea        esp, [esp]

// block 004340e0
004340e0  mov        cl, byte ptr [eax]
004340e2  mov        byte ptr [edx + eax], cl
004340e5  inc        eax
004340e6  test       cl, cl
004340e8  jne        0x4340e0

// block 004340ea
004340ea  mov        dword ptr [ebp + 0x1f0], 0
004340f4  mov        ecx, 0x4a0ee4
004340f9  mov        eax, esi
004340fb  jmp        0x434100

// block 00434100
00434100  mov        dl, byte ptr [eax]
00434102  cmp        dl, byte ptr [ecx]
00434104  jne        0x434120

// block 00434106
00434106  test       dl, dl
00434108  je         0x43411c

// block 0043410a
0043410a  mov        dl, byte ptr [eax + 1]
0043410d  cmp        dl, byte ptr [ecx + 1]
00434110  jne        0x434120

// block 00434112
00434112  add        eax, 2
00434115  add        ecx, 2
00434118  test       dl, dl
0043411a  jne        0x434100

// block 0043411c
0043411c  xor        eax, eax
0043411e  jmp        0x434125

// block 00434120
00434120  sbb        eax, eax
00434122  sbb        eax, -1

// block 00434125
00434125  test       eax, eax
00434127  je         0x434132

// block 00434129
00434129  push       -1
0043412b  mov        eax, edi
0043412d  call       0x464970

// block 00434132
00434132  mov        eax, 8
00434137  jmp        0x434140

// block 00434140
00434140  cmp        byte ptr [eax + ebp + 0x2cb], 0x20
00434148  jne        0x43414f

// block 0043414a
0043414a  dec        eax
0043414b  test       eax, eax
0043414d  jg         0x434140

// block 0043414f
0043414f  mov        dword ptr [ebp + 0x1f0], eax

// block 00434155
00434155  mov        edx, 7
0043415a  call       0x453d90

// block 0043415f
0043415f  pop        edi
00434160  pop        esi
00434161  pop        ebx
00434162  pop        ebp
00434163  mov        ecx, dword ptr [esp + 0x54]
00434167  xor        ecx, esp
00434169  call       0x46c932

// block 0043416e
0043416e  add        esp, 0x58
00434171  ret        4

// block 00434174
00434174  test       eax, 0x102
00434179  je         0x43483f

// block 0043417f
0043417f  lea        eax, [ebp + 0x10]
00434182  push       0
00434184  mov        dword ptr [ebp + 4], 0xe
0043418b  mov        dword ptr [esp + 0x14], eax
0043418f  call       0x4067e0

// block 00434194
00434194  lea        edi, [ebp + 0x1e8]
0043419a  mov        eax, edi
0043419c  call       0x461d40

// block 004341a1
004341a1  mov        eax, esi
004341a3  call       0x464940

// block 004341a8
004341a8  movzx      ebx, word ptr [esi]
004341ab  mov        ecx, dword ptr [edi]
004341ad  add        bx, 7
004341b1  push       ecx
004341b2  mov        dword ptr [ebp + 0x40], 4
004341b9  mov        dword ptr [ebp + 0x108], 1
004341c3  call       0x461970

// block 004341c8
004341c8  lea        esi, [ebp + 0x204]
004341ce  mov        edi, 0x19

// block 004341d3
004341d3  mov        ebx, dword ptr [esi]
004341d5  test       ebx, ebx
004341d7  je         0x4341e8

// block 004341d9
004341d9  push       ebx
004341da  call       0x43b450

// block 004341df
004341df  push       ebx
004341e0  call       0x46ca4f

// block 004341e5
004341e5  add        esp, 4

// block 004341e8
004341e8  mov        dword ptr [esi], 0
004341ee  add        esi, 4
004341f1  sub        edi, 1
004341f4  jne        0x4341d3

// block 004341f6
004341f6  mov        eax, dword ptr [esp + 0x10]
004341fa  push       edi
004341fb  mov        dword ptr [ebp + 4], 0xe
00434202  call       0x4067e0

// block 00434207
00434207  lea        edx, [edi + 9]
0043420a  call       0x453d90

// block 0043420f
0043420f  pop        edi
00434210  pop        esi
00434211  pop        ebx
00434212  pop        ebp
00434213  mov        ecx, dword ptr [esp + 0x54]
00434217  xor        ecx, esp
00434219  call       0x46c932

// block 0043421e
0043421e  add        esp, 0x58
00434221  ret        4

// block 00434224
00434224  cmp        dword ptr [ebp + 0x14], 0xa
00434228  jl         0x43483f

// block 0043422e
0043422e  mov        edx, dword ptr [ebp + 0x110]
00434234  lea        esi, [ebp + 0x110]
0043423a  mov        ebx, 0x10
0043423f  mov        dword ptr [esi + 4], edx
00434242  test       byte ptr [0x4d48c4], bl
00434248  jne        0x434252

// block 0043424a
0043424a  test       byte ptr [0x4d48c0], bl
00434250  je         0x43425b

// block 00434252
00434252  push       -0xd
00434254  mov        eax, esi
00434256  call       0x464970

// block 0043425b
0043425b  test       byte ptr [0x4d48c4], 0x20
00434262  jne        0x43426d

// block 00434264
00434264  test       byte ptr [0x4d48c0], 0x20
0043426b  je         0x434276

// block 0043426d
0043426d  push       0xd
0043426f  mov        eax, esi
00434271  call       0x464970

// block 00434276
00434276  mov        al, 0x40
00434278  test       byte ptr [0x4d48c4], al
0043427e  jne        0x434288

// block 00434280
00434280  test       byte ptr [0x4d48c0], al
00434286  je         0x4342af

// block 00434288
00434288  mov        ecx, dword ptr [esi]
0043428a  mov        eax, 0x4ec4ec4f
0043428f  imul       ecx
00434291  sar        edx, 2
00434294  mov        eax, edx
00434296  shr        eax, 0x1f
00434299  add        eax, edx
0043429b  imul       eax, eax, 0xd
0043429e  sub        ecx, eax
004342a0  mov        eax, esi
004342a2  je         0x4342a8

// block 004342a4
004342a4  push       -1
004342a6  jmp        0x4342aa

// block 004342a8
004342a8  push       0xc

// block 004342aa
004342aa  call       0x464970

// block 004342af
004342af  mov        al, 0x80
004342b1  mov        edi, 1
004342b6  test       byte ptr [0x4d48c4], al
004342bc  jne        0x4342c6

// block 004342be
004342be  test       byte ptr [0x4d48c0], al
004342c4  je         0x4342ef

// block 004342c6
004342c6  mov        ecx, dword ptr [esi]
004342c8  mov        eax, 0x4ec4ec4f
004342cd  imul       ecx
004342cf  sar        edx, 2
004342d2  mov        eax, edx
004342d4  shr        eax, 0x1f
004342d7  add        eax, edx
004342d9  imul       eax, eax, 0xd
004342dc  sub        ecx, eax
004342de  mov        eax, esi
004342e0  cmp        ecx, 0xc
004342e3  je         0x4342e8

// block 004342e5
004342e5  push       edi
004342e6  jmp        0x4342ea

// block 004342e8
004342e8  push       -0xc

// block 004342ea
004342ea  call       0x464970

// block 004342ef
004342ef  mov        ecx, dword ptr [esi + 4]
004342f2  cmp        ecx, dword ptr [esi]
004342f4  je         0x434300

// block 004342f6
004342f6  mov        edx, 0xa
004342fb  call       0x453d90

// block 00434300
00434300  test       dword ptr [0x4d48c4], 0x80001
0043430a  je         0x434354

// block 0043430c
0043430c  mov        eax, dword ptr [esi]
0043430e  cmp        eax, 0x58
00434311  jge        0x4343ac

// block 00434317
00434317  mov        ecx, dword ptr [ebp + 0x1f0]
0043431d  cmp        ecx, 8
00434320  jge        0x43439d

// block 00434322
00434322  mov        dl, byte ptr [eax + 0x4a0e88]
00434328  mov        byte ptr [ecx + ebp + 0x2cc], dl

// block 0043432f
0043432f  add        dword ptr [ebp + 0x1f0], edi
00434335  cmp        dword ptr [ebp + 0x1f0], 8
0043433c  jl         0x43434a

// block 0043433e
0043433e  mov        ecx, 0x5a
00434343  mov        edx, esi
00434345  call       0x40f790

// block 0043434a
0043434a  mov        edx, 7
0043434f  call       0x453d90

// block 00434354
00434354  test       dword ptr [0x4d48c4], 0x102
0043435e  je         0x43483f

// block 00434364
00434364  mov        edx, 9
00434369  call       0x453d90

// block 0043436e
0043436e  mov        eax, dword ptr [ebp + 0x1f0]
00434374  test       eax, eax
00434376  jne        0x4344af

// block 0043437c
0043437c  push       eax
0043437d  lea        eax, [ebp + 0x10]
00434380  mov        dword ptr [ebp + 4], ebx
00434383  call       0x4067e0

// block 00434388
00434388  pop        edi
00434389  pop        esi
0043438a  pop        ebx
0043438b  pop        ebp
0043438c  mov        ecx, dword ptr [esp + 0x54]
00434390  xor        ecx, esp
00434392  call       0x46c932

// block 00434397
00434397  add        esp, 0x58
0043439a  ret        4

// block 0043439d
0043439d  mov        al, byte ptr [eax + 0x4a0e88]
004343a3  mov        byte ptr [ecx + ebp + 0x2cb], al
004343aa  jmp        0x43434a

// block 004343ac
004343ac  jne        0x4343d3

// block 004343ae
004343ae  mov        eax, dword ptr [ebp + 0x1f0]
004343b4  cmp        eax, 8
004343b7  jge        0x4343c6

// block 004343b9
004343b9  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004343c1  jmp        0x43432f

// block 004343c6
004343c6  mov        byte ptr [eax + ebp + 0x2cb], 0x20
004343ce  jmp        0x43434a

// block 004343d3
004343d3  cmp        eax, 0x59
004343d6  jne        0x434414

// block 004343d8
004343d8  mov        eax, dword ptr [ebp + 0x1f0]
004343de  test       eax, eax
004343e0  je         0x43483f

// block 004343e6
004343e6  dec        eax
004343e7  mov        dword ptr [ebp + 0x1f0], eax
004343ed  mov        edx, 9
004343f2  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004343fa  call       0x453d90

// block 004343ff
004343ff  pop        edi
00434400  pop        esi
00434401  pop        ebx
00434402  pop        ebp
00434403  mov        ecx, dword ptr [esp + 0x54]
00434407  xor        ecx, esp
00434409  call       0x46c932

// block 0043440e
0043440e  add        esp, 0x58
00434411  ret        4

// block 00434414
00434414  cmp        eax, 0x5a
00434417  jne        0x43434a

// block 0043441d
0043441d  lea        edx, [eax - 0x48]
00434420  call       0x453d90

// block 00434425
00434425  mov        ecx, dword ptr [ebp + 0x38]
00434428  inc        ecx
00434429  push       ecx
0043442a  lea        edx, [esp + 0x28]
0043442e  push       0x4a1024
00434433  push       edx
00434434  call       0x46cbbb

// block 00434439
00434439  mov        eax, dword ptr [ebp + 0x38]
0043443c  mov        esi, dword ptr [ebp + eax*4 + 0x204]
00434443  add        esp, 0xc
00434446  call       0x43b7c0

// block 0043444b
0043444b  lea        esi, [ebp + 0x2cc]
00434451  push       0
00434453  mov        edx, esi
00434455  lea        ecx, [esp + 0x28]
00434459  call       0x43bc10

// block 0043445e
0043445e  mov        edi, dword ptr [ebp + 0x38]
00434461  lea        ecx, [esp + 0x24]
00434465  push       ecx
00434466  call       0x43b6f0

// block 0043446b
0043446b  mov        dword ptr [ebp + edi*4 + 0x204], eax
00434472  push       0
00434474  lea        eax, [ebp + 0x10]
00434477  mov        dword ptr [ebp + 4], ebx
0043447a  call       0x4067e0

// block 0043447f
0043447f  mov        edx, dword ptr [0x4b451c]
00434485  add        edx, 0x1e9c0
0043448b  mov        eax, esi
0043448d  sub        edx, esi
0043448f  nop        

// block 00434490
00434490  mov        cl, byte ptr [eax]
00434492  mov        byte ptr [edx + eax], cl
00434495  inc        eax
00434496  test       cl, cl
00434498  jne        0x434490

// block 0043449a
0043449a  pop        edi
0043449b  pop        esi
0043449c  pop        ebx
0043449d  pop        ebp
0043449e  mov        ecx, dword ptr [esp + 0x54]
004344a2  xor        ecx, esp
004344a4  call       0x46c932

// block 004344a9
004344a9  add        esp, 0x58
004344ac  ret        4

// block 004344af
004344af  pop        edi
004344b0  dec        eax
004344b1  pop        esi
004344b2  mov        dword ptr [ebp + 0x1f0], eax
004344b8  pop        ebx
004344b9  mov        byte ptr [eax + ebp + 0x2cc], 0x20
004344c1  pop        ebp
004344c2  mov        ecx, dword ptr [esp + 0x54]
004344c6  xor        ecx, esp
004344c8  call       0x46c932

// block 004344cd
004344cd  add        esp, 0x58
004344d0  ret        4

// block 004344d3
004344d3  cmp        dword ptr [ebp + 0x14], 0xa
004344d7  jl         0x43483f

// block 004344dd
004344dd  cmp        dword ptr [ebp + 0x1f8], 0
004344e4  mov        ebx, 1
004344e9  jne        0x4345b9

// block 004344ef
004344ef  mov        edx, dword ptr [ebp + 0x110]
004344f5  lea        esi, [ebp + 0x110]
004344fb  mov        al, 0x10
004344fd  mov        dword ptr [esi + 4], edx
00434500  test       byte ptr [0x4d48c4], al
00434506  jne        0x434510

// block 00434508
00434508  test       byte ptr [0x4d48c0], al
0043450e  je         0x434519

// block 00434510
00434510  push       -0xd
00434512  mov        eax, esi
00434514  call       0x464970

// block 00434519
00434519  test       byte ptr [0x4d48c4], 0x20
00434520  jne        0x43452b

// block 00434522
00434522  test       byte ptr [0x4d48c0], 0x20
00434529  je         0x434534

// block 0043452b
0043452b  push       0xd
0043452d  mov        eax, esi
0043452f  call       0x464970

// block 00434534
00434534  mov        al, 0x40
00434536  test       byte ptr [0x4d48c4], al
0043453c  jne        0x434546

// block 0043453e
0043453e  test       byte ptr [0x4d48c0], al
00434544  je         0x43456d

// block 00434546
00434546  mov        ecx, dword ptr [esi]
00434548  mov        eax, 0x4ec4ec4f
0043454d  imul       ecx
0043454f  sar        edx, 2
00434552  mov        eax, edx
00434554  shr        eax, 0x1f
00434557  add        eax, edx
00434559  imul       eax, eax, 0xd
0043455c  sub        ecx, eax
0043455e  mov        eax, esi
00434560  je         0x434566

// block 00434562
00434562  push       -1
00434564  jmp        0x434568

// block 00434566
00434566  push       0xc

// block 00434568
00434568  call       0x464970

// block 0043456d
0043456d  mov        al, 0x80
0043456f  test       byte ptr [0x4d48c4], al
00434575  jne        0x43457f

// block 00434577
00434577  test       byte ptr [0x4d48c0], al
0043457d  je         0x4345a8

// block 0043457f
0043457f  mov        ecx, dword ptr [esi]
00434581  mov        eax, 0x4ec4ec4f
00434586  imul       ecx
00434588  sar        edx, 2
0043458b  mov        eax, edx
0043458d  shr        eax, 0x1f
00434590  add        eax, edx
00434592  imul       eax, eax, 0xd
00434595  sub        ecx, eax
00434597  mov        eax, esi
00434599  cmp        ecx, 0xc
0043459c  je         0x4345a1

// block 0043459e
0043459e  push       ebx
0043459f  jmp        0x4345a3

// block 004345a1
004345a1  push       -0xc

// block 004345a3
004345a3  call       0x464970

// block 004345a8
004345a8  mov        ecx, dword ptr [esi + 4]
004345ab  cmp        ecx, dword ptr [esi]
004345ad  je         0x4345b9

// block 004345af
004345af  mov        edx, 0xa
004345b4  call       0x453d90

// block 004345b9
004345b9  mov        eax, dword ptr [0x4d48c4]
004345be  test       eax, 0x80001
004345c3  je         0x434731

// block 004345c9
004345c9  cmp        dword ptr [ebp + 0x1f8], 0
004345d0  jne        0x4346eb

// block 004345d6
004345d6  mov        eax, dword ptr [ebp + 0x110]
004345dc  cmp        eax, 0x58
004345df  lea        edx, [ebp + 0x110]
004345e5  jge        0x434633

// block 004345e7
004345e7  mov        ecx, dword ptr [ebp + 0x1f0]
004345ed  cmp        ecx, 8
004345f0  jge        0x434621

// block 004345f2
004345f2  mov        al, byte ptr [eax + 0x4a0e88]
004345f8  mov        byte ptr [ecx + ebp + 0x2cc], al

// block 004345ff
004345ff  add        dword ptr [ebp + 0x1f0], ebx
00434605  cmp        dword ptr [ebp + 0x1f0], 8
0043460c  jl         0x434155

// block 00434612
00434612  mov        ecx, 0x5a
00434617  call       0x40f790

// block 0043461c
0043461c  jmp        0x434155

// block 00434621
00434621  mov        dl, byte ptr [eax + 0x4a0e88]
00434627  mov        byte ptr [ecx + ebp + 0x2cb], dl
0043462e  jmp        0x434155

// block 00434633
00434633  jne        0x434657

// block 00434635
00434635  mov        eax, dword ptr [ebp + 0x1f0]
0043463b  cmp        eax, 8
0043463e  jge        0x43464a

// block 00434640
00434640  mov        byte ptr [eax + ebp + 0x2cc], 0x20
00434648  jmp        0x4345ff

// block 0043464a
0043464a  mov        byte ptr [eax + ebp + 0x2cb], 0x20
00434652  jmp        0x434155

// block 00434657
00434657  cmp        eax, 0x59
0043465a  jne        0x43467e

// block 0043465c
0043465c  mov        eax, dword ptr [ebp + 0x1f0]
00434662  test       eax, eax
00434664  je         0x43483f

// block 0043466a
0043466a  dec        eax
0043466b  mov        dword ptr [ebp + 0x1f0], eax
00434671  mov        byte ptr [eax + ebp + 0x2cc], 0x20
00434679  jmp        0x434155

// block 0043467e
0043467e  cmp        eax, 0x5a
00434681  jne        0x434155

// block 00434687
00434687  mov        ecx, dword ptr [0x4b0ca8]
0043468d  mov        esi, dword ptr [ebp + 0x38]
00434690  mov        edi, dword ptr [0x4b0c90]
00434696  lea        ecx, [ecx + ecx*4]
00434699  lea        ecx, [esi + ecx*2]
0043469c  lea        esi, [ecx*8]
004346a3  sub        esi, ecx
004346a5  mov        ecx, dword ptr [0x4b0c94]
004346ab  lea        ecx, [ecx + edi*2]
004346ae  mov        edi, dword ptr [0x4b451c]
004346b4  imul       ecx, ecx, 0x45f4
004346ba  lea        eax, [ebp + 0x2cc]
004346c0  add        ecx, edi
004346c2  mov        edx, eax
004346c4  lea        esi, [ecx + esi*4 + 0x1e]

// block 004346c8
004346c8  mov        cl, byte ptr [edx]
004346ca  mov        byte ptr [esi], cl
004346cc  add        edx, ebx
004346ce  add        esi, ebx
004346d0  test       cl, cl
004346d2  jne        0x4346c8

// block 004346d4
004346d4  lea        edx, [edi + 0x1e9c0]
004346da  sub        edx, eax
004346dc  lea        esp, [esp]

// block 004346e0
004346e0  mov        cl, byte ptr [eax]
004346e2  mov        byte ptr [edx + eax], cl
004346e5  add        eax, ebx
004346e7  test       cl, cl
004346e9  jne        0x4346e0

// block 004346eb
004346eb  mov        edi, dword ptr [ebp + 0x2dc]
004346f1  push       0x6f
004346f3  lea        edx, [esp + 0x20]
004346f7  push       edx
004346f8  call       0x4357c0

// block 004346fd
004346fd  mov        eax, dword ptr [eax]
004346ff  lea        esi, [ebp + 0x1e8]
00434705  mov        dword ptr [esi], eax
00434707  mov        eax, esi
00434709  call       0x461d40

// block 0043470e
0043470e  mov        dword ptr [ebp + 4], 0xe
00434715  mov        dword ptr [ebp + 0x40], 4
0043471c  mov        dword ptr [ebp + 0x108], ebx
00434722  mov        eax, dword ptr [ebp + 0x40]
00434725  test       eax, eax
00434727  je         0x434789

// block 00434729
00434729  jg         0x434782

// block 0043472b
0043472b  dec        eax
0043472c  mov        dword ptr [ebp + 0x38], eax
0043472f  jmp        0x434790

// block 00434731
00434731  test       eax, 0x102
00434736  je         0x43483f

// block 0043473c
0043473c  cmp        dword ptr [ebp + 0x38], 0
00434740  jl         0x4346eb

// block 00434742
00434742  cmp        dword ptr [ebp + 0x1f0], 0
00434749  je         0x43483f

// block 0043474f
0043474f  mov        edx, 9
00434754  call       0x453d90

// block 00434759
00434759  dec        dword ptr [ebp + 0x1f0]
0043475f  mov        eax, dword ptr [ebp + 0x1f0]
00434765  pop        edi
00434766  pop        esi
00434767  pop        ebx
00434768  mov        byte ptr [eax + ebp + 0x2cc], 0x20
00434770  pop        ebp
00434771  mov        ecx, dword ptr [esp + 0x54]
00434775  xor        ecx, esp
00434777  call       0x46c932

// block 0043477c
0043477c  add        esp, 0x58
0043477f  ret        4

// block 00434782
00434782  xor        eax, eax
00434784  mov        dword ptr [ebp + 0x38], eax
00434787  jmp        0x434790

// block 00434789
00434789  mov        dword ptr [ebp + 0x38], 0

// block 00434790
00434790  mov        ecx, dword ptr [esi]
00434792  push       ecx
00434793  mov        ebx, 3
00434798  call       0x461970

// block 0043479d
0043479d  movzx      ebx, word ptr [ebp + 0x38]
004347a1  mov        edx, dword ptr [esi]
004347a3  add        bx, 7
004347a7  push       edx
004347a8  call       0x461970

// block 004347ad
004347ad  pop        edi
004347ae  pop        esi
004347af  pop        ebx
004347b0  pop        ebp
004347b1  mov        ecx, dword ptr [esp + 0x54]
004347b5  xor        ecx, esp
004347b7  call       0x46c932

// block 004347bc
004347bc  add        esp, 0x58
004347bf  ret        4

// block 004347c2
004347c2  cmp        dword ptr [ebp + 0x14], 0xc
004347c6  jl         0x43483f

// block 004347c8
004347c8  mov        esi, ebp
004347ca  call       0x4339c0

// block 004347cf
004347cf  mov        eax, dword ptr [ebp + 0x38]
004347d2  mov        dword ptr [ebp + 4], 0x1b
004347d9  cmp        eax, 3
004347dc  ja         0x43483f

// block 004347de
004347de  jmp        dword ptr [eax*4 + 0x434874]

// block 004347e5
004347e5  xor        eax, eax
004347e7  cmp        dword ptr [ebp + 0x1f4], eax
004347ed  pop        edi
004347ee  sete       al
004347f1  pop        esi
004347f2  pop        ebx
004347f3  pop        ebp
004347f4  lea        eax, [eax*4 + 0xa]
004347fb  mov        dword ptr [0x4cee40], eax
00434800  mov        ecx, dword ptr [esp + 0x54]
00434804  xor        ecx, esp
00434806  call       0x46c932

// block 0043480b
0043480b  add        esp, 0x58
0043480e  ret        4

// block 00434811
00434811  mov        ecx, 4
00434816  mov        eax, 0x4ce8e8
0043481b  call       0x40f720

// block 00434820
00434820  pop        edi
00434821  pop        esi
00434822  pop        ebx
00434823  pop        ebp
00434824  mov        ecx, dword ptr [esp + 0x54]
00434828  xor        ecx, esp
0043482a  call       0x46c932

// block 0043482f
0043482f  add        esp, 0x58
00434832  ret        4

// block 00434835
00434835  mov        dword ptr [0x4cee40], 0xa

// block 0043483f
0043483f  pop        edi
00434840  pop        esi
00434841  pop        ebx

// block 00434842
00434842  mov        ecx, dword ptr [esp + 0x58]
00434846  pop        ebp
00434847  xor        ecx, esp
00434849  call       0x46c932

// block 0043484e
0043484e  add        esp, 0x58
00434851  ret        4
