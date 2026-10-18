00482da0 55                       push       ebp
00482da1 8bec                     mov        ebp, esp
00482da3 83ec18                   sub        esp, 0x18
00482da6 894dfc                   mov        dword ptr [ebp - 4], ecx
00482da9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482dac 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00482db2 e84995f8ff               call       0x40c300
00482db7 8bc8                     mov        ecx, eax
00482db9 e8d22b0000               call       0x485990
00482dbe 0f57c0                   xorps      xmm0, xmm0
00482dc1 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
00482dc5 7222                     jb         0x482de9
00482dc7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482dca 81c1a0000000             add        ecx, 0xa0
00482dd0 894df0                   mov        dword ptr [ebp - 0x10], ecx
00482dd3 6a04                     push       4
00482dd5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00482dd8 e8a398ffff               call       0x47c680
00482ddd f30f10401c               movss      xmm0, dword ptr [eax + 0x1c]
00482de2 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00482de7 eb1f                     jmp        0x482e08
00482de9 8b55fc                   mov        edx, dword ptr [ebp - 4]
00482dec 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
00482df2 e80995f8ff               call       0x40c300
00482df7 8bc8                     mov        ecx, eax
00482df9 e8922b0000               call       0x485990
00482dfe f30f104004               movss      xmm0, dword ptr [eax + 4]
00482e03 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00482e08 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
00482e0d f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00482e12 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00482e1a f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
00482e22 f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
00482e27 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
00482e2f f30f58c1                 addss      xmm0, xmm1
00482e33 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482e36 f30f104868               movss      xmm1, dword ptr [eax + 0x68]
00482e3b 0f2fc8                   comiss     xmm1, xmm0
00482e3e 7273                     jb         0x482eb3
00482e40 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482e43 81c1a0000000             add        ecx, 0xa0
00482e49 894dec                   mov        dword ptr [ebp - 0x14], ecx
00482e4c 6a04                     push       4
00482e4e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00482e51 e82a98ffff               call       0x47c680
00482e56 8b503c                   mov        edx, dword ptr [eax + 0x3c]
00482e59 83e210                   and        edx, 0x10
00482e5c 754e                     jne        0x482eac
00482e5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482e61 81c184000000             add        ecx, 0x84
00482e67 e87464faff               call       0x4292e0
00482e6c d95de8                   fstp       dword ptr [ebp - 0x18]
00482e6f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00482e74 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00482e7b 51                       push       ecx
00482e7c f30f110424               movss      dword ptr [esp], xmm0
00482e81 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482e84 81c184000000             add        ecx, 0x84
00482e8a e89100fdff               call       0x452f20
00482e8f f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00482e97 f30f5845f4               addss      xmm0, dword ptr [ebp - 0xc]
00482e9c 8b45fc                   mov        eax, dword ptr [ebp - 4]
00482e9f f30f5c4068               subss      xmm0, dword ptr [eax + 0x68]
00482ea4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00482ea7 f30f114168               movss      dword ptr [ecx + 0x68], xmm0
00482eac b801000000               mov        eax, 1
00482eb1 eb02                     jmp        0x482eb5
00482eb3 33c0                     xor        eax, eax
00482eb5 8be5                     mov        esp, ebp
00482eb7 5d                       pop        ebp
00482eb8 c3                       ret        
00482eb9 cc                       int3       
00482eba cc                       int3       
00482ebb cc                       int3       
00482ebc cc                       int3       
00482ebd cc                       int3       
00482ebe cc                       int3       
00482ebf cc                       int3       
