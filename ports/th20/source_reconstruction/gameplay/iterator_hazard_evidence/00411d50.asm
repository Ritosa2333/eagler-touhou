00411d50 55                       push       ebp
00411d51 8bec                     mov        ebp, esp
00411d53 83ec08                   sub        esp, 8
00411d56 894dfc                   mov        dword ptr [ebp - 4], ecx
00411d59 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411d5c 83781000                 cmp        dword ptr [eax + 0x10], 0
00411d60 7474                     je         0x411dd6
00411d62 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411d65 8b5110                   mov        edx, dword ptr [ecx + 0x10]
00411d68 8b02                     mov        eax, dword ptr [edx]
00411d6a 3b45fc                   cmp        eax, dword ptr [ebp - 4]
00411d6d 750c                     jne        0x411d7b
00411d6f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411d72 8b5110                   mov        edx, dword ptr [ecx + 0x10]
00411d75 c70200000000             mov        dword ptr [edx], 0
00411d7b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411d7e 8b4810                   mov        ecx, dword ptr [eax + 0x10]
00411d81 8b5104                   mov        edx, dword ptr [ecx + 4]
00411d84 3b55fc                   cmp        edx, dword ptr [ebp - 4]
00411d87 7543                     jne        0x411dcc
00411d89 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411d8c 8b4810                   mov        ecx, dword ptr [eax + 0x10]
00411d8f 83790400                 cmp        dword ptr [ecx + 4], 0
00411d93 7437                     je         0x411dcc
00411d95 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411d98 8b4210                   mov        eax, dword ptr [edx + 0x10]
00411d9b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411d9e 8b5104                   mov        edx, dword ptr [ecx + 4]
00411da1 895004                   mov        dword ptr [eax + 4], edx
00411da4 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411da7 8b4810                   mov        ecx, dword ptr [eax + 0x10]
00411daa 83790400                 cmp        dword ptr [ecx + 4], 0
00411dae 741c                     je         0x411dcc
00411db0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411db3 8b4210                   mov        eax, dword ptr [edx + 0x10]
00411db6 8b4804                   mov        ecx, dword ptr [eax + 4]
00411db9 894df8                   mov        dword ptr [ebp - 8], ecx
00411dbc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411dbf 8b4210                   mov        eax, dword ptr [edx + 0x10]
00411dc2 50                       push       eax
00411dc3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00411dc6 e8d50f0000               call       0x412da0
00411dcb 90                       nop        
00411dcc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411dcf c7411000000000           mov        dword ptr [ecx + 0x10], 0
00411dd6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411dd9 837a0400                 cmp        dword ptr [edx + 4], 0
00411ddd 740f                     je         0x411dee
00411ddf 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411de2 8b4804                   mov        ecx, dword ptr [eax + 4]
00411de5 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411de8 8b4208                   mov        eax, dword ptr [edx + 8]
00411deb 894108                   mov        dword ptr [ecx + 8], eax
00411dee 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411df1 83790800                 cmp        dword ptr [ecx + 8], 0
00411df5 740f                     je         0x411e06
00411df7 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411dfa 8b4208                   mov        eax, dword ptr [edx + 8]
00411dfd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411e00 8b5104                   mov        edx, dword ptr [ecx + 4]
00411e03 895004                   mov        dword ptr [eax + 4], edx
00411e06 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411e09 c7400400000000           mov        dword ptr [eax + 4], 0
00411e10 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411e13 c7410800000000           mov        dword ptr [ecx + 8], 0
00411e1a 8b55fc                   mov        edx, dword ptr [ebp - 4]
00411e1d c7420c00000000           mov        dword ptr [edx + 0xc], 0
00411e24 8be5                     mov        esp, ebp
00411e26 5d                       pop        ebp
00411e27 c3                       ret        
00411e28 cc                       int3       
00411e29 cc                       int3       
00411e2a cc                       int3       
00411e2b cc                       int3       
00411e2c cc                       int3       
00411e2d cc                       int3       
00411e2e cc                       int3       
00411e2f cc                       int3       
