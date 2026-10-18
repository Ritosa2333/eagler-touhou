0052c030 55                       push       ebp
0052c031 8bec                     mov        ebp, esp
0052c033 6aff                     push       -1
0052c035 688d805600               push       0x56808d
0052c03a 64a100000000             mov        eax, dword ptr fs:[0]
0052c040 50                       push       eax
0052c041 83ec14                   sub        esp, 0x14
0052c044 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c049 33c5                     xor        eax, ebp
0052c04b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c04e 50                       push       eax
0052c04f 8d45f4                   lea        eax, [ebp - 0xc]
0052c052 64a300000000             mov        dword ptr fs:[0], eax
0052c058 894de8                   mov        dword ptr [ebp - 0x18], ecx
0052c05b 6a09                     push       9
0052c05d b940025c00               mov        ecx, 0x5c0240
0052c062 e8a902eeff               call       0x40c310
0052c067 50                       push       eax
0052c068 8d4dec                   lea        ecx, [ebp - 0x14]
0052c06b e8c0f6edff               call       0x40b730
0052c070 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c077 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052c07a 8b8824590000             mov        ecx, dword ptr [eax + 0x5924]
0052c080 51                       push       ecx
0052c081 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052c087 e87467f7ff               call       0x4a2800
0052c08c 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052c08f c7822459000000000000     mov        dword ptr [edx + 0x5924], 0
0052c099 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052c09c c7802459000000000000     mov        dword ptr [eax + 0x5924], 0
0052c0a6 6a00                     push       0
0052c0a8 6a01                     push       1
0052c0aa 6a30                     push       0x30
0052c0ac 6a40                     push       0x40
0052c0ae 6810565700               push       0x575610
0052c0b3 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052c0b9 e81252f4ff               call       0x4712d0
0052c0be 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c0c1 898124590000             mov        dword ptr [ecx + 0x5924], eax
0052c0c7 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052c0ca 8b8224590000             mov        eax, dword ptr [edx + 0x5924]
0052c0d0 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0052c0d3 f30f2a05fc875b00         cvtsi2ss   xmm0, dword ptr [0x5b87fc]
0052c0db 51                       push       ecx
0052c0dc f30f110424               movss      dword ptr [esp], xmm0
0052c0e1 f30f2a05f8875b00         cvtsi2ss   xmm0, dword ptr [0x5b87f8]
0052c0e9 51                       push       ecx
0052c0ea f30f110424               movss      dword ptr [esp], xmm0
0052c0ef 51                       push       ecx
0052c0f0 0f57c0                   xorps      xmm0, xmm0
0052c0f3 f30f110424               movss      dword ptr [esp], xmm0
0052c0f8 51                       push       ecx
0052c0f9 0f57c0                   xorps      xmm0, xmm0
0052c0fc f30f110424               movss      dword ptr [esp], xmm0
0052c101 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c104 e8a714f7ff               call       0x49d5b0
0052c109 51                       push       ecx
0052c10a 0f57c0                   xorps      xmm0, xmm0
0052c10d f30f110424               movss      dword ptr [esp], xmm0
0052c112 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c115 81c128590000             add        ecx, 0x5928
0052c11b e8006ef2ff               call       0x452f20
0052c120 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c123 8b9124590000             mov        edx, dword ptr [ecx + 0x5924]
0052c129 8955e0                   mov        dword ptr [ebp - 0x20], edx
0052c12c 6870d15100               push       0x51d170
0052c131 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052c134 e8170c0000               call       0x52cd50
0052c139 68d0d0d0ff               push       0xffd0d0d0
0052c13e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052c141 81c130590000             add        ecx, 0x5930
0052c147 e8e41effff               call       0x51e030
0052c14c 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052c14f f30f1005a0565700         movss      xmm0, dword ptr [0x5756a0]
0052c157 f30f11802c590000         movss      dword ptr [eax + 0x592c], xmm0
0052c15f c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c166 8d4dec                   lea        ecx, [ebp - 0x14]
0052c169 e892f7edff               call       0x40b900
0052c16e 90                       nop        
0052c16f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c172 64890d00000000           mov        dword ptr fs:[0], ecx
0052c179 59                       pop        ecx
0052c17a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c17d 33cd                     xor        ecx, ebp
0052c17f e86a680100               call       0x5429ee
0052c184 8be5                     mov        esp, ebp
0052c186 5d                       pop        ebp
0052c187 c3                       ret        
0052c188 cc                       int3       
0052c189 cc                       int3       
0052c18a cc                       int3       
0052c18b cc                       int3       
0052c18c cc                       int3       
0052c18d cc                       int3       
0052c18e cc                       int3       
0052c18f cc                       int3       
