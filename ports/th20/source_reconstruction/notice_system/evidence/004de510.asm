004de510 55                       push       ebp
004de511 8bec                     mov        ebp, esp
004de513 51                       push       ecx
004de514 894dfc                   mov        dword ptr [ebp - 4], ecx
004de517 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de51a e881d8f2ff               call       0x40bda0
004de51f 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de522 c700fc1f5700             mov        dword ptr [eax], 0x571ffc
004de528 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004de52b 51                       push       ecx
004de52c e84fcdf2ff               call       0x40b280
004de531 83c404                   add        esp, 4
004de534 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de537 8be5                     mov        esp, ebp
004de539 5d                       pop        ebp
004de53a c20400                   ret        4
004de53d cc                       int3       
004de53e cc                       int3       
004de53f cc                       int3       
