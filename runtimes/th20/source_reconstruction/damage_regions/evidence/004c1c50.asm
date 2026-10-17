004c1c50 55                       push       ebp
004c1c51 8bec                     mov        ebp, esp
004c1c53 51                       push       ecx
004c1c54 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1c57 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1c5a 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1c5d 83c901                   or         ecx, 1
004c1c60 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1c63 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1c66 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1c69 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1c6c 83e1f1                   and        ecx, 0xfffffff1
004c1c6f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1c72 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1c75 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1c78 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1c7b 83e1ef                   and        ecx, 0xffffffef
004c1c7e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1c81 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1c84 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1c87 8b4814                   mov        ecx, dword ptr [eax + 0x14]
004c1c8a 83e1bf                   and        ecx, 0xffffffbf
004c1c8d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1c90 894a14                   mov        dword ptr [edx + 0x14], ecx
004c1c93 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1c96 83c134                   add        ecx, 0x34
004c1c99 e83257feff               call       0x4a73d0
004c1c9e 8b4508                   mov        eax, dword ptr [ebp + 8]
004c1ca1 50                       push       eax
004c1ca2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1ca5 e886030000               call       0x4c2030
004c1caa 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1cad f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004c1cb2 f30f11412c               movss      dword ptr [ecx + 0x2c], xmm0
004c1cb7 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1cba f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004c1cbf f30f114230               movss      dword ptr [edx + 0x30], xmm0
004c1cc4 51                       push       ecx
004c1cc5 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004c1cca f30f110424               movss      dword ptr [esp], xmm0
004c1ccf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1cd2 83c124                   add        ecx, 0x24
004c1cd5 e84612f9ff               call       0x452f20
004c1cda 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1cdd 0f57c0                   xorps      xmm0, xmm0
004c1ce0 f30f114028               movss      dword ptr [eax + 0x28], xmm0
004c1ce5 8b4d18                   mov        ecx, dword ptr [ebp + 0x18]
004c1ce8 51                       push       ecx
004c1ce9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1cec 83c17c                   add        ecx, 0x7c
004c1cef e82c18f6ff               call       0x423520
004c1cf4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1cf7 8b451c                   mov        eax, dword ptr [ebp + 0x1c]
004c1cfa 898294000000             mov        dword ptr [edx + 0x94], eax
004c1d00 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1d03 c7819800000000000000     mov        dword ptr [ecx + 0x98], 0
004c1d0d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1d10 c7829c0000007f969800     mov        dword ptr [edx + 0x9c], 0x98967f
004c1d1a 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1d1d c780a000000001000000     mov        dword ptr [eax + 0xa0], 1
004c1d27 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1d2a c781b400000000000000     mov        dword ptr [ecx + 0xb4], 0
004c1d34 6a00                     push       0
004c1d36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1d39 81c1a4000000             add        ecx, 0xa4
004c1d3f e85cfaf4ff               call       0x4117a0
004c1d44 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c1d47 c782a800000000000000     mov        dword ptr [edx + 0xa8], 0
004c1d51 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c1d54 c780ac00000000000000     mov        dword ptr [eax + 0xac], 0
004c1d5e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1d61 81c18c000000             add        ecx, 0x8c
004c1d67 e894a5f4ff               call       0x40c300
004c1d6c 8be5                     mov        esp, ebp
004c1d6e 5d                       pop        ebp
004c1d6f c21800                   ret        0x18
004c1d72 cc                       int3       
004c1d73 cc                       int3       
004c1d74 cc                       int3       
004c1d75 cc                       int3       
004c1d76 cc                       int3       
004c1d77 cc                       int3       
004c1d78 cc                       int3       
004c1d79 cc                       int3       
004c1d7a cc                       int3       
004c1d7b cc                       int3       
004c1d7c cc                       int3       
004c1d7d cc                       int3       
004c1d7e cc                       int3       
004c1d7f cc                       int3       
