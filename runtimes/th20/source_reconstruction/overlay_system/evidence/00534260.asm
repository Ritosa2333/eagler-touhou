00534260 55                       push       ebp
00534261 8bec                     mov        ebp, esp
00534263 51                       push       ecx
00534264 6a00                     push       0
00534266 e815fef2ff               call       0x464080
0053426b 83c404                   add        esp, 4
0053426e 8bc8                     mov        ecx, eax
00534270 e86b76feff               call       0x51b8e0
00534275 69c0b0040000             imul       eax, eax, 0x4b0
0053427b 99                       cdq        
0053427c b964000000               mov        ecx, 0x64
00534281 f7f9                     idiv       ecx
00534283 8945fc                   mov        dword ptr [ebp - 4], eax
00534286 6a00                     push       0
00534288 e8f3fdf2ff               call       0x464080
0053428d 83c404                   add        esp, 4
00534290 8bc8                     mov        ecx, eax
00534292 e819000000               call       0x5342b0
00534297 0faf45fc                 imul       eax, dword ptr [ebp - 4]
0053429b 99                       cdq        
0053429c b964000000               mov        ecx, 0x64
005342a1 f7f9                     idiv       ecx
005342a3 0345fc                   add        eax, dword ptr [ebp - 4]
005342a6 8be5                     mov        esp, ebp
005342a8 5d                       pop        ebp
005342a9 c3                       ret        
005342aa cc                       int3       
005342ab cc                       int3       
005342ac cc                       int3       
005342ad cc                       int3       
005342ae cc                       int3       
005342af cc                       int3       
