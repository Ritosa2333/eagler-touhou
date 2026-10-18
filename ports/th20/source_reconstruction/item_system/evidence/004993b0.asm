004993b0 55                       push       ebp
004993b1 8bec                     mov        ebp, esp
004993b3 83ec0c                   sub        esp, 0xc
004993b6 894dfc                   mov        dword ptr [ebp - 4], ecx
004993b9 c745f890010000           mov        dword ptr [ebp - 8], 0x190
004993c0 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004993c7 8d45f8                   lea        eax, [ebp - 8]
004993ca 50                       push       eax
004993cb 8d4df4                   lea        ecx, [ebp - 0xc]
004993ce 51                       push       ecx
004993cf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004993d2 83c230                   add        edx, 0x30
004993d5 52                       push       edx
004993d6 e8957dfdff               call       0x471170
004993db 83c40c                   add        esp, 0xc
004993de 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004993e1 8b10                     mov        edx, dword ptr [eax]
004993e3 895130                   mov        dword ptr [ecx + 0x30], edx
004993e6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004993e9 8b4030                   mov        eax, dword ptr [eax + 0x30]
004993ec 8be5                     mov        esp, ebp
004993ee 5d                       pop        ebp
004993ef c3                       ret        
