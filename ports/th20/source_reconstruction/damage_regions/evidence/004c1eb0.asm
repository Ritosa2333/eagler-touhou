004c1eb0 55                       push       ebp
004c1eb1 8bec                     mov        ebp, esp
004c1eb3 83ec08                   sub        esp, 8
004c1eb6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c1eb9 837d0800                 cmp        dword ptr [ebp + 8], 0
004c1ebd 7431                     je         0x4c1ef0
004c1ebf 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1ec2 e819fef4ff               call       0x411ce0
004c1ec7 90                       nop        
004c1ec8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c1ecb e830f1ffff               call       0x4c1000
004c1ed0 0fb6c0                   movzx      eax, al
004c1ed3 85c0                     test       eax, eax
004c1ed5 7519                     jne        0x4c1ef0
004c1ed7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c1eda 81c12cc40000             add        ecx, 0xc42c
004c1ee0 894df8                   mov        dword ptr [ebp - 8], ecx
004c1ee3 8b5508                   mov        edx, dword ptr [ebp + 8]
004c1ee6 52                       push       edx
004c1ee7 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c1eea e8c187f8ff               call       0x44a6b0
004c1eef 90                       nop        
004c1ef0 8be5                     mov        esp, ebp
004c1ef2 5d                       pop        ebp
004c1ef3 c20400                   ret        4
004c1ef6 cc                       int3       
004c1ef7 cc                       int3       
004c1ef8 cc                       int3       
004c1ef9 cc                       int3       
004c1efa cc                       int3       
004c1efb cc                       int3       
004c1efc cc                       int3       
004c1efd cc                       int3       
004c1efe cc                       int3       
004c1eff cc                       int3       
