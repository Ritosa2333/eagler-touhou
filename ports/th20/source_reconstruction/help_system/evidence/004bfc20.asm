004bfc20 55                       push       ebp
004bfc21 8bec                     mov        ebp, esp
004bfc23 83ec0c                   sub        esp, 0xc
004bfc26 894dfc                   mov        dword ptr [ebp - 4], ecx
004bfc29 8b45fc                   mov        eax, dword ptr [ebp - 4]
004bfc2c 8b88b0000000             mov        ecx, dword ptr [eax + 0xb0]
004bfc32 894df8                   mov        dword ptr [ebp - 8], ecx
004bfc35 6a00                     push       0
004bfc37 8b550c                   mov        edx, dword ptr [ebp + 0xc]
004bfc3a 52                       push       edx
004bfc3b 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfc3e 50                       push       eax
004bfc3f 68040c5700               push       0x570c04
004bfc44 8d4df4                   lea        ecx, [ebp - 0xc]
004bfc47 51                       push       ecx
004bfc48 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004bfc4b e8b049faff               call       0x464600
004bfc50 8b10                     mov        edx, dword ptr [eax]
004bfc52 8b4508                   mov        eax, dword ptr [ebp + 8]
004bfc55 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004bfc58 89548170                 mov        dword ptr [ecx + eax*4 + 0x70], edx
004bfc5c 8be5                     mov        esp, ebp
004bfc5e 5d                       pop        ebp
004bfc5f c20800                   ret        8
004bfc62 cc                       int3       
004bfc63 cc                       int3       
004bfc64 cc                       int3       
004bfc65 cc                       int3       
004bfc66 cc                       int3       
004bfc67 cc                       int3       
004bfc68 cc                       int3       
004bfc69 cc                       int3       
004bfc6a cc                       int3       
004bfc6b cc                       int3       
004bfc6c cc                       int3       
004bfc6d cc                       int3       
004bfc6e cc                       int3       
004bfc6f cc                       int3       
