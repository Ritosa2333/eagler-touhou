00461070 55                       push       ebp
00461071 8bec                     mov        ebp, esp
00461073 83ec08                   sub        esp, 8
00461076 894df8                   mov        dword ptr [ebp - 8], ecx
00461079 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046107c 8b4804                   mov        ecx, dword ptr [eax + 4]
0046107f 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
00461082 7e09                     jle        0x46108d
00461084 c745fc01000000           mov        dword ptr [ebp - 4], 1
0046108b eb07                     jmp        0x461094
0046108d c745fc00000000           mov        dword ptr [ebp - 4], 0
00461094 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
00461098 8be5                     mov        esp, ebp
0046109a 5d                       pop        ebp
0046109b c20400                   ret        4
0046109e cc                       int3       
0046109f cc                       int3       
