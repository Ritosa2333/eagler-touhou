00445a40 55                       push       ebp
00445a41 8bec                     mov        ebp, esp
00445a43 83ec0c                   sub        esp, 0xc
00445a46 894dfc                   mov        dword ptr [ebp - 4], ecx
00445a49 8b45fc                   mov        eax, dword ptr [ebp - 4]
00445a4c c780700e000600000000     mov        dword ptr [eax + 0x6000e70], 0
00445a56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00445a59 81c1740e0006             add        ecx, 0x6000e74
00445a5f 894df8                   mov        dword ptr [ebp - 8], ecx
00445a62 8b55fc                   mov        edx, dword ptr [ebp - 4]
00445a65 8b45f8                   mov        eax, dword ptr [ebp - 8]
00445a68 8982740ec007             mov        dword ptr [edx + 0x7c00e74], eax
00445a6e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00445a71 8b55f8                   mov        edx, dword ptr [ebp - 8]
00445a74 8991780ec007             mov        dword ptr [ecx + 0x7c00e78], edx
00445a7a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00445a7d c7807c0ec00700000000     mov        dword ptr [eax + 0x7c00e7c], 0
00445a87 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00445a8a 81c1800ec007             add        ecx, 0x7c00e80
00445a90 894df4                   mov        dword ptr [ebp - 0xc], ecx
00445a93 8b55fc                   mov        edx, dword ptr [ebp - 4]
00445a96 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00445a99 8982800ed407             mov        dword ptr [edx + 0x7d40e80], eax
00445a9f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00445aa2 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00445aa5 8991840ed407             mov        dword ptr [ecx + 0x7d40e84], edx
00445aab 8be5                     mov        esp, ebp
00445aad 5d                       pop        ebp
00445aae c3                       ret        
00445aaf cc                       int3       
