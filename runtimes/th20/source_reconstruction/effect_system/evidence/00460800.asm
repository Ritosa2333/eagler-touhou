00460800 55                       push       ebp
00460801 8bec                     mov        ebp, esp
00460803 51                       push       ecx
00460804 8b4508                   mov        eax, dword ptr [ebp + 8]
00460807 50                       push       eax
00460808 68d8ee5600               push       0x56eed8
0046080d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00460813 e828f8ffff               call       0x460040
00460818 8945fc                   mov        dword ptr [ebp - 4], eax
0046081b 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0046081e 51                       push       ecx
0046081f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00460822 52                       push       edx
00460823 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460826 e8f5fdffff               call       0x460620
0046082b 8be5                     mov        esp, ebp
0046082d 5d                       pop        ebp
0046082e c3                       ret        
0046082f cc                       int3       
