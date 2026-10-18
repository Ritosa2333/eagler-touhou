004d5b10 55                       push       ebp
004d5b11 8bec                     mov        ebp, esp
004d5b13 83ec08                   sub        esp, 8
004d5b16 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004d5b1c e80fe5f4ff               call       0x424030
004d5b21 85c0                     test       eax, eax
004d5b23 7407                     je         0x4d5b2c
004d5b25 b801000000               mov        eax, 1
004d5b2a eb76                     jmp        0x4d5ba2
004d5b2c a128a85b00               mov        eax, dword ptr [0x5ba828]
004d5b31 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004d5b37 c1e90b                   shr        ecx, 0xb
004d5b3a 83e101                   and        ecx, 1
004d5b3d 7407                     je         0x4d5b46
004d5b3f b801000000               mov        eax, 1
004d5b44 eb5c                     jmp        0x4d5ba2
004d5b46 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
004d5b4c e80fe5f4ff               call       0x424060
004d5b51 85c0                     test       eax, eax
004d5b53 7445                     je         0x4d5b9a
004d5b55 b9e4ef5a00               mov        ecx, 0x5aefe4
004d5b5a e88137f5ff               call       0x4292e0
004d5b5f d95dfc                   fstp       dword ptr [ebp - 4]
004d5b62 51                       push       ecx
004d5b63 0f57c0                   xorps      xmm0, xmm0
004d5b66 f30f110424               movss      dword ptr [esp], xmm0
004d5b6b b9e4ef5a00               mov        ecx, 0x5aefe4
004d5b70 e82b37f5ff               call       0x4292a0
004d5b75 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d5b78 e8039bffff               call       0x4cf680
004d5b7d 8945f8                   mov        dword ptr [ebp - 8], eax
004d5b80 51                       push       ecx
004d5b81 f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004d5b86 f30f110424               movss      dword ptr [esp], xmm0
004d5b8b b9e4ef5a00               mov        ecx, 0x5aefe4
004d5b90 e80b37f5ff               call       0x4292a0
004d5b95 8b45f8                   mov        eax, dword ptr [ebp - 8]
004d5b98 eb08                     jmp        0x4d5ba2
004d5b9a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004d5b9d e8de9affff               call       0x4cf680
004d5ba2 8be5                     mov        esp, ebp
004d5ba4 5d                       pop        ebp
004d5ba5 c3                       ret        
004d5ba6 cc                       int3       
004d5ba7 cc                       int3       
004d5ba8 cc                       int3       
004d5ba9 cc                       int3       
004d5baa cc                       int3       
004d5bab cc                       int3       
004d5bac cc                       int3       
004d5bad cc                       int3       
004d5bae cc                       int3       
004d5baf cc                       int3       
