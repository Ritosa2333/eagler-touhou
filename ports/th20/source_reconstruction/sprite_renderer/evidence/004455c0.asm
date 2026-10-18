004455c0 55                       push       ebp
004455c1 8bec                     mov        ebp, esp
004455c3 83ec1c                   sub        esp, 0x1c
004455c6 894dfc                   mov        dword ptr [ebp - 4], ecx
004455c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004455cc 83b8700e000600           cmp        dword ptr [eax + 0x6000e70], 0
004455d3 0f84a3000000             je         0x44567c
004455d9 b9404d5c00               mov        ecx, 0x5c4d40
004455de e84dd1fcff               call       0x412730
004455e3 8945f8                   mov        dword ptr [ebp - 8], eax
004455e6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004455e9 8b11                     mov        edx, dword ptr [ecx]
004455eb 8b8264010000             mov        eax, dword ptr [edx + 0x164]
004455f1 8945f0                   mov        dword ptr [ebp - 0x10], eax
004455f4 6844010000               push       0x144
004455f9 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004455fc 51                       push       ecx
004455fd ff55f0                   call       dword ptr [ebp - 0x10]
00445600 b9404d5c00               mov        ecx, 0x5c4d40
00445605 e826d1fcff               call       0x412730
0044560a 8945f4                   mov        dword ptr [ebp - 0xc], eax
0044560d 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00445610 8b02                     mov        eax, dword ptr [edx]
00445612 8b884c010000             mov        ecx, dword ptr [eax + 0x14c]
00445618 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0044561b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0044561e 8b82780ec007             mov        eax, dword ptr [edx + 0x7c00e78]
00445624 8945ec                   mov        dword ptr [ebp - 0x14], eax
00445627 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044562a 8b91700e0006             mov        edx, dword ptr [ecx + 0x6000e70]
00445630 d1e2                     shl        edx, 1
00445632 8955e8                   mov        dword ptr [ebp - 0x18], edx
00445635 6a1c                     push       0x1c
00445637 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0044563a 50                       push       eax
0044563b 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0044563e 51                       push       ecx
0044563f 6a04                     push       4
00445641 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00445644 52                       push       edx
00445645 ff55e4                   call       dword ptr [ebp - 0x1c]
00445648 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044564b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044564e 8b91740ec007             mov        edx, dword ptr [ecx + 0x7c00e74]
00445654 8990780ec007             mov        dword ptr [eax + 0x7c00e78], edx
0044565a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044565d c780700e000600000000     mov        dword ptr [eax + 0x6000e70], 0
00445667 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044566a 8b91c4000000             mov        edx, dword ptr [ecx + 0xc4]
00445670 83c201                   add        edx, 1
00445673 8b45fc                   mov        eax, dword ptr [ebp - 4]
00445676 8990c4000000             mov        dword ptr [eax + 0xc4], edx
0044567c 8be5                     mov        esp, ebp
0044567e 5d                       pop        ebp
0044567f c3                       ret        
