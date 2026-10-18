004cf940 55                       push       ebp
004cf941 8bec                     mov        ebp, esp
004cf943 83ec10                   sub        esp, 0x10
004cf946 894dfc                   mov        dword ptr [ebp - 4], ecx
004cf949 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cf94c 8b10                     mov        edx, dword ptr [eax]
004cf94e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf951 8b4218                   mov        eax, dword ptr [edx + 0x18]
004cf954 ffd0                     call       eax
004cf956 85c0                     test       eax, eax
004cf958 740a                     je         0x4cf964
004cf95a b801000000               mov        eax, 1
004cf95f e9c3000000               jmp        0x4cfa27
004cf964 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf967 8b11                     mov        edx, dword ptr [ecx]
004cf969 8b424c                   mov        eax, dword ptr [edx + 0x4c]
004cf96c 8945f8                   mov        dword ptr [ebp - 8], eax
004cf96f 6a00                     push       0
004cf971 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf974 ff55f8                   call       dword ptr [ebp - 8]
004cf977 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf97a 81c170070000             add        ecx, 0x770
004cf980 894df4                   mov        dword ptr [ebp - 0xc], ecx
004cf983 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf986 81c170070000             add        ecx, 0x770
004cf98c e83f83f6ff               call       0x437cd0
004cf991 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf994 f30f104278               movss      xmm0, dword ptr [edx + 0x78]
004cf999 f30f5e404c               divss      xmm0, dword ptr [eax + 0x4c]
004cf99e 51                       push       ecx
004cf99f f30f110424               movss      dword ptr [esp], xmm0
004cf9a4 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004cf9a7 e8447efaff               call       0x4777f0
004cf9ac 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cf9af 0570070000               add        eax, 0x770
004cf9b4 8945f0                   mov        dword ptr [ebp - 0x10], eax
004cf9b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf9ba 81c170070000             add        ecx, 0x770
004cf9c0 e80b83f6ff               call       0x437cd0
004cf9c5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf9c8 f30f104174               movss      xmm0, dword ptr [ecx + 0x74]
004cf9cd f30f5e4048               divss      xmm0, dword ptr [eax + 0x48]
004cf9d2 51                       push       ecx
004cf9d3 f30f110424               movss      dword ptr [esp], xmm0
004cf9d8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004cf9db e8e07dfaff               call       0x4777c0
004cf9e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cf9e3 81c170070000             add        ecx, 0x770
004cf9e9 e8e2bbf5ff               call       0x42b5d0
004cf9ee 90                       nop        
004cf9ef 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cf9f2 f30f108280000000         movss      xmm0, dword ptr [edx + 0x80]
004cf9fa 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
004cfa01 9f                       lahf       
004cfa02 f6c444                   test       ah, 0x44
004cfa05 7a0f                     jp         0x4cfa16
004cfa07 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004cfa0a 81c1540d0000             add        ecx, 0xd54
004cfa10 e8bbbbf5ff               call       0x42b5d0
004cfa15 90                       nop        
004cfa16 8b45fc                   mov        eax, dword ptr [ebp - 4]
004cfa19 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004cfa1c 83e1df                   and        ecx, 0xffffffdf
004cfa1f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004cfa22 894a1c                   mov        dword ptr [edx + 0x1c], ecx
004cfa25 33c0                     xor        eax, eax
004cfa27 8be5                     mov        esp, ebp
004cfa29 5d                       pop        ebp
004cfa2a c3                       ret        
004cfa2b cc                       int3       
004cfa2c cc                       int3       
004cfa2d cc                       int3       
004cfa2e cc                       int3       
004cfa2f cc                       int3       
