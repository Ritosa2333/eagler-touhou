0050fd00 55                       push       ebp
0050fd01 8bec                     mov        ebp, esp
0050fd03 83ec0c                   sub        esp, 0xc
0050fd06 894df4                   mov        dword ptr [ebp - 0xc], ecx
0050fd09 68980f0000               push       0xf98
0050fd0e e84a2a0300               call       0x54275d
0050fd13 83c404                   add        esp, 4
0050fd16 8945fc                   mov        dword ptr [ebp - 4], eax
0050fd19 837dfc00                 cmp        dword ptr [ebp - 4], 0
0050fd1d 741a                     je         0x50fd39
0050fd1f 68980f0000               push       0xf98
0050fd24 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd27 e854c3efff               call       0x40c080
0050fd2c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050fd2f e89c000000               call       0x50fdd0
0050fd34 8945f8                   mov        dword ptr [ebp - 8], eax
0050fd37 eb07                     jmp        0x50fd40
0050fd39 c745f800000000           mov        dword ptr [ebp - 8], 0
0050fd40 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050fd43 8be5                     mov        esp, ebp
0050fd45 5d                       pop        ebp
0050fd46 c20400                   ret        4
0050fd49 cc                       int3       
0050fd4a cc                       int3       
0050fd4b cc                       int3       
0050fd4c cc                       int3       
0050fd4d cc                       int3       
0050fd4e cc                       int3       
0050fd4f cc                       int3       
