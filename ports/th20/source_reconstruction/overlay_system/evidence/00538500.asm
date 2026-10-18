00538500 55                       push       ebp
00538501 8bec                     mov        ebp, esp
00538503 83ec14                   sub        esp, 0x14
00538506 56                       push       esi
00538507 894dfc                   mov        dword ptr [ebp - 4], ecx
0053850a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053850d 83c014                   add        eax, 0x14
00538510 8945f8                   mov        dword ptr [ebp - 8], eax
00538513 6a00                     push       0
00538515 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00538518 e873b0eeff               call       0x423590
0053851d 0fb6c8                   movzx      ecx, al
00538520 85c9                     test       ecx, ecx
00538522 7409                     je         0x53852d
00538524 b801000000               mov        eax, 1
00538529 eb75                     jmp        0x5385a0
0053852b eb5e                     jmp        0x53858b
0053852d b9dc695c00               mov        ecx, 0x5c69dc
00538532 e8f96bf1ff               call       0x44f130
00538537 b9dc695c00               mov        ecx, 0x5c69dc
0053853c e8ef74f1ff               call       0x44fa30
00538541 6a00                     push       0
00538543 e838bbf2ff               call       0x464080
00538548 83c404                   add        esp, 4
0053854b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0053854e 6a00                     push       0
00538550 e82bbbf2ff               call       0x464080
00538555 83c404                   add        esp, 4
00538558 8bc8                     mov        ecx, eax
0053855a e8c133feff               call       0x51b920
0053855f 8bf0                     mov        esi, eax
00538561 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538564 83c114                   add        ecx, 0x14
00538567 e8247aedff               call       0x40ff90
0053856c 0faff0                   imul       esi, eax
0053856f e8ecbcffff               call       0x534260
00538574 8bc8                     mov        ecx, eax
00538576 8bc6                     mov        eax, esi
00538578 99                       cdq        
00538579 f7f9                     idiv       ecx
0053857b 8945f4                   mov        dword ptr [ebp - 0xc], eax
0053857e 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00538581 52                       push       edx
00538582 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00538585 e8b663f8ff               call       0x4be940
0053858a 90                       nop        
0053858b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053858e 83c014                   add        eax, 0x14
00538591 8945ec                   mov        dword ptr [ebp - 0x14], eax
00538594 6a00                     push       0
00538596 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00538599 e8820eefff               call       0x429420
0053859e 33c0                     xor        eax, eax
005385a0 5e                       pop        esi
005385a1 8be5                     mov        esp, ebp
005385a3 5d                       pop        ebp
005385a4 c3                       ret        
005385a5 cc                       int3       
005385a6 cc                       int3       
005385a7 cc                       int3       
005385a8 cc                       int3       
005385a9 cc                       int3       
005385aa cc                       int3       
005385ab cc                       int3       
005385ac cc                       int3       
005385ad cc                       int3       
005385ae cc                       int3       
005385af cc                       int3       
