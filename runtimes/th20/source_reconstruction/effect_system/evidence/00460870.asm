00460870 55                       push       ebp
00460871 8bec                     mov        ebp, esp
00460873 6aff                     push       -1
00460875 68178a5600               push       0x568a17
0046087a 64a100000000             mov        eax, dword ptr fs:[0]
00460880 50                       push       eax
00460881 83ec14                   sub        esp, 0x14
00460884 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00460889 33c5                     xor        eax, ebp
0046088b 50                       push       eax
0046088c 8d45f4                   lea        eax, [ebp - 0xc]
0046088f 64a300000000             mov        dword ptr fs:[0], eax
00460895 894de0                   mov        dword ptr [ebp - 0x20], ecx
00460898 68f4030000               push       0x3f4
0046089d e8bb1e0e00               call       0x54275d
004608a2 83c404                   add        esp, 4
004608a5 8945f0                   mov        dword ptr [ebp - 0x10], eax
004608a8 c745fc00000000           mov        dword ptr [ebp - 4], 0
004608af 837df000                 cmp        dword ptr [ebp - 0x10], 0
004608b3 742f                     je         0x4608e4
004608b5 68f4030000               push       0x3f4
004608ba 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004608bd e8beb7faff               call       0x40c080
004608c2 8d450c                   lea        eax, [ebp + 0xc]
004608c5 50                       push       eax
004608c6 e8b5a9faff               call       0x40b280
004608cb 83c404                   add        esp, 4
004608ce 8b08                     mov        ecx, dword ptr [eax]
004608d0 894de8                   mov        dword ptr [ebp - 0x18], ecx
004608d3 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004608d6 52                       push       edx
004608d7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004608da e831000000               call       0x460910
004608df 8945ec                   mov        dword ptr [ebp - 0x14], eax
004608e2 eb07                     jmp        0x4608eb
004608e4 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004608eb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004608ee 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004608f1 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004608f8 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004608fb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004608fe 64890d00000000           mov        dword ptr fs:[0], ecx
00460905 59                       pop        ecx
00460906 8be5                     mov        esp, ebp
00460908 5d                       pop        ebp
00460909 c20800                   ret        8
0046090c cc                       int3       
0046090d cc                       int3       
0046090e cc                       int3       
0046090f cc                       int3       
