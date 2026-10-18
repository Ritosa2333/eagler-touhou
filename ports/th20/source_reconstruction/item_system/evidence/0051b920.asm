0051b920 55                       push       ebp
0051b921 8bec                     mov        ebp, esp
0051b923 83ec0c                   sub        esp, 0xc
0051b926 894dfc                   mov        dword ptr [ebp - 4], ecx
0051b929 c745f8f4010000           mov        dword ptr [ebp - 8], 0x1f4
0051b930 c745f464000000           mov        dword ptr [ebp - 0xc], 0x64
0051b937 8d45f8                   lea        eax, [ebp - 8]
0051b93a 50                       push       eax
0051b93b 8d4df4                   lea        ecx, [ebp - 0xc]
0051b93e 51                       push       ecx
0051b93f 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051b942 83c250                   add        edx, 0x50
0051b945 52                       push       edx
0051b946 e82558f5ff               call       0x471170
0051b94b 83c40c                   add        esp, 0xc
0051b94e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051b951 8b10                     mov        edx, dword ptr [eax]
0051b953 895150                   mov        dword ptr [ecx + 0x50], edx
0051b956 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051b959 8b4050                   mov        eax, dword ptr [eax + 0x50]
0051b95c 8be5                     mov        esp, ebp
0051b95e 5d                       pop        ebp
0051b95f c3                       ret        
