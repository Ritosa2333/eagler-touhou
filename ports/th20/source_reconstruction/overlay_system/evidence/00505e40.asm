00505e40 55                       push       ebp
00505e41 8bec                     mov        ebp, esp
00505e43 83ec28                   sub        esp, 0x28
00505e46 894df8                   mov        dword ptr [ebp - 8], ecx
00505e49 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00505e50 8b4508                   mov        eax, dword ptr [ebp + 8]
00505e53 8945e0                   mov        dword ptr [ebp - 0x20], eax
00505e56 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00505e59 8b9158250100             mov        edx, dword ptr [ecx + 0x12558]
00505e5f 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00505e62 8b8c82d4050000           mov        ecx, dword ptr [edx + eax*4 + 0x5d4]
00505e69 894dfc                   mov        dword ptr [ebp - 4], ecx
00505e6c 8b55fc                   mov        edx, dword ptr [ebp - 4]
00505e6f 0fbe02                   movsx      eax, byte ptr [edx]
00505e72 85c0                     test       eax, eax
00505e74 0f8cf2000000             jl         0x505f6c
00505e7a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00505e7d 0fbe11                   movsx      edx, byte ptr [ecx]
00505e80 85d2                     test       edx, edx
00505e82 746e                     je         0x505ef2
00505e84 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505e87 0fbe4832                 movsx      ecx, byte ptr [eax + 0x32]
00505e8b 85c9                     test       ecx, ecx
00505e8d 752f                     jne        0x505ebe
00505e8f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00505e92 0fbe0a                   movsx      ecx, byte ptr [edx]
00505e95 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00505e98 99                       cdq        
00505e99 f7f9                     idiv       ecx
00505e9b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505e9e 0fbe4801                 movsx      ecx, byte ptr [eax + 1]
00505ea2 3bd1                     cmp        edx, ecx
00505ea4 7509                     jne        0x505eaf
00505ea6 c745f001000000           mov        dword ptr [ebp - 0x10], 1
00505ead eb07                     jmp        0x505eb6
00505eaf c745f000000000           mov        dword ptr [ebp - 0x10], 0
00505eb6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00505eb9 8955e8                   mov        dword ptr [ebp - 0x18], edx
00505ebc eb2e                     jmp        0x505eec
00505ebe 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505ec1 0fbe4832                 movsx      ecx, byte ptr [eax + 0x32]
00505ec5 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00505ec8 99                       cdq        
00505ec9 f7f9                     idiv       ecx
00505ecb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505ece 0fbe4833                 movsx      ecx, byte ptr [eax + 0x33]
00505ed2 3bd1                     cmp        edx, ecx
00505ed4 7509                     jne        0x505edf
00505ed6 c745ec01000000           mov        dword ptr [ebp - 0x14], 1
00505edd eb07                     jmp        0x505ee6
00505edf c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00505ee6 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00505ee9 8955e8                   mov        dword ptr [ebp - 0x18], edx
00505eec 837de800                 cmp        dword ptr [ebp - 0x18], 0
00505ef0 7463                     je         0x505f55
00505ef2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505ef5 0fbe4828                 movsx      ecx, byte ptr [eax + 0x28]
00505ef9 85c9                     test       ecx, ecx
00505efb 7509                     jne        0x505f06
00505efd c745e400000000           mov        dword ptr [ebp - 0x1c], 0
00505f04 eb30                     jmp        0x505f36
00505f06 8b55f8                   mov        edx, dword ptr [ebp - 8]
00505f09 8b8a94250100             mov        ecx, dword ptr [edx + 0x12594]
00505f0f e87ca0f0ff               call       0x40ff90
00505f14 8945d8                   mov        dword ptr [ebp - 0x28], eax
00505f17 8b45fc                   mov        eax, dword ptr [ebp - 4]
00505f1a 0fbe4828                 movsx      ecx, byte ptr [eax + 0x28]
00505f1e 83e10f                   and        ecx, 0xf
00505f21 83e901                   sub        ecx, 1
00505f24 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00505f27 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00505f2a 52                       push       edx
00505f2b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00505f2e e86d010000               call       0x5060a0
00505f33 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00505f36 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
00505f39 50                       push       eax
00505f3a 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
00505f3d 51                       push       ecx
00505f3e 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00505f41 52                       push       edx
00505f42 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00505f45 c1e008                   shl        eax, 8
00505f48 0b45f4                   or         eax, dword ptr [ebp - 0xc]
00505f4b 50                       push       eax
00505f4c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00505f4f e83cfeffff               call       0x505d90
00505f54 90                       nop        
00505f55 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00505f58 83c101                   add        ecx, 1
00505f5b 894df4                   mov        dword ptr [ebp - 0xc], ecx
00505f5e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00505f61 83c278                   add        edx, 0x78
00505f64 8955fc                   mov        dword ptr [ebp - 4], edx
00505f67 e900ffffff               jmp        0x505e6c
00505f6c 33c0                     xor        eax, eax
00505f6e 8be5                     mov        esp, ebp
00505f70 5d                       pop        ebp
00505f71 c21000                   ret        0x10
00505f74 cc                       int3       
00505f75 cc                       int3       
00505f76 cc                       int3       
00505f77 cc                       int3       
00505f78 cc                       int3       
00505f79 cc                       int3       
00505f7a cc                       int3       
00505f7b cc                       int3       
00505f7c cc                       int3       
00505f7d cc                       int3       
00505f7e cc                       int3       
00505f7f cc                       int3       
