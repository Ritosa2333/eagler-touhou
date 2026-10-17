004c4b30 55                       push       ebp
004c4b31 8bec                     mov        ebp, esp
004c4b33 83ec0c                   sub        esp, 0xc
004c4b36 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4b39 c745f840420f00           mov        dword ptr [ebp - 8], 0xf4240
004c4b40 c745f410270000           mov        dword ptr [ebp - 0xc], 0x2710
004c4b47 8d45f8                   lea        eax, [ebp - 8]
004c4b4a 50                       push       eax
004c4b4b 8d4df4                   lea        ecx, [ebp - 0xc]
004c4b4e 51                       push       ecx
004c4b4f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4b52 83c240                   add        edx, 0x40
004c4b55 52                       push       edx
004c4b56 e815c6faff               call       0x471170
004c4b5b 83c40c                   add        esp, 0xc
004c4b5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4b61 8b10                     mov        edx, dword ptr [eax]
004c4b63 895140                   mov        dword ptr [ecx + 0x40], edx
004c4b66 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4b69 8b4040                   mov        eax, dword ptr [eax + 0x40]
004c4b6c 8be5                     mov        esp, ebp
004c4b6e 5d                       pop        ebp
004c4b6f c3                       ret        
