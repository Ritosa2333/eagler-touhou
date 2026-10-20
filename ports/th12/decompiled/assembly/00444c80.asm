
// block 00444c80
00444c80  mov        ecx, dword ptr [0x4b0ca8]
00444c86  xor        eax, eax
00444c88  cmp        ecx, 4
00444c8b  setge      al
00444c8e  push       ebp
00444c8f  mov        ebp, dword ptr [esp + 8]
00444c93  push       esi
00444c94  add        eax, 0x83
00444c99  mov        esi, eax
00444c9b  mov        eax, dword ptr [ebp + 0x24]
00444c9e  cmp        eax, 4
00444ca1  ja         0x4452a6

// block 00444ca7
00444ca7  push       ebx
00444ca8  push       edi
00444ca9  jmp        dword ptr [eax*4 + 0x4452b0]

// block 00444cb0
00444cb0  cmp        dword ptr [ebp + 0x66c], 0
00444cb7  jne        0x444ce5

// block 00444cb9
00444cb9  mov        edx, dword ptr [0x4b43b8]
00444cbf  mov        eax, dword ptr [edx + 0x18fb4]
00444cc5  push       0
00444cc7  push       0x14
00444cc9  lea        ecx, [esp + 0x1c]
00444ccd  push       ecx
00444cce  push       eax
00444ccf  mov        eax, 0x17
00444cd4  xor        ecx, ecx
00444cd6  call       0x4615a0

// block 00444cdb
00444cdb  mov        ecx, dword ptr [esp + 0x14]
00444cdf  mov        dword ptr [ebp + 0x66c], ecx

// block 00444ce5
00444ce5  mov        eax, dword ptr [0x4aebd0]
00444cea  xor        edx, edx
00444cec  cmp        eax, 4
00444cef  setge      dl
00444cf2  mov        ebx, 1
00444cf7  dec        edx
00444cf8  and        edx, 3
00444cfb  inc        edx
00444cfc  mov        dword ptr [ebp + 0x30], edx
00444cff  mov        dword ptr [0x4b0ca8], eax
00444d04  mov        eax, dword ptr [ebp + esi*4 + 0x2c8]
00444d0b  push       eax
00444d0c  call       0x461970

// block 00444d11
00444d11  mov        edi, ebp
00444d13  mov        dword ptr [ebp + esi*4 + 0x2c8], 0
00444d1e  call       0x43efa0

// block 00444d23
00444d23  mov        ecx, dword ptr [ebp + esi*4 + 0x2c8]
00444d2a  push       ecx
00444d2b  mov        ebx, 3
00444d30  call       0x4619e0

// block 00444d35
00444d35  movzx      ebx, word ptr [ebp + 0x28]
00444d39  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
00444d40  add        bx, 0x11
00444d44  push       edx
00444d45  call       0x461970

// block 00444d4a
00444d4a  mov        ecx, dword ptr [ebp + 0x14]
00444d4d  push       0
00444d4f  push       0x6e
00444d51  lea        eax, [esp + 0x1c]
00444d55  push       eax
00444d56  push       ecx
00444d57  mov        eax, 0x17
00444d5c  xor        ecx, ecx
00444d5e  call       0x4615a0

// block 00444d63
00444d63  mov        edx, dword ptr [esp + 0x14]
00444d67  mov        ecx, 1
00444d6c  mov        eax, ebp
00444d6e  mov        dword ptr [ebp + 0x480], edx
00444d74  call       0x43ef40

// block 00444d79
00444d79  cmp        dword ptr [0x4b0ca8], 4
00444d80  jge        0x444fcb

// block 00444d86
00444d86  mov        edi, dword ptr [0x4b451c]
00444d8c  xor        ebx, ebx
00444d8e  cmp        dword ptr [edi + 0x598], ebx
00444d94  je         0x444dbe

// block 00444d96
00444d96  cmp        dword ptr [edi + 0x4b8c], ebx
00444d9c  je         0x444dbe

// block 00444d9e
00444d9e  cmp        dword ptr [edi + 0x9180], ebx
00444da4  je         0x444dbe

// block 00444da6
00444da6  cmp        dword ptr [edi + 0xd774], ebx
00444dac  je         0x444dbe

// block 00444dae
00444dae  cmp        dword ptr [edi + 0x11d68], ebx
00444db4  je         0x444dbe

// block 00444db6
00444db6  cmp        dword ptr [edi + 0x1635c], ebx
00444dbc  jne        0x444e18

// block 00444dbe
00444dbe  mov        eax, dword ptr [ebp + esi*4 + 0x2c8]
00444dc5  mov        edx, dword ptr [0x4ce8cc]
00444dcb  push       eax
00444dcc  call       0x461920

// block 00444dd1
00444dd1  cmp        eax, ebx
00444dd3  jne        0x444ddc

// block 00444dd5
00444dd5  mov        dword ptr [ebp + esi*4 + 0x2c8], ebx

// block 00444ddc
00444ddc  add        eax, 0x10
00444ddf  cmp        eax, ebx
00444de1  je         0x444e0b

// block 00444de3
00444de3  jmp        0x444df0

// block 00444df0
00444df0  mov        ecx, dword ptr [eax]
00444df2  mov        edx, 0xaa
00444df7  cmp        word ptr [ecx + 0x3ea], dx
00444dfe  je         0x444fa4

// block 00444e04
00444e04  mov        eax, dword ptr [eax + 4]
00444e07  cmp        eax, ebx
00444e09  jne        0x444df0

// block 00444e0b
00444e0b  mov        dword ptr [esp + 0x14], ebx

// block 00444e0f
00444e0f  lea        eax, [esp + 0x14]
00444e13  call       0x461d80

// block 00444e18
00444e18  cmp        dword ptr [edi + 0x59c], ebx
00444e1e  je         0x444e48

// block 00444e20
00444e20  cmp        dword ptr [edi + 0x4b90], ebx
00444e26  je         0x444e48

// block 00444e28
00444e28  cmp        dword ptr [edi + 0x9184], ebx
00444e2e  je         0x444e48

// block 00444e30
00444e30  cmp        dword ptr [edi + 0xd778], ebx
00444e36  je         0x444e48

// block 00444e38
00444e38  cmp        dword ptr [edi + 0x11d6c], ebx
00444e3e  je         0x444e48

// block 00444e40
00444e40  cmp        dword ptr [edi + 0x16360], ebx
00444e46  jne        0x444e98

// block 00444e48
00444e48  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
00444e4f  push       edx
00444e50  mov        edx, dword ptr [0x4ce8cc]
00444e56  call       0x461920

// block 00444e5b
00444e5b  cmp        eax, ebx
00444e5d  jne        0x444e66

// block 00444e5f
00444e5f  mov        dword ptr [ebp + esi*4 + 0x2c8], ebx

// block 00444e66
00444e66  add        eax, 0x10
00444e69  cmp        eax, ebx
00444e6b  je         0x444e8b

// block 00444e6d
00444e6d  lea        ecx, [ecx]

// block 00444e70
00444e70  mov        ecx, dword ptr [eax]
00444e72  mov        edx, 0xab
00444e77  cmp        word ptr [ecx + 0x3ea], dx
00444e7e  je         0x444fb1

// block 00444e84
00444e84  mov        eax, dword ptr [eax + 4]
00444e87  cmp        eax, ebx
00444e89  jne        0x444e70

// block 00444e8b
00444e8b  mov        dword ptr [esp + 0x14], ebx

// block 00444e8f
00444e8f  lea        eax, [esp + 0x14]
00444e93  call       0x461d80

// block 00444e98
00444e98  cmp        dword ptr [edi + 0x5a0], ebx
00444e9e  je         0x444ec8

// block 00444ea0
00444ea0  cmp        dword ptr [edi + 0x4b94], ebx
00444ea6  je         0x444ec8

// block 00444ea8
00444ea8  cmp        dword ptr [edi + 0x9188], ebx
00444eae  je         0x444ec8

// block 00444eb0
00444eb0  cmp        dword ptr [edi + 0xd77c], ebx
00444eb6  je         0x444ec8

// block 00444eb8
00444eb8  cmp        dword ptr [edi + 0x11d70], ebx
00444ebe  je         0x444ec8

// block 00444ec0
00444ec0  cmp        dword ptr [edi + 0x16364], ebx
00444ec6  jne        0x444f18

// block 00444ec8
00444ec8  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
00444ecf  push       edx
00444ed0  mov        edx, dword ptr [0x4ce8cc]
00444ed6  call       0x461920

// block 00444edb
00444edb  cmp        eax, ebx
00444edd  jne        0x444ee6

// block 00444edf
00444edf  mov        dword ptr [ebp + esi*4 + 0x2c8], ebx

// block 00444ee6
00444ee6  add        eax, 0x10
00444ee9  cmp        eax, ebx
00444eeb  je         0x444f0b

// block 00444eed
00444eed  lea        ecx, [ecx]

// block 00444ef0
00444ef0  mov        ecx, dword ptr [eax]
00444ef2  mov        edx, 0xac
00444ef7  cmp        word ptr [ecx + 0x3ea], dx
00444efe  je         0x444fbe

// block 00444f04
00444f04  mov        eax, dword ptr [eax + 4]
00444f07  cmp        eax, ebx
00444f09  jne        0x444ef0

// block 00444f0b
00444f0b  mov        dword ptr [esp + 0x14], ebx

// block 00444f0f
00444f0f  lea        eax, [esp + 0x14]
00444f13  call       0x461d80

// block 00444f18
00444f18  cmp        dword ptr [edi + 0x5a4], ebx
00444f1e  je         0x444f4e

// block 00444f20
00444f20  cmp        dword ptr [edi + 0x4b98], ebx
00444f26  je         0x444f4e

// block 00444f28
00444f28  cmp        dword ptr [edi + 0x918c], ebx
00444f2e  je         0x444f4e

// block 00444f30
00444f30  cmp        dword ptr [edi + 0xd780], ebx
00444f36  je         0x444f4e

// block 00444f38
00444f38  cmp        dword ptr [edi + 0x11d74], ebx
00444f3e  je         0x444f4e

// block 00444f40
00444f40  cmp        dword ptr [edi + 0x16368], ebx
00444f46  jne        0x445054

// block 00444f4c
00444f4c  xor        ebx, ebx

// block 00444f4e
00444f4e  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
00444f55  push       edx
00444f56  mov        edx, dword ptr [0x4ce8cc]
00444f5c  call       0x461920

// block 00444f61
00444f61  cmp        eax, ebx
00444f63  jne        0x444f6c

// block 00444f65
00444f65  mov        dword ptr [ebp + esi*4 + 0x2c8], ebx

// block 00444f6c
00444f6c  add        eax, 0x10
00444f6f  cmp        eax, ebx
00444f71  je         0x444f9b

// block 00444f73
00444f73  jmp        0x444f80

// block 00444f80
00444f80  mov        ecx, dword ptr [eax]
00444f82  mov        edx, 0xad
00444f87  cmp        word ptr [ecx + 0x3ea], dx
00444f8e  je         0x445079

// block 00444f94
00444f94  mov        eax, dword ptr [eax + 4]
00444f97  cmp        eax, ebx
00444f99  jne        0x444f80

// block 00444f9b
00444f9b  mov        dword ptr [esp + 0x14], ebx
00444f9f  jmp        0x44504b

// block 00444fa4
00444fa4  mov        eax, ecx
00444fa6  mov        ecx, dword ptr [eax]
00444fa8  mov        dword ptr [esp + 0x14], ecx
00444fac  jmp        0x444e0f

// block 00444fb1
00444fb1  mov        eax, ecx
00444fb3  mov        ecx, dword ptr [eax]
00444fb5  mov        dword ptr [esp + 0x14], ecx
00444fb9  jmp        0x444e8f

// block 00444fbe
00444fbe  mov        eax, ecx
00444fc0  mov        ecx, dword ptr [eax]
00444fc2  mov        dword ptr [esp + 0x14], ecx
00444fc6  jmp        0x444f0f

// block 00444fcb
00444fcb  mov        eax, dword ptr [0x4b451c]
00444fd0  xor        edi, edi
00444fd2  cmp        dword ptr [eax + 0x5a8], edi
00444fd8  je         0x445002

// block 00444fda
00444fda  cmp        dword ptr [eax + 0x4b9c], edi
00444fe0  je         0x445002

// block 00444fe2
00444fe2  cmp        dword ptr [eax + 0x9190], edi
00444fe8  je         0x445002

// block 00444fea
00444fea  cmp        dword ptr [eax + 0xd784], edi
00444ff0  je         0x445002

// block 00444ff2
00444ff2  cmp        dword ptr [eax + 0x11d78], edi
00444ff8  je         0x445002

// block 00444ffa
00444ffa  cmp        dword ptr [eax + 0x1636c], edi
00445000  jne        0x445054

// block 00445002
00445002  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
00445009  push       edx
0044500a  mov        edx, dword ptr [0x4ce8cc]
00445010  call       0x461920

// block 00445015
00445015  cmp        eax, edi
00445017  jne        0x445020

// block 00445019
00445019  mov        dword ptr [ebp + esi*4 + 0x2c8], edi

// block 00445020
00445020  add        eax, 0x10
00445023  cmp        eax, edi
00445025  je         0x445047

// block 00445027
00445027  jmp        0x445030

// block 00445030
00445030  mov        ecx, dword ptr [eax]
00445032  mov        edx, 0xae
00445037  cmp        word ptr [ecx + 0x3ea], dx
0044503e  je         0x445079

// block 00445040
00445040  mov        eax, dword ptr [eax + 4]
00445043  cmp        eax, edi
00445045  jne        0x445030

// block 00445047
00445047  mov        dword ptr [esp + 0x14], edi

// block 0044504b
0044504b  lea        eax, [esp + 0x14]
0044504f  call       0x461d80

// block 00445054
00445054  cmp        dword ptr [ebp + 0x2b8], 6
0044505b  jle        0x4452a4

// block 00445061
00445061  mov        ecx, 2
00445066  mov        eax, ebp
00445068  call       0x43ef40

// block 0044506d
0044506d  pop        edi
0044506e  pop        ebx
0044506f  pop        esi
00445070  mov        eax, 1
00445075  pop        ebp
00445076  ret        4

// block 00445079
00445079  mov        eax, ecx
0044507b  mov        ecx, dword ptr [eax]
0044507d  mov        dword ptr [esp + 0x14], ecx
00445081  jmp        0x44504b

// block 00445083
00445083  cmp        ecx, 4
00445086  jge        0x4450f5

// block 00445088
00445088  mov        edx, dword ptr [ebp + 0x28]
0044508b  lea        edi, [ebp + 0x28]
0044508e  mov        al, 0x10
00445090  mov        dword ptr [edi + 4], edx
00445093  test       byte ptr [0x4d48c4], al
00445099  jne        0x4450a3

// block 0044509b
0044509b  test       byte ptr [0x4d48c0], al
004450a1  je         0x4450ac

// block 004450a3
004450a3  push       -1
004450a5  mov        eax, edi
004450a7  call       0x464970

// block 004450ac
004450ac  mov        al, 0x20
004450ae  test       byte ptr [0x4d48c4], al
004450b4  jne        0x4450be

// block 004450b6
004450b6  test       byte ptr [0x4d48c0], al
004450bc  je         0x4450c7

// block 004450be
004450be  push       1
004450c0  mov        eax, edi
004450c2  call       0x464970

// block 004450c7
004450c7  mov        eax, dword ptr [edi + 4]
004450ca  cmp        eax, dword ptr [edi]
004450cc  je         0x4450f5

// block 004450ce
004450ce  mov        edx, 0xa
004450d3  call       0x453d90

// block 004450d8
004450d8  mov        ecx, esi
004450da  mov        eax, ebp
004450dc  call       0x43f010

// block 004450e1
004450e1  movzx      ebx, word ptr [edi]
004450e4  mov        ecx, dword ptr [ebp + esi*4 + 0x2c8]
004450eb  add        bx, 7
004450ef  push       ecx
004450f0  call       0x461970

// block 004450f5
004450f5  mov        eax, dword ptr [0x4d48c4]
004450fa  test       eax, 0x102
004450ff  je         0x44512a

// block 00445101
00445101  mov        ecx, 4
00445106  mov        eax, ebp
00445108  call       0x43ef40

// block 0044510d
0044510d  mov        edx, 9
00445112  call       0x453d90

// block 00445117
00445117  mov        edi, ebp
00445119  call       0x43efd0

// block 0044511e
0044511e  pop        edi
0044511f  pop        ebx
00445120  pop        esi
00445121  mov        eax, 1
00445126  pop        ebp
00445127  ret        4

// block 0044512a
0044512a  test       eax, 0x80001
0044512f  je         0x4452a4

// block 00445135
00445135  mov        edx, dword ptr [ebp + esi*4 + 0x2c8]
0044513c  lea        esi, [ebp + esi*4 + 0x2c8]
00445143  push       edx
00445144  mov        ebx, 6
00445149  call       0x461970

// block 0044514e
0044514e  cmp        dword ptr [0x4b0ca8], 4
00445155  jge        0x44516c

// block 00445157
00445157  mov        edi, dword ptr [ebp + 0x28]
0044515a  add        edi, 0x79
0044515d  lea        ebx, [esp + 0x14]
00445161  call       0x462060

// block 00445166
00445166  mov        eax, dword ptr [esp + 0x14]
0044516a  jmp        0x4451a4

// block 0044516c
0044516c  mov        ecx, dword ptr [esi]
0044516e  mov        edx, dword ptr [0x4ce8cc]
00445174  push       ecx
00445175  call       0x461920

// block 0044517a
0044517a  test       eax, eax
0044517c  jne        0x445180

// block 0044517e
0044517e  mov        dword ptr [esi], eax

// block 00445180
00445180  add        eax, 0x10
00445183  je         0x4451a2

// block 00445185
00445185  mov        ecx, 0x7d
0044518a  lea        ebx, [ebx]

// block 00445190
00445190  mov        edx, dword ptr [eax]
00445192  cmp        word ptr [edx + 0x3ea], cx
00445199  je         0x4451cd

// block 0044519b
0044519b  mov        eax, dword ptr [eax + 4]
0044519e  test       eax, eax
004451a0  jne        0x445190

// block 004451a2
004451a2  xor        eax, eax

// block 004451a4
004451a4  push       eax
004451a5  mov        ebx, 2
004451aa  call       0x461970

// block 004451af
004451af  lea        ecx, [ebx + 1]
004451b2  mov        eax, ebp
004451b4  call       0x43ef40

// block 004451b9
004451b9  lea        edx, [ebx + 5]
004451bc  call       0x453d90

// block 004451c1
004451c1  pop        edi
004451c2  pop        ebx
004451c3  pop        esi
004451c4  mov        eax, 1
004451c9  pop        ebp
004451ca  ret        4

// block 004451cd
004451cd  mov        eax, edx
004451cf  mov        eax, dword ptr [eax]
004451d1  jmp        0x4451a4

// block 004451d3
004451d3  cmp        dword ptr [ebp + 0x2b8], 0xe
004451da  jl         0x4452a4

// block 004451e0
004451e0  mov        esi, 0x6e
004451e5  mov        edi, ebp
004451e7  call       0x43efd0

// block 004451ec
004451ec  lea        edx, [esi - 0x68]
004451ef  mov        eax, ebp
004451f1  call       0x43eee0

// block 004451f6
004451f6  mov        eax, dword ptr [0x4b0ca8]
004451fb  cmp        eax, 4
004451fe  jge        0x445208

// block 00445200
00445200  mov        eax, dword ptr [ebp + 0x28]
00445203  mov        dword ptr [0x4b0ca8], eax

// block 00445208
00445208  mov        dword ptr [0x4aebd0], eax
0044520d  lea        eax, [ebp + 0x28]
00445210  call       0x464900

// block 00445215
00445215  mov        esi, dword ptr [0x4ce8bc]
0044521b  mov        ecx, esi
0044521d  mov        edx, eax
0044521f  mov        dword ptr [ebp + 0xf8], 1
00445229  mov        dword ptr [ebp + 0x30], 3
00445230  call       0x40f790

// block 00445235
00445235  pop        edi
00445236  pop        ebx
00445237  mov        dword ptr [0x4b0c90], esi
0044523d  pop        esi
0044523e  mov        eax, 1
00445243  pop        ebp
00445244  ret        4

// block 00445247
00445247  cmp        dword ptr [ebp + 0x2b8], 6
0044524e  jl         0x4452a4

// block 00445250
00445250  mov        esi, 0x6e
00445255  mov        edi, ebp
00445257  call       0x43efd0

// block 0044525c
0044525c  mov        ecx, dword ptr [ebp + 0x66c]
00445262  push       ecx
00445263  lea        ebx, [esi - 0x6d]
00445266  call       0x461970

// block 0044526b
0044526b  mov        edx, ebx
0044526d  mov        eax, ebp
0044526f  mov        dword ptr [ebp + 0x66c], 0
00445279  call       0x43eee0

// block 0044527e
0044527e  cmp        dword ptr [0x4b0ca8], 4
00445285  jge        0x44528c

// block 00445287
00445287  mov        eax, dword ptr [ebp + 0x28]
0044528a  jmp        0x445292

// block 0044528c
0044528c  mov        eax, dword ptr [ebp + 0x598c]

// block 00445292
00445292  mov        dword ptr [0x4b0ca8], eax
00445297  mov        dword ptr [0x4aebd0], eax
0044529c  lea        eax, [ebp + 0x28]
0044529f  call       0x464940

// block 004452a4
004452a4  pop        edi
004452a5  pop        ebx

// block 004452a6
004452a6  pop        esi
004452a7  mov        eax, 1
004452ac  pop        ebp
004452ad  ret        4
