
// block 004657a0
004657a0  push       -1
004657a2  push       0x496e4b
004657a7  mov        eax, dword ptr fs:[0]
004657ad  push       eax
004657ae  sub        esp, 0x4c
004657b1  mov        eax, dword ptr [0x4ad138]
004657b6  xor        eax, esp
004657b8  mov        dword ptr [esp + 0x48], eax
004657bc  push       ebx
004657bd  push       ebp
004657be  push       esi
004657bf  push       edi
004657c0  mov        eax, dword ptr [0x4ad138]
004657c5  xor        eax, esp
004657c7  push       eax
004657c8  lea        eax, [esp + 0x60]
004657cc  mov        dword ptr fs:[0], eax
004657d2  mov        edx, dword ptr [esp + 0x74]
004657d6  mov        eax, dword ptr [esp + 0x70]
004657da  mov        ebx, dword ptr [esp + 0x8c]
004657e1  mov        dword ptr [esp + 0x20], ecx
004657e5  mov        ecx, dword ptr [esp + 0x78]
004657e9  mov        dword ptr [esp + 0x4c], edx
004657ed  mov        edx, dword ptr [esp + 0x7c]
004657f1  mov        dword ptr [esp + 0x50], ecx
004657f5  mov        ecx, dword ptr [esp + 0x80]
004657fc  xor        esi, esi
004657fe  mov        dword ptr [esp + 0x1c], eax
00465802  mov        dword ptr [esp + 0x54], edx
00465806  mov        dword ptr [esp + 0x58], ecx
0046580a  cmp        dword ptr [eax], esi
0046580c  jne        0x465818

// block 0046580e
0046580e  mov        eax, 0x800401f0
00465813  jmp        0x465a48

// block 00465818
00465818  push       0x9c
0046581d  mov        dword ptr [esp + 0x1c], esi
00465821  mov        dword ptr [esp + 0x18], esi
00465825  call       0x46c9ea

// block 0046582a
0046582a  add        esp, 4
0046582d  cmp        eax, esi
0046582f  je         0x465843

// block 00465831
00465831  mov        dword ptr [eax + 0x90], esi
00465837  mov        dword ptr [eax], esi
00465839  mov        dword ptr [eax + 0x2c], esi
0046583c  mov        dword ptr [eax + 0x7c], esi
0046583f  mov        ebp, eax
00465841  jmp        0x465845

// block 00465843
00465843  xor        ebp, ebp

// block 00465845
00465845  mov        dword ptr [ebp + 0x7c], esi
00465848  mov        edi, 0x4a07e8
0046584d  mov        esi, ebp
0046584f  mov        dword ptr [ebp + 0x78], 1
00465856  call       0x466b10

// block 0046585b
0046585b  test       eax, eax
0046585d  je         0x46588f

// block 0046585f
0046585f  cmp        dword ptr [ebp + 0x78], 1
00465863  jne        0x46587c

// block 00465865
00465865  mov        edx, dword ptr [ebp + 0x8c]
0046586b  push       edx
0046586c  call       dword ptr [0x4980a4]

// block 00465872
00465872  mov        dword ptr [ebp + 0x8c], 0xffffffff

// block 0046587c
0046587c  push       ebp
0046587d  call       0x46ca4f

// block 00465882
00465882  add        esp, 4

// block 00465885
00465885  mov        eax, 0x80004005
0046588a  jmp        0x465a48

// block 0046588f
0046588f  mov        edx, dword ptr [esp + 0x54]
00465893  mov        ebx, dword ptr [esp + 0x84]
0046589a  mov        ecx, dword ptr [esp + 0x50]
0046589e  xor        eax, eax
004658a0  mov        dword ptr [esp + 0x28], eax
004658a4  mov        dword ptr [esp + 0x2c], eax
004658a8  mov        dword ptr [esp + 0x30], eax
004658ac  mov        dword ptr [esp + 0x38], eax
004658b0  mov        dword ptr [esp + 0x3c], eax
004658b4  mov        dword ptr [esp + 0x40], eax
004658b8  mov        dword ptr [esp + 0x44], eax
004658bc  mov        dword ptr [esp + 0x48], eax
004658c0  mov        dword ptr [esp + 0x34], eax
004658c4  mov        eax, dword ptr [esp + 0x4c]
004658c8  mov        dword ptr [esp + 0x3c], eax
004658cc  mov        eax, dword ptr [esp + 0x58]
004658d0  mov        dword ptr [esp + 0x44], edx
004658d4  mov        edx, dword ptr [esp + 0x1c]
004658d8  mov        dword ptr [esp + 0x48], eax
004658dc  mov        eax, dword ptr [edx]
004658de  mov        esi, ebx
004658e0  mov        dword ptr [esp + 0x40], ecx
004658e4  push       0
004658e6  shl        esi, 4
004658e9  mov        dword ptr [esp + 0x2c], 0x24
004658f1  mov        dword ptr [esp + 0x30], 0x18188
004658f9  mov        dword ptr [esp + 0x34], esi
004658fd  mov        ecx, dword ptr [ebp + 0x90]
00465903  lea        edx, [esp + 0x1c]
00465907  push       edx
00465908  add        ecx, 0x20
0046590b  lea        edx, [esp + 0x30]
0046590f  mov        dword ptr [esp + 0x40], ecx
00465913  mov        ecx, dword ptr [eax]
00465915  push       edx
00465916  push       eax
00465917  mov        eax, dword ptr [ecx + 0xc]
0046591a  call       eax

// block 0046591c
0046591c  test       eax, eax
0046591e  jl         0x465885

// block 00465924
00465924  mov        eax, dword ptr [esp + 0x18]
00465928  mov        ecx, dword ptr [eax]
0046592a  lea        edx, [esp + 0x14]
0046592e  push       edx
0046592f  push       0x49981c
00465934  push       eax
00465935  mov        eax, dword ptr [ecx]
00465937  call       eax

// block 00465939
00465939  test       eax, eax
0046593b  jl         0x465885

// block 00465941
00465941  xor        ecx, ecx
00465943  mov        eax, 0x10
00465948  mov        edx, 8
0046594d  mul        edx
0046594f  seto       cl
00465952  neg        ecx
00465954  or         ecx, eax
00465956  push       ecx
00465957  call       0x46c9ea

// block 0046595c
0046595c  mov        edi, eax
0046595e  add        esp, 4
00465961  test       edi, edi
00465963  jne        0x46596f

// block 00465965
00465965  mov        eax, 0x8007000e
0046596a  jmp        0x465a48

// block 0046596f
0046596f  xor        eax, eax
00465971  lea        ecx, [ebx - 1]
00465974  jmp        0x465980

// block 00465980
00465980  mov        edx, dword ptr [esp + 0x88]
00465987  mov        dword ptr [edi + eax*8], ecx
0046598a  mov        dword ptr [edi + eax*8 + 4], edx
0046598e  inc        eax
0046598f  add        ecx, ebx
00465991  cmp        eax, 0x10
00465994  jb         0x465980

// block 00465996
00465996  mov        eax, dword ptr [esp + 0x14]
0046599a  mov        ecx, dword ptr [eax]
0046599c  mov        edx, dword ptr [ecx + 0xc]
0046599f  push       edi
004659a0  push       0x10
004659a2  push       eax
004659a3  call       edx

// block 004659a5
004659a5  test       eax, eax
004659a7  mov        eax, dword ptr [esp + 0x14]
004659ab  jge        0x4659d1

// block 004659ad
004659ad  test       eax, eax
004659af  je         0x4659c1

// block 004659b1
004659b1  mov        ecx, dword ptr [eax]
004659b3  mov        edx, dword ptr [ecx + 8]
004659b6  push       eax
004659b7  call       edx

// block 004659b9
004659b9  mov        dword ptr [esp + 0x14], 0

// block 004659c1
004659c1  push       edi
004659c2  call       0x46ca4f

// block 004659c7
004659c7  add        esp, 4
004659ca  mov        eax, 0x80004005
004659cf  jmp        0x465a48

// block 004659d1
004659d1  test       eax, eax
004659d3  je         0x4659e5

// block 004659d5
004659d5  mov        ecx, dword ptr [eax]
004659d7  mov        edx, dword ptr [ecx + 8]
004659da  push       eax
004659db  call       edx

// block 004659dd
004659dd  mov        dword ptr [esp + 0x14], 0

// block 004659e5
004659e5  push       edi
004659e6  call       0x46ca4f

// block 004659eb
004659eb  push       0x7c
004659ed  call       0x46c9ea

// block 004659f2
004659f2  mov        edx, eax
004659f4  add        esp, 8
004659f7  mov        dword ptr [esp + 0x24], edx
004659fb  xor        eax, eax
004659fd  mov        dword ptr [esp + 0x68], eax
00465a01  cmp        edx, eax
00465a03  je         0x465a14

// block 00465a05
00465a05  mov        eax, dword ptr [esp + 0x18]
00465a09  push       ebx
00465a0a  push       eax
00465a0b  mov        eax, ebp
00465a0d  mov        ecx, esi
00465a0f  call       0x466650

// block 00465a14
00465a14  mov        edx, dword ptr [esp + 0x20]
00465a18  mov        dword ptr [edx], eax
00465a1a  lea        edi, [eax + 0x38]
00465a1d  mov        eax, dword ptr [esp + 0x1c]
00465a21  mov        ecx, 9
00465a26  lea        esi, [esp + 0x28]

// block 00465a2a
00465a2a  rep movsd  dword ptr es:[edi], dword ptr [esi]

// block 00465a2c
00465a2c  mov        ecx, dword ptr [edx]
00465a2e  mov        dword ptr [ecx + 0x5c], eax
00465a31  mov        ecx, dword ptr [edx]
00465a33  mov        eax, dword ptr [esp + 0x88]
00465a3a  mov        dword ptr [ecx + 0x74], eax
00465a3d  mov        ecx, dword ptr [edx]
00465a3f  mov        dword ptr [ecx + 0x78], 0
00465a46  xor        eax, eax

// block 00465a48
00465a48  mov        ecx, dword ptr [esp + 0x60]
00465a4c  mov        dword ptr fs:[0], ecx
00465a53  pop        ecx
00465a54  pop        edi
00465a55  pop        esi
00465a56  pop        ebp
00465a57  pop        ebx
00465a58  mov        ecx, dword ptr [esp + 0x48]
00465a5c  xor        ecx, esp
00465a5e  call       0x46c932

// block 00465a63
00465a63  add        esp, 0x58
00465a66  ret        0x20
