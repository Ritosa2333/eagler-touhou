00466990 55                       push       ebp
00466991 8bec                     mov        ebp, esp
00466993 51                       push       ecx
00466994 894dfc                   mov        dword ptr [ebp - 4], ecx
00466997 8b4508                   mov        eax, dword ptr [ebp + 8]
0046699a 50                       push       eax
0046699b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046699e e8bd01feff               call       0x446b60
004669a3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004669a6 c7010cf25600             mov        dword ptr [ecx], 0x56f20c
004669ac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004669af 83c108                   add        ecx, 8
004669b2 e879ffffff               call       0x466930
004669b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004669ba 81c1082d0000             add        ecx, 0x2d08
004669c0 e8cbc3fbff               call       0x422d90
004669c5 90                       nop        
004669c6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004669c9 8be5                     mov        esp, ebp
004669cb 5d                       pop        ebp
004669cc c20400                   ret        4
004669cf cc                       int3       
