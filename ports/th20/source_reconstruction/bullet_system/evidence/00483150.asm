00483150 55                       push       ebp
00483151 8bec                     mov        ebp, esp
00483153 83ec18                   sub        esp, 0x18
00483156 894dfc                   mov        dword ptr [ebp - 4], ecx
00483159 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048315c 8b8820050000             mov        ecx, dword ptr [eax + 0x520]
00483162 e89991f8ff               call       0x40c300
00483167 8bc8                     mov        ecx, eax
00483169 e822280000               call       0x485990
0048316e 0f57c0                   xorps      xmm0, xmm0
00483171 0f2f4004                 comiss     xmm0, dword ptr [eax + 4]
00483175 7222                     jb         0x483199
00483177 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048317a 81c1a0000000             add        ecx, 0xa0
00483180 894df0                   mov        dword ptr [ebp - 0x10], ecx
00483183 6a04                     push       4
00483185 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00483188 e8f394ffff               call       0x47c680
0048318d f30f10401c               movss      xmm0, dword ptr [eax + 0x1c]
00483192 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00483197 eb1f                     jmp        0x4831b8
00483199 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048319c 8b8a20050000             mov        ecx, dword ptr [edx + 0x520]
004831a2 e85991f8ff               call       0x40c300
004831a7 8bc8                     mov        ecx, eax
004831a9 e8e2270000               call       0x485990
004831ae f30f104004               movss      xmm0, dword ptr [eax + 4]
004831b3 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
004831b8 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004831bd f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
004831c2 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
004831ca f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
004831d2 f30f104df4               movss      xmm1, dword ptr [ebp - 0xc]
004831d7 f30f5e0dd0c85600         divss      xmm1, dword ptr [0x56c8d0]
004831df f30f5cc1                 subss      xmm0, xmm1
004831e3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004831e6 0f2f4068                 comiss     xmm0, dword ptr [eax + 0x68]
004831ea 7673                     jbe        0x48325f
004831ec 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004831ef 81c1a0000000             add        ecx, 0xa0
004831f5 894dec                   mov        dword ptr [ebp - 0x14], ecx
004831f8 6a04                     push       4
004831fa 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004831fd e87e94ffff               call       0x47c680
00483202 8b503c                   mov        edx, dword ptr [eax + 0x3c]
00483205 83e210                   and        edx, 0x10
00483208 754e                     jne        0x483258
0048320a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048320d 81c184000000             add        ecx, 0x84
00483213 e8c860faff               call       0x4292e0
00483218 d95de8                   fstp       dword ptr [ebp - 0x18]
0048321b f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00483220 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00483227 51                       push       ecx
00483228 f30f110424               movss      dword ptr [esp], xmm0
0048322d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483230 81c184000000             add        ecx, 0x84
00483236 e8e5fcfcff               call       0x452f20
0048323b f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00483243 f30f5c45f4               subss      xmm0, dword ptr [ebp - 0xc]
00483248 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048324b f30f5c4068               subss      xmm0, dword ptr [eax + 0x68]
00483250 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00483253 f30f114168               movss      dword ptr [ecx + 0x68], xmm0
00483258 b801000000               mov        eax, 1
0048325d eb02                     jmp        0x483261
0048325f 33c0                     xor        eax, eax
00483261 8be5                     mov        esp, ebp
00483263 5d                       pop        ebp
00483264 c3                       ret        
00483265 cc                       int3       
00483266 cc                       int3       
00483267 cc                       int3       
00483268 cc                       int3       
00483269 cc                       int3       
0048326a cc                       int3       
0048326b cc                       int3       
0048326c cc                       int3       
0048326d cc                       int3       
0048326e cc                       int3       
0048326f cc                       int3       
