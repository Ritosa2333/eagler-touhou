004e0fc0 55                       push       ebp
004e0fc1 8bec                     mov        ebp, esp
004e0fc3 83ec08                   sub        esp, 8
004e0fc6 894df8                   mov        dword ptr [ebp - 8], ecx
004e0fc9 0fbe05864f5c00           movsx      eax, byte ptr [0x5c4f86]
004e0fd0 a30c005c00               mov        dword ptr [0x5c000c], eax
004e0fd5 b930a85b00               mov        ecx, 0x5ba830
004e0fda e861ffffff               call       0x4e0f40
004e0fdf 0fbe0d874f5c00           movsx      ecx, byte ptr [0x5c4f87]
004e0fe6 890d10005c00             mov        dword ptr [0x5c0010], ecx
004e0fec f30f2a050c005c00         cvtsi2ss   xmm0, dword ptr [0x5c000c]
004e0ff4 f30f5e054ce05600         divss      xmm0, dword ptr [0x56e04c]
004e0ffc f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004e1001 833d10005c0000           cmp        dword ptr [0x5c0010], 0
004e1008 7461                     je         0x4e106b
004e100a f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e1012 f30f5c45fc               subss      xmm0, dword ptr [ebp - 4]
004e1017 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004e101c f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004e1021 f30f5945fc               mulss      xmm0, dword ptr [ebp - 4]
004e1026 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004e102b f30f1045fc               movss      xmm0, dword ptr [ebp - 4]
004e1030 f30f5945fc               mulss      xmm0, dword ptr [ebp - 4]
004e1035 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004e103a f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e1042 f30f5c45fc               subss      xmm0, dword ptr [ebp - 4]
004e1047 f30f1145fc               movss      dword ptr [ebp - 4], xmm0
004e104c f30f1005b8cd5600         movss      xmm0, dword ptr [0x56cdb8]
004e1054 f30f5945fc               mulss      xmm0, dword ptr [ebp - 4]
004e1059 f30f2cd0                 cvttss2si  edx, xmm0
004e105d 81ea88130000             sub        edx, 0x1388
004e1063 891514005c00             mov        dword ptr [0x5c0014], edx
004e1069 eb0a                     jmp        0x4e1075
004e106b c70514005c00f0d8ffff     mov        dword ptr [0x5c0014], 0xffffd8f0
004e1075 8be5                     mov        esp, ebp
004e1077 5d                       pop        ebp
004e1078 c3                       ret        
004e1079 cc                       int3       
004e107a cc                       int3       
004e107b cc                       int3       
004e107c cc                       int3       
004e107d cc                       int3       
004e107e cc                       int3       
004e107f cc                       int3       
