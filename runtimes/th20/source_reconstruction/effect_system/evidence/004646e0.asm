004646e0 55                       push       ebp
004646e1 8bec                     mov        ebp, esp
004646e3 6aff                     push       -1
004646e5 680d8b5600               push       0x568b0d
004646ea 64a100000000             mov        eax, dword ptr fs:[0]
004646f0 50                       push       eax
004646f1 51                       push       ecx
004646f2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004646f7 33c5                     xor        eax, ebp
004646f9 50                       push       eax
004646fa 8d45f4                   lea        eax, [ebp - 0xc]
004646fd 64a300000000             mov        dword ptr fs:[0], eax
00464703 894df0                   mov        dword ptr [ebp - 0x10], ecx
00464706 8b4508                   mov        eax, dword ptr [ebp + 8]
00464709 50                       push       eax
0046470a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0046470d e84e24feff               call       0x446b60
00464712 c745fc00000000           mov        dword ptr [ebp - 4], 0
00464719 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0046471c c70198f05600             mov        dword ptr [ecx], 0x56f098
00464722 6870934400               push       0x449370
00464727 68408b4400               push       0x448b40
0046472c 6a04                     push       4
0046472e 68e4050000               push       0x5e4
00464733 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00464736 83c208                   add        edx, 8
00464739 52                       push       edx
0046473a e870e70d00               call       0x542eaf
0046473f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00464742 81c198170000             add        ecx, 0x1798
00464748 e8f343feff               call       0x448b40
0046474d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00464750 c7807c1d000000000000     mov        dword ptr [eax + 0x1d7c], 0
0046475a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0046475d c781801d000000000000     mov        dword ptr [ecx + 0x1d80], 0
00464767 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0046476a c782841d000000000000     mov        dword ptr [edx + 0x1d84], 0
00464774 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0046477b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0046477e 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00464781 64890d00000000           mov        dword ptr fs:[0], ecx
00464788 59                       pop        ecx
00464789 8be5                     mov        esp, ebp
0046478b 5d                       pop        ebp
0046478c c20400                   ret        4
0046478f cc                       int3       
