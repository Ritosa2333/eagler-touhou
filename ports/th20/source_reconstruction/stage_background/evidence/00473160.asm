00473160 55                       push       ebp
00473161 8bec                     mov        ebp, esp
00473163 83ec40                   sub        esp, 0x40
00473166 56                       push       esi
00473167 57                       push       edi
00473168 894dfc                   mov        dword ptr [ebp - 4], ecx
0047316b 837d0c00                 cmp        dword ptr [ebp + 0xc], 0
0047316f 750a                     jne        0x47317b
00473171 8b45fc                   mov        eax, dword ptr [ebp - 4]
00473174 a39c065c00               mov        dword ptr [0x5c069c], eax
00473179 eb09                     jmp        0x473184
0047317b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047317e 890da0065c00             mov        dword ptr [0x5c06a0], ecx
00473184 b968a55b00               mov        ecx, 0x5ba568
00473189 e8d21b0000               call       0x474d60
0047318e 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473191 898258330000             mov        dword ptr [edx + 0x3358], eax
00473197 8b4508                   mov        eax, dword ptr [ebp + 8]
0047319a 50                       push       eax
0047319b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047319e e89d1c0000               call       0x474e40
004731a3 85c0                     test       eax, eax
004731a5 741a                     je         0x4731c1
004731a7 6834f85600               push       0x56f834
004731ac 6878065c00               push       0x5c0678
004731b1 e87a10feff               call       0x454230
004731b6 83c408                   add        esp, 8
004731b9 83c8ff                   or         eax, 0xffffffff
004731bc e92c020000               jmp        0x4733ed
004731c1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004731c4 8b55fc                   mov        edx, dword ptr [ebp - 4]
004731c7 899180030000             mov        dword ptr [ecx + 0x380], edx
004731cd b86c010000               mov        eax, 0x16c
004731d2 6bf003                   imul       esi, eax, 3
004731d5 81c6b84f5c00             add        esi, 0x5c4fb8
004731db 8b7dfc                   mov        edi, dword ptr [ebp - 4]
004731de 81c714020000             add        edi, 0x214
004731e4 b95b000000               mov        ecx, 0x5b
004731e9 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
004731eb 51                       push       ecx
004731ec f30f100570fa5600         movss      xmm0, dword ptr [0x56fa70]
004731f4 f30f110424               movss      dword ptr [esp], xmm0
004731f9 51                       push       ecx
004731fa 0f57c0                   xorps      xmm0, xmm0
004731fd f30f110424               movss      dword ptr [esp], xmm0
00473202 51                       push       ecx
00473203 0f57c0                   xorps      xmm0, xmm0
00473206 f30f110424               movss      dword ptr [esp], xmm0
0047320b 8d4df0                   lea        ecx, [ebp - 0x10]
0047320e e8bdfbfaff               call       0x422dd0
00473213 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473216 81c114020000             add        ecx, 0x214
0047321c 8b10                     mov        edx, dword ptr [eax]
0047321e 8911                     mov        dword ptr [ecx], edx
00473220 8b5004                   mov        edx, dword ptr [eax + 4]
00473223 895104                   mov        dword ptr [ecx + 4], edx
00473226 8b4008                   mov        eax, dword ptr [eax + 8]
00473229 894108                   mov        dword ptr [ecx + 8], eax
0047322c 51                       push       ecx
0047322d f30f100538fa5600         movss      xmm0, dword ptr [0x56fa38]
00473235 f30f110424               movss      dword ptr [esp], xmm0
0047323a 51                       push       ecx
0047323b f30f10059cec5600         movss      xmm0, dword ptr [0x56ec9c]
00473243 f30f110424               movss      dword ptr [esp], xmm0
00473248 51                       push       ecx
00473249 0f57c0                   xorps      xmm0, xmm0
0047324c f30f110424               movss      dword ptr [esp], xmm0
00473251 8d4de4                   lea        ecx, [ebp - 0x1c]
00473254 e877fbfaff               call       0x422dd0
00473259 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047325c 81c120020000             add        ecx, 0x220
00473262 8b10                     mov        edx, dword ptr [eax]
00473264 8911                     mov        dword ptr [ecx], edx
00473266 8b5004                   mov        edx, dword ptr [eax + 4]
00473269 895104                   mov        dword ptr [ecx + 4], edx
0047326c 8b4008                   mov        eax, dword ptr [eax + 8]
0047326f 894108                   mov        dword ptr [ecx + 8], eax
00473272 51                       push       ecx
00473273 0f57c0                   xorps      xmm0, xmm0
00473276 f30f110424               movss      dword ptr [esp], xmm0
0047327b 51                       push       ecx
0047327c f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00473284 f30f110424               movss      dword ptr [esp], xmm0
00473289 51                       push       ecx
0047328a 0f57c0                   xorps      xmm0, xmm0
0047328d f30f110424               movss      dword ptr [esp], xmm0
00473292 8d4dd8                   lea        ecx, [ebp - 0x28]
00473295 e836fbfaff               call       0x422dd0
0047329a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047329d 81c12c020000             add        ecx, 0x22c
004732a3 8b10                     mov        edx, dword ptr [eax]
004732a5 8911                     mov        dword ptr [ecx], edx
004732a7 8b5004                   mov        edx, dword ptr [eax + 4]
004732aa 895104                   mov        dword ptr [ecx + 4], edx
004732ad 8b4008                   mov        eax, dword ptr [eax + 8]
004732b0 894108                   mov        dword ptr [ecx + 8], eax
004732b3 51                       push       ecx
004732b4 0f57c0                   xorps      xmm0, xmm0
004732b7 f30f110424               movss      dword ptr [esp], xmm0
004732bc 51                       push       ecx
004732bd 0f57c0                   xorps      xmm0, xmm0
004732c0 f30f110424               movss      dword ptr [esp], xmm0
004732c5 51                       push       ecx
004732c6 0f57c0                   xorps      xmm0, xmm0
004732c9 f30f110424               movss      dword ptr [esp], xmm0
004732ce 8d4dcc                   lea        ecx, [ebp - 0x34]
004732d1 e8fafafaff               call       0x422dd0
004732d6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004732d9 81c150020000             add        ecx, 0x250
004732df 8b10                     mov        edx, dword ptr [eax]
004732e1 8911                     mov        dword ptr [ecx], edx
004732e3 8b5004                   mov        edx, dword ptr [eax + 4]
004732e6 895104                   mov        dword ptr [ecx + 4], edx
004732e9 8b4008                   mov        eax, dword ptr [eax + 8]
004732ec 894108                   mov        dword ptr [ecx + 8], eax
004732ef 51                       push       ecx
004732f0 0f57c0                   xorps      xmm0, xmm0
004732f3 f30f110424               movss      dword ptr [esp], xmm0
004732f8 51                       push       ecx
004732f9 0f57c0                   xorps      xmm0, xmm0
004732fc f30f110424               movss      dword ptr [esp], xmm0
00473301 51                       push       ecx
00473302 0f57c0                   xorps      xmm0, xmm0
00473305 f30f110424               movss      dword ptr [esp], xmm0
0047330a 8d4dc0                   lea        ecx, [ebp - 0x40]
0047330d e8befafaff               call       0x422dd0
00473312 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473315 81c15c020000             add        ecx, 0x25c
0047331b 8b10                     mov        edx, dword ptr [eax]
0047331d 8911                     mov        dword ptr [ecx], edx
0047331f 8b5004                   mov        edx, dword ptr [eax + 4]
00473322 895104                   mov        dword ptr [ecx + 4], edx
00473325 8b4008                   mov        eax, dword ptr [eax + 8]
00473328 894108                   mov        dword ptr [ecx + 8], eax
0047332b f30f10054cfa5600         movss      xmm0, dword ptr [0x56fa4c]
00473333 f30f59054cfa5600         mulss      xmm0, dword ptr [0x56fa4c]
0047333b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047333e f30f1181c4320000         movss      dword ptr [ecx + 0x32c4], xmm0
00473346 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473349 52                       push       edx
0047334a 68d0394700               push       0x4739d0
0047334f 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00473352 83c016                   add        eax, 0x16
00473355 50                       push       eax
00473356 e8b5eff9ff               call       0x412310
0047335b 83c40c                   add        esp, 0xc
0047335e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473361 894108                   mov        dword ptr [ecx + 8], eax
00473364 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473367 52                       push       edx
00473368 6870484700               push       0x474870
0047336d 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00473370 83c003                   add        eax, 3
00473373 50                       push       eax
00473374 e837f0f9ff               call       0x4123b0
00473379 83c40c                   add        esp, 0xc
0047337c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047337f 89410c                   mov        dword ptr [ecx + 0xc], eax
00473382 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473385 52                       push       edx
00473386 6860484700               push       0x474860
0047338b 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0047338e 83c006                   add        eax, 6
00473391 50                       push       eax
00473392 e819f0f9ff               call       0x4123b0
00473397 83c40c                   add        esp, 0xc
0047339a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047339d 898168330000             mov        dword ptr [ecx + 0x3368], eax
004733a3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004733a6 c7825c33000000000000     mov        dword ptr [edx + 0x335c], 0
004733b0 6a00                     push       0
004733b2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004733b5 83c110                   add        ecx, 0x10
004733b8 e86301fbff               call       0x423520
004733bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004733c0 83c148                   add        ecx, 0x48
004733c3 e8c865fbff               call       0x429990
004733c8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004733cb 81c19c000000             add        ecx, 0x9c
004733d1 e8ba65fbff               call       0x429990
004733d6 8b45fc                   mov        eax, dword ptr [ebp - 4]
004733d9 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
004733df 83c901                   or         ecx, 1
004733e2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004733e5 898a44330000             mov        dword ptr [edx + 0x3344], ecx
004733eb 33c0                     xor        eax, eax
004733ed 5f                       pop        edi
004733ee 5e                       pop        esi
004733ef 8be5                     mov        esp, ebp
004733f1 5d                       pop        ebp
004733f2 c20800                   ret        8
004733f5 cc                       int3       
004733f6 cc                       int3       
004733f7 cc                       int3       
004733f8 cc                       int3       
004733f9 cc                       int3       
004733fa cc                       int3       
004733fb cc                       int3       
004733fc cc                       int3       
004733fd cc                       int3       
004733fe cc                       int3       
004733ff cc                       int3       
