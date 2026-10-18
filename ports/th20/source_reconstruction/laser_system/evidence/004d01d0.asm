004d01d0 55                       push       ebp
004d01d1 8bec                     mov        ebp, esp
004d01d3 83ec10                   sub        esp, 0x10
004d01d6 894dfc                   mov        dword ptr [ebp - 4], ecx
004d01d9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d01dc 0570070000               add        eax, 0x770
004d01e1 8945f8                   mov        dword ptr [ebp - 8], eax
004d01e4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d01e7 83c158                   add        ecx, 0x58
004d01ea 51                       push       ecx
004d01eb 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d01ee e8fd08faff               call       0x470af0
004d01f3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d01f6 81c270070000             add        edx, 0x770
004d01fc 8955f4                   mov        dword ptr [ebp - 0xc], edx
004d01ff f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
004d0207 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004d020f 51                       push       ecx
004d0210 f30f110424               movss      dword ptr [esp], xmm0
004d0215 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d0218 51                       push       ecx
004d0219 f30f104070               movss      xmm0, dword ptr [eax + 0x70]
004d021e f30f110424               movss      dword ptr [esp], xmm0
004d0223 e8d883f6ff               call       0x438600
004d0228 83c404                   add        esp, 4
004d022b d91c24                   fstp       dword ptr [esp]
004d022e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004d0231 e85a03f8ff               call       0x450590
004d0236 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d0239 81c170070000             add        ecx, 0x770
004d023f e82cc3f7ff               call       0x44c570
004d0244 90                       nop        
004d0245 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d0248 f30f108180000000         movss      xmm0, dword ptr [ecx + 0x80]
004d0250 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004d0257 9f                       lahf       
004d0258 f6c444                   test       ah, 0x44
004d025b 7a2a                     jp         0x4d0287
004d025d 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d0260 81c2540d0000             add        edx, 0xd54
004d0266 8955f0                   mov        dword ptr [ebp - 0x10], edx
004d0269 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d026c 83c058                   add        eax, 0x58
004d026f 50                       push       eax
004d0270 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004d0273 e87808faff               call       0x470af0
004d0278 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d027b 81c1540d0000             add        ecx, 0xd54
004d0281 e8eac2f7ff               call       0x44c570
004d0286 90                       nop        
004d0287 33c0                     xor        eax, eax
004d0289 8be5                     mov        esp, ebp
004d028b 5d                       pop        ebp
004d028c c3                       ret        
004d028d cc                       int3       
004d028e cc                       int3       
004d028f cc                       int3       
