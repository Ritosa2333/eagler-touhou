0052ccd0 55                       push       ebp
0052ccd1 8bec                     mov        ebp, esp
0052ccd3 83ec0c                   sub        esp, 0xc
0052ccd6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052ccd9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052ccdc 0564010000               add        eax, 0x164
0052cce1 8945f8                   mov        dword ptr [ebp - 8], eax
0052cce4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cce7 51                       push       ecx
0052cce8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cceb e89078eeff               call       0x414580
0052ccf0 8b10                     mov        edx, dword ptr [eax]
0052ccf2 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052ccf5 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052ccf8 50                       push       eax
0052ccf9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052ccfc 51                       push       ecx
0052ccfd 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052cd03 e88821f2ff               call       0x44ee90
0052cd08 90                       nop        
0052cd09 8be5                     mov        esp, ebp
0052cd0b 5d                       pop        ebp
0052cd0c c20800                   ret        8
0052cd0f cc                       int3       
