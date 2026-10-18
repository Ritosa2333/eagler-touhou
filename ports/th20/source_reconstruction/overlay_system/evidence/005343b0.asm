005343b0 55                       push       ebp
005343b1 8bec                     mov        ebp, esp
005343b3 83ec08                   sub        esp, 8
005343b6 894df8                   mov        dword ptr [ebp - 8], ecx
005343b9 e862c1ffff               call       0x530520
005343be 0fb6c0                   movzx      eax, al
005343c1 85c0                     test       eax, eax
005343c3 754b                     jne        0x534410
005343c5 6a00                     push       0
005343c7 e864c4f2ff               call       0x460830
005343cc 83c404                   add        esp, 4
005343cf 85c0                     test       eax, eax
005343d1 743d                     je         0x534410
005343d3 6a00                     push       0
005343d5 e856c4f2ff               call       0x460830
005343da 83c404                   add        esp, 4
005343dd 8bc8                     mov        ecx, eax
005343df e8ccb2fcff               call       0x4ff6b0
005343e4 8bc8                     mov        ecx, eax
005343e6 e87520fdff               call       0x506460
005343eb 85c0                     test       eax, eax
005343ed 7c21                     jl         0x534410
005343ef 6a00                     push       0
005343f1 e83ac4f2ff               call       0x460830
005343f6 83c404                   add        esp, 4
005343f9 8bc8                     mov        ecx, eax
005343fb e8e0b1fcff               call       0x4ff5e0
00534400 0fb6c8                   movzx      ecx, al
00534403 85c9                     test       ecx, ecx
00534405 7509                     jne        0x534410
00534407 c745fc01000000           mov        dword ptr [ebp - 4], 1
0053440e eb07                     jmp        0x534417
00534410 c745fc00000000           mov        dword ptr [ebp - 4], 0
00534417 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0053441b 8be5                     mov        esp, ebp
0053441d 5d                       pop        ebp
0053441e c3                       ret        
0053441f cc                       int3       
