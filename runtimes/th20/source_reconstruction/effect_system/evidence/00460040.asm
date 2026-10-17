00460040 55                       push       ebp
00460041 8bec                     mov        ebp, esp
00460043 6aff                     push       -1
00460045 68d7895600               push       0x5689d7
0046004a 64a100000000             mov        eax, dword ptr fs:[0]
00460050 50                       push       eax
00460051 83ec14                   sub        esp, 0x14
00460054 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00460059 33c5                     xor        eax, ebp
0046005b 50                       push       eax
0046005c 8d45f4                   lea        eax, [ebp - 0xc]
0046005f 64a300000000             mov        dword ptr fs:[0], eax
00460065 894de0                   mov        dword ptr [ebp - 0x20], ecx
00460068 68e4030000               push       0x3e4
0046006d e8eb260e00               call       0x54275d
00460072 83c404                   add        esp, 4
00460075 8945f0                   mov        dword ptr [ebp - 0x10], eax
00460078 c745fc00000000           mov        dword ptr [ebp - 4], 0
0046007f 837df000                 cmp        dword ptr [ebp - 0x10], 0
00460083 742f                     je         0x4600b4
00460085 68e4030000               push       0x3e4
0046008a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0046008d e8eebffaff               call       0x40c080
00460092 8d450c                   lea        eax, [ebp + 0xc]
00460095 50                       push       eax
00460096 e8e5b1faff               call       0x40b280
0046009b 83c404                   add        esp, 4
0046009e 8b08                     mov        ecx, dword ptr [eax]
004600a0 894de8                   mov        dword ptr [ebp - 0x18], ecx
004600a3 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
004600a6 52                       push       edx
004600a7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004600aa e8d1000000               call       0x460180
004600af 8945ec                   mov        dword ptr [ebp - 0x14], eax
004600b2 eb07                     jmp        0x4600bb
004600b4 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004600bb 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004600be 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004600c1 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004600c8 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004600cb 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004600ce 64890d00000000           mov        dword ptr fs:[0], ecx
004600d5 59                       pop        ecx
004600d6 8be5                     mov        esp, ebp
004600d8 5d                       pop        ebp
004600d9 c20800                   ret        8
004600dc cc                       int3       
004600dd cc                       int3       
004600de cc                       int3       
004600df cc                       int3       
