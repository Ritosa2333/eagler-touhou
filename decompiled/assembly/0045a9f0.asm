
// block 0045a9f0
0045a9f0  push       ecx
0045a9f1  push       ebx
0045a9f2  push       esi
0045a9f3  push       edi
0045a9f4  push       0x4d4804
0045a9f9  mov        esi, eax
0045a9fb  push       0x4d47e8
0045aa00  mov        edi, 0x4d483c
0045aa05  mov        ebx, 0x4d4820
0045aa0a  call       0x45a570

// block 0045aa0f
0045aa0f  mov        ecx, dword ptr [esp + 0x14]
0045aa13  push       1
0045aa15  mov        eax, esi
0045aa17  call       0x459e50

// block 0045aa1c
0045aa1c  pop        edi
0045aa1d  pop        esi
0045aa1e  pop        ebx
0045aa1f  pop        ecx
0045aa20  ret        4
