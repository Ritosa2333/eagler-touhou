004de5d0 55                       push       ebp
004de5d1 8bec                     mov        ebp, esp
004de5d3 51                       push       ecx
004de5d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004de5d7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004de5da e8c1d7f2ff               call       0x40bda0
004de5df 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de5e2 c7006c205700             mov        dword ptr [eax], 0x57206c
004de5e8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004de5eb 51                       push       ecx
004de5ec e88fccf2ff               call       0x40b280
004de5f1 83c404                   add        esp, 4
004de5f4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004de5f7 8be5                     mov        esp, ebp
004de5f9 5d                       pop        ebp
004de5fa c20400                   ret        4
004de5fd cc                       int3       
004de5fe cc                       int3       
004de5ff cc                       int3       
