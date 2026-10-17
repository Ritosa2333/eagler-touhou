00424260 55                       push       ebp
00424261 8bec                     mov        ebp, esp
00424263 83ec10                   sub        esp, 0x10
00424266 833d18a55b0000           cmp        dword ptr [0x5ba518], 0
0042426d 740a                     je         0x424279
0042426f b807000000               mov        eax, 7
00424274 e9d2000000               jmp        0x42434b
00424279 8b4508                   mov        eax, dword ptr [ebp + 8]
0042427c 83781c00                 cmp        dword ptr [eax + 0x1c], 0
00424280 746f                     je         0x4242f1
00424282 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00424285 83c130                   add        ecx, 0x30
00424288 894dfc                   mov        dword ptr [ebp - 4], ecx
0042428b 8b5508                   mov        edx, dword ptr [ebp + 8]
0042428e 8b421c                   mov        eax, dword ptr [edx + 0x1c]
00424291 50                       push       eax
00424292 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00424295 e8c6f2ffff               call       0x423560
0042429a 0fb6c8                   movzx      ecx, al
0042429d 85c9                     test       ecx, ecx
0042429f 7433                     je         0x4242d4
004242a1 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004242a4 83c130                   add        ecx, 0x30
004242a7 e824f9ffff               call       0x423bd0
004242ac d95df8                   fstp       dword ptr [ebp - 8]
004242af f30f1045f8               movss      xmm0, dword ptr [ebp - 8]
004242b4 f30f59059ccd5600         mulss      xmm0, dword ptr [0x56cd9c]
004242bc 8b5508                   mov        edx, dword ptr [ebp + 8]
004242bf f30f2a4a1c               cvtsi2ss   xmm1, dword ptr [edx + 0x1c]
004242c4 f30f5ec1                 divss      xmm0, xmm1
004242c8 f30f2cc0                 cvttss2si  eax, xmm0
004242cc 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004242cf 894118                   mov        dword ptr [ecx + 0x18], eax
004242d2 eb0a                     jmp        0x4242de
004242d4 8b5508                   mov        edx, dword ptr [ebp + 8]
004242d7 c74218ff000000           mov        dword ptr [edx + 0x18], 0xff
004242de 8b4508                   mov        eax, dword ptr [ebp + 8]
004242e1 83781800                 cmp        dword ptr [eax + 0x18], 0
004242e5 7d0a                     jge        0x4242f1
004242e7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004242ea c7411800000000           mov        dword ptr [ecx + 0x18], 0
004242f1 8b5508                   mov        edx, dword ptr [ebp + 8]
004242f4 83c230                   add        edx, 0x30
004242f7 8955f4                   mov        dword ptr [ebp - 0xc], edx
004242fa 8b4508                   mov        eax, dword ptr [ebp + 8]
004242fd 8b481c                   mov        ecx, dword ptr [eax + 0x1c]
00424300 83c102                   add        ecx, 2
00424303 51                       push       ecx
00424304 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00424307 e8b4f2ffff               call       0x4235c0
0042430c 0fb6d0                   movzx      edx, al
0042430f 85d2                     test       edx, edx
00424311 7407                     je         0x42431a
00424313 b807000000               mov        eax, 7
00424318 eb31                     jmp        0x42434b
0042431a 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00424321 740f                     je         0x424332
00424323 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
00424329 e802fdffff               call       0x424030
0042432e 85c0                     test       eax, eax
00424330 7514                     jne        0x424346
00424332 8b4508                   mov        eax, dword ptr [ebp + 8]
00424335 83c030                   add        eax, 0x30
00424338 8945f0                   mov        dword ptr [ebp - 0x10], eax
0042433b 6a00                     push       0
0042433d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00424340 e8fbf1ffff               call       0x423540
00424345 90                       nop        
00424346 b801000000               mov        eax, 1
0042434b 8be5                     mov        esp, ebp
0042434d 5d                       pop        ebp
0042434e c3                       ret        
0042434f cc                       int3       
