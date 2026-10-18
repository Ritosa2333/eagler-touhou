004cfa30 55                       push       ebp
004cfa31 8bec                     mov        ebp, esp
004cfa33 83ec14                   sub        esp, 0x14
004cfa36 894dfc                   mov        dword ptr [ebp - 4], ecx
004cfa39 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfa3c 8b10                     mov        edx, dword ptr [eax]
004cfa3e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa41 8b4218                   mov        eax, dword ptr [edx + 0x18]
004cfa44 ffd0                     call       eax
004cfa46 85c0                     test       eax, eax
004cfa48 740a                     je         0x4cfa54
004cfa4a b801000000               mov        eax, 1
004cfa4f e9e4000000               jmp        0x4cfb38
004cfa54 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa57 8b11                     mov        edx, dword ptr [ecx]
004cfa59 8b424c                   mov        eax, dword ptr [edx + 0x4c]
004cfa5c 8945f8                   mov        dword ptr [ebp - 8], eax
004cfa5f 6a00                     push       0
004cfa61 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa64 ff55f8                   call       dword ptr [ebp - 8]
004cfa67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa6a 81c14c070000             add        ecx, 0x74c
004cfa70 894df4                   mov        dword ptr [ebp - 0xc], ecx
004cfa73 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa76 81c14c070000             add        ecx, 0x74c
004cfa7c e84f82f6ff               call       0x437cd0
004cfa81 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cfa84 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004cfa89 f30f5e404c               divss      xmm0, dword ptr [eax + 0x4c]
004cfa8e 51                       push       ecx
004cfa8f f30f110424               movss      dword ptr [esp], xmm0
004cfa94 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004cfa97 e8547dfaff               call       0x4777f0
004cfa9c 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfa9f 054c070000               add        eax, 0x74c
004cfaa4 8945f0                   mov        dword ptr [ebp - 0x10], eax
004cfaa7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfaaa 81c14c070000             add        ecx, 0x74c
004cfab0 e81b82f6ff               call       0x437cd0
004cfab5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfab8 f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004cfabd f30f5e4048               divss      xmm0, dword ptr [eax + 0x48]
004cfac2 51                       push       ecx
004cfac3 f30f110424               movss      dword ptr [esp], xmm0
004cfac8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004cfacb e8f07cfaff               call       0x4777c0
004cfad0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfad3 81c14c070000             add        ecx, 0x74c
004cfad9 e8f2baf5ff               call       0x42b5d0
004cfade 90                       nop        
004cfadf 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cfae2 f30f108280000000         movss      xmm0, dword ptr [edx + 0x80]
004cfaea 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004cfaf1 9f                       lahf       
004cfaf2 f6c444                   test       ah, 0x44
004cfaf5 7a0f                     jp         0x4cfb06
004cfaf7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfafa 81c1300d0000             add        ecx, 0xd30
004cfb00 e8cbbaf5ff               call       0x42b5d0
004cfb05 90                       nop        
004cfb06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfb09 81c114130000             add        ecx, 0x1314
004cfb0f e8bcbaf5ff               call       0x42b5d0
004cfb14 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfb17 83c048                   add        eax, 0x48
004cfb1a 8945ec                   mov        dword ptr [ebp - 0x14], eax
004cfb1d 6a00                     push       0
004cfb1f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004cfb22 e8193af5ff               call       0x423540
004cfb27 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfb2a 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
004cfb2d 83e2df                   and        edx, 0xffffffdf
004cfb30 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfb33 89501c                   mov        dword ptr [eax + 0x1c], edx
004cfb36 33c0                     xor        eax, eax
004cfb38 8be5                     mov        esp, ebp
004cfb3a 5d                       pop        ebp
004cfb3b c3                       ret        
004cfb3c cc                       int3       
004cfb3d cc                       int3       
004cfb3e cc                       int3       
004cfb3f cc                       int3       
