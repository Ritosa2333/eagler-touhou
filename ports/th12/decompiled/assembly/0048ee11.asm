
// block 0048ee11
0048ee11  mov        edi, edi
0048ee13  push       ebp
0048ee14  mov        ebp, esp
0048ee16  sub        esp, 0x74
0048ee19  mov        eax, dword ptr [0x4ad138]
0048ee1e  xor        eax, ebp
0048ee20  mov        dword ptr [ebp - 4], eax
0048ee23  push       ebx
0048ee24  mov        ebx, dword ptr [ebp + 0x1c]
0048ee27  push       esi
0048ee28  push       edi
0048ee29  lea        esi, [ebp + 8]
0048ee2c  lea        edi, [ebp - 0x10]
0048ee2f  movsd      dword ptr es:[edi], dword ptr [esi]
0048ee30  movsd      dword ptr es:[edi], dword ptr [esi]
0048ee31  movsw      word ptr es:[edi], word ptr [esi]
0048ee33  mov        edx, dword ptr [ebp - 8]
0048ee36  mov        ecx, edx
0048ee38  mov        eax, 0x8000
0048ee3d  and        ecx, eax
0048ee3f  and        edx, 0x7fff
0048ee45  mov        dword ptr [ebp - 0x60], ebx
0048ee48  mov        byte ptr [ebp - 0x30], 0xcc
0048ee4c  mov        byte ptr [ebp - 0x2f], 0xcc
0048ee50  mov        byte ptr [ebp - 0x2e], 0xcc
0048ee54  mov        byte ptr [ebp - 0x2d], 0xcc
0048ee58  mov        byte ptr [ebp - 0x2c], 0xcc
0048ee5c  mov        byte ptr [ebp - 0x2b], 0xcc
0048ee60  mov        byte ptr [ebp - 0x2a], 0xcc
0048ee64  mov        byte ptr [ebp - 0x29], 0xcc
0048ee68  mov        byte ptr [ebp - 0x28], 0xcc
0048ee6c  mov        byte ptr [ebp - 0x27], 0xcc
0048ee70  mov        byte ptr [ebp - 0x26], 0xfb
0048ee74  mov        byte ptr [ebp - 0x25], 0x3f
0048ee78  mov        dword ptr [ebp - 0x74], 1
0048ee7f  mov        dword ptr [ebp - 0x70], ecx
0048ee82  test       cx, cx
0048ee85  je         0x48ee8d

// block 0048ee87
0048ee87  mov        byte ptr [ebx + 2], 0x2d
0048ee8b  jmp        0x48ee91

// block 0048ee8d
0048ee8d  mov        byte ptr [ebx + 2], 0x20

// block 0048ee91
0048ee91  mov        esi, dword ptr [ebp - 0xc]
0048ee94  mov        edi, dword ptr [ebp - 0x10]
0048ee97  test       dx, dx
0048ee9a  jne        0x48eecb

// block 0048ee9c
0048ee9c  test       esi, esi
0048ee9e  jne        0x48eecb

// block 0048eea0
0048eea0  test       edi, edi
0048eea2  jne        0x48eecb

// block 0048eea4
0048eea4  xor        edx, edx
0048eea6  cmp        cx, ax
0048eea9  setne      al
0048eeac  dec        al
0048eeae  and        al, 0xd
0048eeb0  add        al, 0x20
0048eeb2  mov        word ptr [ebx], dx
0048eeb5  mov        byte ptr [ebx + 2], al
0048eeb8  mov        byte ptr [ebx + 3], 1
0048eebc  mov        byte ptr [ebx + 4], 0x30
0048eec0  mov        byte ptr [ebx + 5], dl

// block 0048eec3
0048eec3  xor        eax, eax
0048eec5  inc        eax
0048eec6  jmp        0x48f6e9

// block 0048eecb
0048eecb  mov        eax, 0x7fff
0048eed0  cmp        dx, ax
0048eed3  jne        0x48ef78

// block 0048eed9
0048eed9  xor        eax, eax
0048eedb  inc        eax
0048eedc  mov        word ptr [ebx], ax
0048eedf  mov        eax, 0x80000000
0048eee4  cmp        esi, eax
0048eee6  jne        0x48eeec

// block 0048eee8
0048eee8  test       edi, edi
0048eeea  je         0x48eefb

// block 0048eeec
0048eeec  test       esi, 0x40000000
0048eef2  jne        0x48eefb

// block 0048eef4
0048eef4  push       0x49f378
0048eef9  jmp        0x48ef4c

// block 0048eefb
0048eefb  test       cx, cx
0048eefe  je         0x48ef13

// block 0048ef00
0048ef00  cmp        esi, 0xc0000000
0048ef06  jne        0x48ef13

// block 0048ef08
0048ef08  test       edi, edi
0048ef0a  jne        0x48ef47

// block 0048ef0c
0048ef0c  push       0x49f370
0048ef11  jmp        0x48ef20

// block 0048ef13
0048ef13  cmp        esi, eax
0048ef15  jne        0x48ef47

// block 0048ef17
0048ef17  test       edi, edi
0048ef19  jne        0x48ef47

// block 0048ef1b
0048ef1b  push       0x49f368

// block 0048ef20
0048ef20  lea        eax, [ebx + 4]
0048ef23  push       0x16
0048ef25  push       eax
0048ef26  call       0x47a2b9

// block 0048ef2b
0048ef2b  add        esp, 0xc
0048ef2e  xor        esi, esi
0048ef30  test       eax, eax
0048ef32  je         0x48ef41

// block 0048ef34
0048ef34  push       esi
0048ef35  push       esi
0048ef36  push       esi
0048ef37  push       esi
0048ef38  push       esi
0048ef39  call       0x470dc6

// block 0048ef3e
0048ef3e  add        esp, 0x14

// block 0048ef41
0048ef41  mov        byte ptr [ebx + 3], 5
0048ef45  jmp        0x48ef71

// block 0048ef47
0048ef47  push       0x49f360

// block 0048ef4c
0048ef4c  lea        eax, [ebx + 4]
0048ef4f  push       0x16
0048ef51  push       eax
0048ef52  call       0x47a2b9

// block 0048ef57
0048ef57  add        esp, 0xc
0048ef5a  xor        esi, esi
0048ef5c  test       eax, eax
0048ef5e  je         0x48ef6d

// block 0048ef60
0048ef60  push       esi
0048ef61  push       esi
0048ef62  push       esi
0048ef63  push       esi
0048ef64  push       esi
0048ef65  call       0x470dc6

// block 0048ef6a
0048ef6a  add        esp, 0x14

// block 0048ef6d
0048ef6d  mov        byte ptr [ebx + 3], 6

// block 0048ef71
0048ef71  xor        eax, eax
0048ef73  jmp        0x48f6e9

// block 0048ef78
0048ef78  movzx      ecx, dx
0048ef7b  mov        ebx, ecx
0048ef7d  imul       ecx, ecx, 0x4d10
0048ef83  shr        ebx, 8
0048ef86  mov        eax, esi
0048ef88  shr        eax, 0x18
0048ef8b  lea        eax, [ebx + eax*2]
0048ef8e  imul       eax, eax, 0x4d
0048ef91  lea        eax, [eax + ecx - 0x134312f4]
0048ef98  sar        eax, 0x10
0048ef9b  movzx      eax, ax
0048ef9e  xor        ecx, ecx
0048efa0  movsx      ebx, ax
0048efa3  mov        word ptr [ebp - 0x20], cx
0048efa7  mov        ecx, 0x4adfe0
0048efac  neg        ebx
0048efae  sub        ecx, 0x60
0048efb1  mov        dword ptr [ebp - 0x4c], eax
0048efb4  mov        word ptr [ebp - 0x16], dx
0048efb8  mov        dword ptr [ebp - 0x1a], esi
0048efbb  mov        dword ptr [ebp - 0x1e], edi
0048efbe  mov        dword ptr [ebp - 0x64], ecx
0048efc1  test       ebx, ebx
0048efc3  je         0x48f265

// block 0048efc9
0048efc9  jge        0x48efd8

// block 0048efcb
0048efcb  mov        eax, 0x4ae140
0048efd0  neg        ebx
0048efd2  sub        eax, 0x60
0048efd5  mov        dword ptr [ebp - 0x64], eax

// block 0048efd8
0048efd8  test       ebx, ebx
0048efda  je         0x48f265

// block 0048efe0
0048efe0  add        dword ptr [ebp - 0x64], 0x54
0048efe4  mov        ecx, ebx
0048efe6  and        ecx, 7
0048efe9  sar        ebx, 3
0048efec  test       ecx, ecx
0048efee  je         0x48f25b

// block 0048eff4
0048eff4  imul       ecx, ecx, 0xc
0048eff7  add        ecx, dword ptr [ebp - 0x64]
0048effa  mov        eax, ecx
0048effc  mov        dword ptr [ebp - 0x44], ecx
0048efff  mov        ecx, 0x8000
0048f004  cmp        word ptr [eax], cx
0048f007  jb         0x48f01a

// block 0048f009
0048f009  mov        esi, eax
0048f00b  lea        edi, [ebp - 0x3c]
0048f00e  movsd      dword ptr es:[edi], dword ptr [esi]
0048f00f  movsd      dword ptr es:[edi], dword ptr [esi]
0048f010  lea        eax, [ebp - 0x3c]
0048f013  movsd      dword ptr es:[edi], dword ptr [esi]
0048f014  dec        dword ptr [ebp - 0x3a]
0048f017  mov        dword ptr [ebp - 0x44], eax

// block 0048f01a
0048f01a  movzx      edx, word ptr [eax + 0xa]
0048f01e  xor        ecx, ecx
0048f020  mov        dword ptr [ebp - 0x54], ecx
0048f023  mov        dword ptr [ebp - 0x10], ecx
0048f026  mov        dword ptr [ebp - 0xc], ecx
0048f029  mov        dword ptr [ebp - 8], ecx
0048f02c  mov        ecx, dword ptr [ebp - 0x16]
0048f02f  mov        esi, edx
0048f031  xor        esi, ecx
0048f033  and        esi, 0x8000
0048f039  mov        dword ptr [ebp - 0x48], esi
0048f03c  mov        esi, 0x7fff
0048f041  and        ecx, esi
0048f043  and        edx, esi
0048f045  lea        esi, [edx + ecx]
0048f048  movzx      edi, si
0048f04b  mov        esi, 0x7fff
0048f050  cmp        cx, si
0048f053  jae        0x48f305

// block 0048f059
0048f059  cmp        dx, si
0048f05c  jae        0x48f305

// block 0048f062
0048f062  mov        esi, 0xbffd
0048f067  cmp        di, si
0048f06a  ja         0x48f305

// block 0048f070
0048f070  mov        esi, 0x3fbf
0048f075  cmp        di, si
0048f078  ja         0x48f08a

// block 0048f07a
0048f07a  xor        esi, esi

// block 0048f07c
0048f07c  mov        dword ptr [ebp - 0x18], esi

// block 0048f07f
0048f07f  mov        dword ptr [ebp - 0x1c], esi
0048f082  mov        dword ptr [ebp - 0x20], esi
0048f085  jmp        0x48f25d

// block 0048f08a
0048f08a  xor        esi, esi
0048f08c  cmp        cx, si
0048f08f  jne        0x48f0b0

// block 0048f091
0048f091  inc        edi
0048f092  test       dword ptr [ebp - 0x18], 0x7fffffff
0048f099  jne        0x48f0b0

// block 0048f09b
0048f09b  cmp        dword ptr [ebp - 0x1c], esi
0048f09e  jne        0x48f0b0

// block 0048f0a0
0048f0a0  cmp        dword ptr [ebp - 0x20], esi
0048f0a3  jne        0x48f0b0

// block 0048f0a5
0048f0a5  xor        eax, eax
0048f0a7  mov        word ptr [ebp - 0x16], ax
0048f0ab  jmp        0x48f25d

// block 0048f0b0
0048f0b0  cmp        dx, si
0048f0b3  jne        0x48f0c8

// block 0048f0b5
0048f0b5  inc        edi
0048f0b6  test       dword ptr [eax + 8], 0x7fffffff
0048f0bd  jne        0x48f0c8

// block 0048f0bf
0048f0bf  cmp        dword ptr [eax + 4], esi
0048f0c2  jne        0x48f0c8

// block 0048f0c4
0048f0c4  cmp        dword ptr [eax], esi
0048f0c6  je         0x48f07c

// block 0048f0c8
0048f0c8  and        dword ptr [ebp - 0x58], esi
0048f0cb  lea        esi, [ebp - 0xc]
0048f0ce  mov        dword ptr [ebp - 0x40], 5

// block 0048f0d5
0048f0d5  mov        ecx, dword ptr [ebp - 0x58]
0048f0d8  mov        edx, dword ptr [ebp - 0x40]
0048f0db  add        ecx, ecx
0048f0dd  mov        dword ptr [ebp - 0x50], edx
0048f0e0  test       edx, edx
0048f0e2  jle        0x48f139

// block 0048f0e4
0048f0e4  lea        ecx, [ebp + ecx - 0x20]
0048f0e8  add        eax, 8
0048f0eb  mov        dword ptr [ebp - 0x6c], ecx
0048f0ee  mov        dword ptr [ebp - 0x68], eax

// block 0048f0f1
0048f0f1  mov        eax, dword ptr [ebp - 0x6c]
0048f0f4  movzx      ecx, word ptr [eax]
0048f0f7  mov        eax, dword ptr [ebp - 0x68]
0048f0fa  movzx      eax, word ptr [eax]
0048f0fd  mov        edx, dword ptr [esi - 4]
0048f100  imul       ecx, eax
0048f103  and        dword ptr [ebp - 0x5c], 0
0048f107  lea        eax, [edx + ecx]
0048f10a  cmp        eax, edx
0048f10c  jb         0x48f112

// block 0048f10e
0048f10e  cmp        eax, ecx
0048f110  jae        0x48f119

// block 0048f112
0048f112  mov        dword ptr [ebp - 0x5c], 1

// block 0048f119
0048f119  cmp        dword ptr [ebp - 0x5c], 0
0048f11d  mov        dword ptr [esi - 4], eax
0048f120  je         0x48f125

// block 0048f122
0048f122  inc        word ptr [esi]

// block 0048f125
0048f125  add        dword ptr [ebp - 0x6c], 2
0048f129  sub        dword ptr [ebp - 0x68], 2
0048f12d  dec        dword ptr [ebp - 0x50]
0048f130  cmp        dword ptr [ebp - 0x50], 0
0048f134  jg         0x48f0f1

// block 0048f136
0048f136  mov        eax, dword ptr [ebp - 0x44]

// block 0048f139
0048f139  inc        esi
0048f13a  inc        esi
0048f13b  inc        dword ptr [ebp - 0x58]
0048f13e  dec        dword ptr [ebp - 0x40]
0048f141  cmp        dword ptr [ebp - 0x40], 0
0048f145  jg         0x48f0d5

// block 0048f147
0048f147  add        edi, 0xc002
0048f14d  test       di, di
0048f150  jle        0x48f18d

// block 0048f152
0048f152  test       dword ptr [ebp - 8], 0x80000000
0048f159  jne        0x48f188

// block 0048f15b
0048f15b  mov        eax, dword ptr [ebp - 0xc]
0048f15e  mov        ecx, dword ptr [ebp - 0x10]
0048f161  shl        dword ptr [ebp - 0x10], 1
0048f164  mov        edx, eax
0048f166  add        eax, eax
0048f168  shr        ecx, 0x1f
0048f16b  or         eax, ecx
0048f16d  mov        dword ptr [ebp - 0xc], eax
0048f170  mov        eax, dword ptr [ebp - 8]
0048f173  shr        edx, 0x1f
0048f176  add        eax, eax
0048f178  or         eax, edx
0048f17a  add        edi, 0xffff
0048f180  mov        dword ptr [ebp - 8], eax
0048f183  test       di, di
0048f186  jg         0x48f152

// block 0048f188
0048f188  test       di, di
0048f18b  jg         0x48f1da

// block 0048f18d
0048f18d  add        edi, 0xffff
0048f193  test       di, di
0048f196  jge        0x48f1da

// block 0048f198
0048f198  mov        eax, edi
0048f19a  neg        eax
0048f19c  movzx      eax, ax
0048f19f  add        edi, eax

// block 0048f1a1
0048f1a1  test       byte ptr [ebp - 0x10], 1
0048f1a5  je         0x48f1aa

// block 0048f1a7
0048f1a7  inc        dword ptr [ebp - 0x54]

// block 0048f1aa
0048f1aa  mov        ecx, dword ptr [ebp - 8]
0048f1ad  mov        esi, dword ptr [ebp - 0xc]
0048f1b0  mov        edx, dword ptr [ebp - 0xc]
0048f1b3  shr        dword ptr [ebp - 8], 1
0048f1b6  shl        ecx, 0x1f
0048f1b9  shr        esi, 1
0048f1bb  or         esi, ecx
0048f1bd  mov        ecx, dword ptr [ebp - 0x10]
0048f1c0  shl        edx, 0x1f
0048f1c3  shr        ecx, 1
0048f1c5  or         ecx, edx
0048f1c7  dec        eax
0048f1c8  mov        dword ptr [ebp - 0xc], esi
0048f1cb  mov        dword ptr [ebp - 0x10], ecx
0048f1ce  jne        0x48f1a1

// block 0048f1d0
0048f1d0  cmp        dword ptr [ebp - 0x54], eax
0048f1d3  je         0x48f1da

// block 0048f1d5
0048f1d5  or         word ptr [ebp - 0x10], 1

// block 0048f1da
0048f1da  mov        eax, 0x8000
0048f1df  mov        ecx, eax
0048f1e1  cmp        word ptr [ebp - 0x10], cx
0048f1e5  ja         0x48f1f8

// block 0048f1e7
0048f1e7  mov        ecx, dword ptr [ebp - 0x10]
0048f1ea  and        ecx, 0x1ffff
0048f1f0  cmp        ecx, 0x18000
0048f1f6  jne        0x48f22c

// block 0048f1f8
0048f1f8  cmp        dword ptr [ebp - 0xe], -1
0048f1fc  jne        0x48f229

// block 0048f1fe
0048f1fe  and        dword ptr [ebp - 0xe], 0
0048f202  cmp        dword ptr [ebp - 0xa], -1
0048f206  jne        0x48f224

// block 0048f208
0048f208  and        dword ptr [ebp - 0xa], 0
0048f20c  mov        ecx, 0xffff
0048f211  cmp        word ptr [ebp - 6], cx
0048f215  jne        0x48f21e

// block 0048f217
0048f217  mov        word ptr [ebp - 6], ax
0048f21b  inc        edi
0048f21c  jmp        0x48f22c

// block 0048f21e
0048f21e  inc        word ptr [ebp - 6]
0048f222  jmp        0x48f22c

// block 0048f224
0048f224  inc        dword ptr [ebp - 0xa]
0048f227  jmp        0x48f22c

// block 0048f229
0048f229  inc        dword ptr [ebp - 0xe]

// block 0048f22c
0048f22c  mov        eax, 0x7fff
0048f231  cmp        di, ax
0048f234  jb         0x48f2e5

// block 0048f23a
0048f23a  xor        eax, eax
0048f23c  xor        ecx, ecx
0048f23e  cmp        word ptr [ebp - 0x48], ax
0048f242  mov        dword ptr [ebp - 0x1c], eax
0048f245  sete       cl
0048f248  mov        dword ptr [ebp - 0x20], eax
0048f24b  dec        ecx
0048f24c  and        ecx, 0x80000000
0048f252  add        ecx, 0x7fff8000
0048f258  mov        dword ptr [ebp - 0x18], ecx

// block 0048f25b
0048f25b  xor        esi, esi

// block 0048f25d
0048f25d  cmp        ebx, esi
0048f25f  jne        0x48efe0

// block 0048f265
0048f265  mov        ecx, dword ptr [ebp - 0x18]
0048f268  shr        ecx, 0x10
0048f26b  mov        edx, 0x3fff
0048f270  mov        eax, 0x7fff
0048f275  cmp        cx, dx
0048f278  jb         0x48f521

// block 0048f27e
0048f27e  inc        dword ptr [ebp - 0x4c]
0048f281  xor        edx, edx
0048f283  mov        dword ptr [ebp - 0x50], edx
0048f286  mov        dword ptr [ebp - 0x10], edx
0048f289  mov        dword ptr [ebp - 0xc], edx
0048f28c  mov        dword ptr [ebp - 8], edx
0048f28f  mov        edx, dword ptr [ebp - 0x26]
0048f292  movzx      ecx, cx
0048f295  mov        ebx, edx
0048f297  xor        ebx, ecx
0048f299  and        ecx, eax
0048f29b  and        edx, eax
0048f29d  and        ebx, 0x8000
0048f2a3  mov        edi, eax
0048f2a5  lea        esi, [edx + ecx]
0048f2a8  mov        dword ptr [ebp - 0x5c], ebx
0048f2ab  movzx      esi, si
0048f2ae  cmp        cx, di
0048f2b1  jae        0x48f503

// block 0048f2b7
0048f2b7  cmp        dx, ax
0048f2ba  jae        0x48f503

// block 0048f2c0
0048f2c0  mov        eax, 0xbffd
0048f2c5  cmp        si, ax
0048f2c8  ja         0x48f503

// block 0048f2ce
0048f2ce  mov        eax, 0x3fbf
0048f2d3  cmp        si, ax
0048f2d6  ja         0x48f323

// block 0048f2d8
0048f2d8  xor        eax, eax

// block 0048f2da
0048f2da  mov        dword ptr [ebp - 0x1c], eax
0048f2dd  mov        dword ptr [ebp - 0x20], eax
0048f2e0  jmp        0x48f51e

// block 0048f2e5
0048f2e5  mov        ax, word ptr [ebp - 0xe]
0048f2e9  or         edi, dword ptr [ebp - 0x48]
0048f2ec  mov        word ptr [ebp - 0x20], ax
0048f2f0  mov        eax, dword ptr [ebp - 0xc]
0048f2f3  mov        dword ptr [ebp - 0x1e], eax
0048f2f6  mov        eax, dword ptr [ebp - 8]
0048f2f9  mov        dword ptr [ebp - 0x1a], eax
0048f2fc  mov        word ptr [ebp - 0x16], di
0048f300  jmp        0x48f25b

// block 0048f305
0048f305  xor        eax, eax
0048f307  xor        esi, esi
0048f309  cmp        word ptr [ebp - 0x48], si
0048f30d  sete       al
0048f310  dec        eax
0048f311  and        eax, 0x80000000
0048f316  add        eax, 0x7fff8000
0048f31b  mov        dword ptr [ebp - 0x18], eax
0048f31e  jmp        0x48f07f

// block 0048f323
0048f323  xor        eax, eax
0048f325  cmp        cx, ax
0048f328  jne        0x48f347

// block 0048f32a
0048f32a  inc        esi
0048f32b  test       dword ptr [ebp - 0x18], 0x7fffffff
0048f332  jne        0x48f347

// block 0048f334
0048f334  cmp        dword ptr [ebp - 0x1c], eax
0048f337  jne        0x48f347

// block 0048f339
0048f339  cmp        dword ptr [ebp - 0x20], eax
0048f33c  jne        0x48f347

// block 0048f33e
0048f33e  mov        word ptr [ebp - 0x16], ax
0048f342  jmp        0x48f521

// block 0048f347
0048f347  cmp        dx, ax
0048f34a  jne        0x48f364

// block 0048f34c
0048f34c  inc        esi
0048f34d  test       dword ptr [ebp - 0x28], 0x7fffffff
0048f354  jne        0x48f364

// block 0048f356
0048f356  cmp        dword ptr [ebp - 0x2c], eax
0048f359  jne        0x48f364

// block 0048f35b
0048f35b  cmp        dword ptr [ebp - 0x30], eax
0048f35e  je         0x48f2da

// block 0048f364
0048f364  mov        dword ptr [ebp - 0x58], eax
0048f367  lea        edi, [ebp - 0xc]
0048f36a  mov        dword ptr [ebp - 0x40], 5

// block 0048f371
0048f371  mov        eax, dword ptr [ebp - 0x58]
0048f374  mov        ecx, dword ptr [ebp - 0x40]
0048f377  add        eax, eax
0048f379  mov        dword ptr [ebp - 0x54], ecx
0048f37c  test       ecx, ecx
0048f37e  jle        0x48f3ca

// block 0048f380
0048f380  lea        ecx, [ebp - 0x28]
0048f383  mov        dword ptr [ebp - 0x48], ecx
0048f386  lea        eax, [ebp + eax - 0x20]

// block 0048f38a
0048f38a  mov        ecx, dword ptr [ebp - 0x48]
0048f38d  movzx      edx, word ptr [eax]
0048f390  movzx      ecx, word ptr [ecx]
0048f393  and        dword ptr [ebp - 0x44], 0
0048f397  imul       ecx, edx
0048f39a  mov        edx, dword ptr [edi - 4]
0048f39d  lea        ebx, [edx + ecx]
0048f3a0  cmp        ebx, edx
0048f3a2  jb         0x48f3a8

// block 0048f3a4
0048f3a4  cmp        ebx, ecx
0048f3a6  jae        0x48f3af

// block 0048f3a8
0048f3a8  mov        dword ptr [ebp - 0x44], 1

// block 0048f3af
0048f3af  cmp        dword ptr [ebp - 0x44], 0
0048f3b3  mov        dword ptr [edi - 4], ebx
0048f3b6  je         0x48f3bb

// block 0048f3b8
0048f3b8  inc        word ptr [edi]

// block 0048f3bb
0048f3bb  sub        dword ptr [ebp - 0x48], 2
0048f3bf  inc        eax
0048f3c0  inc        eax
0048f3c1  dec        dword ptr [ebp - 0x54]
0048f3c4  cmp        dword ptr [ebp - 0x54], 0
0048f3c8  jg         0x48f38a

// block 0048f3ca
0048f3ca  inc        edi
0048f3cb  inc        edi
0048f3cc  inc        dword ptr [ebp - 0x58]
0048f3cf  dec        dword ptr [ebp - 0x40]
0048f3d2  cmp        dword ptr [ebp - 0x40], 0
0048f3d6  jg         0x48f371

// block 0048f3d8
0048f3d8  add        esi, 0xc002
0048f3de  test       si, si
0048f3e1  jle        0x48f41a

// block 0048f3e3
0048f3e3  mov        edi, dword ptr [ebp - 8]
0048f3e6  test       edi, edi
0048f3e8  js         0x48f415

// block 0048f3ea
0048f3ea  mov        eax, dword ptr [ebp - 0xc]
0048f3ed  mov        ecx, dword ptr [ebp - 0x10]
0048f3f0  shl        dword ptr [ebp - 0x10], 1
0048f3f3  mov        edx, eax
0048f3f5  add        eax, eax
0048f3f7  shr        ecx, 0x1f
0048f3fa  or         eax, ecx
0048f3fc  mov        dword ptr [ebp - 0xc], eax
0048f3ff  shr        edx, 0x1f
0048f402  lea        eax, [edi + edi]
0048f405  or         eax, edx
0048f407  add        esi, 0xffff
0048f40d  mov        dword ptr [ebp - 8], eax
0048f410  test       si, si
0048f413  jg         0x48f3e3

// block 0048f415
0048f415  test       si, si
0048f418  jg         0x48f467

// block 0048f41a
0048f41a  add        esi, 0xffff
0048f420  test       si, si
0048f423  jge        0x48f467

// block 0048f425
0048f425  mov        eax, esi
0048f427  neg        eax
0048f429  movzx      eax, ax
0048f42c  add        esi, eax

// block 0048f42e
0048f42e  test       byte ptr [ebp - 0x10], 1
0048f432  je         0x48f437

// block 0048f434
0048f434  inc        dword ptr [ebp - 0x50]

// block 0048f437
0048f437  mov        ecx, dword ptr [ebp - 8]
0048f43a  mov        edi, dword ptr [ebp - 0xc]
0048f43d  mov        edx, dword ptr [ebp - 0xc]
0048f440  shr        dword ptr [ebp - 8], 1
0048f443  shl        ecx, 0x1f
0048f446  shr        edi, 1
0048f448  or         edi, ecx
0048f44a  mov        ecx, dword ptr [ebp - 0x10]
0048f44d  shl        edx, 0x1f
0048f450  shr        ecx, 1
0048f452  or         ecx, edx
0048f454  dec        eax
0048f455  mov        dword ptr [ebp - 0xc], edi
0048f458  mov        dword ptr [ebp - 0x10], ecx
0048f45b  jne        0x48f42e

// block 0048f45d
0048f45d  cmp        dword ptr [ebp - 0x50], eax
0048f460  je         0x48f467

// block 0048f462
0048f462  or         word ptr [ebp - 0x10], 1

// block 0048f467
0048f467  mov        eax, 0x8000
0048f46c  mov        ecx, eax
0048f46e  cmp        word ptr [ebp - 0x10], cx
0048f472  ja         0x48f485

// block 0048f474
0048f474  mov        ecx, dword ptr [ebp - 0x10]
0048f477  and        ecx, 0x1ffff
0048f47d  cmp        ecx, 0x18000
0048f483  jne        0x48f4b9

// block 0048f485
0048f485  cmp        dword ptr [ebp - 0xe], -1
0048f489  jne        0x48f4b6

// block 0048f48b
0048f48b  and        dword ptr [ebp - 0xe], 0
0048f48f  cmp        dword ptr [ebp - 0xa], -1
0048f493  jne        0x48f4b1

// block 0048f495
0048f495  and        dword ptr [ebp - 0xa], 0
0048f499  mov        ecx, 0xffff
0048f49e  cmp        word ptr [ebp - 6], cx
0048f4a2  jne        0x48f4ab

// block 0048f4a4
0048f4a4  mov        word ptr [ebp - 6], ax
0048f4a8  inc        esi
0048f4a9  jmp        0x48f4b9

// block 0048f4ab
0048f4ab  inc        word ptr [ebp - 6]
0048f4af  jmp        0x48f4b9

// block 0048f4b1
0048f4b1  inc        dword ptr [ebp - 0xa]
0048f4b4  jmp        0x48f4b9

// block 0048f4b6
0048f4b6  inc        dword ptr [ebp - 0xe]

// block 0048f4b9
0048f4b9  mov        eax, 0x7fff
0048f4be  cmp        si, ax
0048f4c1  jb         0x48f4e6

// block 0048f4c3
0048f4c3  xor        eax, eax
0048f4c5  xor        ecx, ecx
0048f4c7  cmp        word ptr [ebp - 0x5c], ax
0048f4cb  mov        dword ptr [ebp - 0x1c], eax
0048f4ce  sete       cl
0048f4d1  mov        dword ptr [ebp - 0x20], eax
0048f4d4  dec        ecx
0048f4d5  and        ecx, 0x80000000
0048f4db  add        ecx, 0x7fff8000
0048f4e1  mov        dword ptr [ebp - 0x18], ecx
0048f4e4  jmp        0x48f521

// block 0048f4e6
0048f4e6  mov        ax, word ptr [ebp - 0xe]
0048f4ea  or         esi, dword ptr [ebp - 0x5c]
0048f4ed  mov        word ptr [ebp - 0x20], ax
0048f4f1  mov        eax, dword ptr [ebp - 0xc]
0048f4f4  mov        dword ptr [ebp - 0x1e], eax
0048f4f7  mov        eax, dword ptr [ebp - 8]
0048f4fa  mov        dword ptr [ebp - 0x1a], eax
0048f4fd  mov        word ptr [ebp - 0x16], si
0048f501  jmp        0x48f521

// block 0048f503
0048f503  xor        eax, eax
0048f505  test       bx, bx
0048f508  sete       al
0048f50b  and        dword ptr [ebp - 0x1c], 0
0048f50f  dec        eax
0048f510  and        eax, 0x80000000
0048f515  add        eax, 0x7fff8000
0048f51a  and        dword ptr [ebp - 0x20], 0

// block 0048f51e
0048f51e  mov        dword ptr [ebp - 0x18], eax

// block 0048f521
0048f521  test       byte ptr [ebp + 0x18], 1
0048f525  mov        edx, dword ptr [ebp - 0x60]
0048f528  mov        eax, dword ptr [ebp - 0x4c]
0048f52b  mov        edi, dword ptr [ebp + 0x14]
0048f52e  mov        word ptr [edx], ax
0048f531  je         0x48f565

// block 0048f533
0048f533  cwde       
0048f534  add        edi, eax
0048f536  test       edi, edi
0048f538  jg         0x48f565

// block 0048f53a
0048f53a  xor        eax, eax
0048f53c  mov        word ptr [edx], ax
0048f53f  mov        eax, 0x8000
0048f544  cmp        word ptr [ebp - 0x70], ax
0048f548  mov        byte ptr [edx + 3], 1
0048f54c  setne      al
0048f54f  dec        al
0048f551  and        al, 0xd
0048f553  add        al, 0x20
0048f555  mov        byte ptr [edx + 2], al
0048f558  mov        byte ptr [edx + 4], 0x30
0048f55c  mov        byte ptr [edx + 5], 0
0048f560  jmp        0x48eec3

// block 0048f565
0048f565  cmp        edi, 0x15
0048f568  jle        0x48f56d

// block 0048f56a
0048f56a  push       0x15
0048f56c  pop        edi

// block 0048f56d
0048f56d  mov        esi, dword ptr [ebp - 0x18]
0048f570  shr        esi, 0x10
0048f573  sub        esi, 0x3ffe
0048f579  xor        eax, eax
0048f57b  mov        word ptr [ebp - 0x16], ax
0048f57f  mov        dword ptr [ebp - 0x44], 8

// block 0048f586
0048f586  mov        eax, dword ptr [ebp - 0x20]
0048f589  mov        ebx, dword ptr [ebp - 0x1c]
0048f58c  mov        ecx, dword ptr [ebp - 0x1c]
0048f58f  shl        dword ptr [ebp - 0x20], 1
0048f592  shr        eax, 0x1f
0048f595  add        ebx, ebx
0048f597  or         ebx, eax
0048f599  mov        eax, dword ptr [ebp - 0x18]
0048f59c  shr        ecx, 0x1f
0048f59f  add        eax, eax
0048f5a1  or         eax, ecx
0048f5a3  dec        dword ptr [ebp - 0x44]
0048f5a6  mov        dword ptr [ebp - 0x1c], ebx
0048f5a9  mov        dword ptr [ebp - 0x18], eax
0048f5ac  jne        0x48f586

// block 0048f5ae
0048f5ae  test       esi, esi
0048f5b0  jge        0x48f5e4

// block 0048f5b2
0048f5b2  neg        esi
0048f5b4  and        esi, 0xff
0048f5ba  jle        0x48f5e4

// block 0048f5bc
0048f5bc  mov        eax, dword ptr [ebp - 0x18]
0048f5bf  mov        ebx, dword ptr [ebp - 0x1c]
0048f5c2  mov        ecx, dword ptr [ebp - 0x1c]
0048f5c5  shr        dword ptr [ebp - 0x18], 1
0048f5c8  shl        eax, 0x1f
0048f5cb  shr        ebx, 1
0048f5cd  or         ebx, eax
0048f5cf  mov        eax, dword ptr [ebp - 0x20]
0048f5d2  shl        ecx, 0x1f
0048f5d5  shr        eax, 1
0048f5d7  or         eax, ecx
0048f5d9  dec        esi
0048f5da  mov        dword ptr [ebp - 0x1c], ebx
0048f5dd  mov        dword ptr [ebp - 0x20], eax
0048f5e0  test       esi, esi
0048f5e2  jg         0x48f5bc

// block 0048f5e4
0048f5e4  lea        eax, [edi + 1]
0048f5e7  lea        ebx, [edx + 4]
0048f5ea  mov        dword ptr [ebp - 0x40], ebx
0048f5ed  mov        dword ptr [ebp - 0x4c], eax
0048f5f0  test       eax, eax
0048f5f2  jle        0x48f6ad

// block 0048f5f8
0048f5f8  mov        edx, dword ptr [ebp - 0x20]
0048f5fb  mov        eax, dword ptr [ebp - 0x1c]
0048f5fe  lea        esi, [ebp - 0x20]
0048f601  lea        edi, [ebp - 0x3c]
0048f604  movsd      dword ptr es:[edi], dword ptr [esi]
0048f605  movsd      dword ptr es:[edi], dword ptr [esi]
0048f606  movsd      dword ptr es:[edi], dword ptr [esi]
0048f607  shl        dword ptr [ebp - 0x20], 1
0048f60a  mov        edi, dword ptr [ebp - 0x20]
0048f60d  shl        dword ptr [ebp - 0x20], 1
0048f610  shr        edx, 0x1f
0048f613  lea        ecx, [eax + eax]
0048f616  or         ecx, edx
0048f618  mov        edx, dword ptr [ebp - 0x18]
0048f61b  mov        esi, eax
0048f61d  shr        esi, 0x1f
0048f620  add        edx, edx
0048f622  or         edx, esi
0048f624  mov        eax, ecx
0048f626  lea        esi, [ecx + ecx]
0048f629  shr        eax, 0x1f
0048f62c  lea        ecx, [edx + edx]
0048f62f  mov        edx, dword ptr [ebp - 0x3c]
0048f632  shr        edi, 0x1f
0048f635  or         ecx, eax
0048f637  mov        eax, dword ptr [ebp - 0x20]
0048f63a  or         esi, edi
0048f63c  lea        edi, [edx + eax]
0048f63f  cmp        edi, eax
0048f641  jb         0x48f647

// block 0048f643
0048f643  cmp        edi, edx
0048f645  jae        0x48f65f

// block 0048f647
0048f647  lea        eax, [esi + 1]
0048f64a  xor        edx, edx
0048f64c  cmp        eax, esi
0048f64e  jb         0x48f655

// block 0048f650
0048f650  cmp        eax, 1
0048f653  jae        0x48f658

// block 0048f655
0048f655  xor        edx, edx
0048f657  inc        edx

// block 0048f658
0048f658  mov        esi, eax
0048f65a  test       edx, edx
0048f65c  je         0x48f65f

// block 0048f65e
0048f65e  inc        ecx

// block 0048f65f
0048f65f  mov        eax, dword ptr [ebp - 0x38]
0048f662  lea        edx, [eax + esi]
0048f665  mov        dword ptr [ebp - 0x44], edx
0048f668  cmp        edx, esi
0048f66a  jb         0x48f670

// block 0048f66c
0048f66c  cmp        edx, eax
0048f66e  jae        0x48f671

// block 0048f670
0048f670  inc        ecx

// block 0048f671
0048f671  add        ecx, dword ptr [ebp - 0x34]
0048f674  shr        edx, 0x1f
0048f677  add        ecx, ecx
0048f679  or         ecx, edx
0048f67b  lea        esi, [edi + edi]
0048f67e  mov        dword ptr [ebp - 0x20], esi
0048f681  mov        esi, dword ptr [ebp - 0x44]
0048f684  mov        dword ptr [ebp - 0x18], ecx
0048f687  shr        ecx, 0x18
0048f68a  add        esi, esi
0048f68c  add        cl, 0x30
0048f68f  mov        eax, edi
0048f691  shr        eax, 0x1f
0048f694  or         esi, eax
0048f696  mov        byte ptr [ebx], cl
0048f698  inc        ebx
0048f699  dec        dword ptr [ebp - 0x4c]
0048f69c  cmp        dword ptr [ebp - 0x4c], 0
0048f6a0  mov        dword ptr [ebp - 0x1c], esi
0048f6a3  mov        byte ptr [ebp - 0x15], 0
0048f6a7  jg         0x48f5f8

// block 0048f6ad
0048f6ad  dec        ebx
0048f6ae  mov        al, byte ptr [ebx]
0048f6b0  dec        ebx
0048f6b1  cmp        al, 0x35
0048f6b3  jge        0x48f6c3

// block 0048f6b5
0048f6b5  mov        ecx, dword ptr [ebp - 0x40]
0048f6b8  jmp        0x48f6fe

// block 0048f6ba
0048f6ba  cmp        byte ptr [ebx], 0x39
0048f6bd  jne        0x48f6c8

// block 0048f6bf
0048f6bf  mov        byte ptr [ebx], 0x30
0048f6c2  dec        ebx

// block 0048f6c3
0048f6c3  cmp        ebx, dword ptr [ebp - 0x40]
0048f6c6  jae        0x48f6ba

// block 0048f6c8
0048f6c8  mov        eax, dword ptr [ebp - 0x60]
0048f6cb  cmp        ebx, dword ptr [ebp - 0x40]
0048f6ce  jae        0x48f6d4

// block 0048f6d0
0048f6d0  inc        ebx
0048f6d1  inc        word ptr [eax]

// block 0048f6d4
0048f6d4  inc        byte ptr [ebx]

// block 0048f6d6
0048f6d6  sub        bl, al
0048f6d8  sub        bl, 3
0048f6db  movsx      ecx, bl
0048f6de  mov        byte ptr [eax + 3], bl
0048f6e1  mov        byte ptr [ecx + eax + 4], 0
0048f6e6  mov        eax, dword ptr [ebp - 0x74]

// block 0048f6e9
0048f6e9  mov        ecx, dword ptr [ebp - 4]
0048f6ec  pop        edi
0048f6ed  pop        esi
0048f6ee  xor        ecx, ebp
0048f6f0  pop        ebx
0048f6f1  call       0x46c932

// block 0048f6f6
0048f6f6  leave      
0048f6f7  ret        

// block 0048f6f8
0048f6f8  cmp        byte ptr [ebx], 0x30
0048f6fb  jne        0x48f702

// block 0048f6fd
0048f6fd  dec        ebx

// block 0048f6fe
0048f6fe  cmp        ebx, ecx
0048f700  jae        0x48f6f8

// block 0048f702
0048f702  mov        eax, dword ptr [ebp - 0x60]
0048f705  cmp        ebx, ecx
0048f707  jae        0x48f6d6

// block 0048f709
0048f709  xor        edx, edx
0048f70b  mov        word ptr [eax], dx
0048f70e  mov        edx, 0x8000
0048f713  cmp        word ptr [ebp - 0x70], dx
0048f717  mov        byte ptr [eax + 3], 1
0048f71b  setne      dl
0048f71e  dec        dl
0048f720  and        dl, 0xd
0048f723  add        dl, 0x20
0048f726  mov        byte ptr [eax + 2], dl
0048f729  mov        byte ptr [ecx], 0x30
0048f72c  mov        byte ptr [eax + 5], 0
0048f730  jmp        0x48eec3
