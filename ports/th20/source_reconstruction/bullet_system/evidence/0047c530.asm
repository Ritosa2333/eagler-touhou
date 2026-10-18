0047c530 55                       push       ebp
0047c531 8bec                     mov        ebp, esp
0047c533 83ec10                   sub        esp, 0x10
0047c536 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c539 8b4508                   mov        eax, dword ptr [ebp + 8]
0047c53c 50                       push       eax
0047c53d e83eedf8ff               call       0x40b280
0047c542 83c404                   add        esp, 4
0047c545 50                       push       eax
0047c546 8d4df0                   lea        ecx, [ebp - 0x10]
0047c549 e8d2f2ffff               call       0x47b820
0047c54e 8945f8                   mov        dword ptr [ebp - 8], eax
0047c551 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c554 51                       push       ecx
0047c555 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047c558 e833a10000               call       0x486690
0047c55d 8d4df0                   lea        ecx, [ebp - 0x10]
0047c560 e89bfdffff               call       0x47c300
0047c565 90                       nop        
0047c566 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c569 8be5                     mov        esp, ebp
0047c56b 5d                       pop        ebp
0047c56c c20400                   ret        4
0047c56f cc                       int3       
