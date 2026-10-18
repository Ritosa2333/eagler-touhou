00411ce0 55                       push       ebp
00411ce1 8bec                     mov        ebp, esp
00411ce3 83ec08                   sub        esp, 8
00411ce6 894dfc                   mov        dword ptr [ebp - 4], ecx
00411ce9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411cec 83780c00                 cmp        dword ptr [eax + 0xc], 0
00411cf0 7418                     je         0x411d0a
00411cf2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411cf5 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00411cf8 8955f8                   mov        dword ptr [ebp - 8], edx
00411cfb 8b45fc                   mov        eax, dword ptr [ebp - 4]
00411cfe 50                       push       eax
00411cff 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00411d02 e819000000               call       0x411d20
00411d07 90                       nop        
00411d08 eb09                     jmp        0x411d13
00411d0a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00411d0d e83e000000               call       0x411d50
00411d12 90                       nop        
00411d13 8be5                     mov        esp, ebp
00411d15 5d                       pop        ebp
00411d16 c3                       ret        
00411d17 cc                       int3       
00411d18 cc                       int3       
00411d19 cc                       int3       
00411d1a cc                       int3       
00411d1b cc                       int3       
00411d1c cc                       int3       
00411d1d cc                       int3       
00411d1e cc                       int3       
00411d1f cc                       int3       
