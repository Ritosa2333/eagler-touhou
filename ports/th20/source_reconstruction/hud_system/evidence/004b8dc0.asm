004b8dc0 55                       push       ebp
004b8dc1 8bec                     mov        ebp, esp
004b8dc3 83ec14                   sub        esp, 0x14
004b8dc6 894df4                   mov        dword ptr [ebp - 0xc], ecx
004b8dc9 c745fc00000000           mov        dword ptr [ebp - 4], 0
004b8dd0 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004b8dd3 83c06c                   add        eax, 0x6c
004b8dd6 8945f0                   mov        dword ptr [ebp - 0x10], eax
004b8dd9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8ddc 51                       push       ecx
004b8ddd 6aff                     push       -1
004b8ddf 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b8de2 e88938f9ff               call       0x44c670
004b8de7 8945f8                   mov        dword ptr [ebp - 8], eax
004b8dea 837df800                 cmp        dword ptr [ebp - 8], 0
004b8dee 7424                     je         0x4b8e14
004b8df0 f30f104508               movss      xmm0, dword ptr [ebp + 8]
004b8df5 f30f580590cd5600         addss      xmm0, dword ptr [0x56cd90]
004b8dfd 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004b8e00 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
004b8e05 e8b642faff               call       0x45d0c0
004b8e0a f30f1045ec               movss      xmm0, dword ptr [ebp - 0x14]
004b8e0f f30f114010               movss      dword ptr [eax + 0x10], xmm0
004b8e14 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8e17 83c201                   add        edx, 1
004b8e1a 8955fc                   mov        dword ptr [ebp - 4], edx
004b8e1d 837df800                 cmp        dword ptr [ebp - 8], 0
004b8e21 75ad                     jne        0x4b8dd0
004b8e23 8be5                     mov        esp, ebp
004b8e25 5d                       pop        ebp
004b8e26 c20800                   ret        8
004b8e29 cc                       int3       
004b8e2a cc                       int3       
004b8e2b cc                       int3       
004b8e2c cc                       int3       
004b8e2d cc                       int3       
004b8e2e cc                       int3       
004b8e2f cc                       int3       
