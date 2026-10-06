
// block 00424cc0
00424cc0  push       ebp
00424cc1  mov        ebp, esp
00424cc3  and        esp, 0xffffffc0
00424cc6  sub        esp, 0x74
00424cc9  push       ebx
00424cca  push       esi
00424ccb  push       edi
00424ccc  push       0x1000
00424cd1  call       0x46d04a

// block 00424cd6
00424cd6  push       0x4a0238
00424cdb  mov        edi, eax
00424cdd  call       0x46d585

// block 00424ce2
00424ce2  add        esp, 8
00424ce5  mov        esi, 0x4a00f8
00424cea  call       0x463fb0

// block 00424cef
00424cef  test       eax, eax
00424cf1  jne        0x42560c

// block 00424cf7
00424cf7  push       0x4a0240
00424cfc  push       edi
00424cfd  call       0x46cbbb

// block 00424d02
00424d02  mov        eax, edi
00424d04  add        esp, 8
00424d07  lea        edx, [eax + 1]
00424d0a  lea        ebx, [ebx]

// block 00424d10
00424d10  mov        cl, byte ptr [eax]
00424d12  inc        eax
00424d13  test       cl, cl
00424d15  jne        0x424d10

// block 00424d17
00424d17  sub        eax, edx
00424d19  mov        esi, eax
00424d1b  mov        edx, edi
00424d1d  call       0x464130

// block 00424d22
00424d22  push       0x4a0280
00424d27  push       edi
00424d28  call       0x46cbbb

// block 00424d2d
00424d2d  mov        eax, edi
00424d2f  add        esp, 8
00424d32  lea        edx, [eax + 1]

// block 00424d35
00424d35  mov        cl, byte ptr [eax]
00424d37  inc        eax
00424d38  test       cl, cl
00424d3a  jne        0x424d35

// block 00424d3c
00424d3c  sub        eax, edx
00424d3e  mov        esi, eax
00424d40  mov        edx, edi
00424d42  call       0x464130

// block 00424d47
00424d47  push       0x4a02a4
00424d4c  push       edi
00424d4d  call       0x46cbbb

// block 00424d52
00424d52  mov        eax, edi
00424d54  add        esp, 8
00424d57  lea        edx, [eax + 1]
00424d5a  lea        ebx, [ebx]

// block 00424d60
00424d60  mov        cl, byte ptr [eax]
00424d62  inc        eax
00424d63  test       cl, cl
00424d65  jne        0x424d60

// block 00424d67
00424d67  sub        eax, edx
00424d69  mov        esi, eax
00424d6b  mov        edx, edi
00424d6d  call       0x464130

// block 00424d72
00424d72  push       0x4a02a8
00424d77  push       edi
00424d78  call       0x46cbbb

// block 00424d7d
00424d7d  mov        eax, edi
00424d7f  add        esp, 8
00424d82  lea        edx, [eax + 1]

// block 00424d85
00424d85  mov        cl, byte ptr [eax]
00424d87  inc        eax
00424d88  test       cl, cl
00424d8a  jne        0x424d85

// block 00424d8c
00424d8c  sub        eax, edx
00424d8e  mov        esi, eax
00424d90  mov        edx, edi
00424d92  call       0x464130

// block 00424d97
00424d97  push       0x4a02e0
00424d9c  push       edi
00424d9d  call       0x46cbbb

// block 00424da2
00424da2  mov        eax, edi
00424da4  add        esp, 8
00424da7  lea        edx, [eax + 1]
00424daa  lea        ebx, [ebx]

// block 00424db0
00424db0  mov        cl, byte ptr [eax]
00424db2  inc        eax
00424db3  test       cl, cl
00424db5  jne        0x424db0

// block 00424db7
00424db7  sub        eax, edx
00424db9  mov        esi, eax
00424dbb  mov        edx, edi
00424dbd  call       0x464130

// block 00424dc2
00424dc2  push       0x4a02a4
00424dc7  push       edi
00424dc8  call       0x46cbbb

// block 00424dcd
00424dcd  mov        eax, edi
00424dcf  add        esp, 8
00424dd2  lea        edx, [eax + 1]

// block 00424dd5
00424dd5  mov        cl, byte ptr [eax]
00424dd7  inc        eax
00424dd8  test       cl, cl
00424dda  jne        0x424dd5

// block 00424ddc
00424ddc  sub        eax, edx
00424dde  mov        esi, eax
00424de0  mov        edx, edi
00424de2  call       0x464130

// block 00424de7
00424de7  lea        eax, [esp + 0x78]
00424deb  push       eax
00424dec  call       0x46d5b7

// block 00424df1
00424df1  lea        ecx, [esp + 0x7c]
00424df5  push       ecx
00424df6  call       0x46d8e4

// block 00424dfb
00424dfb  mov        edx, dword ptr [eax + 4]
00424dfe  mov        ecx, dword ptr [eax + 8]
00424e01  push       edx
00424e02  mov        edx, dword ptr [eax + 0xc]
00424e05  push       ecx
00424e06  mov        ecx, dword ptr [eax + 0x10]
00424e09  push       edx
00424e0a  mov        edx, dword ptr [eax + 0x14]
00424e0d  inc        ecx
00424e0e  push       ecx
00424e0f  add        edx, 0x76c
00424e15  push       edx
00424e16  push       0x4a0328
00424e1b  push       edi
00424e1c  call       0x46cbbb

// block 00424e21
00424e21  mov        eax, edi
00424e23  add        esp, 0x24
00424e26  lea        edx, [eax + 1]
00424e29  lea        esp, [esp]

// block 00424e30
00424e30  mov        cl, byte ptr [eax]
00424e32  inc        eax
00424e33  test       cl, cl
00424e35  jne        0x424e30

// block 00424e37
00424e37  sub        eax, edx
00424e39  mov        esi, eax
00424e3b  mov        edx, edi
00424e3d  call       0x464130

// block 00424e42
00424e42  push       0x4a0374
00424e47  push       edi
00424e48  call       0x46cbbb

// block 00424e4d
00424e4d  mov        eax, edi
00424e4f  add        esp, 8
00424e52  lea        edx, [eax + 1]

// block 00424e55
00424e55  mov        cl, byte ptr [eax]
00424e57  inc        eax
00424e58  test       cl, cl
00424e5a  jne        0x424e55

// block 00424e5c
00424e5c  sub        eax, edx
00424e5e  mov        esi, eax
00424e60  mov        edx, edi
00424e62  call       0x464130

// block 00424e67
00424e67  push       0x4a015c
00424e6c  push       0x4a037c
00424e71  push       edi
00424e72  call       0x46cbbb

// block 00424e77
00424e77  mov        eax, edi
00424e79  add        esp, 0xc
00424e7c  lea        edx, [eax + 1]
00424e7f  nop        

// block 00424e80
00424e80  mov        cl, byte ptr [eax]
00424e82  inc        eax
00424e83  test       cl, cl
00424e85  jne        0x424e80

// block 00424e87
00424e87  sub        eax, edx
00424e89  mov        esi, eax
00424e8b  mov        edx, edi
00424e8d  call       0x464130

// block 00424e92
00424e92  mov        eax, dword ptr [ebp + 8]
00424e95  add        eax, 0x7c
00424e98  mov        dword ptr [esp + 0x30], 0
00424ea0  mov        dword ptr [esp + 0x38], eax
00424ea4  jmp        0x424eb0

// block 00424eb0
00424eb0  mov        ecx, dword ptr [esp + 0x38]
00424eb4  mov        eax, dword ptr [ecx]
00424eb6  mov        dword ptr [esp + 0x34], 0
00424ebe  test       eax, eax
00424ec0  je         0x4255f5

// block 00424ec6
00424ec6  mov        ebx, dword ptr [eax]
00424ec8  cmp        dword ptr [ebx + 0x70], 0
00424ecc  mov        ecx, dword ptr [eax + 4]
00424ecf  mov        dword ptr [esp + 0x3c], ecx
00424ed3  jne        0x424edc

// block 00424ed5
00424ed5  mov        eax, ecx
00424ed7  jmp        0x42557b

// block 00424edc
00424edc  cmp        dword ptr [esp + 0x34], 0
00424ee1  jne        0x424fc4

// block 00424ee7
00424ee7  push       0x4a0390
00424eec  push       edi
00424eed  call       0x46cbbb

// block 00424ef2
00424ef2  mov        eax, edi
00424ef4  add        esp, 8
00424ef7  lea        edx, [eax + 1]
00424efa  lea        ebx, [ebx]

// block 00424f00
00424f00  mov        cl, byte ptr [eax]
00424f02  inc        eax
00424f03  test       cl, cl
00424f05  jne        0x424f00

// block 00424f07
00424f07  sub        eax, edx
00424f09  mov        esi, eax
00424f0b  mov        eax, dword ptr [0x4ae590]
00424f10  cmp        eax, -1
00424f13  je         0x424f54

// block 00424f15
00424f15  push       0
00424f17  lea        edx, [esp + 0x44]
00424f1b  push       edx
00424f1c  push       esi
00424f1d  push       edi
00424f1e  push       eax
00424f1f  call       dword ptr [0x4980ac]

// block 00424f25
00424f25  cmp        esi, dword ptr [esp + 0x40]
00424f29  je         0x424f54

// block 00424f2b
00424f2b  mov        eax, dword ptr [0x4ae590]
00424f30  push       eax
00424f31  call       dword ptr [0x4980a4]

// block 00424f37
00424f37  test       dword ptr [0x4cee78], 0x8000
00424f41  je         0x424f54

// block 00424f43
00424f43  push       0x4cf128
00424f48  call       dword ptr [0x49808c]

// block 00424f4e
00424f4e  dec        byte ptr [0x4cf21a]

// block 00424f54
00424f54  mov        ecx, dword ptr [esp + 0x30]
00424f58  push       ecx
00424f59  push       0x4a03b8
00424f5e  push       edi
00424f5f  call       0x46cbbb

// block 00424f64
00424f64  mov        eax, edi
00424f66  add        esp, 0xc
00424f69  lea        edx, [eax + 1]
00424f6c  lea        esp, [esp]

// block 00424f70
00424f70  mov        cl, byte ptr [eax]
00424f72  inc        eax
00424f73  test       cl, cl
00424f75  jne        0x424f70

// block 00424f77
00424f77  sub        eax, edx
00424f79  mov        esi, eax
00424f7b  mov        eax, dword ptr [0x4ae590]
00424f80  cmp        eax, -1
00424f83  je         0x424fc4

// block 00424f85
00424f85  push       0
00424f87  lea        edx, [esp + 0x48]
00424f8b  push       edx
00424f8c  push       esi
00424f8d  push       edi
00424f8e  push       eax
00424f8f  call       dword ptr [0x4980ac]

// block 00424f95
00424f95  cmp        esi, dword ptr [esp + 0x44]
00424f99  je         0x424fc4

// block 00424f9b
00424f9b  mov        eax, dword ptr [0x4ae590]
00424fa0  push       eax
00424fa1  call       dword ptr [0x4980a4]

// block 00424fa7
00424fa7  test       dword ptr [0x4cee78], 0x8000
00424fb1  je         0x424fc4

// block 00424fb3
00424fb3  push       0x4cf128
00424fb8  call       dword ptr [0x49808c]

// block 00424fbe
00424fbe  dec        byte ptr [0x4cf21a]

// block 00424fc4
00424fc4  mov        ecx, dword ptr [esp + 0x30]
00424fc8  push       ecx
00424fc9  push       0x4a03c8
00424fce  push       edi
00424fcf  call       0x46cbbb

// block 00424fd4
00424fd4  mov        eax, edi
00424fd6  add        esp, 0xc
00424fd9  lea        edx, [eax + 1]
00424fdc  lea        esp, [esp]

// block 00424fe0
00424fe0  mov        cl, byte ptr [eax]
00424fe2  inc        eax
00424fe3  test       cl, cl
00424fe5  jne        0x424fe0

// block 00424fe7
00424fe7  sub        eax, edx
00424fe9  cmp        dword ptr [0x4ae590], -1
00424ff0  mov        esi, eax
00424ff2  je         0x425039

// block 00424ff4
00424ff4  mov        eax, dword ptr [0x4ae590]
00424ff9  push       0
00424ffb  lea        edx, [esp + 0x4c]
00424fff  push       edx
00425000  push       esi
00425001  push       edi
00425002  push       eax
00425003  call       dword ptr [0x4980ac]

// block 00425009
00425009  cmp        esi, dword ptr [esp + 0x48]
0042500d  je         0x425039

// block 0042500f
0042500f  mov        ecx, dword ptr [0x4ae590]
00425015  push       ecx
00425016  call       dword ptr [0x4980a4]

// block 0042501c
0042501c  test       dword ptr [0x4cee78], 0x8000
00425026  je         0x425039

// block 00425028
00425028  push       0x4cf128
0042502d  call       dword ptr [0x49808c]

// block 00425033
00425033  dec        byte ptr [0x4cf21a]

// block 00425039
00425039  mov        eax, dword ptr [ebx + 0x70]
0042503c  test       eax, eax
0042503e  jle        0x42504f

// block 00425040
00425040  push       eax
00425041  push       0x4a03d0
00425046  push       edi
00425047  call       0x46cbbb

// block 0042504c
0042504c  add        esp, 0xc

// block 0042504f
0042504f  mov        eax, edi
00425051  lea        edx, [eax + 1]

// block 00425054
00425054  mov        cl, byte ptr [eax]
00425056  inc        eax
00425057  test       cl, cl
00425059  jne        0x425054

// block 0042505b
0042505b  sub        eax, edx
0042505d  cmp        dword ptr [0x4ae590], -1
00425064  mov        esi, eax
00425066  je         0x4250ad

// block 00425068
00425068  mov        eax, dword ptr [0x4ae590]
0042506d  push       0
0042506f  lea        edx, [esp + 0x50]
00425073  push       edx
00425074  push       esi
00425075  push       edi
00425076  push       eax
00425077  call       dword ptr [0x4980ac]

// block 0042507d
0042507d  cmp        esi, dword ptr [esp + 0x4c]
00425081  je         0x4250ad

// block 00425083
00425083  mov        ecx, dword ptr [0x4ae590]
00425089  push       ecx
0042508a  call       dword ptr [0x4980a4]

// block 00425090
00425090  test       dword ptr [0x4cee78], 0x8000
0042509a  je         0x4250ad

// block 0042509c
0042509c  push       0x4cf128
004250a1  call       dword ptr [0x49808c]

// block 004250a7
004250a7  dec        byte ptr [0x4cf21a]

// block 004250ad
004250ad  lea        edx, [ebx + 0x1c]
004250b0  push       edx
004250b1  push       0x4a03e0
004250b6  push       edi
004250b7  call       0x46cbbb

// block 004250bc
004250bc  mov        eax, edi
004250be  add        esp, 0xc
004250c1  lea        edx, [eax + 1]

// block 004250c4
004250c4  mov        cl, byte ptr [eax]
004250c6  inc        eax
004250c7  test       cl, cl
004250c9  jne        0x4250c4

// block 004250cb
004250cb  sub        eax, edx
004250cd  mov        esi, eax
004250cf  mov        eax, dword ptr [0x4ae590]
004250d4  cmp        eax, -1
004250d7  je         0x425119

// block 004250d9
004250d9  push       0
004250db  lea        ecx, [esp + 0x54]
004250df  push       ecx
004250e0  push       esi
004250e1  push       edi
004250e2  push       eax
004250e3  call       dword ptr [0x4980ac]

// block 004250e9
004250e9  cmp        esi, dword ptr [esp + 0x50]
004250ed  je         0x425119

// block 004250ef
004250ef  mov        edx, dword ptr [0x4ae590]
004250f5  push       edx
004250f6  call       dword ptr [0x4980a4]

// block 004250fc
004250fc  test       dword ptr [0x4cee78], 0x8000
00425106  je         0x425119

// block 00425108
00425108  push       0x4cf128
0042510d  call       dword ptr [0x49808c]

// block 00425113
00425113  dec        byte ptr [0x4cf21a]

// block 00425119
00425119  fld        dword ptr [ebx + 4]
0042511c  call       0x4931e0

// block 00425121
00425121  fld        dword ptr [ebx]
00425123  push       eax
00425124  call       0x4931e0

// block 00425129
00425129  push       eax
0042512a  push       0x4a03f0
0042512f  push       edi
00425130  call       0x46cbbb

// block 00425135
00425135  mov        eax, edi
00425137  add        esp, 0x10
0042513a  lea        edx, [eax + 1]
0042513d  lea        ecx, [ecx]

// block 00425140
00425140  mov        cl, byte ptr [eax]
00425142  inc        eax
00425143  test       cl, cl
00425145  jne        0x425140

// block 00425147
00425147  sub        eax, edx
00425149  mov        esi, eax
0042514b  mov        eax, dword ptr [0x4ae590]
00425150  cmp        eax, -1
00425153  je         0x425195

// block 00425155
00425155  push       0
00425157  lea        ecx, [esp + 0x58]
0042515b  push       ecx
0042515c  push       esi
0042515d  push       edi
0042515e  push       eax
0042515f  call       dword ptr [0x4980ac]

// block 00425165
00425165  cmp        esi, dword ptr [esp + 0x54]
00425169  je         0x425195

// block 0042516b
0042516b  mov        edx, dword ptr [0x4ae590]
00425171  push       edx
00425172  call       dword ptr [0x4980a4]

// block 00425178
00425178  test       dword ptr [0x4cee78], 0x8000
00425182  je         0x425195

// block 00425184
00425184  push       0x4cf128
00425189  call       dword ptr [0x49808c]

// block 0042518f
0042518f  dec        byte ptr [0x4cf21a]

// block 00425195
00425195  mov        eax, dword ptr [ebx + 0x60]
00425198  push       eax
00425199  push       0x4a0404
0042519e  push       edi
0042519f  call       0x46cbbb

// block 004251a4
004251a4  mov        eax, edi
004251a6  add        esp, 0xc
004251a9  lea        edx, [eax + 1]
004251ac  lea        esp, [esp]

// block 004251b0
004251b0  mov        cl, byte ptr [eax]
004251b2  inc        eax
004251b3  test       cl, cl
004251b5  jne        0x4251b0

// block 004251b7
004251b7  sub        eax, edx
004251b9  mov        esi, eax
004251bb  mov        eax, dword ptr [0x4ae590]
004251c0  cmp        eax, -1
004251c3  je         0x425205

// block 004251c5
004251c5  push       0
004251c7  lea        ecx, [esp + 0x5c]
004251cb  push       ecx
004251cc  push       esi
004251cd  push       edi
004251ce  push       eax
004251cf  call       dword ptr [0x4980ac]

// block 004251d5
004251d5  cmp        esi, dword ptr [esp + 0x58]
004251d9  je         0x425205

// block 004251db
004251db  mov        edx, dword ptr [0x4ae590]
004251e1  push       edx
004251e2  call       dword ptr [0x4980a4]

// block 004251e8
004251e8  test       dword ptr [0x4cee78], 0x8000
004251f2  je         0x425205

// block 004251f4
004251f4  push       0x4cf128
004251f9  call       dword ptr [0x49808c]

// block 004251ff
004251ff  dec        byte ptr [0x4cf21a]

// block 00425205
00425205  mov        ecx, dword ptr [ebx + 0x68]
00425208  xor        eax, eax
0042520a  lea        ebx, [ebx]

// block 00425210
00425210  cmp        dword ptr [eax*8 + 0x4aef14], ecx
00425217  je         0x42564e

// block 0042521d
0042521d  inc        eax
0042521e  cmp        eax, 0x3d
00425221  jl         0x425210

// block 00425223
00425223  xor        eax, eax

// block 00425225
00425225  push       eax
00425226  push       0x4a0414
0042522b  push       edi
0042522c  call       0x46cbbb

// block 00425231
00425231  mov        eax, edi
00425233  add        esp, 0xc
00425236  lea        edx, [eax + 1]
00425239  lea        esp, [esp]

// block 00425240
00425240  mov        cl, byte ptr [eax]
00425242  inc        eax
00425243  test       cl, cl
00425245  jne        0x425240

// block 00425247
00425247  sub        eax, edx
00425249  mov        esi, eax
0042524b  mov        eax, dword ptr [0x4ae590]
00425250  cmp        eax, -1
00425253  je         0x425295

// block 00425255
00425255  push       0
00425257  lea        ecx, [esp + 0x60]
0042525b  push       ecx
0042525c  push       esi
0042525d  push       edi
0042525e  push       eax
0042525f  call       dword ptr [0x4980ac]

// block 00425265
00425265  cmp        esi, dword ptr [esp + 0x5c]
00425269  je         0x425295

// block 0042526b
0042526b  mov        edx, dword ptr [0x4ae590]
00425271  push       edx
00425272  call       dword ptr [0x4980a4]

// block 00425278
00425278  test       dword ptr [0x4cee78], 0x8000
00425282  je         0x425295

// block 00425284
00425284  push       0x4cf128
00425289  call       dword ptr [0x49808c]

// block 0042528f
0042528f  dec        byte ptr [0x4cf21a]

// block 00425295
00425295  mov        ecx, dword ptr [ebx + 0x64]
00425298  xor        eax, eax
0042529a  lea        ebx, [ebx]

// block 004252a0
004252a0  cmp        dword ptr [eax*8 + 0x4aeefc], ecx
004252a7  je         0x42565a

// block 004252ad
004252ad  inc        eax
004252ae  cmp        eax, 3
004252b1  jl         0x4252a0

// block 004252b3
004252b3  xor        eax, eax

// block 004252b5
004252b5  push       eax
004252b6  push       0x4a0424
004252bb  push       edi
004252bc  call       0x46cbbb

// block 004252c1
004252c1  mov        eax, edi
004252c3  add        esp, 0xc
004252c6  lea        edx, [eax + 1]
004252c9  lea        esp, [esp]

// block 004252d0
004252d0  mov        cl, byte ptr [eax]
004252d2  inc        eax
004252d3  test       cl, cl
004252d5  jne        0x4252d0

// block 004252d7
004252d7  sub        eax, edx
004252d9  mov        esi, eax
004252db  mov        eax, dword ptr [0x4ae590]
004252e0  cmp        eax, -1
004252e3  je         0x425325

// block 004252e5
004252e5  push       0
004252e7  lea        ecx, [esp + 0x64]
004252eb  push       ecx
004252ec  push       esi
004252ed  push       edi
004252ee  push       eax
004252ef  call       dword ptr [0x4980ac]

// block 004252f5
004252f5  cmp        esi, dword ptr [esp + 0x60]
004252f9  je         0x425325

// block 004252fb
004252fb  mov        edx, dword ptr [0x4ae590]
00425301  push       edx
00425302  call       dword ptr [0x4980a4]

// block 00425308
00425308  test       dword ptr [0x4cee78], 0x8000
00425312  je         0x425325

// block 00425314
00425314  push       0x4cf128
00425319  call       dword ptr [0x49808c]

// block 0042531f
0042531f  dec        byte ptr [0x4cf21a]

// block 00425325
00425325  mov        eax, dword ptr [ebx + 0x6c]
00425328  push       eax
00425329  push       0x4a0434
0042532e  push       edi
0042532f  call       0x46cbbb

// block 00425334
00425334  mov        eax, edi
00425336  add        esp, 0xc
00425339  lea        edx, [eax + 1]
0042533c  lea        esp, [esp]

// block 00425340
00425340  mov        cl, byte ptr [eax]
00425342  inc        eax
00425343  test       cl, cl
00425345  jne        0x425340

// block 00425347
00425347  sub        eax, edx
00425349  mov        esi, eax
0042534b  mov        eax, dword ptr [0x4ae590]
00425350  cmp        eax, -1
00425353  je         0x425395

// block 00425355
00425355  push       0
00425357  lea        ecx, [esp + 0x68]
0042535b  push       ecx
0042535c  push       esi
0042535d  push       edi
0042535e  push       eax
0042535f  call       dword ptr [0x4980ac]

// block 00425365
00425365  cmp        esi, dword ptr [esp + 0x64]
00425369  je         0x425395

// block 0042536b
0042536b  mov        edx, dword ptr [0x4ae590]
00425371  push       edx
00425372  call       dword ptr [0x4980a4]

// block 00425378
00425378  test       dword ptr [0x4cee78], 0x8000
00425382  je         0x425395

// block 00425384
00425384  push       0x4cf128
00425389  call       dword ptr [0x49808c]

// block 0042538f
0042538f  dec        byte ptr [0x4cf21a]

// block 00425395
00425395  movzx      eax, byte ptr [ebx + 0x87]
0042539c  push       eax
0042539d  push       0x4a0444
004253a2  push       edi
004253a3  call       0x46cbbb

// block 004253a8
004253a8  mov        eax, edi
004253aa  add        esp, 0xc
004253ad  lea        edx, [eax + 1]

// block 004253b0
004253b0  mov        cl, byte ptr [eax]
004253b2  inc        eax
004253b3  test       cl, cl
004253b5  jne        0x4253b0

// block 004253b7
004253b7  sub        eax, edx
004253b9  mov        esi, eax
004253bb  mov        eax, dword ptr [0x4ae590]
004253c0  cmp        eax, -1
004253c3  je         0x425405

// block 004253c5
004253c5  push       0
004253c7  lea        ecx, [esp + 0x6c]
004253cb  push       ecx
004253cc  push       esi
004253cd  push       edi
004253ce  push       eax
004253cf  call       dword ptr [0x4980ac]

// block 004253d5
004253d5  cmp        esi, dword ptr [esp + 0x68]
004253d9  je         0x425405

// block 004253db
004253db  mov        edx, dword ptr [0x4ae590]
004253e1  push       edx
004253e2  call       dword ptr [0x4980a4]

// block 004253e8
004253e8  test       dword ptr [0x4cee78], 0x8000
004253f2  je         0x425405

// block 004253f4
004253f4  push       0x4cf128
004253f9  call       dword ptr [0x49808c]

// block 004253ff
004253ff  dec        byte ptr [0x4cf21a]

// block 00425405
00425405  movzx      eax, byte ptr [ebx + 0x84]
0042540c  movzx      ecx, byte ptr [ebx + 0x85]
00425413  movzx      edx, byte ptr [ebx + 0x86]
0042541a  push       eax
0042541b  push       ecx
0042541c  push       edx
0042541d  push       0x4a0454
00425422  push       edi
00425423  call       0x46cbbb

// block 00425428
00425428  mov        eax, edi
0042542a  add        esp, 0x14
0042542d  lea        edx, [eax + 1]

// block 00425430
00425430  mov        cl, byte ptr [eax]
00425432  inc        eax
00425433  test       cl, cl
00425435  jne        0x425430

// block 00425437
00425437  sub        eax, edx
00425439  mov        esi, eax
0042543b  mov        eax, dword ptr [0x4ae590]
00425440  cmp        eax, -1
00425443  je         0x425485

// block 00425445
00425445  push       0
00425447  lea        ecx, [esp + 0x70]
0042544b  push       ecx
0042544c  push       esi
0042544d  push       edi
0042544e  push       eax
0042544f  call       dword ptr [0x4980ac]

// block 00425455
00425455  cmp        esi, dword ptr [esp + 0x6c]
00425459  je         0x425485

// block 0042545b
0042545b  mov        edx, dword ptr [0x4ae590]
00425461  push       edx
00425462  call       dword ptr [0x4980a4]

// block 00425468
00425468  test       dword ptr [0x4cee78], 0x8000
00425472  je         0x425485

// block 00425474
00425474  push       0x4cf128
00425479  call       dword ptr [0x49808c]

// block 0042547f
0042547f  dec        byte ptr [0x4cf21a]

// block 00425485
00425485  fld        dword ptr [ebx + 0x7c]
00425488  sub        esp, 8
0042548b  fstp       qword ptr [esp]
0042548e  push       0x4a046c
00425493  push       edi
00425494  call       0x46cbbb

// block 00425499
00425499  mov        eax, edi
0042549b  add        esp, 0x10
0042549e  lea        edx, [eax + 1]

// block 004254a1
004254a1  mov        cl, byte ptr [eax]
004254a3  inc        eax
004254a4  test       cl, cl
004254a6  jne        0x4254a1

// block 004254a8
004254a8  sub        eax, edx
004254aa  mov        esi, eax
004254ac  mov        eax, dword ptr [0x4ae590]
004254b1  cmp        eax, -1
004254b4  je         0x4254f6

// block 004254b6
004254b6  push       0
004254b8  lea        ecx, [esp + 0x74]
004254bc  push       ecx
004254bd  push       esi
004254be  push       edi
004254bf  push       eax
004254c0  call       dword ptr [0x4980ac]

// block 004254c6
004254c6  cmp        esi, dword ptr [esp + 0x70]
004254ca  je         0x4254f6

// block 004254cc
004254cc  mov        edx, dword ptr [0x4ae590]
004254d2  push       edx
004254d3  call       dword ptr [0x4980a4]

// block 004254d9
004254d9  test       dword ptr [0x4cee78], 0x8000
004254e3  je         0x4254f6

// block 004254e5
004254e5  push       0x4cf128
004254ea  call       dword ptr [0x49808c]

// block 004254f0
004254f0  dec        byte ptr [0x4cf21a]

// block 004254f6
004254f6  mov        eax, dword ptr [esp + 0x30]
004254fa  push       eax
004254fb  push       0x4a047c
00425500  push       edi
00425501  call       0x46cbbb

// block 00425506
00425506  mov        eax, edi
00425508  add        esp, 0xc
0042550b  lea        edx, [eax + 1]
0042550e  mov        edi, edi

// block 00425510
00425510  mov        cl, byte ptr [eax]
00425512  inc        eax
00425513  test       cl, cl
00425515  jne        0x425510

// block 00425517
00425517  sub        eax, edx
00425519  mov        esi, eax
0042551b  mov        eax, dword ptr [0x4ae590]
00425520  cmp        eax, -1
00425523  je         0x425565

// block 00425525
00425525  push       0
00425527  lea        ecx, [esp + 0x78]
0042552b  push       ecx
0042552c  push       esi
0042552d  push       edi
0042552e  push       eax
0042552f  call       dword ptr [0x4980ac]

// block 00425535
00425535  cmp        esi, dword ptr [esp + 0x74]
00425539  je         0x425565

// block 0042553b
0042553b  mov        edx, dword ptr [0x4ae590]
00425541  push       edx
00425542  call       dword ptr [0x4980a4]

// block 00425548
00425548  test       dword ptr [0x4cee78], 0x8000
00425552  je         0x425565

// block 00425554
00425554  push       0x4cf128
00425559  call       dword ptr [0x49808c]

// block 0042555f
0042555f  dec        byte ptr [0x4cf21a]

// block 00425565
00425565  mov        eax, dword ptr [esp + 0x34]
00425569  inc        eax
0042556a  cmp        eax, 0xff
0042556f  mov        dword ptr [esp + 0x34], eax
00425573  jge        0x425583

// block 00425575
00425575  mov        eax, dword ptr [esp + 0x3c]
00425579  mov        ecx, eax

// block 0042557b
0042557b  test       ecx, ecx
0042557d  jne        0x424ec6

// block 00425583
00425583  cmp        dword ptr [esp + 0x34], 0
00425588  je         0x4255f5

// block 0042558a
0042558a  push       0x4a0484
0042558f  push       edi
00425590  call       0x46cbbb

// block 00425595
00425595  mov        eax, edi
00425597  add        esp, 8
0042559a  lea        edx, [eax + 1]
0042559d  lea        ecx, [ecx]

// block 004255a0
004255a0  mov        cl, byte ptr [eax]
004255a2  inc        eax
004255a3  test       cl, cl
004255a5  jne        0x4255a0

// block 004255a7
004255a7  sub        eax, edx
004255a9  mov        esi, eax
004255ab  mov        eax, dword ptr [0x4ae590]
004255b0  cmp        eax, -1
004255b3  je         0x4255f5

// block 004255b5
004255b5  push       0
004255b7  lea        ecx, [esp + 0x7c]
004255bb  push       ecx
004255bc  push       esi
004255bd  push       edi
004255be  push       eax
004255bf  call       dword ptr [0x4980ac]

// block 004255c5
004255c5  cmp        esi, dword ptr [esp + 0x78]
004255c9  je         0x4255f5

// block 004255cb
004255cb  mov        edx, dword ptr [0x4ae590]
004255d1  push       edx
004255d2  call       dword ptr [0x4980a4]

// block 004255d8
004255d8  test       dword ptr [0x4cee78], 0x8000
004255e2  je         0x4255f5

// block 004255e4
004255e4  push       0x4cf128
004255e9  call       dword ptr [0x49808c]

// block 004255ef
004255ef  dec        byte ptr [0x4cf21a]

// block 004255f5
004255f5  mov        eax, dword ptr [esp + 0x30]
004255f9  add        dword ptr [esp + 0x38], 0xc
004255fe  inc        eax
004255ff  cmp        eax, 8
00425602  mov        dword ptr [esp + 0x30], eax
00425606  jl         0x424eb0

// block 0042560c
0042560c  mov        eax, dword ptr [0x4ae590]
00425611  cmp        eax, -1
00425614  je         0x42563a

// block 00425616
00425616  push       eax
00425617  call       dword ptr [0x4980a4]

// block 0042561d
0042561d  test       dword ptr [0x4cee78], 0x8000
00425627  je         0x42563a

// block 00425629
00425629  push       0x4cf128
0042562e  call       dword ptr [0x49808c]

// block 00425634
00425634  dec        byte ptr [0x4cf21a]

// block 0042563a
0042563a  push       edi
0042563b  call       0x46c941

// block 00425640
00425640  add        esp, 4
00425643  pop        edi
00425644  pop        esi
00425645  xor        eax, eax
00425647  pop        ebx
00425648  mov        esp, ebp
0042564a  pop        ebp
0042564b  ret        4

// block 0042564e
0042564e  mov        eax, dword ptr [eax*8 + 0x4aef10]
00425655  jmp        0x425225

// block 0042565a
0042565a  mov        eax, dword ptr [eax*8 + 0x4aeef8]
00425661  jmp        0x4252b5
