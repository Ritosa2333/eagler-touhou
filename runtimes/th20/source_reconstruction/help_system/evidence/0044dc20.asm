0044dc20 55                       push       ebp
0044dc21 8bec                     mov        ebp, esp
0044dc23 83ec2c                   sub        esp, 0x2c
0044dc26 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044dc2b 33c5                     xor        eax, ebp
0044dc2d 8945fc                   mov        dword ptr [ebp - 4], eax
0044dc30 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0044dc33 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0044dc36 50                       push       eax
0044dc37 e874f3ffff               call       0x44cfb0
0044dc3c 83c404                   add        esp, 4
0044dc3f 894514                   mov        dword ptr [ebp + 0x14], eax
0044dc42 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044dc45 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0044dc48 895108                   mov        dword ptr [ecx + 8], edx
0044dc4b 8b4508                   mov        eax, dword ptr [ebp + 8]
0044dc4e 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0044dc51 83e1fc                   and        ecx, 0xfffffffc
0044dc54 8b5508                   mov        edx, dword ptr [ebp + 8]
0044dc57 894a14                   mov        dword ptr [edx + 0x14], ecx
0044dc5a c745f800000000           mov        dword ptr [ebp - 8], 0
0044dc61 8b4508                   mov        eax, dword ptr [ebp + 8]
0044dc64 8b08                     mov        ecx, dword ptr [eax]
0044dc66 894dec                   mov        dword ptr [ebp - 0x14], ecx
0044dc69 8b5508                   mov        edx, dword ptr [ebp + 8]
0044dc6c 8b02                     mov        eax, dword ptr [edx]
0044dc6e 8b08                     mov        ecx, dword ptr [eax]
0044dc70 8b5148                   mov        edx, dword ptr [ecx + 0x48]
0044dc73 8955e8                   mov        dword ptr [ebp - 0x18], edx
0044dc76 8d45f8                   lea        eax, [ebp - 8]
0044dc79 50                       push       eax
0044dc7a 6a00                     push       0
0044dc7c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0044dc7f 51                       push       ecx
0044dc80 ff55e8                   call       dword ptr [ebp - 0x18]
0044dc83 90                       nop        
0044dc84 837d1c00                 cmp        dword ptr [ebp + 0x1c], 0
0044dc88 7520                     jne        0x44dcaa
0044dc8a 6a00                     push       0
0044dc8c 6a00                     push       0
0044dc8e 6a01                     push       1
0044dc90 6a00                     push       0
0044dc92 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0044dc95 52                       push       edx
0044dc96 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044dc99 50                       push       eax
0044dc9a 6a00                     push       0
0044dc9c 6a00                     push       0
0044dc9e 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0044dca1 51                       push       ecx
0044dca2 e8ef290f00               call       0x540696
0044dca7 90                       nop        
0044dca8 eb5c                     jmp        0x44dd06
0044dcaa 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0044dcad 8955f0                   mov        dword ptr [ebp - 0x10], edx
0044dcb0 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0044dcb3 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0044dcb6 03481c                   add        ecx, dword ptr [eax + 0x1c]
0044dcb9 894df4                   mov        dword ptr [ebp - 0xc], ecx
0044dcbc c745d400000000           mov        dword ptr [ebp - 0x2c], 0
0044dcc3 c745d800000000           mov        dword ptr [ebp - 0x28], 0
0044dcca 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
0044dccd 0fbf4208                 movsx      eax, word ptr [edx + 8]
0044dcd1 8945dc                   mov        dword ptr [ebp - 0x24], eax
0044dcd4 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044dcd7 0fbf510a                 movsx      edx, word ptr [ecx + 0xa]
0044dcdb 8955e0                   mov        dword ptr [ebp - 0x20], edx
0044dcde 6a00                     push       0
0044dce0 6a00                     push       0
0044dce2 6a01                     push       1
0044dce4 6a00                     push       0
0044dce6 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0044dce9 50                       push       eax
0044dcea 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044dced 8b511c                   mov        edx, dword ptr [ecx + 0x1c]
0044dcf0 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044dcf3 8d4c1010                 lea        ecx, [eax + edx + 0x10]
0044dcf7 51                       push       ecx
0044dcf8 6a00                     push       0
0044dcfa 6a00                     push       0
0044dcfc 8b55f8                   mov        edx, dword ptr [ebp - 8]
0044dcff 52                       push       edx
0044dd00 e891290f00               call       0x540696
0044dd05 90                       nop        
0044dd06 8b45f8                   mov        eax, dword ptr [ebp - 8]
0044dd09 8b08                     mov        ecx, dword ptr [eax]
0044dd0b 8b55f8                   mov        edx, dword ptr [ebp - 8]
0044dd0e 52                       push       edx
0044dd0f 8b4108                   mov        eax, dword ptr [ecx + 8]
0044dd12 ffd0                     call       eax
0044dd14 90                       nop        
0044dd15 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044dd18 8b11                     mov        edx, dword ptr [ecx]
0044dd1a 52                       push       edx
0044dd1b 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044dd1e e8ed3b0000               call       0x451910
0044dd23 8b4508                   mov        eax, dword ptr [ebp + 8]
0044dd26 8b4d14                   mov        ecx, dword ptr [ebp + 0x14]
0044dd29 8b148d2ce25600           mov        edx, dword ptr [ecx*4 + 0x56e22c]
0044dd30 89500c                   mov        dword ptr [eax + 0xc], edx
0044dd33 33c0                     xor        eax, eax
0044dd35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044dd38 33cd                     xor        ecx, ebp
0044dd3a e8af4c0f00               call       0x5429ee
0044dd3f 8be5                     mov        esp, ebp
0044dd41 5d                       pop        ebp
0044dd42 c21800                   ret        0x18
0044dd45 cc                       int3       
0044dd46 cc                       int3       
0044dd47 cc                       int3       
0044dd48 cc                       int3       
0044dd49 cc                       int3       
0044dd4a cc                       int3       
0044dd4b cc                       int3       
0044dd4c cc                       int3       
0044dd4d cc                       int3       
0044dd4e cc                       int3       
0044dd4f cc                       int3       
