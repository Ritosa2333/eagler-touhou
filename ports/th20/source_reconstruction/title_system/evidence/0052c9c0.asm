0052c9c0 55                       push       ebp
0052c9c1 8bec                     mov        ebp, esp
0052c9c3 83ec08                   sub        esp, 8
0052c9c6 894df8                   mov        dword ptr [ebp - 8], ecx
0052c9c9 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052c9cc 83781403                 cmp        dword ptr [eax + 0x14], 3
0052c9d0 7509                     jne        0x52c9db
0052c9d2 c745fc01000000           mov        dword ptr [ebp - 4], 1
0052c9d9 eb07                     jmp        0x52c9e2
0052c9db c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c9e2 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0052c9e6 8be5                     mov        esp, ebp
0052c9e8 5d                       pop        ebp
0052c9e9 c3                       ret        
0052c9ea cc                       int3       
0052c9eb cc                       int3       
0052c9ec cc                       int3       
0052c9ed cc                       int3       
0052c9ee cc                       int3       
0052c9ef cc                       int3       
