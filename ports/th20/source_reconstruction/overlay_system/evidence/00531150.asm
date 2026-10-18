00531150 55                       push       ebp
00531151 8bec                     mov        ebp, esp
00531153 83ec10                   sub        esp, 0x10
00531156 894df0                   mov        dword ptr [ebp - 0x10], ecx
00531159 8b4508                   mov        eax, dword ptr [ebp + 8]
0053115c 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
00531162 894dfc                   mov        dword ptr [ebp - 4], ecx
00531165 6a00                     push       0
00531167 e8c4f6f2ff               call       0x460830
0053116c 83c404                   add        esp, 4
0053116f 8bc8                     mov        ecx, eax
00531171 e8daf6f2ff               call       0x460850
00531176 50                       push       eax
00531177 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0053117a e8e1cbf2ff               call       0x45dd60
0053117f 51                       push       ecx
00531180 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00531188 f30f110424               movss      dword ptr [esp], xmm0
0053118d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531190 e80bfcf6ff               call       0x4a0da0
00531195 8bc8                     mov        ecx, eax
00531197 e8342aefff               call       0x423bd0
0053119c d95df8                   fstp       dword ptr [ebp - 8]
0053119f f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
005311a4 f30f590580fb5600         mulss      xmm0, dword ptr [0x56fb80]
005311ac f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
005311b1 e8aa300000               call       0x534260
005311b6 f30f2ac0                 cvtsi2ss   xmm0, eax
005311ba f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
005311bf f30f5ec8                 divss      xmm1, xmm0
005311c3 51                       push       ecx
005311c4 f30f110c24               movss      dword ptr [esp], xmm1
005311c9 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005311cc e87ffcf3ff               call       0x470e50
005311d1 33c0                     xor        eax, eax
005311d3 8be5                     mov        esp, ebp
005311d5 5d                       pop        ebp
005311d6 c20400                   ret        4
005311d9 cc                       int3       
005311da cc                       int3       
005311db cc                       int3       
005311dc cc                       int3       
005311dd cc                       int3       
005311de cc                       int3       
005311df cc                       int3       
