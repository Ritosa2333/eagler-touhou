0052cbf0 55                       push       ebp
0052cbf1 8bec                     mov        ebp, esp
0052cbf3 83ec0c                   sub        esp, 0xc
0052cbf6 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cbf9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cbfc 0564010000               add        eax, 0x164
0052cc01 8945f8                   mov        dword ptr [ebp - 8], eax
0052cc04 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cc07 51                       push       ecx
0052cc08 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052cc0b e87079eeff               call       0x414580
0052cc10 8b10                     mov        edx, dword ptr [eax]
0052cc12 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052cc15 6a02                     push       2
0052cc17 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052cc1a 50                       push       eax
0052cc1b 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052cc21 e89a23f2ff               call       0x44efc0
0052cc26 90                       nop        
0052cc27 8be5                     mov        esp, ebp
0052cc29 5d                       pop        ebp
0052cc2a c20400                   ret        4
0052cc2d cc                       int3       
0052cc2e cc                       int3       
0052cc2f cc                       int3       
