
// block 004dcb71
004dcb71  sub        esp, 0x14
004dcb74  mov        eax, dword ptr [esp + 0x1c]
004dcb78  push       ebx
004dcb79  push       ebp
004dcb7a  push       esi
004dcb7b  mov        dword ptr [eax], 0
004dcb81  mov        eax, dword ptr [esp + 0x24]
004dcb85  push       edi
004dcb86  xor        edi, edi
004dcb88  test       eax, eax
004dcb8a  mov        esi, ecx
004dcb8c  mov        dword ptr [esp + 0x10], edi
004dcb90  jbe        0x4dcdf1

// block 004dcb96
004dcb96  lea        ecx, [esi + 0x10]
004dcb99  call       0x4dc821

// block 004dcb9e
004dcb9e  cmp        eax, 0x100
004dcba3  jae        0x4dcbb8

// block 004dcba5
004dcba5  mov        ecx, dword ptr [esi]
004dcba7  mov        byte ptr [ecx], al
004dcba9  mov        ecx, dword ptr [esi]
004dcbab  inc        ecx
004dcbac  inc        edi
004dcbad  mov        dword ptr [esi], ecx
004dcbaf  mov        dword ptr [esp + 0x10], edi
004dcbb3  jmp        0x4dcde1

// block 004dcbb8
004dcbb8  cmp        eax, 0x2d0
004dcbbd  jae        0x4dcdd6

// block 004dcbc3
004dcbc3  add        eax, 0xffffff00
004dcbc8  mov        ebp, eax
004dcbca  and        eax, 7
004dcbcd  shr        ebp, 3
004dcbd0  lea        edx, [eax + 2]
004dcbd3  cmp        eax, 7
004dcbd6  mov        dword ptr [esp + 0x14], edx
004dcbda  jne        0x4dcc74

// block 004dcbe0
004dcbe0  lea        ecx, [esi + 0xa0]
004dcbe6  call       0x4dc821

// block 004dcbeb
004dcbeb  mov        ecx, dword ptr [esi + 8]
004dcbee  xor        ebx, ebx
004dcbf0  push       esi
004dcbf1  call       0x4dcb63

// block 004dcbf6
004dcbf6  mov        bl, byte ptr [eax + esi + 0x46b4b6]
004dcbfd  pop        esi
004dcbfe  cmp        ecx, 8
004dcc01  jb         0x4dcc35

// block 004dcc03
004dcc03  mov        ecx, dword ptr [esi + 4]
004dcc06  mov        dl, byte ptr [ecx]
004dcc08  inc        ecx
004dcc09  mov        byte ptr [esp + 0x18], dl
004dcc0d  mov        dword ptr [esi + 4], ecx
004dcc10  mov        ecx, dword ptr [esi + 0xc]
004dcc13  mov        edx, dword ptr [esp + 0x18]
004dcc17  shl        ecx, 8
004dcc1a  and        edx, 0xff
004dcc20  or         ecx, edx
004dcc22  mov        edx, dword ptr [esi + 8]
004dcc25  add        edx, -8
004dcc28  mov        dword ptr [esi + 0xc], ecx
004dcc2b  mov        ecx, edx
004dcc2d  mov        dword ptr [esi + 8], edx
004dcc30  cmp        ecx, 8
004dcc33  jae        0x4dcc03

// block 004dcc35
004dcc35  mov        edi, dword ptr [esi + 8]
004dcc38  mov        edx, dword ptr [esi + 0xc]
004dcc3b  mov        ecx, 8
004dcc40  sub        ecx, edi
004dcc42  add        edi, ebx
004dcc44  shr        edx, cl
004dcc46  mov        ecx, 0x18
004dcc4b  mov        dword ptr [esi + 8], edi
004dcc4e  sub        ecx, ebx
004dcc50  and        edx, 0xffffff
004dcc56  shr        edx, cl
004dcc58  xor        ecx, ecx
004dcc5a  push       esi
004dcc5b  call       0x4dcb63

// block 004dcc60
004dcc60  mov        cl, byte ptr [eax + esi + 0x46b49a]
004dcc67  pop        esi
004dcc68  mov        eax, dword ptr [esp + 0x14]
004dcc6c  add        ecx, edx
004dcc6e  add        eax, ecx
004dcc70  mov        dword ptr [esp + 0x14], eax

// block 004dcc74
004dcc74  mov        al, byte ptr [esi + 0x264]
004dcc7a  mov        ebx, dword ptr [esi + ebp*4 + 0x268]
004dcc81  xor        edx, edx
004dcc83  push       esi
004dcc84  call       0x4dcb63

// block 004dcc89
004dcc89  mov        dl, byte ptr [ebp + esi + 0x46b4d2]
004dcc90  pop        esi
004dcc91  test       al, al
004dcc93  mov        edi, edx
004dcc95  je         0x4dcd0d

// block 004dcc97
004dcc97  cmp        edi, 3
004dcc9a  jb         0x4dcd0d

// block 004dcc9c
004dcc9c  mov        eax, dword ptr [esi + 8]
004dcc9f  lea        ebp, [edi - 3]
004dcca2  cmp        eax, 8
004dcca5  jb         0x4dccd8

// block 004dcca7
004dcca7  mov        eax, dword ptr [esi + 4]
004dccaa  mov        edx, dword ptr [esi + 0xc]
004dccad  shl        edx, 8
004dccb0  mov        cl, byte ptr [eax]
004dccb2  inc        eax
004dccb3  mov        byte ptr [esp + 0x1c], cl
004dccb7  mov        ecx, dword ptr [esi + 8]
004dccba  mov        dword ptr [esi + 4], eax
004dccbd  mov        eax, dword ptr [esp + 0x1c]
004dccc1  and        eax, 0xff
004dccc6  add        ecx, -8
004dccc9  or         edx, eax
004dcccb  mov        eax, ecx
004dcccd  cmp        eax, 8
004dccd0  mov        dword ptr [esi + 0xc], edx
004dccd3  mov        dword ptr [esi + 8], ecx
004dccd6  jae        0x4dcca7

// block 004dccd8
004dccd8  mov        eax, dword ptr [esi + 8]
004dccdb  mov        edi, dword ptr [esi + 0xc]
004dccde  mov        ecx, 8
004dcce3  sub        ecx, eax
004dcce5  add        eax, ebp
004dcce7  shr        edi, cl
004dcce9  mov        ecx, 0x18
004dccee  mov        dword ptr [esi + 8], eax
004dccf1  sub        ecx, ebp
004dccf3  and        edi, 0xffffff
004dccf9  shr        edi, cl
004dccfb  lea        ecx, [esi + 0x130]
004dcd01  call       0x4dc821

// block 004dcd06
004dcd06  add        eax, ebx
004dcd08  lea        ebx, [eax + edi*8]
004dcd0b  jmp        0x4dcd68

// block 004dcd0d
004dcd0d  cmp        dword ptr [esi + 8], 8
004dcd11  jb         0x4dcd44

// block 004dcd13
004dcd13  mov        eax, dword ptr [esi + 4]
004dcd16  mov        edx, dword ptr [esi + 0xc]
004dcd19  shl        edx, 8
004dcd1c  mov        cl, byte ptr [eax]
004dcd1e  inc        eax
004dcd1f  mov        byte ptr [esp + 0x20], cl
004dcd23  mov        ecx, dword ptr [esi + 8]
004dcd26  mov        dword ptr [esi + 4], eax
004dcd29  mov        eax, dword ptr [esp + 0x20]
004dcd2d  and        eax, 0xff
004dcd32  add        ecx, -8
004dcd35  or         edx, eax
004dcd37  mov        eax, ecx
004dcd39  cmp        eax, 8
004dcd3c  mov        dword ptr [esi + 0xc], edx
004dcd3f  mov        dword ptr [esi + 8], ecx
004dcd42  jae        0x4dcd13

// block 004dcd44
004dcd44  mov        edx, dword ptr [esi + 8]
004dcd47  mov        eax, dword ptr [esi + 0xc]
004dcd4a  mov        ecx, 8
004dcd4f  sub        ecx, edx
004dcd51  add        edx, edi
004dcd53  shr        eax, cl
004dcd55  mov        ecx, 0x18
004dcd5a  mov        dword ptr [esi + 8], edx
004dcd5d  sub        ecx, edi
004dcd5f  and        eax, 0xffffff
004dcd64  shr        eax, cl
004dcd66  add        ebx, eax

// block 004dcd68
004dcd68  cmp        ebx, 3
004dcd6b  jae        0x4dcd87

// block 004dcd6d
004dcd6d  mov        ecx, dword ptr [esi + ebx*4 + 0x250]
004dcd74  test       ebx, ebx
004dcd76  je         0x4dcda8

// block 004dcd78
004dcd78  mov        edx, dword ptr [esi + 0x250]
004dcd7e  mov        dword ptr [esi + ebx*4 + 0x250], edx
004dcd85  jmp        0x4dcda2

// block 004dcd87
004dcd87  mov        eax, dword ptr [esi + 0x254]
004dcd8d  mov        edx, dword ptr [esi + 0x250]
004dcd93  lea        ecx, [ebx - 3]
004dcd96  mov        dword ptr [esi + 0x258], eax
004dcd9c  mov        dword ptr [esi + 0x254], edx

// block 004dcda2
004dcda2  mov        dword ptr [esi + 0x250], ecx

// block 004dcda8
004dcda8  mov        eax, dword ptr [esi]
004dcdaa  mov        edi, dword ptr [esp + 0x14]
004dcdae  inc        ecx
004dcdaf  lea        edx, [eax + edi]
004dcdb2  cmp        eax, edx
004dcdb4  mov        dword ptr [esi], edx
004dcdb6  jae        0x4dcdc8

// block 004dcdb8
004dcdb8  mov        edx, eax
004dcdba  sub        edx, ecx
004dcdbc  inc        eax
004dcdbd  mov        dl, byte ptr [edx]
004dcdbf  mov        byte ptr [eax - 1], dl
004dcdc2  mov        edx, dword ptr [esi]
004dcdc4  cmp        eax, edx
004dcdc6  jb         0x4dcdb8

// block 004dcdc8
004dcdc8  mov        eax, dword ptr [esp + 0x10]
004dcdcc  add        eax, edi
004dcdce  mov        dword ptr [esp + 0x10], eax
004dcdd2  mov        edi, eax
004dcdd4  jmp        0x4dcde1

// block 004dcdd6
004dcdd6  mov        ecx, esi
004dcdd8  call       0x4dc9d4

// block 004dcddd
004dcddd  test       al, al
004dcddf  je         0x4dcdfd

// block 004dcde1
004dcde1  cmp        edi, dword ptr [esp + 0x28]
004dcde5  jb         0x4dcb96

// block 004dcdeb
004dcdeb  mov        eax, dword ptr [esp + 0x2c]
004dcdef  mov        dword ptr [eax], edi

// block 004dcdf1
004dcdf1  pop        edi
004dcdf2  pop        esi
004dcdf3  pop        ebp
004dcdf4  mov        al, 1
004dcdf6  pop        ebx
004dcdf7  add        esp, 0x14
004dcdfa  ret        8

// block 004dcdfd
004dcdfd  pop        edi
004dcdfe  pop        esi
004dcdff  pop        ebp
004dce00  xor        al, al
004dce02  pop        ebx
004dce03  add        esp, 0x14
004dce06  ret        8
