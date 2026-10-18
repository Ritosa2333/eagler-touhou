00424180 55                       push       ebp
00424181 8bec                     mov        ebp, esp
00424183 83ec14                   sub        esp, 0x14
00424186 8b4508                   mov        eax, dword ptr [ebp + 8]
00424189 83782c00                 cmp        dword ptr [eax + 0x2c], 0
0042418d 755b                     jne        0x4241ea
0042418f 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424192 83791c00                 cmp        dword ptr [ecx + 0x1c], 0
00424196 7450                     je         0x4241e8
00424198 8b5508                   mov        edx, dword ptr [ebp + 8]
0042419b 83c230                   add        edx, 0x30
0042419e 8955fc                   mov        dword ptr [ebp - 4], edx
004241a1 8b4508                   mov        eax, dword ptr [ebp + 8]
004241a4 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
004241a7 51                       push       ecx
004241a8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004241ab e8e0f3ffff               call       0x423590
004241b0 0fb6d0                   movzx      edx, al
004241b3 85d2                     test       edx, edx
004241b5 7431                     je         0x4241e8
004241b7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004241ba 83c130                   add        ecx, 0x30
004241bd e80efaffff               call       0x423bd0
004241c2 d95df8                   fstp       dword ptr [ebp - 8]
004241c5 f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004241ca f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
004241d2 8b4508                   mov        eax, dword ptr [ebp + 8]
004241d5 f30f2a481c               cvtsi2ss   xmm1, dword ptr [eax + 0x1c]
004241da f30f5ec1                 divss      xmm0, xmm1
004241de f30f2cc8                 cvttss2si  ecx, xmm0
004241e2 8b5508                   mov        edx, dword ptr [ebp + 8]
004241e5 894a18                   mov        dword ptr [edx + 0x18], ecx
004241e8 eb57                     jmp        0x424241
004241ea 8b4508                   mov        eax, dword ptr [ebp + 8]
004241ed 83c030                   add        eax, 0x30
004241f0 8945f4                   mov        dword ptr [ebp - 0xc], eax
004241f3 6a08                     push       8
004241f5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004241f8 e893f3ffff               call       0x423590
004241fd 0fb6c8                   movzx      ecx, al
00424200 85c9                     test       ecx, ecx
00424202 7436                     je         0x42423a
00424204 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424207 83c130                   add        ecx, 0x30
0042420a e8c1f9ffff               call       0x423bd0
0042420f d95df0                   fstp       dword ptr [ebp - 0x10]
00424212 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
00424217 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
0042421f f30f5e05bcd75600         divss      xmm0, dword ptr [0x56d7bc]
00424227 f30f2cd0                 cvttss2si  edx, xmm0
0042422b b880000000               mov        eax, 0x80
00424230 2bc2                     sub        eax, edx
00424232 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424235 894118                   mov        dword ptr [ecx + 0x18], eax
00424238 eb07                     jmp        0x424241
0042423a b807000000               mov        eax, 7
0042423f eb18                     jmp        0x424259
00424241 8b5508                   mov        edx, dword ptr [ebp + 8]
00424244 83c230                   add        edx, 0x30
00424247 8955ec                   mov        dword ptr [ebp - 0x14], edx
0042424a 6a00                     push       0
0042424c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0042424f e8ecf2ffff               call       0x423540
00424254 b801000000               mov        eax, 1
00424259 8be5                     mov        esp, ebp
0042425b 5d                       pop        ebp
0042425c c3                       ret        
0042425d cc                       int3       
0042425e cc                       int3       
0042425f cc                       int3       
