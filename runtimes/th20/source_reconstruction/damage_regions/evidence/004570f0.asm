004570f0 55                       push       ebp
004570f1 8bec                     mov        ebp, esp
004570f3 83ec44                   sub        esp, 0x44
004570f6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004570fb 33c5                     xor        eax, ebp
004570fd 8945fc                   mov        dword ptr [ebp - 4], eax
00457100 68102e4200               push       0x422e10
00457105 6a02                     push       2
00457107 6a0c                     push       0xc
00457109 8d45e4                   lea        eax, [ebp - 0x1c]
0045710c 50                       push       eax
0045710d e80e4bfbff               call       0x40bc20
00457112 51                       push       ecx
00457113 0f57c0                   xorps      xmm0, xmm0
00457116 f30f110424               movss      dword ptr [esp], xmm0
0045711b 51                       push       ecx
0045711c 0f57c0                   xorps      xmm0, xmm0
0045711f f30f110424               movss      dword ptr [esp], xmm0
00457124 51                       push       ecx
00457125 f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
0045712a f30f110424               movss      dword ptr [esp], xmm0
0045712f 8d4dd8                   lea        ecx, [ebp - 0x28]
00457132 e899bcfcff               call       0x422dd0
00457137 90                       nop        
00457138 c745d400000000           mov        dword ptr [ebp - 0x2c], 0
0045713f eb09                     jmp        0x45714a
00457141 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00457144 83c101                   add        ecx, 1
00457147 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0045714a 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
0045714d 3b5520                   cmp        edx, dword ptr [ebp + 0x20]
00457150 0f8d8f010000             jge        0x4572e5
00457156 51                       push       ecx
00457157 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0045715c f30f110424               movss      dword ptr [esp], xmm0
00457161 8d45d8                   lea        eax, [ebp - 0x28]
00457164 50                       push       eax
00457165 b90c000000               mov        ecx, 0xc
0045716a 6bd100                   imul       edx, ecx, 0
0045716d 8d4415e4                 lea        eax, [ebp + edx - 0x1c]
00457171 50                       push       eax
00457172 e8291e0000               call       0x458fa0
00457177 83c40c                   add        esp, 0xc
0045717a f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00457182 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
0045718a f30f2a4d20               cvtsi2ss   xmm1, dword ptr [ebp + 0x20]
0045718f f30f5ec1                 divss      xmm0, xmm1
00457193 f30f58451c               addss      xmm0, dword ptr [ebp + 0x1c]
00457198 f30f11451c               movss      dword ptr [ebp + 0x1c], xmm0
0045719d 51                       push       ecx
0045719e f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004571a3 f30f110424               movss      dword ptr [esp], xmm0
004571a8 e89313feff               call       0x438540
004571ad 83c404                   add        esp, 4
004571b0 d95d1c                   fstp       dword ptr [ebp + 0x1c]
004571b3 51                       push       ecx
004571b4 f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
004571b9 f30f110424               movss      dword ptr [esp], xmm0
004571be 8d4dd8                   lea        ecx, [ebp - 0x28]
004571c1 51                       push       ecx
004571c2 ba0c000000               mov        edx, 0xc
004571c7 c1e200                   shl        edx, 0
004571ca 8d4415e4                 lea        eax, [ebp + edx - 0x1c]
004571ce 50                       push       eax
004571cf e8cc1d0000               call       0x458fa0
004571d4 83c40c                   add        esp, 0xc
004571d7 51                       push       ecx
004571d8 0f57c0                   xorps      xmm0, xmm0
004571db f30f110424               movss      dword ptr [esp], xmm0
004571e0 51                       push       ecx
004571e1 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
004571e6 f30f110424               movss      dword ptr [esp], xmm0
004571eb 51                       push       ecx
004571ec f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004571f1 f30f110424               movss      dword ptr [esp], xmm0
004571f6 8d4dc8                   lea        ecx, [ebp - 0x38]
004571f9 e8d2bbfcff               call       0x422dd0
004571fe 8d4dc8                   lea        ecx, [ebp - 0x38]
00457201 51                       push       ecx
00457202 ba0c000000               mov        edx, 0xc
00457207 6bc200                   imul       eax, edx, 0
0045720a 8d4c05e4                 lea        ecx, [ebp + eax - 0x1c]
0045720e e8cd24fdff               call       0x4296e0
00457213 90                       nop        
00457214 51                       push       ecx
00457215 0f57c0                   xorps      xmm0, xmm0
00457218 f30f110424               movss      dword ptr [esp], xmm0
0045721d 51                       push       ecx
0045721e f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457223 f30f110424               movss      dword ptr [esp], xmm0
00457228 51                       push       ecx
00457229 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
0045722e f30f110424               movss      dword ptr [esp], xmm0
00457233 8d4dbc                   lea        ecx, [ebp - 0x44]
00457236 e895bbfcff               call       0x422dd0
0045723b 8d4dbc                   lea        ecx, [ebp - 0x44]
0045723e 51                       push       ecx
0045723f ba0c000000               mov        edx, 0xc
00457244 c1e200                   shl        edx, 0
00457247 8d4c15e4                 lea        ecx, [ebp + edx - 0x1c]
0045724b e89024fdff               call       0x4296e0
00457250 90                       nop        
00457251 51                       push       ecx
00457252 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00457257 f30f110424               movss      dword ptr [esp], xmm0
0045725c 51                       push       ecx
0045725d f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
00457262 f30f110424               movss      dword ptr [esp], xmm0
00457267 51                       push       ecx
00457268 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
0045726d f30f110424               movss      dword ptr [esp], xmm0
00457272 51                       push       ecx
00457273 f30f104508               movss      xmm0, dword ptr [ebp + 8]
00457278 f30f110424               movss      dword ptr [esp], xmm0
0045727d b80c000000               mov        eax, 0xc
00457282 c1e000                   shl        eax, 0
00457285 51                       push       ecx
00457286 f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
0045728c f30f110424               movss      dword ptr [esp], xmm0
00457291 b90c000000               mov        ecx, 0xc
00457296 c1e100                   shl        ecx, 0
00457299 51                       push       ecx
0045729a f30f10440de4             movss      xmm0, dword ptr [ebp + ecx - 0x1c]
004572a0 f30f110424               movss      dword ptr [esp], xmm0
004572a5 ba0c000000               mov        edx, 0xc
004572aa 6bc200                   imul       eax, edx, 0
004572ad 51                       push       ecx
004572ae f30f104405e8             movss      xmm0, dword ptr [ebp + eax - 0x18]
004572b4 f30f110424               movss      dword ptr [esp], xmm0
004572b9 b90c000000               mov        ecx, 0xc
004572be 6bd100                   imul       edx, ecx, 0
004572c1 51                       push       ecx
004572c2 f30f104415e4             movss      xmm0, dword ptr [ebp + edx - 0x1c]
004572c8 f30f110424               movss      dword ptr [esp], xmm0
004572cd e84ef6ffff               call       0x456920
004572d2 83c420                   add        esp, 0x20
004572d5 0fb6c0                   movzx      eax, al
004572d8 85c0                     test       eax, eax
004572da 7404                     je         0x4572e0
004572dc 32c0                     xor        al, al
004572de eb07                     jmp        0x4572e7
004572e0 e95cfeffff               jmp        0x457141
004572e5 b001                     mov        al, 1
004572e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004572ea 33cd                     xor        ecx, ebp
004572ec e8fdb60e00               call       0x5429ee
004572f1 8be5                     mov        esp, ebp
004572f3 5d                       pop        ebp
004572f4 c3                       ret        
004572f5 cc                       int3       
004572f6 cc                       int3       
004572f7 cc                       int3       
004572f8 cc                       int3       
004572f9 cc                       int3       
004572fa cc                       int3       
004572fb cc                       int3       
004572fc cc                       int3       
004572fd cc                       int3       
004572fe cc                       int3       
004572ff cc                       int3       
