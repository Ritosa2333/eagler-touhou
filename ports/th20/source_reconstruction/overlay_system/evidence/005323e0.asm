005323e0 55                       push       ebp
005323e1 8bec                     mov        ebp, esp
005323e3 83ec0c                   sub        esp, 0xc
005323e6 894df4                   mov        dword ptr [ebp - 0xc], ecx
005323e9 6a00                     push       0
005323eb e8901cf3ff               call       0x464080
005323f0 83c404                   add        esp, 4
005323f3 8bc8                     mov        ecx, eax
005323f5 e80620f3ff               call       0x464400
005323fa 0fb6c0                   movzx      eax, al
005323fd 85c0                     test       eax, eax
005323ff 741a                     je         0x53241b
00532401 6a00                     push       0
00532403 e8781cf3ff               call       0x464080
00532408 83c404                   add        esp, 4
0053240b 8945fc                   mov        dword ptr [ebp - 4], eax
0053240e 6a07                     push       7
00532410 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532413 e8f8b8f8ff               call       0x4bdd10
00532418 90                       nop        
00532419 eb18                     jmp        0x532433
0053241b 6a00                     push       0
0053241d e85e1cf3ff               call       0x464080
00532422 83c404                   add        esp, 4
00532425 8945f8                   mov        dword ptr [ebp - 8], eax
00532428 6a0c                     push       0xc
0053242a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053242d e8deb8f8ff               call       0x4bdd10
00532432 90                       nop        
00532433 8be5                     mov        esp, ebp
00532435 5d                       pop        ebp
00532436 c3                       ret        
00532437 cc                       int3       
00532438 cc                       int3       
00532439 cc                       int3       
0053243a cc                       int3       
0053243b cc                       int3       
0053243c cc                       int3       
0053243d cc                       int3       
0053243e cc                       int3       
0053243f cc                       int3       
