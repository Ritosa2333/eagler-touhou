0051f7e0 55                       push       ebp
0051f7e1 8bec                     mov        ebp, esp
0051f7e3 8b0d24615c00             mov        ecx, dword ptr [0x5c6124]
0051f7e9 e8d2fbffff               call       0x51f3c0
0051f7ee 85c0                     test       eax, eax
0051f7f0 7404                     je         0x51f7f6
0051f7f2 eb7d                     jmp        0x51f871
0051f7f4 eb7b                     jmp        0x51f871
0051f7f6 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0051f7fc e87fe0f2ff               call       0x44d880
0051f801 0fb6c0                   movzx      eax, al
0051f804 85c0                     test       eax, eax
0051f806 750b                     jne        0x51f813
0051f808 6a01                     push       1
0051f80a ff156cc15600             call       dword ptr [0x56c16c]
0051f810 90                       nop        
0051f811 ebe3                     jmp        0x51f7f6
0051f813 833d2c4d5c0000           cmp        dword ptr [0x5c4d2c], 0
0051f81a 7439                     je         0x51f855
0051f81c 8b0d2c4d5c00             mov        ecx, dword ptr [0x5c4d2c]
0051f822 e859ca0000               call       0x52c280
0051f827 3db4000000               cmp        eax, 0xb4
0051f82c 7d1b                     jge        0x51f849
0051f82e 8b0d88585c00             mov        ecx, dword ptr [0x5c5888]
0051f834 c1e905                   shr        ecx, 5
0051f837 83e103                   and        ecx, 3
0051f83a 7402                     je         0x51f83e
0051f83c eb0b                     jmp        0x51f849
0051f83e 6a10                     push       0x10
0051f840 ff156cc15600             call       dword ptr [0x56c16c]
0051f846 90                       nop        
0051f847 ebd3                     jmp        0x51f81c
0051f849 8b0d2c4d5c00             mov        ecx, dword ptr [0x5c4d2c]
0051f84f e89cc90000               call       0x52c1f0
0051f854 90                       nop        
0051f855 8b1524615c00             mov        edx, dword ptr [0x5c6124]
0051f85b 8b4a08                   mov        ecx, dword ptr [edx + 8]
0051f85e e81d35efff               call       0x412d80
0051f863 c70558885b0001000000     mov        dword ptr [0x5b8858], 1
0051f86d 33c0                     xor        eax, eax
0051f86f eb0e                     jmp        0x51f87f
0051f871 6a03                     push       3
0051f873 b9404d5c00               mov        ecx, 0x5c4d40
0051f878 e83317f8ff               call       0x4a0fb0
0051f87d 33c0                     xor        eax, eax
0051f87f 5d                       pop        ebp
0051f880 c20400                   ret        4
0051f883 cc                       int3       
0051f884 cc                       int3       
0051f885 cc                       int3       
0051f886 cc                       int3       
0051f887 cc                       int3       
0051f888 cc                       int3       
0051f889 cc                       int3       
0051f88a cc                       int3       
0051f88b cc                       int3       
0051f88c cc                       int3       
0051f88d cc                       int3       
0051f88e cc                       int3       
0051f88f cc                       int3       
