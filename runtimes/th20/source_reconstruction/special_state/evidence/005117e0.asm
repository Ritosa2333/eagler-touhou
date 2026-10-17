005117e0 55                       push       ebp
005117e1 8bec                     mov        ebp, esp
005117e3 6aff                     push       -1
005117e5 68e7a95600               push       0x56a9e7
005117ea 64a100000000             mov        eax, dword ptr fs:[0]
005117f0 50                       push       eax
005117f1 83ec10                   sub        esp, 0x10
005117f4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005117f9 33c5                     xor        eax, ebp
005117fb 50                       push       eax
005117fc 8d45f4                   lea        eax, [ebp - 0xc]
005117ff 64a300000000             mov        dword ptr fs:[0], eax
00511805 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00511808 68b4000000               push       0xb4
0051180d e84b0f0300               call       0x54275d
00511812 83c404                   add        esp, 4
00511815 8945f0                   mov        dword ptr [ebp - 0x10], eax
00511818 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051181f 837df000                 cmp        dword ptr [ebp - 0x10], 0
00511823 741a                     je         0x51183f
00511825 68b4000000               push       0xb4
0051182a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051182d e84ea8efff               call       0x40c080
00511832 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00511835 e846010000               call       0x511980
0051183a 8945ec                   mov        dword ptr [ebp - 0x14], eax
0051183d eb07                     jmp        0x511846
0051183f c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00511846 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00511849 8945e8                   mov        dword ptr [ebp - 0x18], eax
0051184c c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00511853 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00511856 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00511859 64890d00000000           mov        dword ptr fs:[0], ecx
00511860 59                       pop        ecx
00511861 8be5                     mov        esp, ebp
00511863 5d                       pop        ebp
00511864 c20400                   ret        4
00511867 cc                       int3       
00511868 cc                       int3       
00511869 cc                       int3       
0051186a cc                       int3       
0051186b cc                       int3       
0051186c cc                       int3       
0051186d cc                       int3       
0051186e cc                       int3       
0051186f cc                       int3       
