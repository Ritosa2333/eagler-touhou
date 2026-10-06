
// block 00478e16
00478e16  mov        edi, edi
00478e18  push       ebp
00478e19  mov        ebp, esp
00478e1b  sub        esp, 0x200
00478e21  mov        eax, dword ptr [0x4ad138]
00478e26  xor        eax, ebp
00478e28  mov        dword ptr [ebp - 4], eax
00478e2b  mov        ecx, dword ptr [ebp + 0x14]
00478e2e  mov        eax, dword ptr [ebp + 8]
00478e31  push       esi
00478e32  xor        esi, esi
00478e34  push       edi
00478e35  mov        edi, dword ptr [ebp + 0xc]
00478e38  mov        dword ptr [ebp - 0x1e0], ecx
00478e3e  lea        ecx, [ebp - 0x184]
00478e44  mov        dword ptr [ebp - 0x19c], eax
00478e4a  mov        dword ptr [ebp - 0x1ac], edi
00478e50  mov        dword ptr [ebp - 0x1a4], ecx
00478e56  mov        dword ptr [ebp - 0x1d8], 0x15e
00478e60  mov        dword ptr [ebp - 0x1d0], esi
00478e66  mov        dword ptr [ebp - 0x1ec], esi
00478e6c  mov        dword ptr [ebp - 0x188], esi
00478e72  mov        dword ptr [ebp - 0x1f0], esi
00478e78  cmp        edi, esi
00478e7a  jne        0x478e9c

// block 00478e7c
00478e7c  call       0x46e96f

// block 00478e81
00478e81  push       esi
00478e82  push       esi
00478e83  push       esi
00478e84  push       esi
00478e85  push       esi
00478e86  mov        dword ptr [eax], 0x16
00478e8c  call       0x470f2d

// block 00478e91
00478e91  add        esp, 0x14
00478e94  or         eax, 0xffffffff
00478e97  jmp        0x479f2c

// block 00478e9c
00478e9c  cmp        eax, esi
00478e9e  je         0x478e7c

// block 00478ea0
00478ea0  test       byte ptr [eax + 0xc], 0x40
00478ea4  push       ebx
00478ea5  jne        0x478f21

// block 00478ea7
00478ea7  push       eax
00478ea8  call       0x47c1da

// block 00478ead
00478ead  pop        ecx
00478eae  mov        edx, 0x4adbb0
00478eb3  cmp        eax, -1
00478eb6  je         0x478ed3

// block 00478eb8
00478eb8  cmp        eax, -2
00478ebb  je         0x478ed3

// block 00478ebd
00478ebd  mov        ecx, eax
00478ebf  and        ecx, 0x1f
00478ec2  mov        ebx, eax
00478ec4  sar        ebx, 5
00478ec7  shl        ecx, 6
00478eca  add        ecx, dword ptr [ebx*4 + 0x4d6320]
00478ed1  jmp        0x478ed5

// block 00478ed3
00478ed3  mov        ecx, edx

// block 00478ed5
00478ed5  test       byte ptr [ecx + 0x24], 0x7f
00478ed9  jne        0x478f01

// block 00478edb
00478edb  cmp        eax, -1
00478ede  je         0x478ef9

// block 00478ee0
00478ee0  cmp        eax, -2
00478ee3  je         0x478ef9

// block 00478ee5
00478ee5  mov        ecx, eax
00478ee7  and        eax, 0x1f
00478eea  sar        ecx, 5
00478eed  shl        eax, 6
00478ef0  add        eax, dword ptr [ecx*4 + 0x4d6320]
00478ef7  jmp        0x478efb

// block 00478ef9
00478ef9  mov        eax, edx

// block 00478efb
00478efb  test       byte ptr [eax + 0x24], 0x80
00478eff  je         0x478f21

// block 00478f01
00478f01  call       0x46e96f

// block 00478f06
00478f06  push       esi
00478f07  push       esi
00478f08  push       esi
00478f09  push       esi
00478f0a  push       esi
00478f0b  mov        dword ptr [eax], 0x16
00478f11  call       0x470f2d

// block 00478f16
00478f16  add        esp, 0x14
00478f19  or         eax, 0xffffffff
00478f1c  jmp        0x479f2b

// block 00478f21
00478f21  push       dword ptr [ebp + 0x10]
00478f24  lea        ecx, [ebp - 0x200]
00478f2a  call       0x46d3b4

// block 00478f2f
00478f2f  mov        al, byte ptr [edi]
00478f31  mov        byte ptr [ebp - 0x19d], 0
00478f38  mov        dword ptr [ebp - 0x18c], esi
00478f3e  mov        dword ptr [ebp - 0x1c4], esi
00478f44  test       al, al
00478f46  je         0x479f12

// block 00478f4c
00478f4c  movzx      eax, al
00478f4f  push       eax
00478f50  call       0x48bb76

// block 00478f55
00478f55  pop        ecx
00478f56  test       eax, eax
00478f58  je         0x478fa0

// block 00478f5a
00478f5a  push       dword ptr [ebp - 0x19c]
00478f60  dec        dword ptr [ebp - 0x18c]
00478f66  push       dword ptr [ebp - 0x19c]
00478f6c  lea        esi, [ebp - 0x18c]
00478f72  call       0x478dec

// block 00478f77
00478f77  pop        ecx
00478f78  push       eax
00478f79  call       0x478dd9

// block 00478f7e
00478f7e  mov        esi, dword ptr [ebp - 0x1ac]
00478f84  pop        ecx
00478f85  pop        ecx

// block 00478f86
00478f86  inc        esi
00478f87  movzx      eax, byte ptr [esi]
00478f8a  push       eax
00478f8b  call       0x48bb76

// block 00478f90
00478f90  pop        ecx
00478f91  test       eax, eax
00478f93  jne        0x478f86

// block 00478f95
00478f95  mov        dword ptr [ebp - 0x1ac], esi
00478f9b  jmp        0x479e00

// block 00478fa0
00478fa0  mov        esi, dword ptr [ebp - 0x1ac]
00478fa6  mov        al, byte ptr [esi]
00478fa8  cmp        al, 0x25
00478faa  jne        0x479d7c

// block 00478fb0
00478fb0  cmp        byte ptr [esi + 1], al
00478fb3  je         0x479d6e

// block 00478fb9
00478fb9  xor        edi, edi
00478fbb  mov        dword ptr [ebp - 0x1c8], edi
00478fc1  mov        byte ptr [ebp - 0x1d1], 0
00478fc8  mov        dword ptr [ebp - 0x1a8], edi
00478fce  mov        dword ptr [ebp - 0x1c0], edi
00478fd4  mov        dword ptr [ebp - 0x194], edi
00478fda  mov        dword ptr [ebp - 0x1cc], edi
00478fe0  mov        byte ptr [ebp - 0x19e], 0
00478fe7  mov        byte ptr [ebp - 0x19f], 0
00478fee  mov        byte ptr [ebp - 0x18f], 0
00478ff5  mov        byte ptr [ebp - 0x18d], 0
00478ffc  mov        byte ptr [ebp - 0x196], 0
00479003  mov        byte ptr [ebp - 0x18e], 0
0047900a  mov        byte ptr [ebp - 0x195], 1
00479011  mov        dword ptr [ebp - 0x1b0], edi

// block 00479017
00479017  inc        esi
00479018  movzx      ebx, byte ptr [esi]
0047901b  movzx      eax, bl
0047901e  push       eax
0047901f  call       0x48ba71

// block 00479024
00479024  pop        ecx
00479025  test       eax, eax
00479027  je         0x479047

// block 00479029
00479029  mov        eax, dword ptr [ebp - 0x194]
0047902f  inc        dword ptr [ebp - 0x1c0]
00479035  imul       eax, eax, 0xa
00479038  lea        eax, [eax + ebx - 0x30]
0047903c  mov        dword ptr [ebp - 0x194], eax
00479042  jmp        0x47910c

// block 00479047
00479047  cmp        ebx, 0x4e
0047904a  jg         0x4790d3

// block 00479050
00479050  je         0x47910c

// block 00479056
00479056  cmp        ebx, 0x2a
00479059  je         0x4790cb

// block 0047905b
0047905b  cmp        ebx, 0x46
0047905e  je         0x47910c

// block 00479064
00479064  cmp        ebx, 0x49
00479067  je         0x479079

// block 00479069
00479069  cmp        ebx, 0x4c
0047906c  jne        0x4790e2

// block 0047906e
0047906e  inc        byte ptr [ebp - 0x195]
00479074  jmp        0x47910c

// block 00479079
00479079  mov        cl, byte ptr [esi + 1]
0047907c  cmp        cl, 0x36
0047907f  jne        0x47909f

// block 00479081
00479081  lea        eax, [esi + 2]
00479084  cmp        byte ptr [eax], 0x34
00479087  jne        0x47909f

// block 00479089
00479089  inc        dword ptr [ebp - 0x1b0]
0047908f  mov        esi, eax
00479091  mov        dword ptr [ebp - 0x1b8], edi
00479097  mov        dword ptr [ebp - 0x1b4], edi
0047909d  jmp        0x47910c

// block 0047909f
0047909f  cmp        cl, 0x33
004790a2  jne        0x4790b0

// block 004790a4
004790a4  lea        eax, [esi + 2]
004790a7  cmp        byte ptr [eax], 0x32
004790aa  jne        0x4790b0

// block 004790ac
004790ac  mov        esi, eax
004790ae  jmp        0x47910c

// block 004790b0
004790b0  cmp        cl, 0x64
004790b3  je         0x47910c

// block 004790b5
004790b5  cmp        cl, 0x69
004790b8  je         0x47910c

// block 004790ba
004790ba  cmp        cl, 0x6f
004790bd  je         0x47910c

// block 004790bf
004790bf  cmp        cl, 0x78
004790c2  je         0x47910c

// block 004790c4
004790c4  cmp        cl, 0x58
004790c7  jne        0x4790e2

// block 004790c9
004790c9  jmp        0x47910c

// block 004790cb
004790cb  inc        byte ptr [ebp - 0x18f]
004790d1  jmp        0x47910c

// block 004790d3
004790d3  cmp        ebx, 0x68
004790d6  je         0x479100

// block 004790d8
004790d8  cmp        ebx, 0x6c
004790db  je         0x4790ea

// block 004790dd
004790dd  cmp        ebx, 0x77
004790e0  je         0x4790f8

// block 004790e2
004790e2  inc        byte ptr [ebp - 0x18d]
004790e8  jmp        0x47910c

// block 004790ea
004790ea  lea        eax, [esi + 1]
004790ed  cmp        byte ptr [eax], 0x6c
004790f0  je         0x479089

// block 004790f2
004790f2  inc        byte ptr [ebp - 0x195]

// block 004790f8
004790f8  inc        byte ptr [ebp - 0x18e]
004790fe  jmp        0x47910c

// block 00479100
00479100  dec        byte ptr [ebp - 0x195]
00479106  dec        byte ptr [ebp - 0x18e]

// block 0047910c
0047910c  cmp        byte ptr [ebp - 0x18d], 0
00479113  je         0x479017

// block 00479119
00479119  cmp        byte ptr [ebp - 0x18f], 0
00479120  mov        dword ptr [ebp - 0x1ac], esi
00479126  jne        0x479141

// block 00479128
00479128  mov        eax, dword ptr [ebp - 0x1e0]
0047912e  mov        ebx, dword ptr [eax]
00479130  mov        dword ptr [ebp - 0x1e8], eax
00479136  add        eax, 4
00479139  mov        dword ptr [ebp - 0x1e0], eax
0047913f  jmp        0x479143

// block 00479141
00479141  xor        ebx, ebx

// block 00479143
00479143  cmp        byte ptr [ebp - 0x18e], 0
0047914a  mov        dword ptr [ebp - 0x1bc], ebx
00479150  mov        byte ptr [ebp - 0x18d], 0
00479157  jne        0x479171

// block 00479159
00479159  mov        al, byte ptr [esi]
0047915b  cmp        al, 0x53
0047915d  je         0x47916a

// block 0047915f
0047915f  mov        byte ptr [ebp - 0x18e], 0xff
00479166  cmp        al, 0x43
00479168  jne        0x479171

// block 0047916a
0047916a  mov        byte ptr [ebp - 0x18e], 1

// block 00479171
00479171  movzx      edi, byte ptr [esi]
00479174  or         edi, 0x20
00479177  mov        dword ptr [ebp - 0x1e4], edi
0047917d  cmp        edi, 0x6e
00479180  je         0x4791cc

// block 00479182
00479182  cmp        edi, 0x63
00479185  je         0x4791a0

// block 00479187
00479187  cmp        edi, 0x7b
0047918a  je         0x4791a0

// block 0047918c
0047918c  push       dword ptr [ebp - 0x19c]
00479192  lea        esi, [ebp - 0x18c]
00479198  call       0x478dec

// block 0047919d
0047919d  pop        ecx
0047919e  jmp        0x4791b1

// block 004791a0
004791a0  mov        edx, dword ptr [ebp - 0x19c]
004791a6  inc        dword ptr [ebp - 0x18c]
004791ac  call       0x478dc3

// block 004791b1
004791b1  mov        dword ptr [ebp - 0x188], eax
004791b7  cmp        eax, -1
004791ba  je         0x479ea7

// block 004791c0
004791c0  mov        ebx, dword ptr [ebp - 0x1bc]
004791c6  mov        esi, dword ptr [ebp - 0x1ac]

// block 004791cc
004791cc  mov        ecx, dword ptr [ebp - 0x1c0]
004791d2  test       ecx, ecx
004791d4  je         0x4791e3

// block 004791d6
004791d6  cmp        dword ptr [ebp - 0x194], 0
004791dd  je         0x479e0f

// block 004791e3
004791e3  cmp        byte ptr [ebp - 0x18f], 0
004791ea  jne        0x47922d

// block 004791ec
004791ec  cmp        edi, 0x63
004791ef  je         0x4791fb

// block 004791f1
004791f1  cmp        edi, 0x73
004791f4  je         0x4791fb

// block 004791f6
004791f6  cmp        edi, 0x7b
004791f9  jne        0x47922d

// block 004791fb
004791fb  mov        eax, dword ptr [ebp - 0x1e8]
00479201  mov        ebx, dword ptr [eax]
00479203  add        eax, 4
00479206  mov        dword ptr [ebp - 0x1e8], eax
0047920c  add        eax, 4
0047920f  mov        dword ptr [ebp - 0x1e0], eax
00479215  mov        eax, dword ptr [eax - 4]
00479218  mov        dword ptr [ebp - 0x1bc], ebx
0047921e  mov        dword ptr [ebp - 0x1cc], eax
00479224  cmp        eax, 1
00479227  jb         0x479e27

// block 0047922d
0047922d  cmp        edi, 0x6f
00479230  jg         0x4797b5

// block 00479236
00479236  je         0x479a4e

// block 0047923c
0047923c  cmp        edi, 0x63
0047923f  je         0x47964a

// block 00479245
00479245  cmp        edi, 0x64
00479248  je         0x479a4e

// block 0047924e
0047924e  jle        0x4797df

// block 00479254
00479254  cmp        edi, 0x67
00479257  jle        0x47929b

// block 00479259
00479259  cmp        edi, 0x69
0047925c  je         0x47927f

// block 0047925e
0047925e  cmp        edi, 0x6e
00479261  jne        0x4797df

// block 00479267
00479267  cmp        byte ptr [ebp - 0x18f], 0
0047926e  mov        eax, dword ptr [ebp - 0x18c]
00479274  je         0x479c2a

// block 0047927a
0047927a  jmp        0x479d59

// block 0047927f
0047927f  push       0x64
00479281  pop        edi

// block 00479282
00479282  cmp        dword ptr [ebp - 0x188], 0x2d
00479289  jne        0x4798df

// block 0047928f
0047928f  mov        byte ptr [ebp - 0x19f], 1
00479296  jmp        0x4798e8

// block 0047929b
0047929b  xor        ebx, ebx
0047929d  cmp        dword ptr [ebp - 0x188], 0x2d
004792a4  jne        0x4792b2

// block 004792a6
004792a6  mov        eax, dword ptr [ebp - 0x1a4]
004792ac  mov        byte ptr [eax], 0x2d
004792af  inc        ebx
004792b0  jmp        0x4792bb

// block 004792b2
004792b2  cmp        dword ptr [ebp - 0x188], 0x2b
004792b9  jne        0x4792d8

// block 004792bb
004792bb  dec        dword ptr [ebp - 0x194]
004792c1  mov        edx, dword ptr [ebp - 0x19c]
004792c7  inc        dword ptr [ebp - 0x18c]
004792cd  call       0x478dc3

// block 004792d2
004792d2  mov        dword ptr [ebp - 0x188], eax

// block 004792d8
004792d8  cmp        dword ptr [ebp - 0x1c0], 0
004792df  jne        0x4792e8

// block 004792e1
004792e1  or         dword ptr [ebp - 0x194], 0xffffffff

// block 004792e8
004792e8  movzx      eax, byte ptr [ebp - 0x188]
004792ef  jmp        0x47935c

// block 004792f1
004792f1  mov        eax, dword ptr [ebp - 0x194]
004792f7  dec        dword ptr [ebp - 0x194]
004792fd  test       eax, eax
004792ff  je         0x479367

// block 00479301
00479301  mov        al, byte ptr [ebp - 0x188]
00479307  mov        ecx, dword ptr [ebp - 0x1a4]
0047930d  inc        dword ptr [ebp - 0x1a8]
00479313  mov        byte ptr [ebx + ecx], al
00479316  lea        eax, [ebp - 0x1d0]
0047931c  push       eax
0047931d  lea        eax, [ebp - 0x184]
00479323  push       eax
00479324  inc        ebx
00479325  push       ebx
00479326  lea        edi, [ebp - 0x1a4]
0047932c  lea        esi, [ebp - 0x1d8]
00479332  call       0x478d4c

// block 00479337
00479337  add        esp, 0xc
0047933a  test       eax, eax
0047933c  je         0x479ea7

// block 00479342
00479342  mov        edx, dword ptr [ebp - 0x19c]
00479348  inc        dword ptr [ebp - 0x18c]
0047934e  call       0x478dc3

// block 00479353
00479353  mov        dword ptr [ebp - 0x188], eax
00479359  movzx      eax, al

// block 0047935c
0047935c  push       eax
0047935d  call       0x48ba71

// block 00479362
00479362  pop        ecx
00479363  test       eax, eax
00479365  jne        0x4792f1

// block 00479367
00479367  mov        eax, dword ptr [ebp - 0x200]
0047936d  mov        eax, dword ptr [eax + 0xbc]
00479373  mov        eax, dword ptr [eax]
00479375  mov        al, byte ptr [eax]
00479377  mov        byte ptr [ebp - 0x19e], al
0047937d  cmp        al, byte ptr [ebp - 0x188]
00479383  jne        0x47946e

// block 00479389
00479389  mov        eax, dword ptr [ebp - 0x194]
0047938f  dec        dword ptr [ebp - 0x194]
00479395  test       eax, eax
00479397  je         0x47946e

// block 0047939d
0047939d  mov        edx, dword ptr [ebp - 0x19c]
004793a3  inc        dword ptr [ebp - 0x18c]
004793a9  call       0x478dc3

// block 004793ae
004793ae  mov        ecx, dword ptr [ebp - 0x1a4]
004793b4  mov        dword ptr [ebp - 0x188], eax
004793ba  mov        al, byte ptr [ebp - 0x19e]
004793c0  mov        byte ptr [ebx + ecx], al
004793c3  lea        eax, [ebp - 0x1d0]
004793c9  push       eax
004793ca  lea        eax, [ebp - 0x184]
004793d0  push       eax
004793d1  inc        ebx
004793d2  push       ebx
004793d3  lea        edi, [ebp - 0x1a4]
004793d9  lea        esi, [ebp - 0x1d8]
004793df  call       0x478d4c

// block 004793e4
004793e4  add        esp, 0xc
004793e7  test       eax, eax
004793e9  je         0x479ea7

// block 004793ef
004793ef  movzx      eax, byte ptr [ebp - 0x188]
004793f6  jmp        0x479463

// block 004793f8
004793f8  mov        eax, dword ptr [ebp - 0x194]
004793fe  dec        dword ptr [ebp - 0x194]
00479404  test       eax, eax
00479406  je         0x47946e

// block 00479408
00479408  mov        eax, dword ptr [ebp - 0x1a4]
0047940e  mov        cl, byte ptr [ebp - 0x188]
00479414  inc        dword ptr [ebp - 0x1a8]
0047941a  mov        byte ptr [ebx + eax], cl
0047941d  lea        eax, [ebp - 0x1d0]
00479423  push       eax
00479424  lea        eax, [ebp - 0x184]
0047942a  push       eax
0047942b  inc        ebx
0047942c  push       ebx
0047942d  lea        edi, [ebp - 0x1a4]
00479433  lea        esi, [ebp - 0x1d8]
00479439  call       0x478d4c

// block 0047943e
0047943e  add        esp, 0xc
00479441  test       eax, eax
00479443  je         0x479ea7

// block 00479449
00479449  mov        edx, dword ptr [ebp - 0x19c]
0047944f  inc        dword ptr [ebp - 0x18c]
00479455  call       0x478dc3

// block 0047945a
0047945a  mov        dword ptr [ebp - 0x188], eax
00479460  movzx      eax, al

// block 00479463
00479463  push       eax
00479464  call       0x48ba71

// block 00479469
00479469  pop        ecx
0047946a  test       eax, eax
0047946c  jne        0x4793f8

// block 0047946e
0047946e  cmp        dword ptr [ebp - 0x1a8], 0
00479475  je         0x4795da

// block 0047947b
0047947b  cmp        dword ptr [ebp - 0x188], 0x65
00479482  je         0x479491

// block 00479484
00479484  cmp        dword ptr [ebp - 0x188], 0x45
0047948b  jne        0x4795da

// block 00479491
00479491  mov        eax, dword ptr [ebp - 0x194]
00479497  dec        dword ptr [ebp - 0x194]
0047949d  test       eax, eax
0047949f  je         0x4795da

// block 004794a5
004794a5  mov        eax, dword ptr [ebp - 0x1a4]
004794ab  mov        byte ptr [ebx + eax], 0x65
004794af  lea        eax, [ebp - 0x1d0]
004794b5  push       eax
004794b6  lea        eax, [ebp - 0x184]
004794bc  push       eax
004794bd  inc        ebx
004794be  push       ebx
004794bf  lea        edi, [ebp - 0x1a4]
004794c5  lea        esi, [ebp - 0x1d8]
004794cb  call       0x478d4c

// block 004794d0
004794d0  add        esp, 0xc
004794d3  test       eax, eax
004794d5  je         0x479ea7

// block 004794db
004794db  mov        edx, dword ptr [ebp - 0x19c]
004794e1  inc        dword ptr [ebp - 0x18c]
004794e7  call       0x478dc3

// block 004794ec
004794ec  mov        dword ptr [ebp - 0x188], eax
004794f2  cmp        eax, 0x2d
004794f5  jne        0x479523

// block 004794f7
004794f7  mov        eax, dword ptr [ebp - 0x1a4]
004794fd  mov        byte ptr [ebx + eax], 0x2d
00479501  lea        eax, [ebp - 0x1d0]
00479507  push       eax
00479508  lea        eax, [ebp - 0x184]
0047950e  push       eax
0047950f  inc        ebx
00479510  push       ebx
00479511  call       0x478d4c

// block 00479516
00479516  add        esp, 0xc
00479519  test       eax, eax
0047951b  je         0x479ea7

// block 00479521
00479521  jmp        0x47952c

// block 00479523
00479523  cmp        dword ptr [ebp - 0x188], 0x2b
0047952a  jne        0x47955b

// block 0047952c
0047952c  mov        eax, dword ptr [ebp - 0x194]
00479532  dec        dword ptr [ebp - 0x194]
00479538  test       eax, eax
0047953a  jne        0x479544

// block 0047953c
0047953c  and        dword ptr [ebp - 0x194], eax
00479542  jmp        0x47955b

// block 00479544
00479544  mov        edx, dword ptr [ebp - 0x19c]
0047954a  inc        dword ptr [ebp - 0x18c]
00479550  call       0x478dc3

// block 00479555
00479555  mov        dword ptr [ebp - 0x188], eax

// block 0047955b
0047955b  movzx      eax, byte ptr [ebp - 0x188]
00479562  jmp        0x4795cf

// block 00479564
00479564  mov        eax, dword ptr [ebp - 0x194]
0047956a  dec        dword ptr [ebp - 0x194]
00479570  test       eax, eax
00479572  je         0x4795da

// block 00479574
00479574  mov        eax, dword ptr [ebp - 0x1a4]
0047957a  mov        cl, byte ptr [ebp - 0x188]
00479580  inc        dword ptr [ebp - 0x1a8]
00479586  mov        byte ptr [ebx + eax], cl
00479589  lea        eax, [ebp - 0x1d0]
0047958f  push       eax
00479590  lea        eax, [ebp - 0x184]
00479596  push       eax
00479597  inc        ebx
00479598  push       ebx
00479599  lea        edi, [ebp - 0x1a4]
0047959f  lea        esi, [ebp - 0x1d8]
004795a5  call       0x478d4c

// block 004795aa
004795aa  add        esp, 0xc
004795ad  test       eax, eax
004795af  je         0x479ea7

// block 004795b5
004795b5  mov        edx, dword ptr [ebp - 0x19c]
004795bb  inc        dword ptr [ebp - 0x18c]
004795c1  call       0x478dc3

// block 004795c6
004795c6  mov        dword ptr [ebp - 0x188], eax
004795cc  movzx      eax, al

// block 004795cf
004795cf  push       eax
004795d0  call       0x48ba71

// block 004795d5
004795d5  pop        ecx
004795d6  test       eax, eax
004795d8  jne        0x479564

// block 004795da
004795da  push       dword ptr [ebp - 0x19c]
004795e0  dec        dword ptr [ebp - 0x18c]
004795e6  push       dword ptr [ebp - 0x188]
004795ec  call       0x478dd9

// block 004795f1
004795f1  cmp        dword ptr [ebp - 0x1a8], 0
004795f8  pop        ecx
004795f9  pop        ecx
004795fa  je         0x479ea7

// block 00479600
00479600  cmp        byte ptr [ebp - 0x18f], 0
00479607  jne        0x479d59

// block 0047960d
0047960d  mov        eax, dword ptr [ebp - 0x1a4]
00479613  inc        dword ptr [ebp - 0x1c4]
00479619  lea        ecx, [ebp - 0x200]
0047961f  push       ecx
00479620  push       eax
00479621  push       dword ptr [ebp - 0x1bc]
00479627  mov        byte ptr [ebx + eax], 0
0047962b  movsx      eax, byte ptr [ebp - 0x195]
00479632  dec        eax
00479633  push       eax
00479634  push       dword ptr [0x4ade8c]
0047963a  call       0x4751de

// block 0047963f
0047963f  pop        ecx
00479640  call       eax

// block 00479642
00479642  add        esp, 0x10
00479645  jmp        0x479d59

// block 0047964a
0047964a  test       ecx, ecx
0047964c  jne        0x47965e

// block 0047964e
0047964e  inc        dword ptr [ebp - 0x194]
00479654  mov        dword ptr [ebp - 0x1c0], 1

// block 0047965e
0047965e  cmp        byte ptr [ebp - 0x18e], 0
00479665  jle        0x47966e

// block 00479667
00479667  mov        byte ptr [ebp - 0x196], 1

// block 0047966e
0047966e  mov        esi, dword ptr [ebp - 0x19c]
00479674  dec        dword ptr [ebp - 0x18c]
0047967a  push       esi
0047967b  push       dword ptr [ebp - 0x188]
00479681  mov        dword ptr [ebp - 0x1b0], ebx
00479687  call       0x478dd9

// block 0047968c
0047968c  pop        ecx
0047968d  pop        ecx
0047968e  cmp        edi, 0x63
00479691  je         0x479699

// block 00479693
00479693  dec        dword ptr [ebp - 0x1cc]

// block 00479699
00479699  cmp        dword ptr [ebp - 0x1c0], 0
004796a0  je         0x4796b6

// block 004796a2
004796a2  mov        eax, dword ptr [ebp - 0x194]
004796a8  dec        dword ptr [ebp - 0x194]
004796ae  test       eax, eax
004796b0  je         0x4799f8

// block 004796b6
004796b6  inc        dword ptr [ebp - 0x18c]
004796bc  mov        edx, esi
004796be  call       0x478dc3

// block 004796c3
004796c3  mov        dword ptr [ebp - 0x188], eax
004796c9  cmp        eax, -1
004796cc  je         0x4799e9

// block 004796d2
004796d2  cmp        edi, 0x63
004796d5  je         0x479723

// block 004796d7
004796d7  cmp        edi, 0x73
004796da  jne        0x4796ef

// block 004796dc
004796dc  cmp        eax, 9
004796df  jl         0x4796ea

// block 004796e1
004796e1  cmp        eax, 0xd
004796e4  jle        0x4799e9

// block 004796ea
004796ea  cmp        eax, 0x20
004796ed  jne        0x479723

// block 004796ef
004796ef  cmp        edi, 0x7b
004796f2  jne        0x4799e9

// block 004796f8
004796f8  movsx      edi, byte ptr [ebp - 0x19e]
004796ff  xor        edx, edx
00479701  mov        ecx, eax
00479703  and        ecx, 7
00479706  inc        edx
00479707  shl        edx, cl
00479709  mov        ecx, eax
0047970b  sar        ecx, 3
0047970e  movsx      ecx, byte ptr [ebp + ecx - 0x24]
00479713  xor        ecx, edi
00479715  mov        edi, dword ptr [ebp - 0x1e4]
0047971b  test       ecx, edx
0047971d  je         0x4799e9

// block 00479723
00479723  cmp        byte ptr [ebp - 0x18f], 0
0047972a  jne        0x4799de

// block 00479730
00479730  cmp        dword ptr [ebp - 0x1cc], 0
00479737  je         0x479e66

// block 0047973d
0047973d  cmp        byte ptr [ebp - 0x196], 0
00479744  je         0x4799d0

// block 0047974a
0047974a  mov        byte ptr [ebp - 0x1dc], al
00479750  movzx      eax, al
00479753  push       eax
00479754  call       0x47c5db

// block 00479759
00479759  pop        ecx
0047975a  test       eax, eax
0047975c  je         0x479771

// block 0047975e
0047975e  inc        dword ptr [ebp - 0x18c]
00479764  mov        edx, esi
00479766  call       0x478dc3

// block 0047976b
0047976b  mov        byte ptr [ebp - 0x1db], al

// block 00479771
00479771  lea        eax, [ebp - 0x200]
00479777  push       eax
00479778  mov        eax, dword ptr [ebp - 0x200]
0047977e  mov        dword ptr [ebp - 0x1ec], 0x3f
00479788  push       dword ptr [eax + 0xac]
0047978e  lea        eax, [ebp - 0x1dc]
00479794  push       eax
00479795  lea        eax, [ebp - 0x1ec]
0047979b  push       eax
0047979c  call       0x48c167

// block 004797a1
004797a1  mov        ax, word ptr [ebp - 0x1ec]
004797a8  add        esp, 0x10
004797ab  mov        word ptr [ebx], ax
004797ae  inc        ebx
004797af  inc        ebx
004797b0  jmp        0x4799d3

// block 004797b5
004797b5  mov        eax, edi
004797b7  sub        eax, 0x70
004797ba  je         0x479a47

// block 004797c0
004797c0  sub        eax, 3
004797c3  je         0x47965e

// block 004797c9
004797c9  dec        eax
004797ca  dec        eax
004797cb  je         0x479a4e

// block 004797d1
004797d1  sub        eax, 3
004797d4  je         0x479282

// block 004797da
004797da  sub        eax, 3
004797dd  je         0x479812

// block 004797df
004797df  movzx      eax, byte ptr [esi]
004797e2  cmp        eax, dword ptr [ebp - 0x188]
004797e8  jne        0x479e47

// block 004797ee
004797ee  dec        byte ptr [ebp - 0x19d]
004797f4  cmp        byte ptr [ebp - 0x18f], 0
004797fb  jne        0x479d59

// block 00479801
00479801  mov        eax, dword ptr [ebp - 0x1e8]
00479807  mov        dword ptr [ebp - 0x1e0], eax
0047980d  jmp        0x479d59

// block 00479812
00479812  cmp        byte ptr [ebp - 0x18e], 0
00479819  jle        0x479822

// block 0047981b
0047981b  mov        byte ptr [ebp - 0x196], 1

// block 00479822
00479822  inc        esi
00479823  cmp        byte ptr [esi], 0x5e
00479826  jne        0x479830

// block 00479828
00479828  inc        esi
00479829  mov        byte ptr [ebp - 0x19e], 0xff

// block 00479830
00479830  push       0x20
00479832  lea        eax, [ebp - 0x24]
00479835  push       0
00479837  push       eax
00479838  call       0x477420

// block 0047983d
0047983d  add        esp, 0xc
00479840  cmp        byte ptr [esi], 0x5d
00479843  jne        0x47984e

// block 00479845
00479845  mov        dl, 0x5d
00479847  inc        esi
00479848  mov        byte ptr [ebp - 0x19], 0x20
0047984c  jmp        0x4798c0

// block 0047984e
0047984e  mov        dl, byte ptr [ebp - 0x1d1]
00479854  jmp        0x4798c0

// block 00479856
00479856  inc        esi
00479857  cmp        al, 0x2d
00479859  jne        0x4798a3

// block 0047985b
0047985b  test       dl, dl
0047985d  je         0x4798a3

// block 0047985f
0047985f  mov        cl, byte ptr [esi]
00479861  cmp        cl, 0x5d
00479864  je         0x4798a3

// block 00479866
00479866  inc        esi
00479867  cmp        dl, cl
00479869  jae        0x47986f

// block 0047986b
0047986b  mov        al, cl
0047986d  jmp        0x479873

// block 0047986f
0047986f  mov        al, dl
00479871  mov        dl, cl

// block 00479873
00479873  cmp        dl, al
00479875  ja         0x47989f

// block 00479877
00479877  sub        al, dl
00479879  inc        al
0047987b  movzx      edi, dl
0047987e  movzx      edx, al

// block 00479881
00479881  mov        ecx, edi
00479883  and        ecx, 7
00479886  mov        eax, edi
00479888  mov        bl, 1
0047988a  shl        bl, cl
0047988c  shr        eax, 3
0047988f  lea        eax, [ebp + eax - 0x24]
00479893  or         byte ptr [eax], bl
00479895  inc        edi
00479896  dec        edx
00479897  jne        0x479881

// block 00479899
00479899  mov        edi, dword ptr [ebp - 0x1e4]

// block 0047989f
0047989f  xor        dl, dl
004798a1  jmp        0x4798c0

// block 004798a3
004798a3  mov        edi, dword ptr [ebp - 0x1e4]
004798a9  movzx      ecx, al
004798ac  mov        dl, al
004798ae  mov        eax, ecx
004798b0  and        ecx, 7
004798b3  mov        bl, 1
004798b5  shr        eax, 3
004798b8  shl        bl, cl
004798ba  lea        eax, [ebp + eax - 0x24]
004798be  or         byte ptr [eax], bl

// block 004798c0
004798c0  mov        al, byte ptr [esi]
004798c2  cmp        al, 0x5d
004798c4  jne        0x479856

// block 004798c6
004798c6  test       al, al
004798c8  je         0x479ea7

// block 004798ce
004798ce  mov        ebx, dword ptr [ebp - 0x1bc]
004798d4  mov        dword ptr [ebp - 0x1ac], esi
004798da  jmp        0x47966e

// block 004798df
004798df  cmp        dword ptr [ebp - 0x188], 0x2b
004798e6  jne        0x479914

// block 004798e8
004798e8  dec        dword ptr [ebp - 0x194]
004798ee  jne        0x4798fd

// block 004798f0
004798f0  test       ecx, ecx
004798f2  je         0x4798fd

// block 004798f4
004798f4  mov        byte ptr [ebp - 0x18d], 1
004798fb  jmp        0x479914

// block 004798fd
004798fd  mov        edx, dword ptr [ebp - 0x19c]
00479903  inc        dword ptr [ebp - 0x18c]
00479909  call       0x478dc3

// block 0047990e
0047990e  mov        dword ptr [ebp - 0x188], eax

// block 00479914
00479914  cmp        dword ptr [ebp - 0x188], 0x30
0047991b  jne        0x479a95

// block 00479921
00479921  mov        edx, dword ptr [ebp - 0x19c]
00479927  inc        dword ptr [ebp - 0x18c]
0047992d  call       0x478dc3

// block 00479932
00479932  mov        dword ptr [ebp - 0x188], eax
00479938  cmp        al, 0x78
0047993a  je         0x479992

// block 0047993c
0047993c  cmp        al, 0x58
0047993e  je         0x479992

// block 00479940
00479940  mov        dword ptr [ebp - 0x1a8], 1
0047994a  cmp        edi, 0x78
0047994d  je         0x47996a

// block 0047994f
0047994f  cmp        dword ptr [ebp - 0x1c0], 0
00479956  je         0x479966

// block 00479958
00479958  dec        dword ptr [ebp - 0x194]
0047995e  jne        0x479966

// block 00479960
00479960  inc        byte ptr [ebp - 0x18d]

// block 00479966
00479966  push       0x6f
00479968  jmp        0x4799ca

// block 0047996a
0047996a  push       dword ptr [ebp - 0x19c]
00479970  dec        dword ptr [ebp - 0x18c]
00479976  push       dword ptr [ebp - 0x188]
0047997c  call       0x478dd9

// block 00479981
00479981  pop        ecx
00479982  pop        ecx
00479983  mov        dword ptr [ebp - 0x188], 0x30
0047998d  jmp        0x479a95

// block 00479992
00479992  mov        edx, dword ptr [ebp - 0x19c]
00479998  inc        dword ptr [ebp - 0x18c]
0047999e  call       0x478dc3

// block 004799a3
004799a3  cmp        dword ptr [ebp - 0x1c0], 0
004799aa  mov        dword ptr [ebp - 0x188], eax
004799b0  je         0x4799c8

// block 004799b2
004799b2  sub        dword ptr [ebp - 0x194], 2
004799b9  cmp        dword ptr [ebp - 0x194], 1
004799c0  jge        0x4799c8

// block 004799c2
004799c2  inc        byte ptr [ebp - 0x18d]

// block 004799c8
004799c8  push       0x78

// block 004799ca
004799ca  pop        edi
004799cb  jmp        0x479a95

// block 004799d0
004799d0  mov        byte ptr [ebx], al
004799d2  inc        ebx

// block 004799d3
004799d3  mov        dword ptr [ebp - 0x1bc], ebx
004799d9  jmp        0x479693

// block 004799de
004799de  inc        dword ptr [ebp - 0x1b0]
004799e4  jmp        0x479699

// block 004799e9
004799e9  dec        dword ptr [ebp - 0x18c]
004799ef  push       esi
004799f0  push       eax
004799f1  call       0x478dd9

// block 004799f6
004799f6  pop        ecx
004799f7  pop        ecx

// block 004799f8
004799f8  cmp        dword ptr [ebp - 0x1b0], ebx
004799fe  je         0x479ea7

// block 00479a04
00479a04  cmp        byte ptr [ebp - 0x18f], 0
00479a0b  jne        0x479d59

// block 00479a11
00479a11  inc        dword ptr [ebp - 0x1c4]
00479a17  cmp        edi, 0x63
00479a1a  je         0x479d59

// block 00479a20
00479a20  cmp        byte ptr [ebp - 0x196], 0
00479a27  je         0x479a39

// block 00479a29
00479a29  mov        ecx, dword ptr [ebp - 0x1bc]
00479a2f  xor        eax, eax
00479a31  mov        word ptr [ecx], ax
00479a34  jmp        0x479d59

// block 00479a39
00479a39  mov        eax, dword ptr [ebp - 0x1bc]
00479a3f  mov        byte ptr [eax], 0
00479a42  jmp        0x479d59

// block 00479a47
00479a47  mov        byte ptr [ebp - 0x195], 1

// block 00479a4e
00479a4e  cmp        dword ptr [ebp - 0x188], 0x2d
00479a55  jne        0x479a60

// block 00479a57
00479a57  mov        byte ptr [ebp - 0x19f], 1
00479a5e  jmp        0x479a69

// block 00479a60
00479a60  cmp        dword ptr [ebp - 0x188], 0x2b
00479a67  jne        0x479a95

// block 00479a69
00479a69  dec        dword ptr [ebp - 0x194]
00479a6f  jne        0x479a7e

// block 00479a71
00479a71  test       ecx, ecx
00479a73  je         0x479a7e

// block 00479a75
00479a75  mov        byte ptr [ebp - 0x18d], 1
00479a7c  jmp        0x479a95

// block 00479a7e
00479a7e  mov        edx, dword ptr [ebp - 0x19c]
00479a84  inc        dword ptr [ebp - 0x18c]
00479a8a  call       0x478dc3

// block 00479a8f
00479a8f  mov        dword ptr [ebp - 0x188], eax

// block 00479a95
00479a95  cmp        dword ptr [ebp - 0x1b0], 0
00479a9c  je         0x479c4d

// block 00479aa2
00479aa2  cmp        byte ptr [ebp - 0x18d], 0
00479aa9  jne        0x479bca

// block 00479aaf
00479aaf  cmp        edi, 0x78
00479ab2  je         0x479b25

// block 00479ab4
00479ab4  cmp        edi, 0x70
00479ab7  je         0x479b25

// block 00479ab9
00479ab9  movzx      eax, byte ptr [ebp - 0x188]
00479ac0  push       eax
00479ac1  call       0x48ba71

// block 00479ac6
00479ac6  pop        ecx
00479ac7  test       eax, eax
00479ac9  je         0x479bb1

// block 00479acf
00479acf  cmp        edi, 0x6f
00479ad2  jne        0x479b02

// block 00479ad4
00479ad4  cmp        dword ptr [ebp - 0x188], 0x38
00479adb  jge        0x479bb1

// block 00479ae1
00479ae1  mov        eax, dword ptr [ebp - 0x1b8]
00479ae7  mov        ecx, dword ptr [ebp - 0x1b4]
00479aed  shld       ecx, eax, 3
00479af1  shl        eax, 3
00479af4  mov        dword ptr [ebp - 0x1b8], eax
00479afa  mov        dword ptr [ebp - 0x1b4], ecx
00479b00  jmp        0x479b68

// block 00479b02
00479b02  push       0
00479b04  push       0xa
00479b06  push       dword ptr [ebp - 0x1b4]
00479b0c  push       dword ptr [ebp - 0x1b8]
00479b12  call       0x483220

// block 00479b17
00479b17  mov        dword ptr [ebp - 0x1b8], eax
00479b1d  mov        dword ptr [ebp - 0x1b4], edx
00479b23  jmp        0x479b68

// block 00479b25
00479b25  movzx      eax, byte ptr [ebp - 0x188]
00479b2c  push       eax
00479b2d  call       0x48baf5

// block 00479b32
00479b32  pop        ecx
00479b33  test       eax, eax
00479b35  je         0x479bb1

// block 00479b37
00479b37  mov        eax, dword ptr [ebp - 0x1b8]
00479b3d  mov        ecx, dword ptr [ebp - 0x1b4]
00479b43  push       dword ptr [ebp - 0x188]
00479b49  shld       ecx, eax, 4
00479b4d  shl        eax, 4
00479b50  mov        dword ptr [ebp - 0x1b8], eax
00479b56  mov        dword ptr [ebp - 0x1b4], ecx
00479b5c  call       0x478da3

// block 00479b61
00479b61  pop        ecx
00479b62  mov        dword ptr [ebp - 0x188], eax

// block 00479b68
00479b68  mov        eax, dword ptr [ebp - 0x188]
00479b6e  inc        dword ptr [ebp - 0x1a8]
00479b74  add        eax, -0x30
00479b77  cdq        
00479b78  add        dword ptr [ebp - 0x1b8], eax
00479b7e  adc        dword ptr [ebp - 0x1b4], edx
00479b84  cmp        dword ptr [ebp - 0x1c0], 0
00479b8b  je         0x479b95

// block 00479b8d
00479b8d  dec        dword ptr [ebp - 0x194]
00479b93  je         0x479bca

// block 00479b95
00479b95  mov        edx, dword ptr [ebp - 0x19c]
00479b9b  inc        dword ptr [ebp - 0x18c]
00479ba1  call       0x478dc3

// block 00479ba6
00479ba6  mov        dword ptr [ebp - 0x188], eax
00479bac  jmp        0x479aaf

// block 00479bb1
00479bb1  push       dword ptr [ebp - 0x19c]
00479bb7  dec        dword ptr [ebp - 0x18c]
00479bbd  push       dword ptr [ebp - 0x188]
00479bc3  call       0x478dd9

// block 00479bc8
00479bc8  pop        ecx
00479bc9  pop        ecx

// block 00479bca
00479bca  cmp        byte ptr [ebp - 0x19f], 0
00479bd1  je         0x479bf2

// block 00479bd3
00479bd3  mov        eax, dword ptr [ebp - 0x1b8]
00479bd9  mov        ecx, dword ptr [ebp - 0x1b4]
00479bdf  neg        eax
00479be1  adc        ecx, 0
00479be4  neg        ecx
00479be6  mov        dword ptr [ebp - 0x1b8], eax
00479bec  mov        dword ptr [ebp - 0x1b4], ecx

// block 00479bf2
00479bf2  mov        eax, dword ptr [ebp - 0x1c8]

// block 00479bf8
00479bf8  cmp        edi, 0x46
00479bfb  jne        0x479c04

// block 00479bfd
00479bfd  and        dword ptr [ebp - 0x1a8], 0

// block 00479c04
00479c04  cmp        dword ptr [ebp - 0x1a8], 0
00479c0b  je         0x479ea7

// block 00479c11
00479c11  cmp        byte ptr [ebp - 0x18f], 0
00479c18  jne        0x479d59

// block 00479c1e
00479c1e  inc        dword ptr [ebp - 0x1c4]
00479c24  mov        ebx, dword ptr [ebp - 0x1bc]

// block 00479c2a
00479c2a  cmp        dword ptr [ebp - 0x1b0], 0
00479c31  je         0x479d49

// block 00479c37
00479c37  mov        eax, dword ptr [ebp - 0x1b8]
00479c3d  mov        dword ptr [ebx], eax
00479c3f  mov        eax, dword ptr [ebp - 0x1b4]
00479c45  mov        dword ptr [ebx + 4], eax
00479c48  jmp        0x479d59

// block 00479c4d
00479c4d  cmp        byte ptr [ebp - 0x18d], 0
00479c54  jne        0x479d2f

// block 00479c5a
00479c5a  cmp        edi, 0x78
00479c5d  je         0x479ca2

// block 00479c5f
00479c5f  cmp        edi, 0x70
00479c62  je         0x479ca2

// block 00479c64
00479c64  movzx      eax, byte ptr [ebp - 0x188]
00479c6b  push       eax
00479c6c  call       0x48ba71

// block 00479c71
00479c71  pop        ecx
00479c72  test       eax, eax
00479c74  je         0x479d16

// block 00479c7a
00479c7a  cmp        edi, 0x6f
00479c7d  jne        0x479c97

// block 00479c7f
00479c7f  cmp        dword ptr [ebp - 0x188], 0x38
00479c86  jge        0x479d16

// block 00479c8c
00479c8c  mov        eax, dword ptr [ebp - 0x1c8]
00479c92  shl        eax, 3
00479c95  jmp        0x479cd3

// block 00479c97
00479c97  mov        eax, dword ptr [ebp - 0x1c8]
00479c9d  imul       eax, eax, 0xa
00479ca0  jmp        0x479cd3

// block 00479ca2
00479ca2  movzx      eax, byte ptr [ebp - 0x188]
00479ca9  push       eax
00479caa  call       0x48baf5

// block 00479caf
00479caf  pop        ecx
00479cb0  test       eax, eax
00479cb2  je         0x479d16

// block 00479cb4
00479cb4  push       dword ptr [ebp - 0x188]
00479cba  shl        dword ptr [ebp - 0x1c8], 4
00479cc1  call       0x478da3

// block 00479cc6
00479cc6  mov        dword ptr [ebp - 0x188], eax
00479ccc  mov        eax, dword ptr [ebp - 0x1c8]
00479cd2  pop        ecx

// block 00479cd3
00479cd3  mov        ecx, dword ptr [ebp - 0x188]
00479cd9  inc        dword ptr [ebp - 0x1a8]
00479cdf  cmp        dword ptr [ebp - 0x1c0], 0
00479ce6  lea        eax, [eax + ecx - 0x30]
00479cea  mov        dword ptr [ebp - 0x1c8], eax
00479cf0  je         0x479cfa

// block 00479cf2
00479cf2  dec        dword ptr [ebp - 0x194]
00479cf8  je         0x479d2f

// block 00479cfa
00479cfa  mov        edx, dword ptr [ebp - 0x19c]
00479d00  inc        dword ptr [ebp - 0x18c]
00479d06  call       0x478dc3

// block 00479d0b
00479d0b  mov        dword ptr [ebp - 0x188], eax
00479d11  jmp        0x479c5a

// block 00479d16
00479d16  push       dword ptr [ebp - 0x19c]
00479d1c  dec        dword ptr [ebp - 0x18c]
00479d22  push       dword ptr [ebp - 0x188]
00479d28  call       0x478dd9

// block 00479d2d
00479d2d  pop        ecx
00479d2e  pop        ecx

// block 00479d2f
00479d2f  cmp        byte ptr [ebp - 0x19f], 0
00479d36  je         0x479bf2

// block 00479d3c
00479d3c  mov        eax, dword ptr [ebp - 0x1c8]
00479d42  neg        eax
00479d44  jmp        0x479bf8

// block 00479d49
00479d49  cmp        byte ptr [ebp - 0x195], 0
00479d50  je         0x479d56

// block 00479d52
00479d52  mov        dword ptr [ebx], eax
00479d54  jmp        0x479d59

// block 00479d56
00479d56  mov        word ptr [ebx], ax

// block 00479d59
00479d59  mov        esi, dword ptr [ebp - 0x1ac]
00479d5f  inc        byte ptr [ebp - 0x19d]
00479d65  inc        esi
00479d66  mov        dword ptr [ebp - 0x1ac], esi
00479d6c  jmp        0x479ddc

// block 00479d6e
00479d6e  cmp        al, 0x25
00479d70  jne        0x479d7c

// block 00479d72
00479d72  lea        eax, [esi + 1]
00479d75  cmp        byte ptr [eax], 0x25
00479d78  jne        0x479d7c

// block 00479d7a
00479d7a  mov        esi, eax

// block 00479d7c
00479d7c  mov        edi, dword ptr [ebp - 0x19c]
00479d82  inc        dword ptr [ebp - 0x18c]
00479d88  mov        edx, edi
00479d8a  call       0x478dc3

// block 00479d8f
00479d8f  mov        ebx, eax
00479d91  movzx      eax, byte ptr [esi]
00479d94  inc        esi
00479d95  mov        dword ptr [ebp - 0x188], ebx
00479d9b  mov        dword ptr [ebp - 0x1ac], esi
00479da1  cmp        eax, ebx
00479da3  jne        0x479e92

// block 00479da9
00479da9  movzx      eax, bl
00479dac  push       eax
00479dad  call       0x47c5db

// block 00479db2
00479db2  pop        ecx
00479db3  test       eax, eax
00479db5  je         0x479ddc

// block 00479db7
00479db7  inc        dword ptr [ebp - 0x18c]
00479dbd  mov        edx, edi
00479dbf  call       0x478dc3

// block 00479dc4
00479dc4  movzx      ecx, byte ptr [esi]
00479dc7  inc        esi
00479dc8  mov        dword ptr [ebp - 0x1ac], esi
00479dce  cmp        ecx, eax
00479dd0  jne        0x479e96

// block 00479dd6
00479dd6  dec        dword ptr [ebp - 0x18c]

// block 00479ddc
00479ddc  cmp        dword ptr [ebp - 0x188], -1
00479de3  jne        0x479e00

// block 00479de5
00479de5  cmp        byte ptr [esi], 0x25
00479de8  jne        0x479ea7

// block 00479dee
00479dee  mov        eax, dword ptr [ebp - 0x1ac]
00479df4  cmp        byte ptr [eax + 1], 0x6e
00479df8  jne        0x479ea7

// block 00479dfe
00479dfe  mov        esi, eax

// block 00479e00
00479e00  mov        al, byte ptr [esi]
00479e02  test       al, al
00479e04  jne        0x478f4c

// block 00479e0a
00479e0a  jmp        0x479ea7

// block 00479e0f
00479e0f  push       dword ptr [ebp - 0x19c]
00479e15  push       dword ptr [ebp - 0x188]

// block 00479e1b
00479e1b  call       0x478dd9

// block 00479e20
00479e20  pop        ecx
00479e21  pop        ecx
00479e22  jmp        0x479ea7

// block 00479e27
00479e27  cmp        byte ptr [ebp - 0x18e], 0
00479e2e  jle        0x479e37

// block 00479e30
00479e30  xor        eax, eax
00479e32  mov        word ptr [ebx], ax
00479e35  jmp        0x479e3a

// block 00479e37
00479e37  mov        byte ptr [ebx], 0

// block 00479e3a
00479e3a  call       0x46e96f

// block 00479e3f
00479e3f  mov        dword ptr [eax], 0xc
00479e45  jmp        0x479ea7

// block 00479e47
00479e47  push       dword ptr [ebp - 0x19c]
00479e4d  push       dword ptr [ebp - 0x188]
00479e53  call       0x478dd9

// block 00479e58
00479e58  pop        ecx
00479e59  pop        ecx
00479e5a  mov        dword ptr [ebp - 0x1f0], 1
00479e64  jmp        0x479ea7

// block 00479e66
00479e66  call       0x46e96f

// block 00479e6b
00479e6b  cmp        byte ptr [ebp - 0x196], 0
00479e72  mov        dword ptr [eax], 0xc
00479e78  je         0x479e87

// block 00479e7a
00479e7a  mov        ecx, dword ptr [ebp - 0x1b0]
00479e80  xor        eax, eax
00479e82  mov        word ptr [ecx], ax
00479e85  jmp        0x479ea7

// block 00479e87
00479e87  mov        eax, dword ptr [ebp - 0x1b0]
00479e8d  mov        byte ptr [eax], 0
00479e90  jmp        0x479ea7

// block 00479e92
00479e92  push       edi
00479e93  push       ebx
00479e94  jmp        0x479e1b

// block 00479e96
00479e96  push       edi
00479e97  push       eax
00479e98  call       0x478dd9

// block 00479e9d
00479e9d  push       edi
00479e9e  push       ebx
00479e9f  call       0x478dd9

// block 00479ea4
00479ea4  add        esp, 0x10

// block 00479ea7
00479ea7  cmp        dword ptr [ebp - 0x1d0], 1
00479eae  jne        0x479ebc

// block 00479eb0
00479eb0  push       dword ptr [ebp - 0x1a4]
00479eb6  call       0x46c941

// block 00479ebb
00479ebb  pop        ecx

// block 00479ebc
00479ebc  cmp        dword ptr [ebp - 0x188], -1
00479ec3  jne        0x479eef

// block 00479ec5
00479ec5  mov        eax, dword ptr [ebp - 0x1c4]
00479ecb  test       eax, eax
00479ecd  jne        0x479eda

// block 00479ecf
00479ecf  cmp        byte ptr [ebp - 0x19d], al
00479ed5  jne        0x479eda

// block 00479ed7
00479ed7  or         eax, 0xffffffff

// block 00479eda
00479eda  cmp        byte ptr [ebp - 0x1f4], 0
00479ee1  je         0x479f2b

// block 00479ee3
00479ee3  mov        ecx, dword ptr [ebp - 0x1f8]
00479ee9  and        dword ptr [ecx + 0x70], 0xfffffffd
00479eed  jmp        0x479f2b

// block 00479eef
00479eef  cmp        dword ptr [ebp - 0x1f0], 1
00479ef6  jne        0x479f12

// block 00479ef8
00479ef8  call       0x46e96f

// block 00479efd
00479efd  mov        dword ptr [eax], 0x16
00479f03  xor        eax, eax
00479f05  push       eax
00479f06  push       eax
00479f07  push       eax
00479f08  push       eax
00479f09  push       eax
00479f0a  call       0x470f2d

// block 00479f0f
00479f0f  add        esp, 0x14

// block 00479f12
00479f12  cmp        byte ptr [ebp - 0x1f4], 0
00479f19  je         0x479f25

// block 00479f1b
00479f1b  mov        eax, dword ptr [ebp - 0x1f8]
00479f21  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00479f25
00479f25  mov        eax, dword ptr [ebp - 0x1c4]

// block 00479f2b
00479f2b  pop        ebx

// block 00479f2c
00479f2c  mov        ecx, dword ptr [ebp - 4]
00479f2f  pop        edi
00479f30  xor        ecx, ebp
00479f32  pop        esi
00479f33  call       0x46c932

// block 00479f38
00479f38  leave      
00479f39  ret        
