004783a0 55                       push       ebp
004783a1 8bec                     mov        ebp, esp
004783a3 6aff                     push       -1
004783a5 68378f5600               push       0x568f37
004783aa 64a100000000             mov        eax, dword ptr fs:[0]
004783b0 50                       push       eax
004783b1 83ec10                   sub        esp, 0x10
004783b4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004783b9 33c5                     xor        eax, ebp
004783bb 50                       push       eax
004783bc 8d45f4                   lea        eax, [ebp - 0xc]
004783bf 64a300000000             mov        dword ptr fs:[0], eax
004783c5 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004783c8 68cc000000               push       0xcc
004783cd e88ba30c00               call       0x54275d
004783d2 83c404                   add        esp, 4
004783d5 8945f0                   mov        dword ptr [ebp - 0x10], eax
004783d8 c745fc00000000           mov        dword ptr [ebp - 4], 0
004783df 837df000                 cmp        dword ptr [ebp - 0x10], 0
004783e3 741a                     je         0x4783ff
004783e5 68cc000000               push       0xcc
004783ea 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004783ed e88e3cf9ff               call       0x40c080
004783f2 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004783f5 e8e6000000               call       0x4784e0
004783fa 8945ec                   mov        dword ptr [ebp - 0x14], eax
004783fd eb07                     jmp        0x478406
004783ff c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00478406 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00478409 8945e8                   mov        dword ptr [ebp - 0x18], eax
0047840c c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00478413 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
00478416 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00478419 64890d00000000           mov        dword ptr fs:[0], ecx
00478420 59                       pop        ecx
00478421 8be5                     mov        esp, ebp
00478423 5d                       pop        ebp
00478424 c20400                   ret        4
00478427 cc                       int3       
00478428 cc                       int3       
00478429 cc                       int3       
0047842a cc                       int3       
0047842b cc                       int3       
0047842c cc                       int3       
0047842d cc                       int3       
0047842e cc                       int3       
0047842f cc                       int3       
