0050ab10 55                       push       ebp
0050ab11 8bec                     mov        ebp, esp
0050ab13 6aff                     push       -1
0050ab15 685da85600               push       0x56a85d
0050ab1a 64a100000000             mov        eax, dword ptr fs:[0]
0050ab20 50                       push       eax
0050ab21 83ec28                   sub        esp, 0x28
0050ab24 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0050ab29 33c5                     xor        eax, ebp
0050ab2b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0050ab2e 50                       push       eax
0050ab2f 8d45f4                   lea        eax, [ebp - 0xc]
0050ab32 64a300000000             mov        dword ptr fs:[0], eax
0050ab38 894de8                   mov        dword ptr [ebp - 0x18], ecx
0050ab3b 6a06                     push       6
0050ab3d b940025c00               mov        ecx, 0x5c0240
0050ab42 e8c917f0ff               call       0x40c310
0050ab47 50                       push       eax
0050ab48 8d4dec                   lea        ecx, [ebp - 0x14]
0050ab4b e8e00bf0ff               call       0x40b730
0050ab50 c745fc00000000           mov        dword ptr [ebp - 4], 0
0050ab57 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0050ab5a e80111f0ff               call       0x40bc60
0050ab5f 6a00                     push       0
0050ab61 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0050ab64 83c10c                   add        ecx, 0xc
0050ab67 e8340ff0ff               call       0x40baa0
0050ab6c 90                       nop        
0050ab6d 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0050ab70 50                       push       eax
0050ab71 8d4d0c                   lea        ecx, [ebp + 0xc]
0050ab74 51                       push       ecx
0050ab75 8d5508                   lea        edx, [ebp + 8]
0050ab78 52                       push       edx
0050ab79 8d4dcc                   lea        ecx, [ebp - 0x34]
0050ab7c e8eff9f5ff               call       0x46a570
0050ab81 50                       push       eax
0050ab82 8d4dd8                   lea        ecx, [ebp - 0x28]
0050ab85 e8d6feffff               call       0x50aa60
0050ab8a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0050ab8d 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0050ab90 50                       push       eax
0050ab91 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0050ab94 e8270ff0ff               call       0x40bac0
0050ab99 8d4dd8                   lea        ecx, [ebp - 0x28]
0050ab9c e82f0ef0ff               call       0x40b9d0
0050aba1 90                       nop        
0050aba2 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0050aba9 8d4dec                   lea        ecx, [ebp - 0x14]
0050abac e84f0df0ff               call       0x40b900
0050abb1 90                       nop        
0050abb2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0050abb5 64890d00000000           mov        dword ptr fs:[0], ecx
0050abbc 59                       pop        ecx
0050abbd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050abc0 33cd                     xor        ecx, ebp
0050abc2 e8277e0300               call       0x5429ee
0050abc7 8be5                     mov        esp, ebp
0050abc9 5d                       pop        ebp
0050abca c20c00                   ret        0xc
0050abcd cc                       int3       
0050abce cc                       int3       
0050abcf cc                       int3       
