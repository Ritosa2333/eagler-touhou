00511870 55                       push       ebp
00511871 8bec                     mov        ebp, esp
00511873 6aff                     push       -1
00511875 6824aa5600               push       0x56aa24
0051187a 64a100000000             mov        eax, dword ptr fs:[0]
00511880 50                       push       eax
00511881 83ec10                   sub        esp, 0x10
00511884 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00511889 33c5                     xor        eax, ebp
0051188b 50                       push       eax
0051188c 8d45f4                   lea        eax, [ebp - 0xc]
0051188f 64a300000000             mov        dword ptr fs:[0], eax
00511895 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00511898 6a34                     push       0x34
0051189a e8be0e0300               call       0x54275d
0051189f 83c404                   add        esp, 4
005118a2 8945f0                   mov        dword ptr [ebp - 0x10], eax
005118a5 c745fc00000000           mov        dword ptr [ebp - 4], 0
005118ac 837df000                 cmp        dword ptr [ebp - 0x10], 0
005118b0 7417                     je         0x5118c9
005118b2 6a34                     push       0x34
005118b4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005118b7 e8c4a7efff               call       0x40c080
005118bc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
005118bf e81c010000               call       0x5119e0
005118c4 8945ec                   mov        dword ptr [ebp - 0x14], eax
005118c7 eb07                     jmp        0x5118d0
005118c9 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
005118d0 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
005118d3 8945e8                   mov        dword ptr [ebp - 0x18], eax
005118d6 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
005118dd 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
005118e0 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005118e3 64890d00000000           mov        dword ptr fs:[0], ecx
005118ea 59                       pop        ecx
005118eb 8be5                     mov        esp, ebp
005118ed 5d                       pop        ebp
005118ee c20400                   ret        4
005118f1 cc                       int3       
005118f2 cc                       int3       
005118f3 cc                       int3       
005118f4 cc                       int3       
005118f5 cc                       int3       
005118f6 cc                       int3       
005118f7 cc                       int3       
005118f8 cc                       int3       
005118f9 cc                       int3       
005118fa cc                       int3       
005118fb cc                       int3       
005118fc cc                       int3       
005118fd cc                       int3       
005118fe cc                       int3       
005118ff cc                       int3       
