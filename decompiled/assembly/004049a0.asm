
// block 004049a0
004049a0  push       ebp
004049a1  mov        ebp, esp
004049a3  and        esp, 0xfffffff8
004049a6  push       -1
004049a8  push       0x49754c
004049ad  mov        eax, dword ptr fs:[0]
004049b3  push       eax
004049b4  sub        esp, 0xa0
004049ba  push       ebx
004049bb  push       esi
004049bc  push       edi
004049bd  mov        eax, dword ptr [0x4ad138]
004049c2  xor        eax, esp
004049c4  push       eax
004049c5  lea        eax, [esp + 0xb0]
004049cc  mov        dword ptr fs:[0], eax
004049d2  mov        ebx, dword ptr [ebp + 8]
004049d5  mov        eax, dword ptr [ebx + 0x3c]
004049d8  mov        ecx, dword ptr [ebx + 0x4c]
004049db  cmp        dword ptr [ecx], eax
004049dd  jg         0x40522d

// block 004049e3
004049e3  fldz       

// block 004049e5
004049e5  mov        esi, dword ptr [ebx + 0x4c]
004049e8  movsx      eax, word ptr [esi + 4]
004049ec  cmp        eax, 0x12
004049ef  ja         0x405211

// block 004049f5
004049f5  jmp        dword ptr [eax*4 + 0x405568]

// block 004049fc
004049fc  mov        eax, dword ptr [ebx + 0x48]
004049ff  mov        esi, dword ptr [esi + 0xc]
00404a02  mov        dword ptr [esp + 0x10], esi
00404a06  test       al, 1
00404a08  jne        0x404a28

// block 00404a0a
00404a0a  or         eax, 1
00404a0d  fst        dword ptr [ebx + 0x40]
00404a10  mov        dword ptr [ebx + 0x3c], 0
00404a17  mov        dword ptr [ebx + 0x38], 0xfff0bdc1
00404a1e  mov        dword ptr [ebx + 0x44], 0x4b2ed0
00404a25  mov        dword ptr [ebx + 0x48], eax

// block 00404a28
00404a28  fild       dword ptr [esp + 0x10]
00404a2c  mov        dword ptr [ebx + 0x3c], esi
00404a2f  dec        esi
00404a30  mov        dword ptr [ebx + 0x38], esi
00404a33  fstp       dword ptr [ebx + 0x40]
00404a36  mov        edx, dword ptr [ebx + 0x4c]
00404a39  mov        eax, dword ptr [edx + 8]
00404a3c  add        eax, dword ptr [ebx + 0x1c]
00404a3f  mov        dword ptr [ebx + 0x4c], eax
00404a42  jmp        0x40521d

// block 00404a47
00404a47  mov        ecx, dword ptr [ebx + 0x360c]
00404a4d  mov        edx, dword ptr [ebx + 0x3610]
00404a53  mov        eax, dword ptr [ebx + 0x3614]
00404a59  mov        dword ptr [ebx + 0x36fc], ecx
00404a5f  mov        dword ptr [ebx + 0x3700], edx
00404a65  mov        dword ptr [ebx + 0x3704], eax
00404a6b  fld        dword ptr [esi + 8]
00404a6e  fstp       dword ptr [ebx + 0x360c]
00404a74  fld        dword ptr [esi + 0xc]
00404a77  fstp       dword ptr [ebx + 0x3610]
00404a7d  fld        dword ptr [esi + 0x10]
00404a80  fstp       dword ptr [ebx + 0x3614]
00404a86  fld        dword ptr [ebx + 0x360c]
00404a8c  fsub       dword ptr [ebx + 0x36fc]
00404a92  fstp       dword ptr [esp + 0x54]
00404a96  mov        ecx, dword ptr [esp + 0x54]
00404a9a  fld        dword ptr [ebx + 0x3610]
00404aa0  fsub       dword ptr [ebx + 0x3700]
00404aa6  fstp       dword ptr [esp + 0x58]
00404aaa  mov        edx, dword ptr [esp + 0x58]
00404aae  fld        dword ptr [ebx + 0x3614]
00404ab4  fsub       dword ptr [ebx + 0x3704]
00404aba  mov        dword ptr [ebx + 0x36fc], ecx
00404ac0  mov        dword ptr [ebx + 0x3700], edx
00404ac6  fstp       dword ptr [esp + 0x5c]
00404aca  mov        eax, dword ptr [esp + 0x5c]
00404ace  mov        dword ptr [ebx + 0x3704], eax
00404ad4  jmp        0x405211

// block 00404ad9
00404ad9  fld        dword ptr [esi + 0x10]
00404adc  mov        ecx, dword ptr [esi + 8]
00404adf  mov        eax, dword ptr [ebx + 0x360c]
00404ae5  fstp       dword ptr [esp + 0x78]
00404ae9  fld        dword ptr [esi + 0x14]
00404aec  fstp       dword ptr [esp + 0x7c]
00404af0  fld        dword ptr [esi + 0x18]
00404af3  mov        dword ptr [ebx + 0xe0], ecx
00404af9  mov        edx, dword ptr [esi + 0xc]
00404afc  fstp       dword ptr [esp + 0x80]
00404b03  mov        ecx, dword ptr [ebx + 0x3610]
00404b09  mov        dword ptr [ebx + 0xe4], edx
00404b0f  mov        edx, dword ptr [ebx + 0x3614]
00404b15  mov        dword ptr [ebx + 0x9c], eax
00404b1b  mov        eax, dword ptr [esp + 0x78]
00404b1f  mov        dword ptr [ebx + 0xa0], ecx
00404b25  mov        ecx, dword ptr [esp + 0x7c]
00404b29  mov        dword ptr [ebx + 0xa4], edx
00404b2f  mov        edx, dword ptr [esp + 0x80]
00404b36  mov        dword ptr [ebx + 0xa8], eax
00404b3c  mov        dword ptr [ebx + 0xac], ecx
00404b42  mov        dword ptr [ebx + 0xb0], edx

// block 00404b48
00404b48  mov        eax, dword ptr [ebx + 0xdc]
00404b4e  test       al, 1
00404b50  jne        0x404b7f

// block 00404b52
00404b52  mov        dword ptr [ebx + 0xd0], 0
00404b5c  mov        dword ptr [ebx + 0xcc], 0xfff0bdc1
00404b66  fst        dword ptr [ebx + 0xd4]
00404b6c  or         eax, 1
00404b6f  mov        dword ptr [ebx + 0xd8], 0x4b2ed0
00404b79  mov        dword ptr [ebx + 0xdc], eax

// block 00404b7f
00404b7f  mov        dword ptr [ebx + 0xd0], 0
00404b89  fst        dword ptr [ebx + 0xd4]
00404b8f  mov        dword ptr [ebx + 0xcc], 0xffffffff
00404b99  jmp        0x405211

// block 00404b9e
00404b9e  fld        dword ptr [esi + 8]
00404ba1  fstp       dword ptr [ebx + 0x3618]
00404ba7  fld        dword ptr [esi + 0xc]
00404baa  fstp       dword ptr [ebx + 0x361c]
00404bb0  fld        dword ptr [esi + 0x10]
00404bb3  fstp       dword ptr [ebx + 0x3620]
00404bb9  jmp        0x405211

// block 00404bbe
00404bbe  fld        dword ptr [esi + 0x10]
00404bc1  mov        eax, dword ptr [esi + 8]
00404bc4  mov        edx, dword ptr [ebx + 0x3618]
00404bca  fstp       dword ptr [esp + 0x3c]
00404bce  fld        dword ptr [esi + 0x14]
00404bd1  fstp       dword ptr [esp + 0x40]
00404bd5  fld        dword ptr [esi + 0x18]
00404bd8  mov        dword ptr [ebx + 0x94], eax
00404bde  mov        ecx, dword ptr [esi + 0xc]
00404be1  fstp       dword ptr [esp + 0x44]
00404be5  mov        eax, dword ptr [ebx + 0x361c]
00404beb  mov        dword ptr [ebx + 0x98], ecx
00404bf1  mov        ecx, dword ptr [ebx + 0x3620]
00404bf7  mov        dword ptr [ebx + 0x50], edx
00404bfa  mov        edx, dword ptr [esp + 0x3c]
00404bfe  mov        dword ptr [ebx + 0x54], eax
00404c01  mov        eax, dword ptr [esp + 0x40]
00404c05  mov        dword ptr [ebx + 0x58], ecx
00404c08  mov        ecx, dword ptr [esp + 0x44]
00404c0c  mov        dword ptr [ebx + 0x5c], edx
00404c0f  mov        dword ptr [ebx + 0x60], eax
00404c12  mov        dword ptr [ebx + 0x64], ecx

// block 00404c15
00404c15  mov        eax, dword ptr [ebx + 0x90]
00404c1b  test       al, 1
00404c1d  jne        0x404c4c

// block 00404c1f
00404c1f  mov        dword ptr [ebx + 0x84], 0
00404c29  mov        dword ptr [ebx + 0x80], 0xfff0bdc1
00404c33  fst        dword ptr [ebx + 0x88]
00404c39  or         eax, 1
00404c3c  mov        dword ptr [ebx + 0x8c], 0x4b2ed0
00404c46  mov        dword ptr [ebx + 0x90], eax

// block 00404c4c
00404c4c  mov        dword ptr [ebx + 0x84], 0
00404c56  fst        dword ptr [ebx + 0x88]
00404c5c  mov        dword ptr [ebx + 0x80], 0xffffffff
00404c66  jmp        0x405211

// block 00404c6b
00404c6b  fld        dword ptr [esi + 8]
00404c6e  fstp       dword ptr [ebx + 0x3624]
00404c74  fld        dword ptr [esi + 0xc]
00404c77  fstp       dword ptr [ebx + 0x3628]
00404c7d  fld        dword ptr [esi + 0x10]
00404c80  fstp       dword ptr [ebx + 0x362c]
00404c86  jmp        0x405211

// block 00404c8b
00404c8b  fld        dword ptr [esi + 0x10]
00404c8e  mov        edx, dword ptr [esi + 8]
00404c91  mov        ecx, dword ptr [ebx + 0x3624]
00404c97  fstp       dword ptr [esp + 0x6c]
00404c9b  fld        dword ptr [esi + 0x14]
00404c9e  fstp       dword ptr [esp + 0x70]
00404ca2  fld        dword ptr [esi + 0x18]
00404ca5  mov        dword ptr [ebx + 0x12c], edx
00404cab  mov        eax, dword ptr [esi + 0xc]
00404cae  fstp       dword ptr [esp + 0x74]
00404cb2  mov        edx, dword ptr [ebx + 0x3628]
00404cb8  mov        dword ptr [ebx + 0x130], eax
00404cbe  mov        eax, dword ptr [ebx + 0x362c]
00404cc4  mov        dword ptr [ebx + 0xe8], ecx
00404cca  mov        ecx, dword ptr [esp + 0x6c]
00404cce  mov        dword ptr [ebx + 0xec], edx
00404cd4  mov        edx, dword ptr [esp + 0x70]
00404cd8  mov        dword ptr [ebx + 0xf0], eax
00404cde  mov        eax, dword ptr [esp + 0x74]
00404ce2  mov        dword ptr [ebx + 0xf4], ecx
00404ce8  mov        dword ptr [ebx + 0xf8], edx
00404cee  mov        dword ptr [ebx + 0xfc], eax
00404cf4  mov        eax, dword ptr [ebx + 0x128]
00404cfa  test       al, 1
00404cfc  jne        0x404d2b

// block 00404cfe
00404cfe  or         eax, 1
00404d01  fst        dword ptr [ebx + 0x120]
00404d07  mov        dword ptr [ebx + 0x11c], 0
00404d11  mov        dword ptr [ebx + 0x118], 0xfff0bdc1
00404d1b  mov        dword ptr [ebx + 0x124], 0x4b2ed0
00404d25  mov        dword ptr [ebx + 0x128], eax

// block 00404d2b
00404d2b  fst        dword ptr [ebx + 0x120]
00404d31  mov        dword ptr [ebx + 0x11c], 0
00404d3b  mov        dword ptr [ebx + 0x118], 0xffffffff
00404d45  jmp        0x405211

// block 00404d4a
00404d4a  fld        dword ptr [esi + 8]
00404d4d  fstp       dword ptr [ebx + 0x3654]
00404d53  jmp        0x405211

// block 00404d58
00404d58  mov        ecx, dword ptr [esi + 8]
00404d5b  movzx      edx, cl
00404d5e  mov        dword ptr [ebx + 0x3720], ecx
00404d64  movzx      eax, byte ptr [ebx + 0x3721]
00404d6b  movzx      ecx, byte ptr [ebx + 0x3722]
00404d72  mov        dword ptr [esp + 0x10], edx
00404d76  movzx      edx, byte ptr [ebx + 0x3723]
00404d7d  fild       dword ptr [esp + 0x10]
00404d81  mov        dword ptr [esp + 0x10], eax
00404d85  fstp       dword ptr [ebx + 0x3710]
00404d8b  fild       dword ptr [esp + 0x10]
00404d8f  mov        dword ptr [esp + 0x10], ecx
00404d93  fstp       dword ptr [ebx + 0x3714]
00404d99  fild       dword ptr [esp + 0x10]
00404d9d  mov        dword ptr [esp + 0x10], edx
00404da1  fstp       dword ptr [ebx + 0x3718]
00404da7  fild       dword ptr [esp + 0x10]
00404dab  fstp       dword ptr [ebx + 0x371c]
00404db1  fld        dword ptr [esi + 0xc]
00404db4  fstp       dword ptr [ebx + 0x3708]
00404dba  fld        dword ptr [esi + 0x10]
00404dbd  fstp       dword ptr [ebx + 0x370c]
00404dc3  jmp        0x405211

// block 00404dc8
00404dc8  movzx      eax, byte ptr [esi + 0x10]
00404dcc  fstp       st(0)
00404dce  fld        dword ptr [esi + 0x14]
00404dd1  movzx      ecx, byte ptr [esi + 0x11]
00404dd5  fstp       dword ptr [esp + 0x90]
00404ddc  fld        dword ptr [esi + 0x18]
00404ddf  movzx      edx, byte ptr [esi + 0x12]
00404de3  fstp       dword ptr [esp + 0x94]
00404dea  mov        dword ptr [esp + 0x10], eax
00404dee  movzx      eax, byte ptr [esi + 0x13]
00404df2  fild       dword ptr [esp + 0x10]
00404df6  mov        dword ptr [esp + 0x10], ecx
00404dfa  fstp       dword ptr [esp + 0x98]
00404e01  lea        ecx, [esp + 0x90]
00404e08  fild       dword ptr [esp + 0x10]
00404e0c  mov        dword ptr [esp + 0x10], edx
00404e10  fstp       dword ptr [esp + 0x9c]
00404e17  fild       dword ptr [esp + 0x10]
00404e1b  mov        dword ptr [esp + 0x10], eax
00404e1f  fstp       dword ptr [esp + 0xa0]
00404e26  fild       dword ptr [esp + 0x10]
00404e2a  fstp       dword ptr [esp + 0xa4]
00404e31  call       0x406250

// block 00404e36
00404e36  mov        ecx, dword ptr [esi + 8]
00404e39  fldz       
00404e3b  mov        dword ptr [ebx + 0x1b8], ecx
00404e41  mov        edx, dword ptr [esi + 0xc]
00404e44  lea        esi, [ebx + 0x3708]
00404e4a  lea        edi, [ebx + 0x134]
00404e50  mov        ecx, 7

// block 00404e55
00404e55  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00404e57
00404e57  lea        edi, [ebx + 0x150]
00404e5d  mov        ecx, 7
00404e62  lea        esi, [esp + 0x90]
00404e69  mov        dword ptr [ebx + 0x1bc], edx

// block 00404e6f
00404e6f  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00404e71
00404e71  mov        eax, dword ptr [ebx + 0x1b4]
00404e77  test       al, 1
00404e79  jne        0x404ea8

// block 00404e7b
00404e7b  or         eax, 1
00404e7e  fst        dword ptr [ebx + 0x1ac]
00404e84  mov        dword ptr [ebx + 0x1a8], 0
00404e8e  mov        dword ptr [ebx + 0x1a4], 0xfff0bdc1
00404e98  mov        dword ptr [ebx + 0x1b0], 0x4b2ed0
00404ea2  mov        dword ptr [ebx + 0x1b4], eax

// block 00404ea8
00404ea8  fst        dword ptr [ebx + 0x1ac]
00404eae  mov        dword ptr [ebx + 0x1a8], 0
00404eb8  mov        dword ptr [ebx + 0x1a4], 0xffffffff
00404ec2  jmp        0x405211

// block 00404ec7
00404ec7  fld        dword ptr [esi + 0x10]
00404eca  mov        eax, dword ptr [esi + 8]
00404ecd  fstp       dword ptr [esp + 0x24]
00404ed1  mov        ecx, dword ptr [ebx + 0x360c]
00404ed7  fld        dword ptr [esi + 0x14]
00404eda  mov        edx, dword ptr [ebx + 0x3610]
00404ee0  fstp       dword ptr [esp + 0x28]
00404ee4  fld        dword ptr [esi + 0x18]
00404ee7  fstp       dword ptr [esp + 0x2c]
00404eeb  fld        dword ptr [esi + 0x1c]
00404eee  fstp       dword ptr [esp + 0x84]
00404ef5  fld        dword ptr [esi + 0x20]
00404ef8  fstp       dword ptr [esp + 0x88]
00404eff  fld        dword ptr [esi + 0x24]
00404f02  fstp       dword ptr [esp + 0x8c]
00404f09  fld        dword ptr [esi + 0x28]
00404f0c  fstp       dword ptr [esp + 0x30]
00404f10  fld        dword ptr [esi + 0x2c]
00404f13  fstp       dword ptr [esp + 0x34]
00404f17  fld        dword ptr [esi + 0x30]
00404f1a  mov        dword ptr [ebx + 0xe0], eax
00404f20  mov        eax, dword ptr [ebx + 0x3614]
00404f26  fstp       dword ptr [esp + 0x38]
00404f2a  mov        dword ptr [ebx + 0x9c], ecx
00404f30  mov        ecx, dword ptr [esp + 0x24]
00404f34  mov        dword ptr [ebx + 0xa0], edx
00404f3a  mov        edx, dword ptr [esp + 0x28]
00404f3e  mov        dword ptr [ebx + 0xa4], eax
00404f44  mov        eax, dword ptr [esp + 0x2c]
00404f48  mov        dword ptr [ebx + 0xb4], ecx
00404f4e  mov        ecx, dword ptr [esp + 0x84]
00404f55  mov        dword ptr [ebx + 0xb8], edx
00404f5b  mov        edx, dword ptr [esp + 0x88]
00404f62  mov        dword ptr [ebx + 0xbc], eax
00404f68  mov        eax, dword ptr [esp + 0x8c]
00404f6f  mov        dword ptr [ebx + 0xa8], ecx
00404f75  mov        ecx, dword ptr [esp + 0x30]
00404f79  mov        dword ptr [ebx + 0xac], edx
00404f7f  mov        edx, dword ptr [esp + 0x34]
00404f83  mov        dword ptr [ebx + 0xb0], eax
00404f89  mov        eax, dword ptr [esp + 0x38]
00404f8d  mov        dword ptr [ebx + 0xc0], ecx
00404f93  mov        dword ptr [ebx + 0xc4], edx
00404f99  mov        dword ptr [ebx + 0xe4], 8
00404fa3  mov        dword ptr [ebx + 0xc8], eax
00404fa9  jmp        0x404b48

// block 00404fae
00404fae  fld        dword ptr [esi + 0x10]
00404fb1  mov        ecx, dword ptr [esi + 8]
00404fb4  fstp       dword ptr [esp + 0x48]
00404fb8  mov        edx, dword ptr [ebx + 0x3618]
00404fbe  fld        dword ptr [esi + 0x14]
00404fc1  mov        eax, dword ptr [ebx + 0x361c]
00404fc7  fstp       dword ptr [esp + 0x4c]
00404fcb  fld        dword ptr [esi + 0x18]
00404fce  fstp       dword ptr [esp + 0x50]
00404fd2  fld        dword ptr [esi + 0x1c]
00404fd5  fstp       dword ptr [esp + 0x60]
00404fd9  fld        dword ptr [esi + 0x20]
00404fdc  fstp       dword ptr [esp + 0x64]
00404fe0  fld        dword ptr [esi + 0x24]
00404fe3  fstp       dword ptr [esp + 0x68]
00404fe7  fld        dword ptr [esi + 0x28]
00404fea  fstp       dword ptr [esp + 0x18]
00404fee  fld        dword ptr [esi + 0x2c]
00404ff1  fstp       dword ptr [esp + 0x1c]
00404ff5  fld        dword ptr [esi + 0x30]
00404ff8  mov        dword ptr [ebx + 0x94], ecx
00404ffe  mov        ecx, dword ptr [ebx + 0x3620]
00405004  fstp       dword ptr [esp + 0x20]
00405008  mov        dword ptr [ebx + 0x50], edx
0040500b  mov        edx, dword ptr [esp + 0x48]
0040500f  mov        dword ptr [ebx + 0x54], eax
00405012  mov        eax, dword ptr [esp + 0x4c]
00405016  mov        dword ptr [ebx + 0x58], ecx
00405019  mov        ecx, dword ptr [esp + 0x50]
0040501d  mov        dword ptr [ebx + 0x68], edx
00405020  mov        edx, dword ptr [esp + 0x60]
00405024  mov        dword ptr [ebx + 0x6c], eax
00405027  mov        eax, dword ptr [esp + 0x64]
0040502b  mov        dword ptr [ebx + 0x70], ecx
0040502e  mov        ecx, dword ptr [esp + 0x68]
00405032  mov        dword ptr [ebx + 0x5c], edx
00405035  mov        edx, dword ptr [esp + 0x18]
00405039  mov        dword ptr [ebx + 0x60], eax
0040503c  mov        eax, dword ptr [esp + 0x1c]
00405040  mov        dword ptr [ebx + 0x64], ecx
00405043  mov        ecx, dword ptr [esp + 0x20]
00405047  mov        dword ptr [ebx + 0x74], edx
0040504a  mov        dword ptr [ebx + 0x78], eax
0040504d  mov        dword ptr [ebx + 0x98], 8
00405057  mov        dword ptr [ebx + 0x7c], ecx
0040505a  jmp        0x404c15

// block 0040505f
0040505f  mov        al, byte ptr [esi + 8]
00405062  xor        ecx, ecx
00405064  mov        byte ptr [ebx + 0x20], al
00405067  cmp        al, cl
00405069  jne        0x40507d

// block 0040506b
0040506b  fst        dword ptr [ebx + 0x3648]
00405071  fst        dword ptr [ebx + 0x364c]
00405077  fst        dword ptr [ebx + 0x3650]

// block 0040507d
0040507d  mov        eax, dword ptr [ebx + 0x34]
00405080  test       al, 1
00405082  jne        0x40509e

// block 00405084
00405084  or         eax, 1
00405087  fst        dword ptr [ebx + 0x2c]
0040508a  mov        dword ptr [ebx + 0x28], ecx
0040508d  mov        dword ptr [ebx + 0x24], 0xfff0bdc1
00405094  mov        dword ptr [ebx + 0x30], 0x4b2ed0
0040509b  mov        dword ptr [ebx + 0x34], eax

// block 0040509e
0040509e  fst        dword ptr [ebx + 0x2c]
004050a1  mov        dword ptr [ebx + 0x28], ecx
004050a4  mov        dword ptr [ebx + 0x24], 0xffffffff
004050ab  jmp        0x405211

// block 004050b0
004050b0  mov        edx, dword ptr [esi + 8]
004050b3  mov        dword ptr [0x4cf2a8], edx
004050b9  jmp        0x405211

// block 004050be
004050be  mov        eax, dword ptr [esi + 0xc]
004050c1  mov        dword ptr [esp + 0x10], eax
004050c5  test       eax, eax
004050c7  jl         0x405106

// block 004050c9
004050c9  mov        eax, dword ptr [esi + 8]
004050cc  fstp       st(0)
004050ce  mov        edi, dword ptr [ebx + 0x1c4]
004050d4  imul       eax, eax, 0x4b4
004050da  lea        esi, [eax + ebx + 0x1cc]
004050e1  call       0x402520

// block 004050e6
004050e6  mov        ecx, dword ptr [esp + 0x10]
004050ea  push       ecx
004050eb  push       esi
004050ec  mov        ecx, edi
004050ee  mov        byte ptr [esi + 0x49d], 0x10
004050f5  mov        byte ptr [esi + 0x49c], 0x10
004050fc  call       0x454d10

// block 00405101
00405101  jmp        0x40520f

// block 00405106
00405106  mov        edx, dword ptr [esi + 8]
00405109  imul       edx, edx, 0x4b4
0040510f  and        dword ptr [edx + ebx + 0x648], 0xfffffffe
00405117  lea        eax, [edx + ebx + 0x1cc]
0040511e  jmp        0x405211

// block 00405123
00405123  mov        esi, dword ptr [ebx + 0x35dc]
00405129  xor        edi, edi
0040512b  cmp        esi, edi
0040512d  je         0x405141

// block 0040512f
0040512f  fstp       st(0)
00405131  call       0x402870

// block 00405136
00405136  push       esi
00405137  call       0x46ca4f

// block 0040513c
0040513c  fldz       
0040513e  add        esp, 4

// block 00405141
00405141  fld        dword ptr [0x4a3f28]
00405147  push       ecx
00405148  fstp       dword ptr [ebx + 0x35e0]
0040514e  mov        dword ptr [ebx + 0x35dc], edi
00405154  fld        dword ptr [0x4a3d88]
0040515a  mov        dword ptr [ebx + 0x35e8], 0xffffffff
00405164  fstp       dword ptr [ebx + 0x35e4]
0040516a  fstp       dword ptr [esp]
0040516d  call       0x4646e0

// block 00405172
00405172  fstp       dword ptr [esp + 0x10]
00405176  fld        dword ptr [esp + 0x10]
0040517a  fst        dword ptr [ebx + 0x35ec]
00405180  fstp       dword ptr [ebx + 0x35f0]
00405186  mov        eax, dword ptr [ebx + 0x4c]
00405189  fldz       
0040518b  mov        ecx, dword ptr [eax + 8]
0040518e  fcom       dword ptr [ebx + 0x35e0]
00405194  mov        dword ptr [ebx + 0x35f4], ecx
0040519a  fnstsw     ax
0040519c  test       ah, 5
0040519f  jp         0x405211

// block 004051a1
004051a1  fstp       st(0)
004051a3  push       0x18
004051a5  cmp        ecx, 1
004051a8  jne        0x4051d1

// block 004051aa
004051aa  call       0x46c9ea

// block 004051af
004051af  mov        esi, eax
004051b1  add        esp, 4
004051b4  mov        dword ptr [esp + 0x10], esi
004051b8  mov        dword ptr [esp + 0xb8], edi
004051bf  cmp        esi, edi
004051c1  je         0x4051fc

// block 004051c3
004051c3  push       7
004051c5  mov        eax, 0x11
004051ca  call       0x40ff10

// block 004051cf
004051cf  jmp        0x4051fe

// block 004051d1
004051d1  call       0x46c9ea

// block 004051d6
004051d6  mov        esi, eax
004051d8  add        esp, 4
004051db  mov        dword ptr [esp + 0x10], esi
004051df  mov        dword ptr [esp + 0xb8], 1
004051ea  cmp        esi, edi
004051ec  je         0x4051fc

// block 004051ee
004051ee  push       0x11
004051f0  mov        eax, 0x11
004051f5  call       0x40ff10

// block 004051fa
004051fa  jmp        0x4051fe

// block 004051fc
004051fc  xor        eax, eax

// block 004051fe
004051fe  mov        dword ptr [esp + 0xb8], 0xffffffff
00405209  mov        dword ptr [ebx + 0x35dc], eax

// block 0040520f
0040520f  fldz       

// block 00405211
00405211  mov        eax, dword ptr [ebx + 0x4c]
00405214  movsx      ecx, word ptr [eax + 6]
00405218  add        ecx, eax
0040521a  mov        dword ptr [ebx + 0x4c], ecx

// block 0040521d
0040521d  mov        edx, dword ptr [ebx + 0x4c]
00405220  mov        eax, dword ptr [edx]
00405222  cmp        eax, dword ptr [ebx + 0x3c]
00405225  jle        0x4049e5

// block 0040522b
0040522b  fstp       st(0)

// block 0040522d
0040522d  lea        esi, [ebx + 0x38]
00405230  call       0x464a80

// block 00405235
00405235  jmp        0x405239

// block 00405237
00405237  fstp       st(0)

// block 00405239
00405239  cmp        dword ptr [ebx + 0x94], 0
00405240  je         0x40526d

// block 00405242
00405242  lea        edi, [ebx + 0x50]
00405245  lea        ebx, [esp + 0x18]
00405249  call       0x405900

// block 0040524e
0040524e  mov        edx, dword ptr [eax]
00405250  mov        ecx, dword ptr [ebp + 8]
00405253  mov        dword ptr [ecx + 0x3618], edx
00405259  mov        edx, dword ptr [eax + 4]
0040525c  mov        dword ptr [ecx + 0x361c], edx
00405262  mov        eax, dword ptr [eax + 8]
00405265  mov        dword ptr [ecx + 0x3620], eax
0040526b  mov        ebx, ecx

// block 0040526d
0040526d  cmp        dword ptr [ebx + 0xe0], 0
00405274  je         0x4052a4

// block 00405276
00405276  lea        edi, [ebx + 0x9c]
0040527c  lea        ebx, [esp + 0x18]
00405280  call       0x405900

// block 00405285
00405285  mov        edx, dword ptr [eax]
00405287  mov        ecx, dword ptr [ebp + 8]
0040528a  mov        dword ptr [ecx + 0x360c], edx
00405290  mov        edx, dword ptr [eax + 4]
00405293  mov        dword ptr [ecx + 0x3610], edx
00405299  mov        eax, dword ptr [eax + 8]
0040529c  mov        dword ptr [ecx + 0x3614], eax
004052a2  mov        ebx, ecx

// block 004052a4
004052a4  cmp        dword ptr [ebx + 0x1b8], 0
004052ab  je         0x4052d5

// block 004052ad
004052ad  lea        ecx, [esp + 0x90]
004052b4  push       ecx
004052b5  add        ebx, 0x134
004052bb  call       0x405c90

// block 004052c0
004052c0  mov        edi, dword ptr [ebp + 8]
004052c3  mov        ebx, dword ptr [ebp + 8]
004052c6  add        edi, 0x3708
004052cc  mov        ecx, 7
004052d1  mov        esi, eax

// block 004052d3
004052d3  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 004052d5
004052d5  cmp        dword ptr [ebx + 0x12c], 0
004052dc  je         0x40530c

// block 004052de
004052de  lea        edi, [ebx + 0xe8]
004052e4  lea        ebx, [esp + 0x18]
004052e8  call       0x405900

// block 004052ed
004052ed  mov        edx, dword ptr [eax]
004052ef  mov        ecx, dword ptr [ebp + 8]
004052f2  mov        dword ptr [ecx + 0x3624], edx
004052f8  mov        edx, dword ptr [eax + 4]
004052fb  mov        dword ptr [ecx + 0x3628], edx
00405301  mov        eax, dword ptr [eax + 8]
00405304  mov        dword ptr [ecx + 0x362c], eax
0040530a  mov        ebx, ecx

// block 0040530c
0040530c  mov        al, byte ptr [ebx + 0x20]
0040530f  test       al, al
00405311  je         0x40554e

// block 00405317
00405317  movzx      eax, al
0040531a  dec        eax
0040531b  cmp        eax, 6
0040531e  ja         0x40554e

// block 00405324
00405324  jmp        dword ptr [eax*4 + 0x4055b4]

// block 0040532b
0040532b  fld        dword ptr [ebx + 0x2c]
0040532e  push       ecx
0040532f  fmul       qword ptr [0x4a3cd8]
00405335  fadd       st(0), st(0)
00405337  fmul       qword ptr [0x4a4318]
0040533d  fstp       dword ptr [esp + 0x14]
00405341  fld        dword ptr [esp + 0x14]
00405345  fstp       dword ptr [esp]
00405348  call       0x4646e0

// block 0040534d
0040534d  fstp       dword ptr [esp + 0x10]
00405351  fld        dword ptr [esp + 0x10]
00405355  call       0x4939f0

// block 0040535a
0040535a  fstp       dword ptr [esp + 0x10]
0040535e  fld        dword ptr [esp + 0x10]
00405362  fsub       qword ptr [0x4a3ce0]
00405368  fld        qword ptr [0x4a4310]
0040536e  fmul       st(1)
00405370  fstp       dword ptr [ebx + 0x3648]
00405376  fmul       qword ptr [0x4a4308]
0040537c  jmp        0x40552e

// block 00405381
00405381  fld        dword ptr [ebx + 0x2c]
00405384  push       ecx
00405385  fmul       qword ptr [0x4a3cd8]
0040538b  fadd       st(0), st(0)
0040538d  fmul       qword ptr [0x4a4300]
00405393  fstp       dword ptr [esp + 0x14]
00405397  fld        dword ptr [esp + 0x14]
0040539b  fstp       dword ptr [esp]
0040539e  call       0x4646e0

// block 004053a3
004053a3  fstp       dword ptr [esp + 0x14]
004053a7  fld        dword ptr [esp + 0x14]
004053ab  call       0x4938c0

// block 004053b0
004053b0  fstp       dword ptr [esp + 0x10]
004053b4  fld        dword ptr [esp + 0x10]
004053b8  push       ecx
004053b9  fmul       qword ptr [0x4a3f88]
004053bf  fstp       dword ptr [ebx + 0x3648]
004053c5  fld        dword ptr [esp + 0x18]
004053c9  fadd       st(0), st(0)
004053cb  fstp       dword ptr [esp + 0x18]
004053cf  fld        dword ptr [esp + 0x18]
004053d3  fstp       dword ptr [esp]
004053d6  call       0x4646e0

// block 004053db
004053db  call       0x4938c0

// block 004053e0
004053e0  fstp       dword ptr [esp + 0x14]
004053e4  fld        dword ptr [esp + 0x14]
004053e8  fmul       qword ptr [0x4a3f88]
004053ee  fstp       dword ptr [ebx + 0x3650]
004053f4  fld        dword ptr [esp + 0x10]
004053f8  fchs       
004053fa  fmul       qword ptr [0x4a40e0]

// block 00405400
00405400  lea        esi, [ebx + 0x24]
00405403  fstp       dword ptr [ebx + 0x3624]
00405409  call       0x464a80

// block 0040540e
0040540e  cmp        dword ptr [ebx + 0x28], 0x800
00405415  jmp        0x405543

// block 0040541a
0040541a  fld        dword ptr [ebx + 0x2c]
0040541d  fld        qword ptr [0x4a3cd8]
00405423  fmul       st(1), st(0)
00405425  fxch       st(1)
00405427  fadd       st(0), st(0)
00405429  fmul       qword ptr [0x4a4300]
0040542f  fsubrp     st(1)
00405431  fstp       dword ptr [esp + 0x10]
00405435  fld        dword ptr [esp + 0x10]
00405439  call       0x4938c0

// block 0040543e
0040543e  fstp       dword ptr [esp + 0x14]
00405442  fld        dword ptr [esp + 0x14]
00405446  push       ecx
00405447  fmul       qword ptr [0x4a42f8]
0040544d  fstp       dword ptr [ebx + 0x3648]
00405453  fld        dword ptr [esp + 0x14]
00405457  fadd       st(0), st(0)
00405459  fstp       dword ptr [esp + 0x14]
0040545d  fld        dword ptr [esp + 0x14]
00405461  fstp       dword ptr [esp]
00405464  call       0x4646e0

// block 00405469
00405469  call       0x4938c0

// block 0040546e
0040546e  fstp       dword ptr [esp + 0x10]
00405472  fld        dword ptr [esp + 0x10]
00405476  fmul       qword ptr [0x4a42f0]
0040547c  fstp       dword ptr [ebx + 0x3650]
00405482  fld        dword ptr [esp + 0x14]
00405486  fchs       
00405488  fmul       qword ptr [0x4a3e28]
0040548e  jmp        0x405400

// block 00405493
00405493  fld        dword ptr [ebx + 0x2c]
00405496  fld        qword ptr [0x4a3cd8]
0040549c  fmul       st(1), st(0)
0040549e  fxch       st(1)
004054a0  fadd       st(0), st(0)
004054a2  fmul       qword ptr [0x4a42e8]
004054a8  fsubrp     st(1)
004054aa  fstp       dword ptr [esp + 0x10]
004054ae  fld        dword ptr [esp + 0x10]
004054b2  call       0x4938c0

// block 004054b7
004054b7  fstp       dword ptr [esp + 0x10]
004054bb  fld        dword ptr [esp + 0x10]
004054bf  lea        esi, [ebx + 0x24]
004054c2  fld        qword ptr [0x4a4310]
004054c8  fmul       st(1)
004054ca  fstp       dword ptr [ebx + 0x3648]
004054d0  fchs       
004054d2  fmul       qword ptr [0x4a3e28]
004054d8  fstp       dword ptr [ebx + 0x3624]
004054de  call       0x464a80

// block 004054e3
004054e3  cmp        dword ptr [ebx + 0x28], 0x400
004054ea  jmp        0x405543

// block 004054ec
004054ec  fld        dword ptr [ebx + 0x2c]
004054ef  fld        qword ptr [0x4a3cd8]
004054f5  fmul       st(1), st(0)
004054f7  fxch       st(1)
004054f9  fadd       st(0), st(0)
004054fb  fmul       qword ptr [0x4a4318]
00405501  fsubrp     st(1)
00405503  fstp       dword ptr [esp + 0x10]
00405507  fld        dword ptr [esp + 0x10]
0040550b  call       0x4938c0

// block 00405510
00405510  fstp       dword ptr [esp + 0x10]
00405514  fld        dword ptr [esp + 0x10]
00405518  fld        qword ptr [0x4a42e0]
0040551e  fmul       st(1)
00405520  fstp       dword ptr [ebx + 0x3648]
00405526  fchs       
00405528  fmul       qword ptr [0x4a42d8]

// block 0040552e
0040552e  lea        esi, [ebx + 0x24]
00405531  fstp       dword ptr [ebx + 0x3624]
00405537  call       0x464a80

// block 0040553c
0040553c  cmp        dword ptr [ebx + 0x28], 0x200

// block 00405543
00405543  jl         0x40554e

// block 00405545
00405545  push       0
00405547  mov        eax, esi
00405549  call       0x4067e0

// block 0040554e
0040554e  xor        eax, eax
00405550  mov        ecx, dword ptr [esp + 0xb0]
00405557  mov        dword ptr fs:[0], ecx
0040555e  pop        ecx
0040555f  pop        edi
00405560  pop        esi
00405561  pop        ebx
00405562  mov        esp, ebp
00405564  pop        ebp
00405565  ret        4
