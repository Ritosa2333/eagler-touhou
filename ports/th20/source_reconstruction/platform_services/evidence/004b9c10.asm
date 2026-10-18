004b9c10 55                       push       ebp
004b9c11 8bec                     mov        ebp, esp
004b9c13 6aff                     push       -1
004b9c15 6800765600               push       0x567600
004b9c1a 64a100000000             mov        eax, dword ptr fs:[0]
004b9c20 50                       push       eax
004b9c21 51                       push       ecx
004b9c22 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004b9c27 33c5                     xor        eax, ebp
004b9c29 50                       push       eax
004b9c2a 8d45f4                   lea        eax, [ebp - 0xc]
004b9c2d 64a300000000             mov        dword ptr fs:[0], eax
004b9c33 894df0                   mov        dword ptr [ebp - 0x10], ecx
004b9c36 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9c39 c70002002000             mov        dword ptr [eax], 0x200002
004b9c3f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9c42 c74104b0000000           mov        dword ptr [ecx + 4], 0xb0
004b9c49 6810fb4100               push       0x41fb10
004b9c4e 6a02                     push       2
004b9c50 6a30                     push       0x30
004b9c52 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9c55 83c208                   add        edx, 8
004b9c58 52                       push       edx
004b9c59 e8c21ff5ff               call       0x40bc20
004b9c5e 33c0                     xor        eax, eax
004b9c60 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9c63 83c168                   add        ecx, 0x68
004b9c66 8901                     mov        dword ptr [ecx], eax
004b9c68 894104                   mov        dword ptr [ecx + 4], eax
004b9c6b ba58020000               mov        edx, 0x258
004b9c70 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9c73 66895070                 mov        word ptr [eax + 0x70], dx
004b9c77 b958020000               mov        ecx, 0x258
004b9c7c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9c7f 66894a72                 mov        word ptr [edx + 0x72], cx
004b9c83 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9c86 c6407400                 mov        byte ptr [eax + 0x74], 0
004b9c8a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9c8d c6417501                 mov        byte ptr [ecx + 0x75], 1
004b9c91 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9c94 c6427601                 mov        byte ptr [edx + 0x76], 1
004b9c98 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9c9b c7407808000000           mov        dword ptr [eax + 0x78], 8
004b9ca2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9ca5 c6417c00                 mov        byte ptr [ecx + 0x7c], 0
004b9ca9 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9cac c6427d02                 mov        byte ptr [edx + 0x7d], 2
004b9cb0 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9cb3 c6407e64                 mov        byte ptr [eax + 0x7e], 0x64
004b9cb7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9cba c6417f50                 mov        byte ptr [ecx + 0x7f], 0x50
004b9cbe 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9cc1 c6828000000000           mov        byte ptr [edx + 0x80], 0
004b9cc8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9ccb c6808100000002           mov        byte ptr [eax + 0x81], 2
004b9cd2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9cd5 c6818200000000           mov        byte ptr [ecx + 0x82], 0
004b9cdc 33d2                     xor        edx, edx
004b9cde 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9ce1 0583000000               add        eax, 0x83
004b9ce6 8810                     mov        byte ptr [eax], dl
004b9ce8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9ceb 81c184000000             add        ecx, 0x84
004b9cf1 e88afeffff               call       0x4b9b80
004b9cf6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9cf9 c7818800000000000080     mov        dword ptr [ecx + 0x88], 0x80000000
004b9d03 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9d06 c7828c00000000000080     mov        dword ptr [edx + 0x8c], 0x80000000
004b9d10 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9d13 c7809000000001000000     mov        dword ptr [eax + 0x90], 1
004b9d1d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9d20 c6819400000000           mov        byte ptr [ecx + 0x94], 0
004b9d27 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9d2a c6829500000000           mov        byte ptr [edx + 0x95], 0
004b9d31 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9d34 c6809600000000           mov        byte ptr [eax + 0x96], 0
004b9d3b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9d3e c6819700000000           mov        byte ptr [ecx + 0x97], 0
004b9d45 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9d48 c6829800000000           mov        byte ptr [edx + 0x98], 0
004b9d4f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9d52 c6809900000000           mov        byte ptr [eax + 0x99], 0
004b9d59 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9d5c c6819a00000001           mov        byte ptr [ecx + 0x9a], 1
004b9d63 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9d66 c6829b00000000           mov        byte ptr [edx + 0x9b], 0
004b9d6d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9d70 c6809c00000000           mov        byte ptr [eax + 0x9c], 0
004b9d77 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9d7a c6819d00000000           mov        byte ptr [ecx + 0x9d], 0
004b9d81 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9d84 c6829e00000000           mov        byte ptr [edx + 0x9e], 0
004b9d8b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9d8e c6809f00000000           mov        byte ptr [eax + 0x9f], 0
004b9d95 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9d98 c681a000000000           mov        byte ptr [ecx + 0xa0], 0
004b9d9f 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9da2 c682a100000000           mov        byte ptr [edx + 0xa1], 0
004b9da9 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9dac c680a200000000           mov        byte ptr [eax + 0xa2], 0
004b9db3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9db6 c681a300000000           mov        byte ptr [ecx + 0xa3], 0
004b9dbd 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004b9dc0 c682a400000000           mov        byte ptr [edx + 0xa4], 0
004b9dc7 33c0                     xor        eax, eax
004b9dc9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004b9dcc 81c1a8000000             add        ecx, 0xa8
004b9dd2 8901                     mov        dword ptr [ecx], eax
004b9dd4 894104                   mov        dword ptr [ecx + 4], eax
004b9dd7 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004b9dda 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004b9ddd 64890d00000000           mov        dword ptr fs:[0], ecx
004b9de4 59                       pop        ecx
004b9de5 8be5                     mov        esp, ebp
004b9de7 5d                       pop        ebp
004b9de8 c3                       ret        
004b9de9 cc                       int3       
004b9dea cc                       int3       
004b9deb cc                       int3       
004b9dec cc                       int3       
004b9ded cc                       int3       
004b9dee cc                       int3       
004b9def cc                       int3       
004b9df0 55                       push       ebp
004b9df1 8bec                     mov        ebp, esp
004b9df3 6aff                     push       -1
004b9df5 6800765600               push       0x567600
