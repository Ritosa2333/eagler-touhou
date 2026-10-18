0052cd10 55                       push       ebp
0052cd11 8bec                     mov        ebp, esp
0052cd13 83ec0c                   sub        esp, 0xc
0052cd16 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cd19 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cd1c 0564010000               add        eax, 0x164
0052cd21 8945f8                   mov        dword ptr [ebp - 8], eax
0052cd24 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cd27 51                       push       ecx
0052cd28 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cd2b e85078eeff               call       0x414580
0052cd30 8b10                     mov        edx, dword ptr [eax]
0052cd32 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052cd35 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052cd38 50                       push       eax
0052cd39 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052cd3c 51                       push       ecx
0052cd3d 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052cd43 e87822f2ff               call       0x44efc0
0052cd48 90                       nop        
0052cd49 8be5                     mov        esp, ebp
0052cd4b 5d                       pop        ebp
0052cd4c c20800                   ret        8
0052cd4f cc                       int3       
