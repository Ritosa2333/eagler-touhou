00485a10 55                       push       ebp
00485a11 8bec                     mov        ebp, esp
00485a13 83ec08                   sub        esp, 8
00485a16 894df8                   mov        dword ptr [ebp - 8], ecx
00485a19 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00485a1c e8af74fcff               call       0x44ced0
00485a21 85c0                     test       eax, eax
00485a23 7409                     je         0x485a2e
00485a25 c745fc01000000           mov        dword ptr [ebp - 4], 1
00485a2c eb07                     jmp        0x485a35
00485a2e c745fc00000000           mov        dword ptr [ebp - 4], 0
00485a35 8b45fc                   mov        eax, dword ptr [ebp - 4]
00485a38 8be5                     mov        esp, ebp
00485a3a 5d                       pop        ebp
00485a3b c3                       ret        
00485a3c cc                       int3       
00485a3d cc                       int3       
00485a3e cc                       int3       
00485a3f cc                       int3       
