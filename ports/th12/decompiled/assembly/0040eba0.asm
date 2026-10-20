
// block 0040eba0
0040eba0  push       -1
0040eba2  push       0x497779
0040eba7  mov        eax, dword ptr fs:[0]
0040ebad  push       eax
0040ebae  push       ebx
0040ebaf  push       ebp
0040ebb0  push       esi
0040ebb1  push       edi
0040ebb2  mov        eax, dword ptr [0x4ad138]
0040ebb7  xor        eax, esp
0040ebb9  push       eax
0040ebba  lea        eax, [esp + 0x14]
0040ebbe  mov        dword ptr fs:[0], eax
0040ebc4  mov        esi, dword ptr [esp + 0x24]
0040ebc8  mov        dword ptr [esp + 0x1c], 1
0040ebd0  call       0x40e850

// block 0040ebd5
0040ebd5  mov        edi, dword ptr [esi + 8]
0040ebd8  mov        ebx, dword ptr [0x4ce89c]
0040ebde  mov        ebp, dword ptr [0x498088]
0040ebe4  test       edi, edi
0040ebe6  je         0x40ec27

// block 0040ebe8
0040ebe8  test       dword ptr [0x4cee78], 0x8000
0040ebf2  je         0x40ec01

// block 0040ebf4
0040ebf4  push       0x4cf0f8
0040ebf9  call       ebp

// block 0040ebfb
0040ebfb  inc        byte ptr [0x4cf218]

// block 0040ec01
0040ec01  mov        ecx, edi
0040ec03  mov        edx, ebx
0040ec05  call       0x462890

// block 0040ec0a
0040ec0a  test       dword ptr [0x4cee78], 0x8000
0040ec14  je         0x40ec27

// block 0040ec16
0040ec16  push       0x4cf0f8
0040ec1b  call       dword ptr [0x49808c]

// block 0040ec21
0040ec21  dec        byte ptr [0x4cf218]

// block 0040ec27
0040ec27  mov        edi, dword ptr [esi + 0xc]
0040ec2a  mov        ebx, dword ptr [0x4ce89c]
0040ec30  test       edi, edi
0040ec32  je         0x40ec73

// block 0040ec34
0040ec34  test       dword ptr [0x4cee78], 0x8000
0040ec3e  je         0x40ec4d

// block 0040ec40
0040ec40  push       0x4cf0f8
0040ec45  call       ebp

// block 0040ec47
0040ec47  inc        byte ptr [0x4cf218]

// block 0040ec4d
0040ec4d  mov        ecx, edi
0040ec4f  mov        edx, ebx
0040ec51  call       0x462890

// block 0040ec56
0040ec56  test       dword ptr [0x4cee78], 0x8000
0040ec60  je         0x40ec73

// block 0040ec62
0040ec62  push       0x4cf0f8
0040ec67  call       dword ptr [0x49808c]

// block 0040ec6d
0040ec6d  dec        byte ptr [0x4cf218]

// block 0040ec73
0040ec73  mov        edi, dword ptr [0x4b43e4]
0040ec79  test       edi, edi
0040ec7b  je         0x40ec8c

// block 0040ec7d
0040ec7d  push       edi
0040ec7e  call       0x41dc50

// block 0040ec83
0040ec83  push       edi
0040ec84  call       0x46ca4f

// block 0040ec89
0040ec89  add        esp, 4

// block 0040ec8c
0040ec8c  mov        edi, dword ptr [0x4b4514]
0040ec92  test       edi, edi
0040ec94  je         0x40eca5

// block 0040ec96
0040ec96  push       edi
0040ec97  call       0x436270

// block 0040ec9c
0040ec9c  push       edi
0040ec9d  call       0x46ca4f

// block 0040eca2
0040eca2  add        esp, 4

// block 0040eca5
0040eca5  mov        edi, dword ptr [0x4b43c8]
0040ecab  test       edi, edi
0040ecad  je         0x40ecbe

// block 0040ecaf
0040ecaf  push       edi
0040ecb0  call       0x4096d0

// block 0040ecb5
0040ecb5  push       edi
0040ecb6  call       0x46ca4f

// block 0040ecbb
0040ecbb  add        esp, 4

// block 0040ecbe
0040ecbe  mov        edi, dword ptr [0x4b43dc]
0040ecc4  test       edi, edi
0040ecc6  je         0x40ecd8

// block 0040ecc8
0040ecc8  mov        eax, edi
0040ecca  call       0x412f10

// block 0040eccf
0040eccf  push       edi
0040ecd0  call       0x46ca4f

// block 0040ecd5
0040ecd5  add        esp, 4

// block 0040ecd8
0040ecd8  mov        edi, dword ptr [0x4b43c4]
0040ecde  test       edi, edi
0040ece0  je         0x40ecf1

// block 0040ece2
0040ece2  push       edi
0040ece3  call       0x406930

// block 0040ece8
0040ece8  push       edi
0040ece9  call       0x46ca4f

// block 0040ecee
0040ecee  add        esp, 4

// block 0040ecf1
0040ecf1  mov        edi, dword ptr [0x4b44f0]
0040ecf7  test       edi, edi
0040ecf9  je         0x40ed0a

// block 0040ecfb
0040ecfb  push       edi
0040ecfc  call       0x425a00

// block 0040ed01
0040ed01  push       edi
0040ed02  call       0x46ca4f

// block 0040ed07
0040ed07  add        esp, 4

// block 0040ed0a
0040ed0a  mov        edi, dword ptr [0x4b4524]
0040ed10  test       edi, edi
0040ed12  je         0x40ed23

// block 0040ed14
0040ed14  push       edi
0040ed15  call       0x43dc30

// block 0040ed1a
0040ed1a  push       edi
0040ed1b  call       0x46ca4f

// block 0040ed20
0040ed20  add        esp, 4

// block 0040ed23
0040ed23  mov        eax, dword ptr [esi + 0x740]
0040ed29  mov        dword ptr [0x4b43d0], 0
0040ed33  test       eax, eax
0040ed35  je         0x40ed40

// block 0040ed37
0040ed37  push       eax
0040ed38  call       0x46c941

// block 0040ed3d
0040ed3d  add        esp, 4

// block 0040ed40
0040ed40  mov        dword ptr [esi + 0x740], 0
0040ed4a  add        esi, 0x10
0040ed4d  mov        dword ptr [esi], 0x4a3738
0040ed53  call       0x464c40

// block 0040ed58
0040ed58  mov        ecx, dword ptr [esp + 0x14]
0040ed5c  mov        dword ptr fs:[0], ecx
0040ed63  pop        ecx
0040ed64  pop        edi
0040ed65  pop        esi
0040ed66  pop        ebp
0040ed67  pop        ebx
0040ed68  add        esp, 0xc
0040ed6b  ret        4
