00411c30 55                       push       ebp
00411c31 8bec                     mov        ebp, esp
00411c33 83ec0c                   sub        esp, 0xc
00411c36 894dfc                   mov        dword ptr [ebp - 4], ecx
00411c39 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c3c 833800                   cmp        dword ptr [eax], 0
00411c3f 7413                     je         0x411c54
00411c41 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411c44 8b11                     mov        edx, dword ptr [ecx]
00411c46 8955f8                   mov        dword ptr [ebp - 8], edx
00411c49 6a00                     push       0
00411c4b 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00411c4e e84d110000               call       0x412da0
00411c53 90                       nop        
00411c54 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c57 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411c5a 8b5104                   mov        edx, dword ptr [ecx + 4]
00411c5d 8910                     mov        dword ptr [eax], edx
00411c5f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411c65 8b11                     mov        edx, dword ptr [ecx]
00411c67 895004                   mov        dword ptr [eax + 4], edx
00411c6a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c6d 83780400                 cmp        dword ptr [eax + 4], 0
00411c71 742f                     je         0x411ca2
00411c73 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411c76 8b09                     mov        ecx, dword ptr [ecx]
00411c78 e813e3ffff               call       0x40ff90
00411c7d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411c80 894204                   mov        dword ptr [edx + 4], eax
00411c83 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c86 83780400                 cmp        dword ptr [eax + 4], 0
00411c8a 7416                     je         0x411ca2
00411c8c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411c8f 8b5104                   mov        edx, dword ptr [ecx + 4]
00411c92 8955f4                   mov        dword ptr [ebp - 0xc], edx
00411c95 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411c98 50                       push       eax
00411c99 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00411c9c e8ff100000               call       0x412da0
00411ca1 90                       nop        
00411ca2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411ca5 8be5                     mov        esp, ebp
00411ca7 5d                       pop        ebp
00411ca8 c3                       ret        
00411ca9 cc                       int3       
00411caa cc                       int3       
00411cab cc                       int3       
00411cac cc                       int3       
00411cad cc                       int3       
00411cae cc                       int3       
00411caf cc                       int3       
