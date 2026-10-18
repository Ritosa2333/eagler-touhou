004a0990 55                       push       ebp
004a0991 8bec                     mov        ebp, esp
004a0993 83ec0c                   sub        esp, 0xc
004a0996 894dfc                   mov        dword ptr [ebp - 4], ecx
004a0999 8b45fc                   mov        eax, dword ptr [ebp - 4]
004a099c 8b4878                   mov        ecx, dword ptr [eax + 0x78]
004a099f 51                       push       ecx
004a09a0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a09a3 8b82ec000000             mov        eax, dword ptr [edx + 0xec]
004a09a9 83c00f                   add        eax, 0xf
004a09ac 50                       push       eax
004a09ad 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004a09b3 e878e1faff               call       0x44eb30
004a09b8 8945f4                   mov        dword ptr [ebp - 0xc], eax
004a09bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a09be 81c18c000000             add        ecx, 0x8c
004a09c4 894df8                   mov        dword ptr [ebp - 8], ecx
004a09c7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a09ca 8b82ec000000             mov        eax, dword ptr [edx + 0xec]
004a09d0 50                       push       eax
004a09d1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004a09d4 e8a73bf7ff               call       0x414580
004a09d9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a09dc 8908                     mov        dword ptr [eax], ecx
004a09de 8b55fc                   mov        edx, dword ptr [ebp - 4]
004a09e1 8b427c                   mov        eax, dword ptr [edx + 0x7c]
004a09e4 83e0fb                   and        eax, 0xfffffffb
004a09e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004a09ea 89417c                   mov        dword ptr [ecx + 0x7c], eax
004a09ed 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
004a09f3 e8d8fbffff               call       0x4a05d0
004a09f8 33c0                     xor        eax, eax
004a09fa 8be5                     mov        esp, ebp
004a09fc 5d                       pop        ebp
004a09fd c3                       ret        
004a09fe cc                       int3       
004a09ff cc                       int3       
