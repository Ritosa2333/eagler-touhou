
// block 0040ee80
0040ee80  push       ebp
0040ee81  mov        ebp, esp
0040ee83  and        esp, 0xfffffff8
0040ee86  sub        esp, 0x19c
0040ee8c  mov        eax, dword ptr [0x4ad138]
0040ee91  xor        eax, esp
0040ee93  mov        dword ptr [esp + 0x198], eax
0040ee9a  push       ebx
0040ee9b  push       esi
0040ee9c  mov        esi, ecx
0040ee9e  mov        eax, dword ptr [esi + 0x30]
0040eea1  sub        eax, 0
0040eea4  push       edi
0040eea5  je         0x40f1aa

// block 0040eeab
0040eeab  sub        eax, 1
0040eeae  je         0x40efa5

// block 0040eeb4
0040eeb4  sub        eax, 3
0040eeb7  jne        0x40f332

// block 0040eebd
0040eebd  mov        ecx, dword ptr [0x4d48b8]
0040eec3  mov        eax, dword ptr [0x4d49d0]
0040eec8  mov        dword ptr [0x4d49d0], ecx
0040eece  mov        ecx, 0x4d48b8
0040eed3  mov        dword ptr [0x4d49d4], eax
0040eed8  call       0x40f690

// block 0040eedd
0040eedd  mov        eax, dword ptr [0x4d48c4]
0040eee2  mov        dword ptr [0x4d49dc], eax
0040eee7  test       eax, 0x100
0040eeec  je         0x40f332

// block 0040eef2
0040eef2  mov        eax, dword ptr [0x4b4514]
0040eef7  cmp        dword ptr [eax + 8], 0
0040eefb  mov        ebx, 0xfffffffd
0040ef00  je         0x40ef08

// block 0040ef02
0040ef02  mov        ecx, dword ptr [eax + 8]
0040ef05  and        dword ptr [ecx + 4], ebx

// block 0040ef08
0040ef08  cmp        dword ptr [eax + 0xc], 0
0040ef0c  je         0x40ef14

// block 0040ef0e
0040ef0e  mov        eax, dword ptr [eax + 0xc]
0040ef11  and        dword ptr [eax + 4], ebx

// block 0040ef14
0040ef14  call       0x436100

// block 0040ef19
0040ef19  mov        eax, dword ptr [0x4b43c8]
0040ef1e  cmp        dword ptr [eax + 8], 0
0040ef22  je         0x40ef2a

// block 0040ef24
0040ef24  mov        ecx, dword ptr [eax + 8]
0040ef27  and        dword ptr [ecx + 4], ebx

// block 0040ef2a
0040ef2a  cmp        dword ptr [eax + 0xc], 0
0040ef2e  je         0x40ef36

// block 0040ef30
0040ef30  mov        eax, dword ptr [eax + 0xc]
0040ef33  and        dword ptr [eax + 4], ebx

// block 0040ef36
0040ef36  call       0x409630

// block 0040ef3b
0040ef3b  mov        eax, dword ptr [0x4b43dc]
0040ef40  cmp        dword ptr [eax + 8], 0
0040ef44  je         0x40ef4c

// block 0040ef46
0040ef46  mov        ecx, dword ptr [eax + 8]
0040ef49  and        dword ptr [ecx + 4], ebx

// block 0040ef4c
0040ef4c  cmp        dword ptr [eax + 0xc], 0
0040ef50  je         0x40ef58

// block 0040ef52
0040ef52  mov        ecx, dword ptr [eax + 0xc]
0040ef55  and        dword ptr [ecx + 4], ebx

// block 0040ef58
0040ef58  push       eax
0040ef59  call       0x40e940

// block 0040ef5e
0040ef5e  mov        edi, dword ptr [0x4b44f0]
0040ef64  mov        eax, dword ptr [edi + 8]
0040ef67  test       eax, eax
0040ef69  je         0x40ef6e

// block 0040ef6b
0040ef6b  and        dword ptr [eax + 4], ebx

// block 0040ef6e
0040ef6e  mov        eax, dword ptr [edi + 0xc]
0040ef71  test       eax, eax
0040ef73  je         0x40ef78

// block 0040ef75
0040ef75  and        dword ptr [eax + 4], ebx

// block 0040ef78
0040ef78  push       0x666fc0
0040ef7d  lea        edx, [edi + 0x14]
0040ef80  push       0
0040ef82  push       edx
0040ef83  call       0x477420

// block 0040ef88
0040ef88  add        esp, 0xc
0040ef8b  xor        eax, eax
0040ef8d  mov        dword ptr [edi + 0x666fd8], eax
0040ef93  mov        dword ptr [edi + 0x666fe0], eax
0040ef99  mov        dword ptr [esi + 0x30], 1
0040efa0  jmp        0x40f332

// block 0040efa5
0040efa5  mov        eax, dword ptr [esi + 0x3c]
0040efa8  lea        edi, [esi + 0x3c]
0040efab  mov        dword ptr [edi + 4], eax
0040efae  mov        al, 0x20
0040efb0  test       byte ptr [0x4d48c4], al
0040efb6  jne        0x40efc0

// block 0040efb8
0040efb8  test       byte ptr [0x4d48c0], al
0040efbe  je         0x40efc9

// block 0040efc0
0040efc0  push       1
0040efc2  mov        eax, edi
0040efc4  call       0x464970

// block 0040efc9
0040efc9  mov        al, 0x10
0040efcb  test       byte ptr [0x4d48c4], al
0040efd1  jne        0x40efdb

// block 0040efd3
0040efd3  test       byte ptr [0x4d48c0], al
0040efd9  je         0x40efe4

// block 0040efdb
0040efdb  push       -1
0040efdd  mov        eax, edi
0040efdf  call       0x464970

// block 0040efe4
0040efe4  mov        edi, dword ptr [edi]
0040efe6  sub        edi, 0
0040efe9  je         0x40f111

// block 0040efef
0040efef  sub        edi, 1
0040eff2  je         0x40f01f

// block 0040eff4
0040eff4  sub        edi, 1
0040eff7  jne        0x40f332

// block 0040effd
0040effd  test       dword ptr [0x4d48c4], 0x80001
0040f007  je         0x40f332

// block 0040f00d
0040f00d  lea        ecx, [edi + 3]
0040f010  mov        eax, 0x4ce8e8
0040f015  call       0x40f720

// block 0040f01a
0040f01a  jmp        0x40f332

// block 0040f01f
0040f01f  cmp        dword ptr [0x4b43dc], 0
0040f026  je         0x40f332

// block 0040f02c
0040f02c  mov        ecx, dword ptr [esi + 0x1ec]
0040f032  lea        edi, [esi + 0x1ec]
0040f038  mov        al, 0x80
0040f03a  mov        dword ptr [edi + 4], ecx
0040f03d  test       byte ptr [0x4d48c4], al
0040f043  jne        0x40f04d

// block 0040f045
0040f045  test       byte ptr [0x4d48c0], al
0040f04b  je         0x40f056

// block 0040f04d
0040f04d  push       1
0040f04f  mov        eax, edi
0040f051  call       0x464970

// block 0040f056
0040f056  mov        al, 0x40
0040f058  test       byte ptr [0x4d48c4], al
0040f05e  jne        0x40f068

// block 0040f060
0040f060  test       byte ptr [0x4d48c0], al
0040f066  je         0x40f071

// block 0040f068
0040f068  push       -1
0040f06a  mov        eax, edi
0040f06c  call       0x464970

// block 0040f071
0040f071  test       dword ptr [0x4d48c8], 0x80001
0040f07b  je         0x40f332

// block 0040f081
0040f081  call       0x40ea10

// block 0040f086
0040f086  call       0x436100

// block 0040f08b
0040f08b  mov        eax, dword ptr [0x4b43c8]
0040f090  call       0x40e810

// block 0040f095
0040f095  call       0x409630

// block 0040f09a
0040f09a  call       0x412ee0

// block 0040f09f
0040f09f  mov        edx, dword ptr [0x4b43dc]
0040f0a5  push       edx
0040f0a6  call       0x40e940

// block 0040f0ab
0040f0ab  call       0x41d560

// block 0040f0b0
0040f0b0  mov        eax, dword ptr [0x4b44f0]
0040f0b5  call       0x40e810

// block 0040f0ba
0040f0ba  call       0x40e8b0

// block 0040f0bf
0040f0bf  push       0x50
0040f0c1  lea        eax, [esp + 0x14]
0040f0c5  push       0
0040f0c7  push       eax
0040f0c8  call       0x477420

// block 0040f0cd
0040f0cd  fldz       
0040f0cf  mov        edx, dword ptr [0x4b43dc]
0040f0d5  fst        dword ptr [esp + 0x1c]
0040f0d9  mov        eax, dword ptr [edx + 0x64]
0040f0dc  fst        dword ptr [esp + 0x20]
0040f0e0  mov        edx, dword ptr [eax + 0x8c]
0040f0e6  fstp       dword ptr [esp + 0x24]
0040f0ea  add        esp, 0xc
0040f0ed  lea        ecx, [esp + 0x10]
0040f0f1  push       ecx
0040f0f2  mov        ecx, dword ptr [edi]
0040f0f4  mov        eax, dword ptr [edx + ecx*8]
0040f0f7  push       eax
0040f0f8  mov        dword ptr [esp + 0x2c], 0x2710
0040f100  call       0x412990

// block 0040f105
0040f105  mov        dword ptr [esi + 0x30], 4
0040f10c  jmp        0x40f332

// block 0040f111
0040f111  mov        ecx, dword ptr [esi + 0x114]
0040f117  lea        edi, [esi + 0x114]
0040f11d  mov        al, 0x80
0040f11f  mov        dword ptr [edi + 4], ecx
0040f122  test       byte ptr [0x4d48c4], al
0040f128  jne        0x40f132

// block 0040f12a
0040f12a  test       byte ptr [0x4d48c0], al
0040f130  je         0x40f13b

// block 0040f132
0040f132  push       1
0040f134  mov        eax, edi
0040f136  call       0x464970

// block 0040f13b
0040f13b  mov        al, 0x40
0040f13d  test       byte ptr [0x4d48c4], al
0040f143  jne        0x40f14d

// block 0040f145
0040f145  test       byte ptr [0x4d48c0], al
0040f14b  je         0x40f156

// block 0040f14d
0040f14d  push       -1
0040f14f  mov        eax, edi
0040f151  call       0x464970

// block 0040f156
0040f156  test       dword ptr [0x4d48c4], 0x80001
0040f160  je         0x40f332

// block 0040f166
0040f166  call       0x41e000

// block 0040f16b
0040f16b  call       0x4364d0

// block 0040f170
0040f170  call       0x4098e0

// block 0040f175
0040f175  call       0x406bd0

// block 0040f17a
0040f17a  call       0x425c00

// block 0040f17f
0040f17f  call       0x43ddf0

// block 0040f184
0040f184  call       0x413170

// block 0040f189
0040f189  and        dword ptr [esi + 0x78c], 0xfffffffd
0040f190  push       esi
0040f191  lea        eax, [esi + 0x10]
0040f194  mov        edi, 0x40ed70
0040f199  call       0x464cb0

// block 0040f19e
0040f19e  mov        dword ptr [esi + 0x30], 2
0040f1a5  jmp        0x40f332

// block 0040f1aa
0040f1aa  call       0x40e850

// block 0040f1af
0040f1af  lea        edx, [esp + 0x60]
0040f1b3  push       edx
0040f1b4  push       0x49f7d8
0040f1b9  xor        ebx, ebx
0040f1bb  call       dword ptr [0x498078]

// block 0040f1c1
0040f1c1  mov        edi, eax
0040f1c3  cmp        edi, -1
0040f1c6  je         0x40f1d9

// block 0040f1c8
0040f1c8  lea        eax, [esp + 0x60]
0040f1cc  push       eax
0040f1cd  push       edi
0040f1ce  inc        ebx
0040f1cf  call       dword ptr [0x49807c]

// block 0040f1d5
0040f1d5  test       eax, eax
0040f1d7  jne        0x40f1c8

// block 0040f1d9
0040f1d9  push       edi
0040f1da  call       dword ptr [0x498080]

// block 0040f1e0
0040f1e0  xor        edi, edi
0040f1e2  mov        dword ptr [esi + 0x38], ebx
0040f1e5  cmp        ebx, edi
0040f1e7  je         0x40f286

// block 0040f1ed
0040f1ed  lea        ecx, [ebx*4]
0040f1f4  push       ecx
0040f1f5  call       0x46d04a

// block 0040f1fa
0040f1fa  add        esp, 4
0040f1fd  lea        edx, [esp + 0x60]
0040f201  push       edx
0040f202  push       0x49f7d8
0040f207  mov        dword ptr [esi + 0x34], eax
0040f20a  call       dword ptr [0x498078]

// block 0040f210
0040f210  mov        dword ptr [esp + 0xc], eax
0040f214  test       ebx, ebx
0040f216  jle        0x40f279

// block 0040f218
0040f218  jmp        0x40f220

// block 0040f220
0040f220  lea        eax, [esp + 0x8c]
0040f227  lea        edx, [eax + 1]
0040f22a  lea        ebx, [ebx]

// block 0040f230
0040f230  mov        cl, byte ptr [eax]
0040f232  inc        eax
0040f233  test       cl, cl
0040f235  jne        0x40f230

// block 0040f237
0040f237  sub        eax, edx
0040f239  inc        eax
0040f23a  push       eax
0040f23b  call       0x46d04a

// block 0040f240
0040f240  mov        ecx, dword ptr [esi + 0x34]
0040f243  mov        dword ptr [ecx + edi*4], eax
0040f246  mov        edx, dword ptr [esi + 0x34]
0040f249  mov        edx, dword ptr [edx + edi*4]
0040f24c  add        esp, 4
0040f24f  lea        ecx, [esp + 0x8c]

// block 0040f256
0040f256  mov        al, byte ptr [ecx]
0040f258  mov        byte ptr [edx], al
0040f25a  inc        ecx
0040f25b  inc        edx
0040f25c  test       al, al
0040f25e  jne        0x40f256

// block 0040f260
0040f260  mov        ecx, dword ptr [esp + 0xc]
0040f264  lea        eax, [esp + 0x60]
0040f268  push       eax
0040f269  push       ecx
0040f26a  call       dword ptr [0x49807c]

// block 0040f270
0040f270  test       eax, eax
0040f272  je         0x40f279

// block 0040f274
0040f274  inc        edi
0040f275  cmp        edi, ebx
0040f277  jl         0x40f220

// block 0040f279
0040f279  mov        edx, dword ptr [esp + 0xc]
0040f27d  push       edx
0040f27e  call       dword ptr [0x498080]

// block 0040f284
0040f284  xor        edi, edi

// block 0040f286
0040f286  mov        ecx, 1
0040f28b  mov        dword ptr [esi + 0x44], 3
0040f292  mov        dword ptr [esi + 0x10c], ecx
0040f298  mov        eax, dword ptr [esi + 0x44]
0040f29b  cmp        eax, edi
0040f29d  je         0x40f2ae

// block 0040f29f
0040f29f  jg         0x40f2a7

// block 0040f2a1
0040f2a1  dec        eax
0040f2a2  mov        dword ptr [esi + 0x3c], eax
0040f2a5  jmp        0x40f2b1

// block 0040f2a7
0040f2a7  xor        eax, eax
0040f2a9  mov        dword ptr [esi + 0x3c], eax
0040f2ac  jmp        0x40f2b1

// block 0040f2ae
0040f2ae  mov        dword ptr [esi + 0x3c], edi

// block 0040f2b1
0040f2b1  mov        eax, ebx
0040f2b3  mov        dword ptr [esi + 0x11c], ebx
0040f2b9  mov        dword ptr [esi + 0x1e4], ecx
0040f2bf  cmp        eax, edi
0040f2c1  je         0x40f2d8

// block 0040f2c3
0040f2c3  jg         0x40f2ce

// block 0040f2c5
0040f2c5  dec        eax
0040f2c6  mov        dword ptr [esi + 0x114], eax
0040f2cc  jmp        0x40f2de

// block 0040f2ce
0040f2ce  xor        eax, eax
0040f2d0  mov        dword ptr [esi + 0x114], eax
0040f2d6  jmp        0x40f2de

// block 0040f2d8
0040f2d8  mov        dword ptr [esi + 0x114], edi

// block 0040f2de
0040f2de  mov        eax, ecx
0040f2e0  mov        dword ptr [esi + 0x1f4], ecx
0040f2e6  mov        dword ptr [esi + 0x2bc], ecx
0040f2ec  cmp        eax, edi
0040f2ee  je         0x40f305

// block 0040f2f0
0040f2f0  jg         0x40f2fb

// block 0040f2f2
0040f2f2  dec        eax
0040f2f3  mov        dword ptr [esi + 0x1ec], eax
0040f2f9  jmp        0x40f30b

// block 0040f2fb
0040f2fb  xor        eax, eax
0040f2fd  mov        dword ptr [esi + 0x1ec], eax
0040f303  jmp        0x40f30b

// block 0040f305
0040f305  mov        dword ptr [esi + 0x1ec], edi

// block 0040f30b
0040f30b  fldz       
0040f30d  mov        dword ptr [esi + 0x30], ecx
0040f310  fst        dword ptr [esi + 0x77c]
0040f316  mov        dword ptr [esi + 0x788], 0x3e8
0040f320  fld        dword ptr [0x4a3d78]
0040f326  fstp       dword ptr [esi + 0x780]
0040f32c  fstp       dword ptr [esi + 0x784]

// block 0040f332
0040f332  mov        ecx, dword ptr [esp + 0x1a4]
0040f339  pop        edi
0040f33a  pop        esi
0040f33b  pop        ebx
0040f33c  xor        ecx, esp
0040f33e  mov        eax, 1
0040f343  call       0x46c932

// block 0040f348
0040f348  mov        esp, ebp
0040f34a  pop        ebp
0040f34b  ret        
