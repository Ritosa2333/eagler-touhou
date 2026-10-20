
// block 0045ae50
0045ae50  push       ebx
0045ae51  push       ebp
0045ae52  mov        ebp, dword ptr [esp + 0x10]
0045ae56  push       esi
0045ae57  push       edi
0045ae58  push       ebp
0045ae59  mov        edx, 0x4d483c
0045ae5e  mov        esi, 0x4d4820
0045ae63  mov        edi, 0x4d4804
0045ae68  mov        ebx, 0x4d47e8
0045ae6d  call       0x45aa30

// block 0045ae72
0045ae72  mov        ecx, dword ptr [esp + 0x14]
0045ae76  push       0
0045ae78  mov        eax, ebp
0045ae7a  call       0x459e50

// block 0045ae7f
0045ae7f  pop        edi
0045ae80  pop        esi
0045ae81  pop        ebp
0045ae82  pop        ebx
0045ae83  ret        8
