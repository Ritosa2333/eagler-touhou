00477cf0 55                       push       ebp
00477cf1 8bec                     mov        ebp, esp
00477cf3 83ec0c                   sub        esp, 0xc
00477cf6 894dfc                   mov        dword ptr [ebp - 4], ecx
00477cf9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477cfc 83781400                 cmp        dword ptr [eax + 0x14], 0
00477d00 7426                     je         0x477d28
00477d02 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477d05 8b5114                   mov        edx, dword ptr [ecx + 0x14]
00477d08 8955f8                   mov        dword ptr [ebp - 8], edx
00477d0b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477d0e 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477d11 8b11                     mov        edx, dword ptr [ecx]
00477d13 8b4214                   mov        eax, dword ptr [edx + 0x14]
00477d16 8945f4                   mov        dword ptr [ebp - 0xc], eax
00477d19 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00477d1c 51                       push       ecx
00477d1d 8b5508                   mov        edx, dword ptr [ebp + 8]
00477d20 52                       push       edx
00477d21 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477d24 ff55f4                   call       dword ptr [ebp - 0xc]
00477d27 90                       nop        
00477d28 33c0                     xor        eax, eax
00477d2a 8be5                     mov        esp, ebp
00477d2c 5d                       pop        ebp
00477d2d c20800                   ret        8
