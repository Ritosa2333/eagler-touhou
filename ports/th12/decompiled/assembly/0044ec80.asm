
// block 0044ec80
0044ec80  push       ebp
0044ec81  mov        ebp, dword ptr [esp + 8]
0044ec85  push       esi
0044ec86  push       edi
0044ec87  mov        esi, ecx
0044ec89  test       ebp, ebp
0044ec8b  je         0x44ecd3

// block 0044ec8d
0044ec8d  mov        edx, dword ptr [esi + 0x18]
0044ec90  lea        eax, [esi + 4]
0044ec93  cmp        edx, 0x10
0044ec96  jb         0x44ec9c

// block 0044ec98
0044ec98  mov        ecx, dword ptr [eax]
0044ec9a  jmp        0x44ec9e

// block 0044ec9c
0044ec9c  mov        ecx, eax

// block 0044ec9e
0044ec9e  cmp        ebp, ecx
0044eca0  jb         0x44ecd3

// block 0044eca2
0044eca2  cmp        edx, 0x10
0044eca5  jb         0x44ecab

// block 0044eca7
0044eca7  mov        ecx, dword ptr [eax]
0044eca9  jmp        0x44ecad

// block 0044ecab
0044ecab  mov        ecx, eax

// block 0044ecad
0044ecad  mov        edi, dword ptr [esi + 0x14]
0044ecb0  add        edi, ecx
0044ecb2  cmp        edi, ebp
0044ecb4  jbe        0x44ecd3

// block 0044ecb6
0044ecb6  cmp        edx, 0x10
0044ecb9  jb         0x44ecbd

// block 0044ecbb
0044ecbb  mov        eax, dword ptr [eax]

// block 0044ecbd
0044ecbd  mov        ecx, dword ptr [esp + 0x14]
0044ecc1  push       ecx
0044ecc2  sub        ebp, eax
0044ecc4  push       ebp
0044ecc5  push       esi
0044ecc6  mov        ecx, esi
0044ecc8  call       0x44eaf0

// block 0044eccd
0044eccd  pop        edi
0044ecce  pop        esi
0044eccf  pop        ebp
0044ecd0  ret        8

// block 0044ecd3
0044ecd3  mov        edi, dword ptr [esp + 0x14]
0044ecd7  cmp        edi, -2
0044ecda  jbe        0x44ece1

// block 0044ecdc
0044ecdc  call       0x491687

// block 0044ece1
0044ece1  mov        eax, dword ptr [esi + 0x18]
0044ece4  cmp        eax, edi
0044ece6  jae        0x44ed08

// block 0044ece8
0044ece8  mov        edx, dword ptr [esi + 0x14]
0044eceb  push       edx
0044ecec  push       edi
0044eced  mov        ecx, esi
0044ecef  call       0x44ef10

// block 0044ecf4
0044ecf4  test       edi, edi

// block 0044ecf6
0044ecf6  jbe        0x44ed4e

// block 0044ecf8
0044ecf8  mov        ecx, dword ptr [esi + 0x18]
0044ecfb  push       ebx
0044ecfc  lea        ebx, [esi + 4]
0044ecff  cmp        ecx, 0x10
0044ed02  jb         0x44ed30

// block 0044ed04
0044ed04  mov        eax, dword ptr [ebx]
0044ed06  jmp        0x44ed32

// block 0044ed08
0044ed08  test       edi, edi
0044ed0a  jne        0x44ecf6

// block 0044ed0c
0044ed0c  mov        dword ptr [esi + 0x14], edi
0044ed0f  cmp        eax, 0x10
0044ed12  jb         0x44ed22

// block 0044ed14
0044ed14  mov        eax, dword ptr [esi + 4]
0044ed17  pop        edi
0044ed18  mov        byte ptr [eax], 0
0044ed1b  mov        eax, esi
0044ed1d  pop        esi
0044ed1e  pop        ebp
0044ed1f  ret        8

// block 0044ed22
0044ed22  lea        eax, [esi + 4]
0044ed25  pop        edi
0044ed26  mov        byte ptr [eax], 0
0044ed29  mov        eax, esi
0044ed2b  pop        esi
0044ed2c  pop        ebp
0044ed2d  ret        8

// block 0044ed30
0044ed30  mov        eax, ebx

// block 0044ed32
0044ed32  push       edi
0044ed33  push       ebp
0044ed34  push       ecx
0044ed35  push       eax
0044ed36  call       0x46e210

// block 0044ed3b
0044ed3b  add        esp, 0x10
0044ed3e  cmp        dword ptr [esi + 0x18], 0x10
0044ed42  mov        dword ptr [esi + 0x14], edi
0044ed45  jb         0x44ed49

// block 0044ed47
0044ed47  mov        ebx, dword ptr [ebx]

// block 0044ed49
0044ed49  mov        byte ptr [ebx + edi], 0
0044ed4d  pop        ebx

// block 0044ed4e
0044ed4e  pop        edi
0044ed4f  mov        eax, esi
0044ed51  pop        esi
0044ed52  pop        ebp
0044ed53  ret        8
