
// block 0044eaf0
0044eaf0  push       ebx
0044eaf1  mov        ebx, dword ptr [esp + 8]
0044eaf5  push       ebp
0044eaf6  mov        ebp, dword ptr [esp + 0x10]
0044eafa  push       esi
0044eafb  push       edi
0044eafc  mov        esi, ecx
0044eafe  cmp        dword ptr [ebx + 0x14], ebp
0044eb01  jae        0x44eb08

// block 0044eb03
0044eb03  call       0x4916e6

// block 0044eb08
0044eb08  mov        edi, dword ptr [ebx + 0x14]
0044eb0b  mov        eax, dword ptr [esp + 0x1c]
0044eb0f  sub        edi, ebp
0044eb11  cmp        eax, edi
0044eb13  jae        0x44eb17

// block 0044eb15
0044eb15  mov        edi, eax

// block 0044eb17
0044eb17  cmp        esi, ebx
0044eb19  jne        0x44eb3a

// block 0044eb1b
0044eb1b  push       -1
0044eb1d  add        edi, ebp
0044eb1f  push       edi
0044eb20  mov        ecx, esi
0044eb22  call       0x44ed60

// block 0044eb27
0044eb27  push       ebp
0044eb28  push       0
0044eb2a  mov        ecx, esi
0044eb2c  call       0x44ed60

// block 0044eb31
0044eb31  pop        edi
0044eb32  mov        eax, esi
0044eb34  pop        esi
0044eb35  pop        ebp
0044eb36  pop        ebx
0044eb37  ret        0xc

// block 0044eb3a
0044eb3a  cmp        edi, -2
0044eb3d  jbe        0x44eb44

// block 0044eb3f
0044eb3f  call       0x491687

// block 0044eb44
0044eb44  mov        eax, dword ptr [esi + 0x18]
0044eb47  cmp        eax, edi
0044eb49  jae        0x44eb66

// block 0044eb4b
0044eb4b  mov        eax, dword ptr [esi + 0x14]
0044eb4e  push       eax
0044eb4f  push       edi
0044eb50  mov        ecx, esi
0044eb52  call       0x44ef10

// block 0044eb57
0044eb57  test       edi, edi

// block 0044eb59
0044eb59  jbe        0x44ebc1

// block 0044eb5b
0044eb5b  cmp        dword ptr [ebx + 0x18], 0x10
0044eb5f  jb         0x44eb90

// block 0044eb61
0044eb61  mov        edx, dword ptr [ebx + 4]
0044eb64  jmp        0x44eb93

// block 0044eb66
0044eb66  test       edi, edi
0044eb68  jne        0x44eb59

// block 0044eb6a
0044eb6a  mov        dword ptr [esi + 0x14], edi
0044eb6d  cmp        eax, 0x10
0044eb70  jb         0x44eb81

// block 0044eb72
0044eb72  mov        eax, dword ptr [esi + 4]
0044eb75  pop        edi
0044eb76  mov        byte ptr [eax], 0
0044eb79  mov        eax, esi
0044eb7b  pop        esi
0044eb7c  pop        ebp
0044eb7d  pop        ebx
0044eb7e  ret        0xc

// block 0044eb81
0044eb81  lea        eax, [esi + 4]
0044eb84  pop        edi
0044eb85  mov        byte ptr [eax], 0
0044eb88  mov        eax, esi
0044eb8a  pop        esi
0044eb8b  pop        ebp
0044eb8c  pop        ebx
0044eb8d  ret        0xc

// block 0044eb90
0044eb90  lea        edx, [ebx + 4]

// block 0044eb93
0044eb93  mov        ecx, dword ptr [esi + 0x18]
0044eb96  lea        ebx, [esi + 4]
0044eb99  cmp        ecx, 0x10
0044eb9c  jb         0x44eba2

// block 0044eb9e
0044eb9e  mov        eax, dword ptr [ebx]
0044eba0  jmp        0x44eba4

// block 0044eba2
0044eba2  mov        eax, ebx

// block 0044eba4
0044eba4  push       edi
0044eba5  add        edx, ebp
0044eba7  push       edx
0044eba8  push       ecx
0044eba9  push       eax
0044ebaa  call       0x46e210

// block 0044ebaf
0044ebaf  add        esp, 0x10
0044ebb2  cmp        dword ptr [esi + 0x18], 0x10
0044ebb6  mov        dword ptr [esi + 0x14], edi
0044ebb9  jb         0x44ebbd

// block 0044ebbb
0044ebbb  mov        ebx, dword ptr [ebx]

// block 0044ebbd
0044ebbd  mov        byte ptr [ebx + edi], 0

// block 0044ebc1
0044ebc1  pop        edi
0044ebc2  mov        eax, esi
0044ebc4  pop        esi
0044ebc5  pop        ebp
0044ebc6  pop        ebx
0044ebc7  ret        0xc
