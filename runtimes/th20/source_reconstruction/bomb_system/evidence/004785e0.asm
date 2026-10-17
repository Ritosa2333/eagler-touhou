004785e0 55                       push       ebp
004785e1 8bec                     mov        ebp, esp
004785e3 83ec08                   sub        esp, 8
004785e6 894df8                   mov        dword ptr [ebp - 8], ecx
004785e9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004785ec 8b4804                   mov        ecx, dword ptr [eax + 4]
004785ef 3b4d08                   cmp        ecx, dword ptr [ebp + 8]
004785f2 7509                     jne        0x4785fd
004785f4 c745fc01000000           mov        dword ptr [ebp - 4], 1
004785fb eb07                     jmp        0x478604
004785fd c745fc00000000           mov        dword ptr [ebp - 4], 0
00478604 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
00478608 8be5                     mov        esp, ebp
0047860a 5d                       pop        ebp
0047860b c20400                   ret        4
0047860e cc                       int3       
0047860f cc                       int3       
