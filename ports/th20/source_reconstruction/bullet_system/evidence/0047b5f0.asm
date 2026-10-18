0047b5f0 55                       push       ebp
0047b5f1 8bec                     mov        ebp, esp
0047b5f3 83ec0c                   sub        esp, 0xc
0047b5f6 894df4                   mov        dword ptr [ebp - 0xc], ecx
0047b5f9 68a86d2800               push       0x286da8
0047b5fe e85a710c00               call       0x54275d
0047b603 83c404                   add        esp, 4
0047b606 8945fc                   mov        dword ptr [ebp - 4], eax
0047b609 837dfc00                 cmp        dword ptr [ebp - 4], 0
0047b60d 741a                     je         0x47b629
0047b60f 68a86d2800               push       0x286da8
0047b614 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047b617 e8640af9ff               call       0x40c080
0047b61c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047b61f e87c020000               call       0x47b8a0
0047b624 8945f8                   mov        dword ptr [ebp - 8], eax
0047b627 eb07                     jmp        0x47b630
0047b629 c745f800000000           mov        dword ptr [ebp - 8], 0
0047b630 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047b633 8be5                     mov        esp, ebp
0047b635 5d                       pop        ebp
0047b636 c20400                   ret        4
0047b639 cc                       int3       
0047b63a cc                       int3       
0047b63b cc                       int3       
0047b63c cc                       int3       
0047b63d cc                       int3       
0047b63e cc                       int3       
0047b63f cc                       int3       
