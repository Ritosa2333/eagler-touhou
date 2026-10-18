00504af0 55                       push       ebp
00504af1 8bec                     mov        ebp, esp
00504af3 83ec10                   sub        esp, 0x10
00504af6 894dfc                   mov        dword ptr [ebp - 4], ecx
00504af9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00504afc 0fbe8814010000           movsx      ecx, byte ptr [eax + 0x114]
00504b03 85c9                     test       ecx, ecx
00504b05 0f85b0000000             jne        0x504bbb
00504b0b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504b0e 83c118                   add        ecx, 0x18
00504b11 e8ba83f4ff               call       0x44ced0
00504b16 8945f8                   mov        dword ptr [ebp - 8], eax
00504b19 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504b1c 83c150                   add        ecx, 0x50
00504b1f e87c72f0ff               call       0x40bda0
00504b24 f30f100528ef5600         movss      xmm0, dword ptr [0x56ef28]
00504b2c f30f114008               movss      dword ptr [eax + 8], xmm0
00504b31 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00504b34 e8c71e0000               call       0x506a00
00504b39 8b55fc                   mov        edx, dword ptr [ebp - 4]
00504b3c c7829c00000002000000     mov        dword ptr [edx + 0x9c], 2
00504b46 8b45fc                   mov        eax, dword ptr [ebp - 4]
00504b49 83c050                   add        eax, 0x50
00504b4c 8945f0                   mov        dword ptr [ebp - 0x10], eax
00504b4f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504b52 83c150                   add        ecx, 0x50
00504b55 e8c657f7ff               call       0x47a320
00504b5a d95df4                   fstp       dword ptr [ebp - 0xc]
00504b5d f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
00504b62 f30f5e05bcd75600         divss      xmm0, dword ptr [0x56d7bc]
00504b6a 51                       push       ecx
00504b6b f30f110424               movss      dword ptr [esp], xmm0
00504b70 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00504b73 e8485af7ff               call       0x47a5c0
00504b78 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504b7b 83c150                   add        ecx, 0x50
00504b7e e81d72f0ff               call       0x40bda0
00504b83 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00504b86 81c1bc050000             add        ecx, 0x5bc
00504b8c 8b10                     mov        edx, dword ptr [eax]
00504b8e 8911                     mov        dword ptr [ecx], edx
00504b90 8b5004                   mov        edx, dword ptr [eax + 4]
00504b93 895104                   mov        dword ptr [ecx + 4], edx
00504b96 8b4008                   mov        eax, dword ptr [eax + 8]
00504b99 894108                   mov        dword ptr [ecx + 8], eax
00504b9c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504b9f 81c1dc000000             add        ecx, 0xdc
00504ba5 e856d3fbff               call       0x4c1f00
00504baa 6a00                     push       0
00504bac 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504baf 81c1dc000000             add        ecx, 0xdc
00504bb5 e8e6cbf0ff               call       0x4117a0
00504bba 90                       nop        
00504bbb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00504bbe 8b81ac000000             mov        eax, dword ptr [ecx + 0xac]
00504bc4 8be5                     mov        esp, ebp
00504bc6 5d                       pop        ebp
00504bc7 c3                       ret        
00504bc8 cc                       int3       
00504bc9 cc                       int3       
00504bca cc                       int3       
00504bcb cc                       int3       
00504bcc cc                       int3       
00504bcd cc                       int3       
00504bce cc                       int3       
00504bcf cc                       int3       
