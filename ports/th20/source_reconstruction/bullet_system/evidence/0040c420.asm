0040c420 55                       push       ebp
0040c421 8bec                     mov        ebp, esp
0040c423 83ec10                   sub        esp, 0x10
0040c426 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0040c42b 33c5                     xor        eax, ebp
0040c42d 8945fc                   mov        dword ptr [ebp - 4], eax
0040c430 894df4                   mov        dword ptr [ebp - 0xc], ecx
0040c433 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0040c436 50                       push       eax
0040c437 e844eeffff               call       0x40b280
0040c43c 83c404                   add        esp, 4
0040c43f 8945f0                   mov        dword ptr [ebp - 0x10], eax
0040c442 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0040c445 8b11                     mov        edx, dword ptr [ecx]
0040c447 8955f8                   mov        dword ptr [ebp - 8], edx
0040c44a 8b4508                   mov        eax, dword ptr [ebp + 8]
0040c44d 50                       push       eax
0040c44e e83df9ffff               call       0x40bd90
0040c453 83c404                   add        esp, 4
0040c456 837d0800                 cmp        dword ptr [ebp + 8], 0
0040c45a 7401                     je         0x40c45d
0040c45c 90                       nop        
0040c45d 8b45f8                   mov        eax, dword ptr [ebp - 8]
0040c460 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0040c463 33cd                     xor        ecx, ebp
0040c465 e884651300               call       0x5429ee
0040c46a 8be5                     mov        esp, ebp
0040c46c 5d                       pop        ebp
0040c46d c20400                   ret        4
