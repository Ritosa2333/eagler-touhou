004b82c0 55                       push       ebp
004b82c1 8bec                     mov        ebp, esp
004b82c3 83ec0c                   sub        esp, 0xc
004b82c6 894dfc                   mov        dword ptr [ebp - 4], ecx
004b82c9 c745f840420f00           mov        dword ptr [ebp - 8], 0xf4240
004b82d0 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b82d7 8d45f8                   lea        eax, [ebp - 8]
004b82da 50                       push       eax
004b82db 8d4df4                   lea        ecx, [ebp - 0xc]
004b82de 51                       push       ecx
004b82df 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b82e2 83c244                   add        edx, 0x44
004b82e5 52                       push       edx
004b82e6 e8858efbff               call       0x471170
004b82eb 83c40c                   add        esp, 0xc
004b82ee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b82f1 8b10                     mov        edx, dword ptr [eax]
004b82f3 895144                   mov        dword ptr [ecx + 0x44], edx
004b82f6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b82f9 8b4044                   mov        eax, dword ptr [eax + 0x44]
004b82fc 8be5                     mov        esp, ebp
004b82fe 5d                       pop        ebp
004b82ff c3                       ret        
