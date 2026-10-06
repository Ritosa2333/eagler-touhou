
// block 0044f060
0044f060  push       esi
0044f061  push       edi
0044f062  mov        edi, dword ptr [esp + 0xc]
0044f066  test       edi, edi
0044f068  je         0x44f096

// block 0044f06a
0044f06a  mov        esi, dword ptr [ecx + 0x18]
0044f06d  lea        eax, [ecx + 4]
0044f070  cmp        esi, 0x10
0044f073  jb         0x44f079

// block 0044f075
0044f075  mov        edx, dword ptr [eax]
0044f077  jmp        0x44f07b

// block 0044f079
0044f079  mov        edx, eax

// block 0044f07b
0044f07b  cmp        edi, edx
0044f07d  jb         0x44f096

// block 0044f07f
0044f07f  cmp        esi, 0x10
0044f082  jb         0x44f086

// block 0044f084
0044f084  mov        eax, dword ptr [eax]

// block 0044f086
0044f086  mov        ecx, dword ptr [ecx + 0x14]
0044f089  add        ecx, eax
0044f08b  cmp        ecx, edi
0044f08d  jbe        0x44f096

// block 0044f08f
0044f08f  pop        edi
0044f090  mov        al, 1
0044f092  pop        esi
0044f093  ret        4

// block 0044f096
0044f096  pop        edi
0044f097  xor        al, al
0044f099  pop        esi
0044f09a  ret        4
