0052cc90 55                       push       ebp
0052cc91 8bec                     mov        ebp, esp
0052cc93 83ec0c                   sub        esp, 0xc
0052cc96 894dfc                   mov        dword ptr [ebp - 4], ecx
0052cc99 8b45fc                   mov        eax, dword ptr [ebp - 4]
0052cc9c 0564010000               add        eax, 0x164
0052cca1 8945f8                   mov        dword ptr [ebp - 8], eax
0052cca4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052cca7 51                       push       ecx
0052cca8 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052ccab e8d078eeff               call       0x414580
0052ccb0 8b10                     mov        edx, dword ptr [eax]
0052ccb2 8955f4                   mov        dword ptr [ebp - 0xc], edx
0052ccb5 6a03                     push       3
0052ccb7 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052ccba 50                       push       eax
0052ccbb 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052ccc1 e8fa22f2ff               call       0x44efc0
0052ccc6 90                       nop        
0052ccc7 8be5                     mov        esp, ebp
0052ccc9 5d                       pop        ebp
0052ccca c20400                   ret        4
0052cccd cc                       int3       
0052ccce cc                       int3       
0052cccf cc                       int3       
