00534320 55                       push       ebp
00534321 8bec                     mov        ebp, esp
00534323 83ec08                   sub        esp, 8
00534326 894df8                   mov        dword ptr [ebp - 8], ecx
00534329 e8f2c1ffff               call       0x530520
0053432e 0fb6c0                   movzx      eax, al
00534331 85c0                     test       eax, eax
00534333 754b                     jne        0x534380
00534335 6a00                     push       0
00534337 e8f4c4f2ff               call       0x460830
0053433c 83c404                   add        esp, 4
0053433f 85c0                     test       eax, eax
00534341 743d                     je         0x534380
00534343 6a00                     push       0
00534345 e8e6c4f2ff               call       0x460830
0053434a 83c404                   add        esp, 4
0053434d 8bc8                     mov        ecx, eax
0053434f e85cb3fcff               call       0x4ff6b0
00534354 8bc8                     mov        ecx, eax
00534356 e80521fdff               call       0x506460
0053435b 85c0                     test       eax, eax
0053435d 7c21                     jl         0x534380
0053435f 6a00                     push       0
00534361 e8cac4f2ff               call       0x460830
00534366 83c404                   add        esp, 4
00534369 8bc8                     mov        ecx, eax
0053436b e870b2fcff               call       0x4ff5e0
00534370 0fb6c8                   movzx      ecx, al
00534373 85c9                     test       ecx, ecx
00534375 7409                     je         0x534380
00534377 c745fc01000000           mov        dword ptr [ebp - 4], 1
0053437e eb07                     jmp        0x534387
00534380 c745fc00000000           mov        dword ptr [ebp - 4], 0
00534387 0fb645fc                 movzx      eax, byte ptr [ebp - 4]
0053438b 8be5                     mov        esp, ebp
0053438d 5d                       pop        ebp
0053438e c3                       ret        
0053438f cc                       int3       
