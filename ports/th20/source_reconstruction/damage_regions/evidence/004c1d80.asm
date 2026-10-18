004c1d80 55                       push       ebp
004c1d81 8bec                     mov        ebp, esp
004c1d83 51                       push       ecx
004c1d84 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1d87 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1d8a 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1d8d 83c901                   or         ecx, 1
004c1d90 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1d93 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1d96 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1d99 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1d9c 83e1f1                   and        ecx, 0xfffffff1
004c1d9f 83c902                   or         ecx, 2
004c1da2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1da5 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1da8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1dab 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1dae 83e1ef                   and        ecx, 0xffffffef
004c1db1 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1db4 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1db7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1dba 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1dbd 83e1bf                   and        ecx, 0xffffffbf
004c1dc0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1dc3 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1dc6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1dc9 83c134                   add        ecx, 0x34
004c1dcc e8ff55feff               call       0x4a73d0
004c1dd1 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1dd4 50                       push       eax
004c1dd5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1dd8 e853020000               call       0x4c2030
004c1ddd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1de0 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004c1de5 f30f114118               movss      dword ptr [ecx + 0x18], xmm0
004c1dea 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1ded f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c1df2 f30f114220               movss      dword ptr [edx + 0x20], xmm0
004c1df7 8b4514                   mov        eax, dword ptr [ebp + 0x14]
004c1dfa 50                       push       eax
004c1dfb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1dfe 83c17c                   add        ecx, 0x7c
004c1e01 e81a17f6ff               call       0x423520
004c1e06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1e09 8b5518                   mov        edx, dword ptr [ebp + 0x18]
004c1e0c 899194000000             mov        dword ptr [ecx + 0x94], edx
004c1e12 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1e15 c7809800000000000000     mov        dword ptr [eax + 0x98], 0
004c1e1f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1e22 c7819c0000007f969800     mov        dword ptr [ecx + 0x9c], 0x98967f
004c1e2c 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1e2f c782a000000001000000     mov        dword ptr [edx + 0xa0], 1
004c1e39 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1e3c c780b400000000000000     mov        dword ptr [eax + 0xb4], 0
004c1e46 6a00                     push       0
004c1e48 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1e4b 81c1a4000000             add        ecx, 0xa4
004c1e51 e84af9f4ff               call       0x4117a0
004c1e56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1e59 c781a800000000000000     mov        dword ptr [ecx + 0xa8], 0
004c1e63 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1e66 c782ac00000000000000     mov        dword ptr [edx + 0xac], 0
004c1e70 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1e73 81c18c000000             add        ecx, 0x8c
004c1e79 e882a4f4ff               call       0x40c300
004c1e7e 8be5                     mov        esp, ebp
004c1e80 5d                       pop        ebp
004c1e81 c21400                   ret        0x14
004c1e84 cc                       int3       
004c1e85 cc                       int3       
004c1e86 cc                       int3       
004c1e87 cc                       int3       
004c1e88 cc                       int3       
004c1e89 cc                       int3       
004c1e8a cc                       int3       
004c1e8b cc                       int3       
004c1e8c cc                       int3       
004c1e8d cc                       int3       
004c1e8e cc                       int3       
004c1e8f cc                       int3       
