00535260 55                       push       ebp
00535261 8bec                     mov        ebp, esp
00535263 83ec14                   sub        esp, 0x14
00535266 56                       push       esi
00535267 894dfc                   mov        dword ptr [ebp - 4], ecx
0053526a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053526d 83c014                   add        eax, 0x14
00535270 8945f8                   mov        dword ptr [ebp - 8], eax
00535273 6a00                     push       0
00535275 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00535278 e813e3eeff               call       0x423590
0053527d 0fb6c8                   movzx      ecx, al
00535280 85c9                     test       ecx, ecx
00535282 7409                     je         0x53528d
00535284 b801000000               mov        eax, 1
00535289 eb75                     jmp        0x535300
0053528b eb5e                     jmp        0x5352eb
0053528d b970695c00               mov        ecx, 0x5c6970
00535292 e8999ef1ff               call       0x44f130
00535297 b970695c00               mov        ecx, 0x5c6970
0053529c e88fa7f1ff               call       0x44fa30
005352a1 6a00                     push       0
005352a3 e8d8edf2ff               call       0x464080
005352a8 83c404                   add        esp, 4
005352ab 8945f0                   mov        dword ptr [ebp - 0x10], eax
005352ae 6a00                     push       0
005352b0 e8cbedf2ff               call       0x464080
005352b5 83c404                   add        esp, 4
005352b8 8bc8                     mov        ecx, eax
005352ba e86166feff               call       0x51b920
005352bf 8bf0                     mov        esi, eax
005352c1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005352c4 83c114                   add        ecx, 0x14
005352c7 e8c4acedff               call       0x40ff90
005352cc 0faff0                   imul       esi, eax
005352cf e88cefffff               call       0x534260
005352d4 8bc8                     mov        ecx, eax
005352d6 8bc6                     mov        eax, esi
005352d8 99                       cdq        
005352d9 f7f9                     idiv       ecx
005352db 8945f4                   mov        dword ptr [ebp - 0xc], eax
005352de 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005352e1 52                       push       edx
005352e2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005352e5 e85696f8ff               call       0x4be940
005352ea 90                       nop        
005352eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
005352ee 83c014                   add        eax, 0x14
005352f1 8945ec                   mov        dword ptr [ebp - 0x14], eax
005352f4 6a00                     push       0
005352f6 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
005352f9 e82241efff               call       0x429420
005352fe 33c0                     xor        eax, eax
00535300 5e                       pop        esi
00535301 8be5                     mov        esp, ebp
00535303 5d                       pop        ebp
00535304 c3                       ret        
00535305 cc                       int3       
00535306 cc                       int3       
00535307 cc                       int3       
00535308 cc                       int3       
00535309 cc                       int3       
0053530a cc                       int3       
0053530b cc                       int3       
0053530c cc                       int3       
0053530d cc                       int3       
0053530e cc                       int3       
0053530f cc                       int3       
