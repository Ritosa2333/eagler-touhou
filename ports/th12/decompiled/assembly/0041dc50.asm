
// block 0041dc50
0041dc50  push       -1
0041dc52  push       0x4973d8
0041dc57  mov        eax, dword ptr fs:[0]
0041dc5d  push       eax
0041dc5e  push       ebx
0041dc5f  push       ebp
0041dc60  push       esi
0041dc61  push       edi
0041dc62  mov        eax, dword ptr [0x4ad138]
0041dc67  xor        eax, esp
0041dc69  push       eax
0041dc6a  lea        eax, [esp + 0x14]
0041dc6e  mov        dword ptr fs:[0], eax
0041dc74  mov        ebp, dword ptr [esp + 0x24]
0041dc78  push       ebp
0041dc79  mov        dword ptr [esp + 0x20], 4
0041dc81  call       0x41d9c0

// block 0041dc86
0041dc86  mov        esi, dword ptr [ebp + 8]
0041dc89  mov        edi, dword ptr [0x4ce89c]
0041dc8f  mov        ebx, dword ptr [0x49808c]
0041dc95  test       esi, esi
0041dc97  je         0x41dcd8

// block 0041dc99
0041dc99  test       dword ptr [0x4cee78], 0x8000
0041dca3  je         0x41dcb6

// block 0041dca5
0041dca5  push       0x4cf0f8
0041dcaa  call       dword ptr [0x498088]

// block 0041dcb0
0041dcb0  inc        byte ptr [0x4cf218]

// block 0041dcb6
0041dcb6  mov        ecx, esi
0041dcb8  mov        edx, edi
0041dcba  call       0x462890

// block 0041dcbf
0041dcbf  test       dword ptr [0x4cee78], 0x8000
0041dcc9  je         0x41dcd8

// block 0041dccb
0041dccb  push       0x4cf0f8
0041dcd0  call       ebx

// block 0041dcd2
0041dcd2  dec        byte ptr [0x4cf218]

// block 0041dcd8
0041dcd8  mov        esi, dword ptr [ebp + 0xc]
0041dcdb  mov        edi, dword ptr [0x4ce89c]
0041dce1  test       esi, esi
0041dce3  je         0x41dd24

// block 0041dce5
0041dce5  test       dword ptr [0x4cee78], 0x8000
0041dcef  je         0x41dd02

// block 0041dcf1
0041dcf1  push       0x4cf0f8
0041dcf6  call       dword ptr [0x498088]

// block 0041dcfc
0041dcfc  inc        byte ptr [0x4cf218]

// block 0041dd02
0041dd02  mov        ecx, esi
0041dd04  mov        edx, edi
0041dd06  call       0x462890

// block 0041dd0b
0041dd0b  test       dword ptr [0x4cee78], 0x8000
0041dd15  je         0x41dd24

// block 0041dd17
0041dd17  push       0x4cf0f8
0041dd1c  call       ebx

// block 0041dd1e
0041dd1e  dec        byte ptr [0x4cf218]

// block 0041dd24
0041dd24  mov        esi, dword ptr [ebp + 0x6cd4]
0041dd2a  mov        edi, dword ptr [0x4ce89c]
0041dd30  test       esi, esi
0041dd32  je         0x41dd73

// block 0041dd34
0041dd34  test       dword ptr [0x4cee78], 0x8000
0041dd3e  je         0x41dd51

// block 0041dd40
0041dd40  push       0x4cf0f8
0041dd45  call       dword ptr [0x498088]

// block 0041dd4b
0041dd4b  inc        byte ptr [0x4cf218]

// block 0041dd51
0041dd51  mov        ecx, esi
0041dd53  mov        edx, edi
0041dd55  call       0x462890

// block 0041dd5a
0041dd5a  test       dword ptr [0x4cee78], 0x8000
0041dd64  je         0x41dd73

// block 0041dd66
0041dd66  push       0x4cf0f8
0041dd6b  call       ebx

// block 0041dd6d
0041dd6d  dec        byte ptr [0x4cf218]

// block 0041dd73
0041dd73  mov        dword ptr [ebp + 8], 0
0041dd7a  mov        eax, dword ptr [ebp + 0x6ca4]
0041dd80  push       eax
0041dd81  call       0x461a70

// block 0041dd86
0041dd86  xor        edi, edi
0041dd88  mov        dword ptr [ebp + 0x6ca4], edi
0041dd8e  mov        ecx, dword ptr [ebp + 0x6cb0]
0041dd94  push       ecx
0041dd95  call       0x461a70

// block 0041dd9a
0041dd9a  mov        dword ptr [ebp + 0x6cb0], edi
0041dda0  lea        esi, [ebp + 0x6c40]
0041dda6  lea        ebx, [edi + 0xa]
0041dda9  lea        esp, [esp]

// block 0041ddb0
0041ddb0  mov        edx, dword ptr [esi]
0041ddb2  cmp        edx, edi
0041ddb4  je         0x41de1f

// block 0041ddb6
0041ddb6  mov        eax, dword ptr [0x4ce8cc]
0041ddbb  mov        eax, dword ptr [eax + 0x8856b8]
0041ddc1  cmp        eax, edi
0041ddc3  je         0x41ddd2

// block 0041ddc5
0041ddc5  mov        ecx, dword ptr [eax]
0041ddc7  cmp        dword ptr [ecx], edx
0041ddc9  je         0x41ddf1

// block 0041ddcb
0041ddcb  mov        eax, dword ptr [eax + 4]
0041ddce  cmp        eax, edi
0041ddd0  jne        0x41ddc5

// block 0041ddd2
0041ddd2  mov        ecx, dword ptr [0x4ce8cc]
0041ddd8  mov        eax, dword ptr [ecx + 0x8856c0]
0041ddde  cmp        eax, edi
0041dde0  je         0x41de1f

// block 0041dde2
0041dde2  mov        ecx, dword ptr [eax]
0041dde4  cmp        dword ptr [ecx], edx
0041dde6  je         0x41ddf1

// block 0041dde8
0041dde8  mov        eax, dword ptr [eax + 4]
0041ddeb  cmp        eax, edi
0041dded  jne        0x41dde2

// block 0041ddef
0041ddef  jmp        0x41de1f

// block 0041ddf1
0041ddf1  mov        eax, ecx
0041ddf3  cmp        eax, edi
0041ddf5  je         0x41de1f

// block 0041ddf7
0041ddf7  mov        edx, 0x10000000
0041ddfc  or         dword ptr [eax + 0x47c], edx
0041de02  cmp        dword ptr [eax + 0x18], edi
0041de05  jne        0x41de1f

// block 0041de07
0041de07  mov        eax, dword ptr [eax + 0x14]
0041de0a  cmp        eax, edi
0041de0c  je         0x41de1f

// block 0041de0e
0041de0e  mov        edi, edi

// block 0041de10
0041de10  mov        ecx, dword ptr [eax]
0041de12  or         dword ptr [ecx + 0x47c], edx
0041de18  mov        eax, dword ptr [eax + 4]
0041de1b  cmp        eax, edi
0041de1d  jne        0x41de10

// block 0041de1f
0041de1f  mov        dword ptr [esi], edi
0041de21  add        esi, 4
0041de24  sub        ebx, 1
0041de27  jne        0x41ddb0

// block 0041de29
0041de29  mov        esi, dword ptr [0x4ce8cc]
0041de2f  mov        eax, dword ptr [esi + 0x8856b8]
0041de35  mov        edx, dword ptr [ebp + 0x6d44]
0041de3b  cmp        eax, edi
0041de3d  je         0x41de5d

// block 0041de3f
0041de3f  nop        

// block 0041de40
0041de40  mov        ecx, dword ptr [eax + 4]
0041de43  mov        eax, dword ptr [eax]
0041de45  cmp        dword ptr [eax + 0x3f8], edx
0041de4b  jne        0x41de57

// block 0041de4d
0041de4d  or         dword ptr [eax + 0x47c], 0x10000000

// block 0041de57
0041de57  mov        eax, ecx
0041de59  cmp        ecx, edi
0041de5b  jne        0x41de40

// block 0041de5d
0041de5d  mov        eax, dword ptr [esi + 0x8856c0]
0041de63  cmp        eax, edi
0041de65  je         0x41de8d

// block 0041de67
0041de67  jmp        0x41de70

// block 0041de70
0041de70  mov        ecx, dword ptr [eax + 4]
0041de73  mov        eax, dword ptr [eax]
0041de75  cmp        dword ptr [eax + 0x3f8], edx
0041de7b  jne        0x41de87

// block 0041de7d
0041de7d  or         dword ptr [eax + 0x47c], 0x10000000

// block 0041de87
0041de87  mov        eax, ecx
0041de89  cmp        ecx, edi
0041de8b  jne        0x41de70

// block 0041de8d
0041de8d  mov        eax, dword ptr [ebp + 0x6c04]
0041de93  mov        dword ptr [0x4b43e4], edi
0041de99  cmp        eax, edi
0041de9b  je         0x41dea6

// block 0041de9d
0041de9d  push       eax
0041de9e  call       0x46c941

// block 0041dea3
0041dea3  add        esp, 4

// block 0041dea6
0041dea6  push       0x4026e0
0041deab  push       4
0041dead  push       0x4b4
0041deb2  lea        edx, [ebp + 0x54b8]
0041deb8  push       edx
0041deb9  mov        dword ptr [ebp + 0x6c04], edi
0041debf  mov        byte ptr [esp + 0x2c], 2
0041dec4  call       0x46cf1e

// block 0041dec9
0041dec9  push       0x4026e0
0041dece  push       2
0041ded0  push       0x4b4
0041ded5  lea        eax, [ebp + 0x4b50]
0041dedb  push       eax
0041dedc  mov        byte ptr [esp + 0x2c], 1
0041dee1  call       0x46cf1e

// block 0041dee6
0041dee6  push       0x4026e0
0041deeb  push       8
0041deed  push       0x4b4
0041def2  lea        ecx, [ebp + 0x25b0]
0041def8  push       ecx
0041def9  mov        byte ptr [esp + 0x2c], 0
0041defe  call       0x46cf1e

// block 0041df03
0041df03  push       0x4026e0
0041df08  push       8
0041df0a  push       0x4b4
0041df0f  add        ebp, 0x10
0041df12  push       ebp
0041df13  mov        dword ptr [esp + 0x2c], 0xffffffff
0041df1b  call       0x46cf1e

// block 0041df20
0041df20  mov        ecx, dword ptr [esp + 0x14]
0041df24  mov        dword ptr fs:[0], ecx
0041df2b  pop        ecx
0041df2c  pop        edi
0041df2d  pop        esi
0041df2e  pop        ebp
0041df2f  pop        ebx
0041df30  add        esp, 0xc
0041df33  ret        4
