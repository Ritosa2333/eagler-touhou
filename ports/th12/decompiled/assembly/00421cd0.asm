
// block 00421cd0
00421cd0  push       ebp
00421cd1  mov        ebp, esp
00421cd3  and        esp, 0xfffffff8
00421cd6  sub        esp, 8
00421cd9  mov        eax, dword ptr [0x4ce8cc]
00421cde  push       ebx
00421cdf  push       ebp
00421ce0  push       esi
00421ce1  push       edi
00421ce2  mov        edi, dword ptr [0x4b44e8]
00421ce8  or         dword ptr [edi + 0x60], 4
00421cec  xor        ebp, ebp
00421cee  cmp        dword ptr [eax], ebp
00421cf0  mov        dword ptr [esp + 0x10], edi
00421cf4  jl         0x421d1e

// block 00421cf6
00421cf6  mov        esi, 0x180
00421cfb  jmp        0x421d00

// block 00421d00
00421d00  test       dword ptr [0x4cee78], esi
00421d06  jne        0x42217b

// block 00421d0c
00421d0c  push       1
00421d0e  call       dword ptr [0x498084]

// block 00421d14
00421d14  mov        ecx, dword ptr [0x4ce8cc]
00421d1a  cmp        dword ptr [ecx], ebp
00421d1c  jge        0x421d00

// block 00421d1e
00421d1e  cmp        dword ptr [0x4cee4c], ebp
00421d24  jne        0x421d2e

// block 00421d26
00421d26  push       0x3c
00421d28  call       dword ptr [0x498084]

// block 00421d2e
00421d2e  mov        ecx, dword ptr [0x4ceaa4]
00421d34  mov        eax, dword ptr [ecx + 0x494]
00421d3a  mov        esi, ecx
00421d3c  mov        ebx, 2
00421d41  cmp        eax, ebp
00421d43  je         0x421d49

// block 00421d45
00421d45  mov        edx, ebx
00421d47  call       eax

// block 00421d49
00421d49  mov        edx, ebx
00421d4b  mov        word ptr [esi + 0x3c4], dx
00421d52  mov        eax, dword ptr [0x4ceaa4]
00421d57  push       eax
00421d58  call       0x455630

// block 00421d5d
00421d5d  fld1       
00421d5f  fstp       dword ptr [0x4b2ed0]
00421d65  mov        dword ptr [0x4b0cbc], ebp
00421d6b  mov        dword ptr [0x4b0cc0], ebp
00421d71  cmp        dword ptr [0x4cee4c], ebp
00421d77  je         0x421f8f

// block 00421d7d
00421d7d  cmp        dword ptr [0x4b0cb0], 7
00421d84  jne        0x421d93

// block 00421d86
00421d86  mov        ecx, 4
00421d8b  mov        dword ptr [0x4b0ca8], ecx
00421d91  jmp        0x421d99

// block 00421d93
00421d93  mov        ecx, dword ptr [0x4b0ca8]

// block 00421d99
00421d99  mov        eax, dword ptr [0x4b0c90]
00421d9e  mov        edx, dword ptr [0x4b0c94]
00421da4  lea        edx, [edx + eax*2]
00421da7  imul       edx, edx, 0x45f4
00421dad  add        edx, dword ptr [0x4b451c]
00421db3  mov        eax, ecx
00421db5  imul       eax, eax, 0x118
00421dbb  add        eax, edx
00421dbd  test       byte ptr [0x4b0ce0], 8
00421dc4  mov        edx, dword ptr [eax + 0x18]
00421dc7  mov        dword ptr [0x4b0c40], edx
00421dcd  movsx      eax, byte ptr [eax + 0x1d]
00421dd1  mov        dword ptr [0x4b0cc8], eax
00421dd6  jne        0x421dde

// block 00421dd8
00421dd8  mov        dword ptr [0x4b0cc4], ebp

// block 00421dde
00421dde  mov        eax, dword ptr [0x4b43e4]
00421de3  mov        dword ptr [0x4b0cdc], ebp
00421de9  mov        dword ptr [0x4b0c44], ebp
00421def  mov        dword ptr [0x4b0ca0], ebx
00421df5  cmp        eax, ebp
00421df7  je         0x421e0d

// block 00421df9
00421df9  mov        ecx, dword ptr [0x4b0ca4]
00421dff  push       ecx
00421e00  push       ebx
00421e01  push       eax
00421e02  call       0x41cf40

// block 00421e07
00421e07  mov        ecx, dword ptr [0x4b0ca8]

// block 00421e0d
00421e0d  mov        edx, dword ptr [ecx*4 + 0x4b30e8]
00421e14  imul       edx, edx, 0x64
00421e17  mov        dword ptr [0x4b0cf0], edx
00421e1d  mov        eax, dword ptr [ecx*4 + 0x4b30fc]
00421e24  fild       dword ptr [0x4b0cf0]
00421e2a  imul       eax, eax, 0x64
00421e2d  fld        qword ptr [0x4a3ce8]
00421e33  fdiv       st(1), st(0)
00421e35  mov        dword ptr [0x4b0ca4], ebp
00421e3b  mov        dword ptr [0x4b0c54], ebp
00421e41  mov        dword ptr [0x4b0c50], ebp
00421e47  mov        dword ptr [0x4b0c4c], ebp
00421e4d  mov        dword ptr [0x4b0c58], ebp
00421e53  mov        dword ptr [0x4b0c5c], ebp
00421e59  mov        dword ptr [0x4b0cf4], eax
00421e5e  fxch       st(1)
00421e60  fstp       dword ptr [esp + 0x14]
00421e64  fmul       dword ptr [esp + 0x14]
00421e68  call       0x4931e0

// block 00421e6d
00421e6d  test       byte ptr [0x4b0ce0], 0x10
00421e74  mov        dword ptr [0x4b0c78], eax
00421e79  mov        dword ptr [0x4b0c9c], ebp
00421e7f  jne        0x421e89

// block 00421e81
00421e81  mov        dword ptr [0x4b0c98], ebx
00421e87  jmp        0x421ea4

// block 00421e89
00421e89  mov        eax, dword ptr [0x4b0cec]
00421e8e  cmp        eax, ebp
00421e90  jne        0x421e9e

// block 00421e92
00421e92  mov        dword ptr [0x4b0c98], 9
00421e9c  jmp        0x421ea4

// block 00421e9e
00421e9e  dec        eax
00421e9f  mov        dword ptr [0x4b0c98], eax

// block 00421ea4
00421ea4  call       0x436410

// block 00421ea9
00421ea9  test       eax, eax
00421eab  je         0x422180

// block 00421eb1
00421eb1  mov        eax, dword ptr [0x4b0cb0]
00421eb6  cmp        eax, 1
00421eb9  jle        0x421f08

// block 00421ebb
00421ebb  cmp        eax, 7
00421ebe  je         0x421f08

// block 00421ec0
00421ec0  test       byte ptr [0x4b0ce0], 8
00421ec7  mov        eax, dword ptr [0x4b0cd0]
00421ecc  jne        0x421eeb

// block 00421ece
00421ece  cmp        eax, 0xc8
00421ed3  mov        dword ptr [0x4b0c48], 0xc8
00421edd  jl         0x421f20

// block 00421edf
00421edf  mov        eax, dword ptr [0x4b0cd4]
00421ee4  cmp        eax, 0xc8
00421ee9  jmp        0x421f1e

// block 00421eeb
00421eeb  cmp        eax, 0x190
00421ef0  mov        dword ptr [0x4b0c48], 0x190
00421efa  jl         0x421f20

// block 00421efc
00421efc  mov        eax, dword ptr [0x4b0cd4]
00421f01  cmp        eax, 0x190
00421f06  jmp        0x421f1e

// block 00421f08
00421f08  mov        eax, dword ptr [0x4b0cd0]
00421f0d  cmp        eax, ebp
00421f0f  mov        dword ptr [0x4b0c48], ebp
00421f15  jl         0x421f20

// block 00421f17
00421f17  mov        eax, dword ptr [0x4b0cd4]
00421f1c  cmp        eax, ebp

// block 00421f1e
00421f1e  jle        0x421f25

// block 00421f20
00421f20  mov        dword ptr [0x4b0c48], eax

// block 00421f25
00421f25  mov        ecx, dword ptr [0x4b4514]
00421f2b  push       ecx
00421f2c  call       0x4385b0

// block 00421f31
00421f31  and        dword ptr [0x4b0ce0], 0xfffffffb
00421f38  cmp        dword ptr [edi + 0x74], ebp
00421f3b  jne        0x421f6d

// block 00421f3d
00421f3d  mov        edx, dword ptr [0x4b0c94]
00421f43  mov        eax, dword ptr [0x4b0c90]
00421f48  lea        ecx, [edx + eax*2]
00421f4b  mov        edx, dword ptr [0x4b451c]
00421f51  imul       ecx, ecx, 0x45f4
00421f57  cmp        dword ptr [ecx + edx + 0x590], 0x1869f
00421f62  lea        eax, [ecx + edx + 0x590]
00421f69  jge        0x421f6d

// block 00421f6b
00421f6b  inc        dword ptr [eax]

// block 00421f6d
00421f6d  mov        al, byte ptr [0x4b0ce0]
00421f72  and        al, 8
00421f74  movzx      ecx, al
00421f77  neg        ecx
00421f79  sbb        ecx, ecx
00421f7b  and        ecx, 0xfffffe00
00421f81  mov        dword ptr [0x4b0cd8], ebp
00421f87  mov        dword ptr [0x4b0ccc], ecx
00421f8d  jmp        0x421fa1

// block 00421f8f
00421f8f  mov        eax, dword ptr [0x4b0c44]
00421f94  cmp        eax, dword ptr [0x4b0c40]
00421f9a  jle        0x421fa1

// block 00421f9c
00421f9c  mov        dword ptr [0x4b0c40], eax

// block 00421fa1
00421fa1  mov        ebx, 1
00421fa6  test       byte ptr [0x4b0c8c], bl
00421fac  jne        0x421fbe

// block 00421fae
00421fae  or         dword ptr [0x4b0c8c], ebx
00421fb4  mov        dword ptr [0x4b0c88], 0x4b2ed0

// block 00421fbe
00421fbe  fldz       
00421fc0  push       0x24
00421fc2  fstp       dword ptr [0x4b0c84]
00421fc8  mov        dword ptr [0x4b0c80], ebp
00421fce  mov        dword ptr [0x4b0c7c], 0xffffffff
00421fd8  call       0x46c9ea

// block 00421fdd
00421fdd  add        esp, 4
00421fe0  cmp        eax, ebp
00421fe2  je         0x422000

// block 00421fe4
00421fe4  and        dword ptr [eax + 4], 0xfffffffe
00421fe8  mov        dword ptr [eax + 8], ebp
00421feb  mov        dword ptr [eax + 0xc], ebp
00421fee  mov        dword ptr [eax + 0x10], ebp
00421ff1  mov        dword ptr [eax], ebp
00421ff3  mov        dword ptr [eax + 0x14], eax
00421ff6  mov        dword ptr [eax + 0x18], ebp
00421ff9  mov        dword ptr [eax + 0x1c], ebp
00421ffc  mov        esi, eax
00421ffe  jmp        0x422002

// block 00422000
00422000  xor        esi, esi

// block 00422002
00422002  mov        edx, dword ptr [esi + 4]
00422005  and        edx, 0xfffffffd
00422008  or         edx, ebx
0042200a  mov        ebx, 0xa
0042200f  mov        dword ptr [esi + 8], 0x422bd0
00422016  mov        dword ptr [esi + 0xc], ebp
00422019  mov        dword ptr [esi + 0x10], ebp
0042201c  mov        dword ptr [esi + 0x20], edi
0042201f  mov        dword ptr [esi + 4], edx
00422022  call       0x462380

// block 00422027
00422027  push       0x24
00422029  mov        dword ptr [edi + 8], esi
0042202c  call       0x46c9ea

// block 00422031
00422031  add        esp, 4
00422034  cmp        eax, ebp
00422036  je         0x422054

// block 00422038
00422038  and        dword ptr [eax + 4], 0xfffffffe
0042203c  mov        dword ptr [eax + 8], ebp
0042203f  mov        dword ptr [eax + 0xc], ebp
00422042  mov        dword ptr [eax + 0x10], ebp
00422045  mov        dword ptr [eax], ebp
00422047  mov        dword ptr [eax + 0x14], eax
0042204a  mov        dword ptr [eax + 0x18], ebp
0042204d  mov        dword ptr [eax + 0x1c], ebp
00422050  mov        esi, eax
00422052  jmp        0x422056

// block 00422054
00422054  xor        esi, esi

// block 00422056
00422056  mov        eax, dword ptr [esi + 4]
00422059  and        eax, 0xfffffffd
0042205c  or         eax, 1
0042205f  mov        ebx, 0x39
00422064  mov        dword ptr [esi + 8], 0x422be0
0042206b  mov        dword ptr [esi + 0xc], ebp
0042206e  mov        dword ptr [esi + 0x10], ebp
00422071  mov        dword ptr [esi + 0x20], edi
00422074  mov        dword ptr [esi + 4], eax
00422077  call       0x462420

// block 0042207c
0042207c  mov        eax, dword ptr [esp + 0x10]
00422080  mov        dword ptr [edi + 0xc], esi
00422083  add        edi, 0x24
00422086  mov        ecx, 0xf
0042208b  mov        esi, 0x4ceab0

// block 00422090
00422090  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00422092
00422092  mov        ecx, dword ptr [0x4b452c]
00422098  mov        edx, dword ptr [ecx]
0042209a  mov        dword ptr [eax + 4], edx
0042209d  test       byte ptr [0x4b0ce0], 2
004220a4  jne        0x422117

// block 004220a6
004220a6  mov        eax, dword ptr [eax + 0x74]
004220a9  push       eax
004220aa  call       0x43b600

// block 004220af
004220af  test       eax, eax
004220b1  je         0x422177

// block 004220b7
004220b7  mov        ecx, dword ptr [0x4b452c]
004220bd  mov        edx, dword ptr [ecx + 4]
004220c0  push       edx
004220c1  call       0x402f40

// block 004220c6
004220c6  test       eax, eax
004220c8  je         0x422177

// block 004220ce
004220ce  call       0x41df40

// block 004220d3
004220d3  test       eax, eax
004220d5  je         0x422177

// block 004220db
004220db  call       0x4097f0

// block 004220e0
004220e0  test       eax, eax
004220e2  je         0x422177

// block 004220e8
004220e8  call       0x425b10

// block 004220ed
004220ed  test       eax, eax
004220ef  je         0x422177

// block 004220f5
004220f5  call       0x4281c0

// block 004220fa
004220fa  test       eax, eax
004220fc  je         0x422177

// block 004220fe
004220fe  call       0x431fb0

// block 00422103
00422103  test       eax, eax
00422105  je         0x422177

// block 00422107
00422107  call       0x43dd50

// block 0042210c
0042210c  test       eax, eax
0042210e  je         0x422177

// block 00422110
00422110  call       0x44a230

// block 00422115
00422115  jmp        0x422136

// block 00422117
00422117  call       0x43c730

// block 0042211c
0042211c  mov        eax, dword ptr [0x4b43e4]
00422121  push       eax
00422122  call       0x41d3b0

// block 00422127
00422127  mov        ecx, dword ptr [0x4b452c]
0042212d  mov        edx, dword ptr [ecx + 4]
00422130  push       edx
00422131  call       0x402f40

// block 00422136
00422136  test       eax, eax
00422138  je         0x422177

// block 0042213a
0042213a  test       byte ptr [0x4b0ce0], 9
00422141  jne        0x422157

// block 00422143
00422143  mov        eax, dword ptr [0x4b452c]
00422148  mov        ecx, dword ptr [eax + 0xc]
0042214b  push       ecx
0042214c  call       0x4130e0

// block 00422151
00422151  test       eax, eax
00422153  je         0x422177

// block 00422155
00422155  jmp        0x42215c

// block 00422157
00422157  call       0x421c60

// block 0042215c
0042215c  call       0x40fb00

// block 00422161
00422161  test       eax, eax
00422163  je         0x422177

// block 00422165
00422165  call       0x406b20

// block 0042216a
0042216a  test       eax, eax
0042216c  je         0x422177

// block 0042216e
0042216e  call       0x40daa0

// block 00422173
00422173  test       eax, eax
00422175  jne        0x4221bd

// block 00422177
00422177  mov        edi, dword ptr [esp + 0x10]

// block 0042217b
0042217b  mov        ebx, 2

// block 00422180
00422180  or         dword ptr [edi + 0x60], 8
00422184  mov        esi, 0x4ce8e8
00422189  call       0x430840

// block 0042218e
0042218e  mov        dword ptr [0x4cf0e8], ebp
00422194  mov        dword ptr [0x4cf0e4], 1
0042219e  mov        eax, dword ptr [edi + 8]
004221a1  cmp        eax, ebp
004221a3  je         0x4221a8

// block 004221a5
004221a5  or         dword ptr [eax + 4], ebx

// block 004221a8
004221a8  mov        eax, dword ptr [edi + 0xc]
004221ab  cmp        eax, ebp
004221ad  je         0x4221b2

// block 004221af
004221af  or         dword ptr [eax + 4], ebx

// block 004221b2
004221b2  or         eax, 0xffffffff
004221b5  pop        edi
004221b6  pop        esi
004221b7  pop        ebp
004221b8  pop        ebx
004221b9  mov        esp, ebp
004221bb  pop        ebp
004221bc  ret        

// block 004221bd
004221bd  test       byte ptr [0x4b0ce0], 0x20
004221c4  jne        0x4221ec

// block 004221c6
004221c6  call       0x430240

// block 004221cb
004221cb  mov        edx, dword ptr [0x4b452c]
004221d1  mov        eax, dword ptr [edx + 0x10]
004221d4  push       eax
004221d5  push       ebp
004221d6  call       0x4300d0

// block 004221db
004221db  mov        ecx, dword ptr [0x4b452c]
004221e1  mov        edx, dword ptr [ecx + 0x14]
004221e4  push       edx
004221e5  push       1
004221e7  call       0x4300d0

// block 004221ec
004221ec  fldz       
004221ee  mov        eax, dword ptr [0x4b43e0]
004221f3  mov        esi, dword ptr [esp + 0x10]
004221f7  fst        qword ptr [eax + 0x2c]
004221fa  fstp       qword ptr [eax + 0x24]
004221fd  push       ebp
004221fe  lea        eax, [esi + 0x10]
00422201  call       0x4067e0

// block 00422206
00422206  cmp        dword ptr [0x4d14d4], ebp
0042220c  je         0x422220

// block 0042220e
0042220e  mov        edi, edi

// block 00422210
00422210  push       0x10
00422212  call       dword ptr [0x498084]

// block 00422218
00422218  cmp        dword ptr [0x4d14d4], ebp
0042221e  jne        0x422210

// block 00422220
00422220  cmp        dword ptr [0x4b0cb8], ebp
00422226  je         0x42222e

// block 00422228
00422228  mov        dword ptr [0x4b0cc0], ebp

// block 0042222e
0042222e  mov        edi, 0x4ce8e8
00422233  mov        dword ptr [0x4b0cb8], ebp
00422239  call       0x4306c0

// block 0042223e
0042223e  and        dword ptr [esi + 0x60], 0xfffffffb
00422242  and        dword ptr [0x4b0ce0], 0xfffffff4
00422249  mov        eax, esi
0042224b  mov        dword ptr [0x4cf0e8], ebp
00422251  mov        dword ptr [0x4cf0e4], 1
0042225b  mov        dword ptr [0x4ce55c], ebp
00422261  mov        dword ptr [0x4b450c], ebp
00422267  mov        dword ptr [0x4b4508], ebp
0042226d  call       0x40e810

// block 00422272
00422272  pop        edi
00422273  xor        eax, eax
00422275  pop        esi
00422276  pop        ebp
00422277  pop        ebx
00422278  mov        esp, ebp
0042227a  pop        ebp
0042227b  ret        
