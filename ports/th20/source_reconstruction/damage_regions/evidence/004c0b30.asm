004c0b30 55                       push       ebp
004c0b31 8bec                     mov        ebp, esp
004c0b33 83ec28                   sub        esp, 0x28
004c0b36 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0b39 8b4508                   mov        eax, dword ptr [ebp + 8]
004c0b3c 50                       push       eax
004c0b3d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0b40 e84b150000               call       0x4c2090
004c0b45 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0b48 51                       push       ecx
004c0b49 68b00c4c00               push       0x4c0cb0
004c0b4e 6a1c                     push       0x1c
004c0b50 e8bb17f5ff               call       0x412310
004c0b55 83c40c                   add        esp, 0xc
004c0b58 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0b5b 894208                   mov        dword ptr [edx + 8], eax
004c0b5e 8b4508                   mov        eax, dword ptr [ebp + 8]
004c0b61 50                       push       eax
004c0b62 b968a55b00               mov        ecx, 0x5ba568
004c0b67 e854b0f4ff               call       0x40bbc0
004c0b6c 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c0b6f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0b72 51                       push       ecx
004c0b73 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c0b76 e89521f5ff               call       0x412d10
004c0b7b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0b7e 81c214c40000             add        edx, 0xc414
004c0b84 8955e8                   mov        dword ptr [ebp - 0x18], edx
004c0b87 6a00                     push       0
004c0b89 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c0b8c e8af9af8ff               call       0x44a640
004c0b91 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0b94 052cc40000               add        eax, 0xc42c
004c0b99 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c0b9c 6a00                     push       0
004c0b9e 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0ba1 e89a9af8ff               call       0x44a640
004c0ba6 90                       nop        
004c0ba7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0baa 83c110                   add        ecx, 0x10
004c0bad 894df0                   mov        dword ptr [ebp - 0x10], ecx
004c0bb0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0bb3 e8e8b1f4ff               call       0x40bda0
004c0bb8 8945f8                   mov        dword ptr [ebp - 8], eax
004c0bbb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0bbe e87d000000               call       0x4c0c40
004c0bc3 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c0bc6 eb0c                     jmp        0x4c0bd4
004c0bc8 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c0bcb 81c2c4000000             add        edx, 0xc4
004c0bd1 8955f8                   mov        dword ptr [ebp - 8], edx
004c0bd4 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c0bd7 3b45e0                   cmp        eax, dword ptr [ebp - 0x20]
004c0bda 7433                     je         0x4c0c0f
004c0bdc 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0bdf 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c0be2 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c0be5 8955dc                   mov        dword ptr [ebp - 0x24], edx
004c0be8 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004c0beb 50                       push       eax
004c0bec 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c0bef e87c9af8ff               call       0x44a670
004c0bf4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0bf7 81c12cc40000             add        ecx, 0xc42c
004c0bfd 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004c0c00 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004c0c03 52                       push       edx
004c0c04 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c0c07 e89412f5ff               call       0x411ea0
004c0c0c 90                       nop        
004c0c0d ebb9                     jmp        0x4c0bc8
004c0c0f 8b4508                   mov        eax, dword ptr [ebp + 8]
004c0c12 c1e010                   shl        eax, 0x10
004c0c15 83c801                   or         eax, 1
004c0c18 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0c1b 898110c40000             mov        dword ptr [ecx + 0xc410], eax
004c0c21 6a00                     push       0
004c0c23 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0c26 81c148c40000             add        ecx, 0xc448
004c0c2c e8ef28f6ff               call       0x423520
004c0c31 33c0                     xor        eax, eax
004c0c33 8be5                     mov        esp, ebp
004c0c35 5d                       pop        ebp
004c0c36 c20400                   ret        4
004c0c39 cc                       int3       
004c0c3a cc                       int3       
004c0c3b cc                       int3       
004c0c3c cc                       int3       
004c0c3d cc                       int3       
004c0c3e cc                       int3       
004c0c3f cc                       int3       
