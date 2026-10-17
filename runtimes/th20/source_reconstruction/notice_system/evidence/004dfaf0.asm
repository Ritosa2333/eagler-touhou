004dfaf0 55                       push       ebp
004dfaf1 8bec                     mov        ebp, esp
004dfaf3 83ec0c                   sub        esp, 0xc
004dfaf6 894dfc                   mov        dword ptr [ebp - 4], ecx
004dfaf9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dfafc 83c070                   add        eax, 0x70
004dfaff 8945f8                   mov        dword ptr [ebp - 8], eax
004dfb02 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004dfb05 51                       push       ecx
004dfb06 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004dfb09 e8724af3ff               call       0x414580
004dfb0e 8b10                     mov        edx, dword ptr [eax]
004dfb10 8955f4                   mov        dword ptr [ebp - 0xc], edx
004dfb13 6a01                     push       1
004dfb15 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004dfb18 50                       push       eax
004dfb19 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004dfb1f e86cf3f6ff               call       0x44ee90
004dfb24 90                       nop        
004dfb25 8be5                     mov        esp, ebp
004dfb27 5d                       pop        ebp
004dfb28 c20400                   ret        4
004dfb2b cc                       int3       
004dfb2c cc                       int3       
004dfb2d cc                       int3       
004dfb2e cc                       int3       
004dfb2f cc                       int3       
