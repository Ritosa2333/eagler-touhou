004b8e70 55                       push       ebp
004b8e71 8bec                     mov        ebp, esp
004b8e73 83ec28                   sub        esp, 0x28
004b8e76 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b8e7b 33c5                     xor        eax, ebp
004b8e7d 8945fc                   mov        dword ptr [ebp - 4], eax
004b8e80 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b8e83 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8e86 e825efffff               call       0x4b7db0
004b8e8b 51                       push       ecx
004b8e8c 0f57c0                   xorps      xmm0, xmm0
004b8e8f f30f110424               movss      dword ptr [esp], xmm0
004b8e94 51                       push       ecx
004b8e95 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004b8e9a f30f110424               movss      dword ptr [esp], xmm0
004b8e9f 51                       push       ecx
004b8ea0 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004b8ea5 f30f110424               movss      dword ptr [esp], xmm0
004b8eaa 8d4df0                   lea        ecx, [ebp - 0x10]
004b8ead e81e9ff6ff               call       0x422dd0
004b8eb2 90                       nop        
004b8eb3 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004b8eb9 e892f3ffff               call       0x4b8250
004b8ebe 8945e0                   mov        dword ptr [ebp - 0x20], eax
004b8ec1 6a00                     push       0
004b8ec3 6aff                     push       -1
004b8ec5 51                       push       ecx
004b8ec6 0f57c0                   xorps      xmm0, xmm0
004b8ec9 f30f110424               movss      dword ptr [esp], xmm0
004b8ece 8d45f0                   lea        eax, [ebp - 0x10]
004b8ed1 50                       push       eax
004b8ed2 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
004b8ed5 81c10e010000             add        ecx, 0x10e
004b8edb 51                       push       ecx
004b8edc 6838065700               push       0x570638
004b8ee1 8d55d8                   lea        edx, [ebp - 0x28]
004b8ee4 52                       push       edx
004b8ee5 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004b8ee8 e8f301fcff               call       0x4790e0
004b8eed 8b00                     mov        eax, dword ptr [eax]
004b8eef 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8ef2 89416c                   mov        dword ptr [ecx + 0x6c], eax
004b8ef5 c745e800000000           mov        dword ptr [ebp - 0x18], 0
004b8efc 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8eff 83c26c                   add        edx, 0x6c
004b8f02 8955dc                   mov        dword ptr [ebp - 0x24], edx
004b8f05 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
004b8f08 50                       push       eax
004b8f09 6aff                     push       -1
004b8f0b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004b8f0e e85d37f9ff               call       0x44c670
004b8f13 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004b8f16 837de400                 cmp        dword ptr [ebp - 0x1c], 0
004b8f1a 7412                     je         0x4b8f2e
004b8f1c 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b8f1f e89c41faff               call       0x45d0c0
004b8f24 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004b8f29 f30f114010               movss      dword ptr [eax + 0x10], xmm0
004b8f2e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b8f31 83c101                   add        ecx, 1
004b8f34 894de8                   mov        dword ptr [ebp - 0x18], ecx
004b8f37 837de400                 cmp        dword ptr [ebp - 0x1c], 0
004b8f3b 75bf                     jne        0x4b8efc
004b8f3d 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8f40 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004b8f43 89823c010000             mov        dword ptr [edx + 0x13c], eax
004b8f49 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8f4c 33cd                     xor        ecx, ebp
004b8f4e e89b9a0800               call       0x5429ee
004b8f53 8be5                     mov        esp, ebp
004b8f55 5d                       pop        ebp
004b8f56 c21000                   ret        0x10
004b8f59 cc                       int3       
004b8f5a cc                       int3       
004b8f5b cc                       int3       
004b8f5c cc                       int3       
004b8f5d cc                       int3       
004b8f5e cc                       int3       
004b8f5f cc                       int3       
