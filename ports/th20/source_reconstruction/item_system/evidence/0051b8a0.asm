0051b8a0 55                       push       ebp
0051b8a1 8bec                     mov        ebp, esp
0051b8a3 83ec0c                   sub        esp, 0xc
0051b8a6 894dfc                   mov        dword ptr [ebp - 4], ecx
0051b8a9 c745f8f4010000           mov        dword ptr [ebp - 8], 0x1f4
0051b8b0 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0051b8b7 8d45f8                   lea        eax, [ebp - 8]
0051b8ba 50                       push       eax
0051b8bb 8d4df4                   lea        ecx, [ebp - 0xc]
0051b8be 51                       push       ecx
0051b8bf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051b8c2 83c24c                   add        edx, 0x4c
0051b8c5 52                       push       edx
0051b8c6 e8a558f5ff               call       0x471170
0051b8cb 83c40c                   add        esp, 0xc
0051b8ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051b8d1 8b10                     mov        edx, dword ptr [eax]
0051b8d3 89514c                   mov        dword ptr [ecx + 0x4c], edx
0051b8d6 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051b8d9 8b404c                   mov        eax, dword ptr [eax + 0x4c]
0051b8dc 8be5                     mov        esp, ebp
0051b8de 5d                       pop        ebp
0051b8df c3                       ret        
