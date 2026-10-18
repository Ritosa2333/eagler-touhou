004c4f00 55                       push       ebp
004c4f01 8bec                     mov        ebp, esp
004c4f03 83ec0c                   sub        esp, 0xc
004c4f06 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4f09 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4f0c 83781801                 cmp        dword ptr [eax + 0x18], 1
004c4f10 7525                     jne        0x4c4f37
004c4f12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4f15 83c11c                   add        ecx, 0x1c
004c4f18 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c4f1b 8b5508                   mov        edx, dword ptr [ebp + 8]
004c4f1e 52                       push       edx
004c4f1f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c4f22 e839e6f5ff               call       0x423560
004c4f27 0fb6c0                   movzx      eax, al
004c4f2a 85c0                     test       eax, eax
004c4f2c 7409                     je         0x4c4f37
004c4f2e c745f801000000           mov        dword ptr [ebp - 8], 1
004c4f35 eb07                     jmp        0x4c4f3e
004c4f37 c745f800000000           mov        dword ptr [ebp - 8], 0
004c4f3e 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
004c4f42 8be5                     mov        esp, ebp
004c4f44 5d                       pop        ebp
004c4f45 c20400                   ret        4
004c4f48 cc                       int3       
004c4f49 cc                       int3       
004c4f4a cc                       int3       
004c4f4b cc                       int3       
004c4f4c cc                       int3       
004c4f4d cc                       int3       
004c4f4e cc                       int3       
004c4f4f cc                       int3       
