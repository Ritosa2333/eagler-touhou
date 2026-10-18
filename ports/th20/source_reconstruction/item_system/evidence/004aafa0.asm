004aafa0 55                       push       ebp
004aafa1 8bec                     mov        ebp, esp
004aafa3 51                       push       ecx
004aafa4 894dfc                   mov        dword ptr [ebp - 4], ecx
004aafa7 b8e8030000               mov        eax, 0x3e8
004aafac 8be5                     mov        esp, ebp
004aafae 5d                       pop        ebp
004aafaf c3                       ret        
