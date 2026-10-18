004b89a0 55                       push       ebp
004b89a1 8bec                     mov        ebp, esp
004b89a3 83ec28                   sub        esp, 0x28
004b89a6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b89ab 33c5                     xor        eax, ebp
004b89ad 8945fc                   mov        dword ptr [ebp - 4], eax
004b89b0 894dec                   mov        dword ptr [ebp - 0x14], ecx
004b89b3 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004b89b6 83c06c                   add        eax, 0x6c
004b89b9 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004b89bc 6a00                     push       0
004b89be 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b89c1 8b913c010000             mov        edx, dword ptr [ecx + 0x13c]
004b89c7 81c2a6000000             add        edx, 0xa6
004b89cd 52                       push       edx
004b89ce 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004b89d1 e89a3cf9ff               call       0x44c670
004b89d6 8945e8                   mov        dword ptr [ebp - 0x18], eax
004b89d9 837de800                 cmp        dword ptr [ebp - 0x18], 0
004b89dd 0f8430010000             je         0x4b8b13
004b89e3 8d4df0                   lea        ecx, [ebp - 0x10]
004b89e6 e825a4f6ff               call       0x422e10
004b89eb 8d45f0                   lea        eax, [ebp - 0x10]
004b89ee 50                       push       eax
004b89ef 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b89f2 e8e9ecf7ff               call       0x4376e0
004b89f7 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004b89ff f30f5e0518885b00         divss      xmm0, dword ptr [0x5b8818]
004b8a07 f30f5945f0               mulss      xmm0, dword ptr [ebp - 0x10]
004b8a0c f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8a11 f30f1005d0c85600         movss      xmm0, dword ptr [0x56c8d0]
004b8a19 f30f5e0518885b00         divss      xmm0, dword ptr [0x5b8818]
004b8a21 f30f5945f4               mulss      xmm0, dword ptr [ebp - 0xc]
004b8a26 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004b8a2b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004b8a2e 83b91401000001           cmp        dword ptr [ecx + 0x114], 1
004b8a35 7571                     jne        0x4b8aa8
004b8a37 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b8a3a e881eff7ff               call       0x4379c0
004b8a3f d95de0                   fstp       dword ptr [ebp - 0x20]
004b8a42 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004b8a4a 0f2f45e0                 comiss     xmm0, dword ptr [ebp - 0x20]
004b8a4e 7634                     jbe        0x4b8a84
004b8a50 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b8a53 e868eff7ff               call       0x4379c0
004b8a58 d95ddc                   fstp       dword ptr [ebp - 0x24]
004b8a5b f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
004b8a60 f30f5805ac0a5700         addss      xmm0, dword ptr [0x570aac]
004b8a68 f30f590594cd5600         mulss      xmm0, dword ptr [0x56cd94]
004b8a70 f30f5c05bcd75600         subss      xmm0, dword ptr [0x56d7bc]
004b8a78 f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
004b8a7d f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8a82 eb22                     jmp        0x4b8aa6
004b8a84 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004b8a8c f30f590594cd5600         mulss      xmm0, dword ptr [0x56cd94]
004b8a94 f30f5c05bcd75600         subss      xmm0, dword ptr [0x56d7bc]
004b8a9c f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
004b8aa1 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8aa6 eb5e                     jmp        0x4b8b06
004b8aa8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004b8aab 83ba1401000000           cmp        dword ptr [edx + 0x114], 0
004b8ab2 7514                     jne        0x4b8ac8
004b8ab4 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
004b8ab9 f30f5c05c40a5700         subss      xmm0, dword ptr [0x570ac4]
004b8ac1 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8ac6 eb3e                     jmp        0x4b8b06
004b8ac8 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004b8acb 83b81401000002           cmp        dword ptr [eax + 0x114], 2
004b8ad2 7532                     jne        0x4b8b06
004b8ad4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004b8ad7 e8e4eef7ff               call       0x4379c0
004b8adc d95dd8                   fstp       dword ptr [ebp - 0x28]
004b8adf f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
004b8ae4 f30f5805ac0a5700         addss      xmm0, dword ptr [0x570aac]
004b8aec f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
004b8af4 f30f5c05bcd75600         subss      xmm0, dword ptr [0x56d7bc]
004b8afc f30f5845f0               addss      xmm0, dword ptr [ebp - 0x10]
004b8b01 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
004b8b06 8d4df0                   lea        ecx, [ebp - 0x10]
004b8b09 51                       push       ecx
004b8b0a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004b8b0d e84e52faff               call       0x45dd60
004b8b12 90                       nop        
004b8b13 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8b16 33cd                     xor        ecx, ebp
004b8b18 e8d19e0800               call       0x5429ee
004b8b1d 8be5                     mov        esp, ebp
004b8b1f 5d                       pop        ebp
004b8b20 c20400                   ret        4
004b8b23 cc                       int3       
004b8b24 cc                       int3       
004b8b25 cc                       int3       
004b8b26 cc                       int3       
004b8b27 cc                       int3       
004b8b28 cc                       int3       
004b8b29 cc                       int3       
004b8b2a cc                       int3       
004b8b2b cc                       int3       
004b8b2c cc                       int3       
004b8b2d cc                       int3       
004b8b2e cc                       int3       
004b8b2f cc                       int3       
