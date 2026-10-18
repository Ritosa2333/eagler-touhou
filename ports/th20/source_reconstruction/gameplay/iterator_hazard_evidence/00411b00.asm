00411b00 55                       push       ebp
00411b01 8bec                     mov        ebp, esp
00411b03 6aff                     push       -1
00411b05 6850775600               push       0x567750
00411b0a 64a100000000             mov        eax, dword ptr fs:[0]
00411b10 50                       push       eax
00411b11 83ec0c                   sub        esp, 0xc
00411b14 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00411b19 33c5                     xor        eax, ebp
00411b1b 50                       push       eax
00411b1c 8d45f4                   lea        eax, [ebp - 0xc]
00411b1f 64a300000000             mov        dword ptr fs:[0], eax
00411b25 894df0                   mov        dword ptr [ebp - 0x10], ecx
00411b28 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00411b2b 833800                   cmp        dword ptr [eax], 0
00411b2e 7413                     je         0x411b43
00411b30 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00411b33 8b11                     mov        edx, dword ptr [ecx]
00411b35 8955ec                   mov        dword ptr [ebp - 0x14], edx
00411b38 6a00                     push       0
00411b3a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00411b3d e85e120000               call       0x412da0
00411b42 90                       nop        
00411b43 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00411b46 83780400                 cmp        dword ptr [eax + 4], 0
00411b4a 7414                     je         0x411b60
00411b4c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00411b4f 8b5104                   mov        edx, dword ptr [ecx + 4]
00411b52 8955e8                   mov        dword ptr [ebp - 0x18], edx
00411b55 6a00                     push       0
00411b57 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00411b5a e841120000               call       0x412da0
00411b5f 90                       nop        
00411b60 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00411b63 64890d00000000           mov        dword ptr fs:[0], ecx
00411b6a 59                       pop        ecx
00411b6b 8be5                     mov        esp, ebp
00411b6d 5d                       pop        ebp
00411b6e c3                       ret        
00411b6f cc                       int3       
00411b70 cc                       int3       
00411b71 cc                       int3       
00411b72 cc                       int3       
00411b73 cc                       int3       
00411b74 cc                       int3       
00411b75 cc                       int3       
00411b76 cc                       int3       
00411b77 cc                       int3       
00411b78 cc                       int3       
00411b79 cc                       int3       
00411b7a cc                       int3       
00411b7b cc                       int3       
00411b7c cc                       int3       
00411b7d cc                       int3       
00411b7e cc                       int3       
00411b7f cc                       int3       
