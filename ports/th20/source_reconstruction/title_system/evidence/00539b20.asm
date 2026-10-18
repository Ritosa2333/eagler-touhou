00539b20 55                       push       ebp
00539b21 8bec                     mov        ebp, esp
00539b23 83ec0c                   sub        esp, 0xc
00539b26 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00539b2b 33c5                     xor        eax, ebp
00539b2d 8945fc                   mov        dword ptr [ebp - 4], eax
00539b30 894df4                   mov        dword ptr [ebp - 0xc], ecx
00539b33 837d0800                 cmp        dword ptr [ebp + 8], 0
00539b37 7502                     jne        0x539b3b
00539b39 eb36                     jmp        0x539b71
00539b3b 8b4508                   mov        eax, dword ptr [ebp + 8]
00539b3e 50                       push       eax
00539b3f e88c010000               call       0x539cd0
00539b44 83c404                   add        esp, 4
00539b47 6a01                     push       1
00539b49 b940025c00               mov        ecx, 0x5c0240
00539b4e e8bd27edff               call       0x40c310
00539b53 50                       push       eax
00539b54 8d4df8                   lea        ecx, [ebp - 8]
00539b57 e8d41bedff               call       0x40b730
00539b5c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00539b5f 51                       push       ecx
00539b60 e8978e0000               call       0x5429fc
00539b65 83c404                   add        esp, 4
00539b68 8d4df8                   lea        ecx, [ebp - 8]
00539b6b e8901dedff               call       0x40b900
00539b70 90                       nop        
00539b71 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00539b74 33cd                     xor        ecx, ebp
00539b76 e8738e0000               call       0x5429ee
00539b7b 8be5                     mov        esp, ebp
00539b7d 5d                       pop        ebp
00539b7e c20400                   ret        4
00539b81 cc                       int3       
00539b82 cc                       int3       
00539b83 cc                       int3       
00539b84 cc                       int3       
00539b85 cc                       int3       
00539b86 cc                       int3       
00539b87 cc                       int3       
00539b88 cc                       int3       
00539b89 cc                       int3       
00539b8a cc                       int3       
00539b8b cc                       int3       
00539b8c cc                       int3       
00539b8d cc                       int3       
00539b8e cc                       int3       
00539b8f cc                       int3       
