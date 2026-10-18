0052cf40 55                       push       ebp
0052cf41 8bec                     mov        ebp, esp
0052cf43 83ec08                   sub        esp, 8
0052cf46 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cf49 b830000000               mov        eax, 0x30
0052cf4e 6bc800                   imul       ecx, eax, 0
0052cf51 034dfc                   add        ecx, dword ptr [ebp - 4]
0052cf54 e897fbeeff               call       0x41caf0
0052cf59 8945f8                   mov        dword ptr [ebp - 8], eax
0052cf5c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cf5f 51                       push       ecx
0052cf60 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cf63 e8585eeeff               call       0x412dc0
0052cf68 90                       nop        
0052cf69 8be5                     mov        esp, ebp
0052cf6b 5d                       pop        ebp
0052cf6c c20400                   ret        4
0052cf6f cc                       int3       
