00513f70 55                       push       ebp
00513f71 8bec                     mov        ebp, esp
00513f73 83ec0c                   sub        esp, 0xc
00513f76 894df4                   mov        dword ptr [ebp - 0xc], ecx
00513f79 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00513f7c e8bf1af7ff               call       0x485a40
00513f81 0fb6c0                   movzx      eax, al
00513f84 85c0                     test       eax, eax
00513f86 7428                     je         0x513fb0
00513f88 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00513f8b e8601af7ff               call       0x4859f0
00513f90 85c0                     test       eax, eax
00513f92 741c                     je         0x513fb0
00513f94 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00513f97 e8541af7ff               call       0x4859f0
00513f9c 8bc8                     mov        ecx, eax
00513f9e e85d83efff               call       0x40c300
00513fa3 8b402c                   mov        eax, dword ptr [eax + 0x2c]
00513fa6 e9c6000000               jmp        0x514071
00513fab e9c1000000               jmp        0x514071
00513fb0 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00513fb7 c745f800000000           mov        dword ptr [ebp - 8], 0
00513fbe 6a00                     push       0
00513fc0 e8bb00f5ff               call       0x464080
00513fc5 83c404                   add        esp, 4
00513fc8 8bc8                     mov        ecx, eax
00513fca e8d1feffff               call       0x513ea0
00513fcf 3b45fc                   cmp        eax, dword ptr [ebp - 4]
00513fd2 7e1b                     jle        0x513fef
00513fd4 6a00                     push       0
00513fd6 e8a500f5ff               call       0x464080
00513fdb 83c404                   add        esp, 4
00513fde 8bc8                     mov        ecx, eax
00513fe0 e8bbfeffff               call       0x513ea0
00513fe5 8945fc                   mov        dword ptr [ebp - 4], eax
00513fe8 c745f800000000           mov        dword ptr [ebp - 8], 0
00513fef 6a00                     push       0
00513ff1 e88a00f5ff               call       0x464080
00513ff6 83c404                   add        esp, 4
00513ff9 8bc8                     mov        ecx, eax
00513ffb e8e0fdffff               call       0x513de0
00514000 3b45fc                   cmp        eax, dword ptr [ebp - 4]
00514003 7e1b                     jle        0x514020
00514005 6a00                     push       0
00514007 e87400f5ff               call       0x464080
0051400c 83c404                   add        esp, 4
0051400f 8bc8                     mov        ecx, eax
00514011 e8cafdffff               call       0x513de0
00514016 8945fc                   mov        dword ptr [ebp - 4], eax
00514019 c745f801000000           mov        dword ptr [ebp - 8], 1
00514020 6a00                     push       0
00514022 e85900f5ff               call       0x464080
00514027 83c404                   add        esp, 4
0051402a 8bc8                     mov        ecx, eax
0051402c e8affeffff               call       0x513ee0
00514031 3b45fc                   cmp        eax, dword ptr [ebp - 4]
00514034 7e1b                     jle        0x514051
00514036 6a00                     push       0
00514038 e84300f5ff               call       0x464080
0051403d 83c404                   add        esp, 4
00514040 8bc8                     mov        ecx, eax
00514042 e899feffff               call       0x513ee0
00514047 8945fc                   mov        dword ptr [ebp - 4], eax
0051404a c745f802000000           mov        dword ptr [ebp - 8], 2
00514051 6a00                     push       0
00514053 e82800f5ff               call       0x464080
00514058 83c404                   add        esp, 4
0051405b 8bc8                     mov        ecx, eax
0051405d e8befdffff               call       0x513e20
00514062 3b45fc                   cmp        eax, dword ptr [ebp - 4]
00514065 7e07                     jle        0x51406e
00514067 c745f803000000           mov        dword ptr [ebp - 8], 3
0051406e 8b45f8                   mov        eax, dword ptr [ebp - 8]
00514071 8be5                     mov        esp, ebp
00514073 5d                       pop        ebp
00514074 c3                       ret        
00514075 cc                       int3       
00514076 cc                       int3       
00514077 cc                       int3       
00514078 cc                       int3       
00514079 cc                       int3       
0051407a cc                       int3       
0051407b cc                       int3       
0051407c cc                       int3       
0051407d cc                       int3       
0051407e cc                       int3       
0051407f cc                       int3       
