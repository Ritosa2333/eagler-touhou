004e1ce0 55                       push       ebp
004e1ce1 8bec                     mov        ebp, esp
004e1ce3 6aff                     push       -1
004e1ce5 68b3a05600               push       0x56a0b3
004e1cea 64a100000000             mov        eax, dword ptr fs:[0]
004e1cf0 50                       push       eax
004e1cf1 51                       push       ecx
004e1cf2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004e1cf7 33c5                     xor        eax, ebp
004e1cf9 50                       push       eax
004e1cfa 8d45f4                   lea        eax, [ebp - 0xc]
004e1cfd 64a300000000             mov        dword ptr fs:[0], eax
004e1d03 894df0                   mov        dword ptr [ebp - 0x10], ecx
004e1d06 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d09 e812e0f3ff               call       0x41fd20
004e1d0e c745fc00000000           mov        dword ptr [ebp - 4], 0
004e1d15 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1d18 c700a0245700             mov        dword ptr [eax], 0x5724a0
004e1d1e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d21 83c110                   add        ecx, 0x10
004e1d24 e86710f4ff               call       0x422d90
004e1d29 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d2c 83c120                   add        ecx, 0x20
004e1d2f e85c10f4ff               call       0x422d90
004e1d34 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d37 83c130                   add        ecx, 0x30
004e1d3a e8c1d1fcff               call       0x4aef00
004e1d3f c645fc01                 mov        byte ptr [ebp - 4], 1
004e1d43 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d46 83c17c                   add        ecx, 0x7c
004e1d49 e8b2d1fcff               call       0x4aef00
004e1d4e c645fc02                 mov        byte ptr [ebp - 4], 2
004e1d52 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d55 81c1c8000000             add        ecx, 0xc8
004e1d5b e8603ff4ff               call       0x425cc0
004e1d60 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d63 81c1cc000000             add        ecx, 0xcc
004e1d69 e8523ff4ff               call       0x425cc0
004e1d6e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d71 c781d000000000000000     mov        dword ptr [ecx + 0xd0], 0
004e1d7b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1d7e c782d400000000000000     mov        dword ptr [edx + 0xd4], 0
004e1d88 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1d8b c780d800000000000000     mov        dword ptr [eax + 0xd8], 0
004e1d95 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1d98 c781dc00000000000000     mov        dword ptr [ecx + 0xdc], 0
004e1da2 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1da5 c782e000000000000000     mov        dword ptr [edx + 0xe0], 0
004e1daf 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1db2 c780e400000000000000     mov        dword ptr [eax + 0xe4], 0
004e1dbc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1dbf c781e800000000000000     mov        dword ptr [ecx + 0xe8], 0
004e1dc9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1dcc c782ec00000000000000     mov        dword ptr [edx + 0xec], 0
004e1dd6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1dd9 c780f000000000000000     mov        dword ptr [eax + 0xf0], 0
004e1de3 68c4000000               push       0xc4
004e1de8 6a00                     push       0
004e1dea 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1ded 81c1f4000000             add        ecx, 0xf4
004e1df3 51                       push       ecx
004e1df4 e867290600               call       0x544760
004e1df9 83c40c                   add        esp, 0xc
004e1dfc 33d2                     xor        edx, edx
004e1dfe 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1e01 05b8010000               add        eax, 0x1b8
004e1e06 8910                     mov        dword ptr [eax], edx
004e1e08 895004                   mov        dword ptr [eax + 4], edx
004e1e0b 885008                   mov        byte ptr [eax + 8], dl
004e1e0e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1e11 0f57c0                   xorps      xmm0, xmm0
004e1e14 f30f1181c4010000         movss      dword ptr [ecx + 0x1c4], xmm0
004e1e1c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1e1f 0f57c0                   xorps      xmm0, xmm0
004e1e22 f20f1182c8010000         movsd      qword ptr [edx + 0x1c8], xmm0
004e1e2a 6800010000               push       0x100
004e1e2f 6a00                     push       0
004e1e31 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1e34 05d0010000               add        eax, 0x1d0
004e1e39 50                       push       eax
004e1e3a e821290600               call       0x544760
004e1e3f 83c40c                   add        esp, 0xc
004e1e42 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1e45 81c1d0020000             add        ecx, 0x2d0
004e1e4b e860feffff               call       0x4e1cb0
004e1e50 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004e1e53 c781d402000000000000     mov        dword ptr [ecx + 0x2d4], 0
004e1e5d 68b8245700               push       0x5724b8
004e1e62 e849a8f2ff               call       0x40c6b0
004e1e67 83c404                   add        esp, 4
004e1e6a 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004e1e6d 8915bc605c00             mov        dword ptr [0x5c60bc], edx
004e1e73 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004e1e7a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004e1e7d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004e1e80 64890d00000000           mov        dword ptr fs:[0], ecx
004e1e87 59                       pop        ecx
004e1e88 8be5                     mov        esp, ebp
004e1e8a 5d                       pop        ebp
004e1e8b c3                       ret        
004e1e8c cc                       int3       
004e1e8d cc                       int3       
004e1e8e cc                       int3       
004e1e8f cc                       int3       
