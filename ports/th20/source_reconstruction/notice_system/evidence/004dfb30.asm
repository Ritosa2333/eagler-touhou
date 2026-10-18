004dfb30 55                       push       ebp
004dfb31 8bec                     mov        ebp, esp
004dfb33 83ec14                   sub        esp, 0x14
004dfb36 894dfc                   mov        dword ptr [ebp - 4], ecx
004dfb39 8b45fc                   mov        eax, dword ptr [ebp - 4]
004dfb3c 8b889c000000             mov        ecx, dword ptr [eax + 0x9c]
004dfb42 894df8                   mov        dword ptr [ebp - 8], ecx
004dfb45 6a00                     push       0
004dfb47 8b5508                   mov        edx, dword ptr [ebp + 8]
004dfb4a 52                       push       edx
004dfb4b 68cc1e5700               push       0x571ecc
004dfb50 8d45ec                   lea        eax, [ebp - 0x14]
004dfb53 50                       push       eax
004dfb54 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004dfb57 e824e5fbff               call       0x49e080
004dfb5c 8b08                     mov        ecx, dword ptr [eax]
004dfb5e 894df0                   mov        dword ptr [ebp - 0x10], ecx
004dfb61 8b55fc                   mov        edx, dword ptr [ebp - 4]
004dfb64 83c270                   add        edx, 0x70
004dfb67 8955f4                   mov        dword ptr [ebp - 0xc], edx
004dfb6a 8b4508                   mov        eax, dword ptr [ebp + 8]
004dfb6d 50                       push       eax
004dfb6e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004dfb71 e80a4af3ff               call       0x414580
004dfb76 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004dfb79 8908                     mov        dword ptr [eax], ecx
004dfb7b 8be5                     mov        esp, ebp
004dfb7d 5d                       pop        ebp
004dfb7e c20400                   ret        4
004dfb81 cc                       int3       
004dfb82 cc                       int3       
004dfb83 cc                       int3       
004dfb84 cc                       int3       
004dfb85 cc                       int3       
004dfb86 cc                       int3       
004dfb87 cc                       int3       
004dfb88 cc                       int3       
004dfb89 cc                       int3       
004dfb8a cc                       int3       
004dfb8b cc                       int3       
004dfb8c cc                       int3       
004dfb8d cc                       int3       
004dfb8e cc                       int3       
004dfb8f cc                       int3       
