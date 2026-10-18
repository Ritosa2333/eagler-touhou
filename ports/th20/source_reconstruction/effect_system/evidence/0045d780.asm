0045d780 53                       push       ebx
0045d781 8bdc                     mov        ebx, esp
0045d783 83ec08                   sub        esp, 8
0045d786 83e4f8                   and        esp, 0xfffffff8
0045d789 83c404                   add        esp, 4
0045d78c 55                       push       ebp
0045d78d 8b6b04                   mov        ebp, dword ptr [ebx + 4]
0045d790 896c2404                 mov        dword ptr [esp + 4], ebp
0045d794 8bec                     mov        ebp, esp
0045d796 83ec70                   sub        esp, 0x70
0045d799 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045d79e 33c5                     xor        eax, ebp
0045d7a0 8945fc                   mov        dword ptr [ebp - 4], eax
0045d7a3 894df4                   mov        dword ptr [ebp - 0xc], ecx
0045d7a6 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0045d7a9 05e0010000               add        eax, 0x1e0
0045d7ae 8945dc                   mov        dword ptr [ebp - 0x24], eax
0045d7b1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0045d7b4 e8e7e5faff               call       0x40bda0
0045d7b9 8945ec                   mov        dword ptr [ebp - 0x14], eax
0045d7bc 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0045d7bf e8dc040000               call       0x45dca0
0045d7c4 8945d0                   mov        dword ptr [ebp - 0x30], eax
0045d7c7 eb09                     jmp        0x45d7d2
0045d7c9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045d7cc 83c104                   add        ecx, 4
0045d7cf 894dec                   mov        dword ptr [ebp - 0x14], ecx
0045d7d2 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0045d7d5 3b55d0                   cmp        edx, dword ptr [ebp - 0x30]
0045d7d8 7434                     je         0x45d80e
0045d7da 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0045d7dd 8945c8                   mov        dword ptr [ebp - 0x38], eax
0045d7e0 b9c4a45b00               mov        ecx, 0x5ba4c4
0045d7e5 e8f6c0fcff               call       0x4298e0
0045d7ea d95dcc                   fstp       dword ptr [ebp - 0x34]
0045d7ed f30f1045cc               movss      xmm0, dword ptr [ebp - 0x34]
0045d7f2 f30f590590cd5600         mulss      xmm0, dword ptr [0x56cd90]
0045d7fa f30f580514ed5600         addss      xmm0, dword ptr [0x56ed14]
0045d802 f30f2cc8                 cvttss2si  ecx, xmm0
0045d806 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0045d809 884a02                   mov        byte ptr [edx + 2], cl
0045d80c ebbb                     jmp        0x45d7c9
0045d80e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0045d811 05d0020000               add        eax, 0x2d0
0045d816 8945d8                   mov        dword ptr [ebp - 0x28], eax
0045d819 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0045d81c e87fe5faff               call       0x40bda0
0045d821 8945e8                   mov        dword ptr [ebp - 0x18], eax
0045d824 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0045d827 e874040000               call       0x45dca0
0045d82c 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0045d82f eb09                     jmp        0x45d83a
0045d831 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045d834 83c104                   add        ecx, 4
0045d837 894de8                   mov        dword ptr [ebp - 0x18], ecx
0045d83a 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0045d83d 3b55c4                   cmp        edx, dword ptr [ebp - 0x3c]
0045d840 0f84bb000000             je         0x45d901
0045d846 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0045d849 8945f0                   mov        dword ptr [ebp - 0x10], eax
0045d84c b9c4a45b00               mov        ecx, 0x5ba4c4
0045d851 e88ac0fcff               call       0x4298e0
0045d856 d95dc0                   fstp       dword ptr [ebp - 0x40]
0045d859 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
0045d85e f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045d866 f30f1145b8               movss      dword ptr [ebp - 0x48], xmm0
0045d86b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045d86e f30f1001                 movss      xmm0, dword ptr [ecx]
0045d872 f30f1145bc               movss      dword ptr [ebp - 0x44], xmm0
0045d877 f30f1045bc               movss      xmm0, dword ptr [ebp - 0x44]
0045d87c f30f5845b8               addss      xmm0, dword ptr [ebp - 0x48]
0045d881 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0045d884 f30f1102                 movss      dword ptr [edx], xmm0
0045d888 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045d88d f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
0045d895 f30f584308               addss      xmm0, dword ptr [ebx + 8]
0045d89a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045d89d f30f1008                 movss      xmm1, dword ptr [eax]
0045d8a1 0f2fc8                   comiss     xmm1, xmm0
0045d8a4 761b                     jbe        0x45d8c1
0045d8a6 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045d8ab f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
0045d8b3 f30f584308               addss      xmm0, dword ptr [ebx + 8]
0045d8b8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045d8bb f30f1101                 movss      dword ptr [ecx], xmm0
0045d8bf eb3b                     jmp        0x45d8fc
0045d8c1 f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045d8c6 f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
0045d8ce f30f104b08               movss      xmm1, dword ptr [ebx + 8]
0045d8d3 f30f5cc8                 subss      xmm1, xmm0
0045d8d7 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0045d8da 0f2f0a                   comiss     xmm1, dword ptr [edx]
0045d8dd 761d                     jbe        0x45d8fc
0045d8df f30f104308               movss      xmm0, dword ptr [ebx + 8]
0045d8e4 f30f5e05e0c85600         divss      xmm0, dword ptr [0x56c8e0]
0045d8ec f30f104b08               movss      xmm1, dword ptr [ebx + 8]
0045d8f1 f30f5cc8                 subss      xmm1, xmm0
0045d8f5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0045d8f8 f30f1108                 movss      dword ptr [eax], xmm1
0045d8fc e930ffffff               jmp        0x45d831
0045d901 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0045d908 51                       push       ecx
0045d909 0f57c0                   xorps      xmm0, xmm0
0045d90c f30f110424               movss      dword ptr [esp], xmm0
0045d911 8d4df8                   lea        ecx, [ebp - 8]
0045d914 e8f7b8fcff               call       0x429210
0045d919 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045d91c 81c1d0020000             add        ecx, 0x2d0
0045d922 894db4                   mov        dword ptr [ebp - 0x4c], ecx
0045d925 6a00                     push       0
0045d927 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
0045d92a e8516cfbff               call       0x414580
0045d92f 0f57c0                   xorps      xmm0, xmm0
0045d932 f30f1100                 movss      dword ptr [eax], xmm0
0045d936 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0045d939 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0045d93c 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0045d93f e85ce4faff               call       0x40bda0
0045d944 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0045d947 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0045d94a e871030000               call       0x45dcc0
0045d94f 8945b0                   mov        dword ptr [ebp - 0x50], eax
0045d952 eb09                     jmp        0x45d95d
0045d954 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0045d957 83c008                   add        eax, 8
0045d95a 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0045d95d 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0045d960 3b4db0                   cmp        ecx, dword ptr [ebp - 0x50]
0045d963 0f8487000000             je         0x45d9f0
0045d969 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0045d96c 8955a0                   mov        dword ptr [ebp - 0x60], edx
0045d96f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0045d972 05d0020000               add        eax, 0x2d0
0045d977 8945ac                   mov        dword ptr [ebp - 0x54], eax
0045d97a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0045d97d 51                       push       ecx
0045d97e 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0045d981 e8fa6bfbff               call       0x414580
0045d986 f30f1000                 movss      xmm0, dword ptr [eax]
0045d98a f30f1145a8               movss      dword ptr [ebp - 0x58], xmm0
0045d98f 8d4df8                   lea        ecx, [ebp - 8]
0045d992 e849b9fcff               call       0x4292e0
0045d997 d95da4                   fstp       dword ptr [ebp - 0x5c]
0045d99a 51                       push       ecx
0045d99b f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
0045d9a0 f30f110424               movss      dword ptr [esp], xmm0
0045d9a5 51                       push       ecx
0045d9a6 f30f1045a4               movss      xmm0, dword ptr [ebp - 0x5c]
0045d9ab f30f110424               movss      dword ptr [esp], xmm0
0045d9b0 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
0045d9b3 52                       push       edx
0045d9b4 e877b9fdff               call       0x439330
0045d9b9 83c40c                   add        esp, 0xc
0045d9bc f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0045d9c4 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045d9cc f30f5e0510ed5600         divss      xmm0, dword ptr [0x56ed10]
0045d9d4 51                       push       ecx
0045d9d5 f30f110424               movss      dword ptr [esp], xmm0
0045d9da 8d4df8                   lea        ecx, [ebp - 8]
0045d9dd e8de56ffff               call       0x4530c0
0045d9e2 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0045d9e5 83c001                   add        eax, 1
0045d9e8 8945e0                   mov        dword ptr [ebp - 0x20], eax
0045d9eb e964ffffff               jmp        0x45d954
0045d9f0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045d9f3 894d9c                   mov        dword ptr [ebp - 0x64], ecx
0045d9f6 6a01                     push       1
0045d9f8 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0045d9fb e870fcffff               call       0x45d670
0045da00 8b10                     mov        edx, dword ptr [eax]
0045da02 8b4004                   mov        eax, dword ptr [eax + 4]
0045da05 895590                   mov        dword ptr [ebp - 0x70], edx
0045da08 894594                   mov        dword ptr [ebp - 0x6c], eax
0045da0b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045da0e 894d98                   mov        dword ptr [ebp - 0x68], ecx
0045da11 6a3b                     push       0x3b
0045da13 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0045da16 e855fcffff               call       0x45d670
0045da1b 8b5590                   mov        edx, dword ptr [ebp - 0x70]
0045da1e 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
0045da21 8910                     mov        dword ptr [eax], edx
0045da23 894804                   mov        dword ptr [eax + 4], ecx
0045da26 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045da29 33cd                     xor        ecx, ebp
0045da2b e8be4f0e00               call       0x5429ee
0045da30 8be5                     mov        esp, ebp
0045da32 5d                       pop        ebp
0045da33 8be3                     mov        esp, ebx
0045da35 5b                       pop        ebx
0045da36 c20400                   ret        4
0045da39 cc                       int3       
0045da3a cc                       int3       
0045da3b cc                       int3       
0045da3c cc                       int3       
0045da3d cc                       int3       
0045da3e cc                       int3       
0045da3f cc                       int3       
