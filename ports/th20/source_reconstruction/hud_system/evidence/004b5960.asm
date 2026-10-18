004b5960 55                       push       ebp
004b5961 8bec                     mov        ebp, esp
004b5963 83ec18                   sub        esp, 0x18
004b5966 56                       push       esi
004b5967 894dfc                   mov        dword ptr [ebp - 4], ecx
004b596a a110615c00               mov        eax, dword ptr [0x5c6110]
004b596f 8b4854                   mov        ecx, dword ptr [eax + 0x54]
004b5972 51                       push       ecx
004b5973 6a06                     push       6
004b5975 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004b597b e8b091f9ff               call       0x44eb30
004b5980 8945f8                   mov        dword ptr [ebp - 8], eax
004b5983 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b5986 8b45f8                   mov        eax, dword ptr [ebp - 8]
004b5989 898270010000             mov        dword ptr [edx + 0x170], eax
004b598f 837df800                 cmp        dword ptr [ebp - 8], 0
004b5993 751a                     jne        0x4b59af
004b5995 6810f65600               push       0x56f610
004b599a 6878065c00               push       0x5c0678
004b599f e8ace7f9ff               call       0x454150
004b59a4 83c408                   add        esp, 8
004b59a7 83c8ff                   or         eax, 0xffffffff
004b59aa e91e010000               jmp        0x4b5acd
004b59af 833d144a5c0000           cmp        dword ptr [0x5c4a14], 0
004b59b6 745b                     je         0x4b5a13
004b59b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b59bb 8b15144a5c00             mov        edx, dword ptr [0x5c4a14]
004b59c1 8991c0010000             mov        dword ptr [ecx + 0x1c0], edx
004b59c7 a110615c00               mov        eax, dword ptr [0x5c6110]
004b59cc 83c014                   add        eax, 0x14
004b59cf 8945f4                   mov        dword ptr [ebp - 0xc], eax
004b59d2 b968a55b00               mov        ecx, 0x5ba568
004b59d7 e894250000               call       0x4b7f70
004b59dc 8bf0                     mov        esi, eax
004b59de b968a55b00               mov        ecx, 0x5ba568
004b59e3 e858290000               call       0x4b8340
004b59e8 8d0cf0                   lea        ecx, [eax + esi*8]
004b59eb c1e102                   shl        ecx, 2
004b59ee 894df0                   mov        dword ptr [ebp - 0x10], ecx
004b59f1 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004b59f4 0355f0                   add        edx, dword ptr [ebp - 0x10]
004b59f7 8b02                     mov        eax, dword ptr [edx]
004b59f9 50                       push       eax
004b59fa 6864065700               push       0x570664
004b59ff e8ac6cf5ff               call       0x40c6b0
004b5a04 83c408                   add        esp, 8
004b5a07 c705144a5c0000000000     mov        dword ptr [0x5c4a14], 0
004b5a11 eb75                     jmp        0x4b5a88
004b5a13 8b0d10615c00             mov        ecx, dword ptr [0x5c6110]
004b5a19 83c114                   add        ecx, 0x14
004b5a1c 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b5a1f b968a55b00               mov        ecx, 0x5ba568
004b5a24 e817290000               call       0x4b8340
004b5a29 8bf0                     mov        esi, eax
004b5a2b b968a55b00               mov        ecx, 0x5ba568
004b5a30 e83b250000               call       0x4b7f70
004b5a35 8d14c6                   lea        edx, [esi + eax*8]
004b5a38 c1e202                   shl        edx, 2
004b5a3b 8955e8                   mov        dword ptr [ebp - 0x18], edx
004b5a3e 6a00                     push       0
004b5a40 6a00                     push       0
004b5a42 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004b5a45 0345e8                   add        eax, dword ptr [ebp - 0x18]
004b5a48 8b08                     mov        ecx, dword ptr [eax]
004b5a4a 51                       push       ecx
004b5a4b e8507a0000               call       0x4bd4a0
004b5a50 83c404                   add        esp, 4
004b5a53 50                       push       eax
004b5a54 e847b0f5ff               call       0x410aa0
004b5a59 83c40c                   add        esp, 0xc
004b5a5c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b5a5f 8982c0010000             mov        dword ptr [edx + 0x1c0], eax
004b5a65 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b5a68 83b8c001000000           cmp        dword ptr [eax + 0x1c0], 0
004b5a6f 7517                     jne        0x4b5a88
004b5a71 6810f65600               push       0x56f610
004b5a76 6878065c00               push       0x5c0678
004b5a7b e8d0e6f9ff               call       0x454150
004b5a80 83c408                   add        esp, 8
004b5a83 83c8ff                   or         eax, 0xffffffff
004b5a86 eb45                     jmp        0x4b5acd
004b5a88 6a00                     push       0
004b5a8a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b5a8d 81c140010000             add        ecx, 0x140
004b5a93 e888daf6ff               call       0x423520
004b5a98 b968a55b00               mov        ecx, 0x5ba568
004b5a9d e8ee270000               call       0x4b8290
004b5aa2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b5aa5 898160010000             mov        dword ptr [ecx + 0x160], eax
004b5aab 899164010000             mov        dword ptr [ecx + 0x164], edx
004b5ab1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b5ab4 c782c4010000ffffffff     mov        dword ptr [edx + 0x1c4], 0xffffffff
004b5abe 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b5ac1 c780cc010000ffffffff     mov        dword ptr [eax + 0x1cc], 0xffffffff
004b5acb 33c0                     xor        eax, eax
004b5acd 5e                       pop        esi
004b5ace 8be5                     mov        esp, ebp
004b5ad0 5d                       pop        ebp
004b5ad1 c3                       ret        
004b5ad2 cc                       int3       
004b5ad3 cc                       int3       
004b5ad4 cc                       int3       
004b5ad5 cc                       int3       
004b5ad6 cc                       int3       
004b5ad7 cc                       int3       
004b5ad8 cc                       int3       
004b5ad9 cc                       int3       
004b5ada cc                       int3       
004b5adb cc                       int3       
004b5adc cc                       int3       
004b5add cc                       int3       
004b5ade cc                       int3       
004b5adf cc                       int3       
