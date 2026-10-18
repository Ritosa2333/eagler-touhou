00424fa0 55                       push       ebp
00424fa1 8bec                     mov        ebp, esp
00424fa3 83ec1c                   sub        esp, 0x1c
00424fa6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00424fab 33c5                     xor        eax, ebp
00424fad 8945fc                   mov        dword ptr [ebp - 4], eax
00424fb0 b958675b00               mov        ecx, 0x5b6758
00424fb5 e8167bffff               call       0x41cad0
00424fba f30f2ac0                 cvtsi2ss   xmm0, eax
00424fbe f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00424fc3 b958675b00               mov        ecx, 0x5b6758
00424fc8 e8237bffff               call       0x41caf0
00424fcd f30f2ac0                 cvtsi2ss   xmm0, eax
00424fd1 f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00424fd6 b958675b00               mov        ecx, 0x5b6758
00424fdb e8f07affff               call       0x41cad0
00424fe0 f30f2ac0                 cvtsi2ss   xmm0, eax
00424fe4 b958675b00               mov        ecx, 0x5b6758
00424fe9 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00424fee e81df0ffff               call       0x424010
00424ff3 f30f2ac0                 cvtsi2ss   xmm0, eax
00424ff7 f30f104de8               movss      xmm1, dword ptr [ebp - 0x18]
00424ffc f30f58c8                 addss      xmm1, xmm0
00425000 f30f114df4               movss      dword ptr [ebp - 0xc], xmm1
00425005 b958675b00               mov        ecx, 0x5b6758
0042500a e8e17affff               call       0x41caf0
0042500f f30f2ac0                 cvtsi2ss   xmm0, eax
00425013 b958675b00               mov        ecx, 0x5b6758
00425018 f30f1145e4               movss      dword ptr [ebp - 0x1c], xmm0
0042501d e8eec7ffff               call       0x421810
00425022 f30f2ac0                 cvtsi2ss   xmm0, eax
00425026 f30f104de4               movss      xmm1, dword ptr [ebp - 0x1c]
0042502b f30f58c8                 addss      xmm1, xmm0
0042502f f30f114df8               movss      dword ptr [ebp - 8], xmm1
00425034 8b4508                   mov        eax, dword ptr [ebp + 8]
00425037 8b4818                   mov        ecx, dword ptr [eax + 0x18]
0042503a c1e118                   shl        ecx, 0x18
0042503d 8b5508                   mov        edx, dword ptr [ebp + 8]
00425040 8b4224                   mov        eax, dword ptr [edx + 0x24]
00425043 25ffffff00               and        eax, 0xffffff
00425048 0bc8                     or         ecx, eax
0042504a 51                       push       ecx
0042504b 8d4dec                   lea        ecx, [ebp - 0x14]
0042504e 51                       push       ecx
0042504f e8fce5ffff               call       0x423650
00425054 83c408                   add        esp, 8
00425057 b801000000               mov        eax, 1
0042505c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042505f 33cd                     xor        ecx, ebp
00425061 e888d91100               call       0x5429ee
00425066 8be5                     mov        esp, ebp
00425068 5d                       pop        ebp
00425069 c3                       ret        
0042506a cc                       int3       
0042506b cc                       int3       
0042506c cc                       int3       
0042506d cc                       int3       
0042506e cc                       int3       
0042506f cc                       int3       
