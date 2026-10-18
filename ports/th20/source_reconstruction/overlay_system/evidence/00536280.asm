00536280 55                       push       ebp
00536281 8bec                     mov        ebp, esp
00536283 81ec98000000             sub        esp, 0x98
00536289 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053628e 33c5                     xor        eax, ebp
00536290 8945fc                   mov        dword ptr [ebp - 4], eax
00536293 56                       push       esi
00536294 894db0                   mov        dword ptr [ebp - 0x50], ecx
00536297 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0053629a 83c014                   add        eax, 0x14
0053629d 8945a8                   mov        dword ptr [ebp - 0x58], eax
005362a0 6a00                     push       0
005362a2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
005362a5 e8e6d2eeff               call       0x423590
005362aa 0fb6c8                   movzx      ecx, al
005362ad 85c9                     test       ecx, ecx
005362af 740f                     je         0x5362c0
005362b1 b801000000               mov        eax, 1
005362b6 e98c020000               jmp        0x536547
005362bb e96c020000               jmp        0x53652c
005362c0 6a00                     push       0
005362c2 e8b9ddf2ff               call       0x464080
005362c7 83c404                   add        esp, 4
005362ca 8945a0                   mov        dword ptr [ebp - 0x60], eax
005362cd 6a00                     push       0
005362cf e8acddf2ff               call       0x464080
005362d4 83c404                   add        esp, 4
005362d7 8bc8                     mov        ecx, eax
005362d9 e84256feff               call       0x51b920
005362de 8bf0                     mov        esi, eax
005362e0 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
005362e3 83c114                   add        ecx, 0x14
005362e6 e8a59cedff               call       0x40ff90
005362eb 0faff0                   imul       esi, eax
005362ee e86ddfffff               call       0x534260
005362f3 8bc8                     mov        ecx, eax
005362f5 8bc6                     mov        eax, esi
005362f7 99                       cdq        
005362f8 f7f9                     idiv       ecx
005362fa 8945a4                   mov        dword ptr [ebp - 0x5c], eax
005362fd 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
00536300 52                       push       edx
00536301 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00536304 e83786f8ff               call       0x4be940
00536309 90                       nop        
0053630a c745ac00000000           mov        dword ptr [ebp - 0x54], 0
00536311 eb09                     jmp        0x53631c
00536313 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
00536316 83c001                   add        eax, 1
00536319 8945ac                   mov        dword ptr [ebp - 0x54], eax
0053631c 6a00                     push       0
0053631e e85dddf2ff               call       0x464080
00536323 83c404                   add        esp, 4
00536326 8bc8                     mov        ecx, eax
00536328 e8731ef8ff               call       0x4b81a0
0053632d 3945ac                   cmp        dword ptr [ebp - 0x54], eax
00536330 0f8df6010000             jge        0x53652c
00536336 6a00                     push       0
00536338 e8f3a4f2ff               call       0x460830
0053633d 83c404                   add        esp, 4
00536340 89459c                   mov        dword ptr [ebp - 0x64], eax
00536343 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00536346 51                       push       ecx
00536347 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0053634a e871f4ffff               call       0x5357c0
0053634f 8bc8                     mov        ecx, eax
00536351 e81ab6eeff               call       0x421970
00536356 8b10                     mov        edx, dword ptr [eax]
00536358 8955ec                   mov        dword ptr [ebp - 0x14], edx
0053635b 8b4804                   mov        ecx, dword ptr [eax + 4]
0053635e 894df0                   mov        dword ptr [ebp - 0x10], ecx
00536361 8b5008                   mov        edx, dword ptr [eax + 8]
00536364 8955f4                   mov        dword ptr [ebp - 0xc], edx
00536367 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
0053636a 50                       push       eax
0053636b b984695c00               mov        ecx, 0x5c6984
00536370 e80be2edff               call       0x414580
00536375 894598                   mov        dword ptr [ebp - 0x68], eax
00536378 6a00                     push       0
0053637a 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0053637d e8ae30f1ff               call       0x449430
00536382 0fb6c8                   movzx      ecx, al
00536385 85c9                     test       ecx, ecx
00536387 0f849c000000             je         0x536429
0053638d 8d4db4                   lea        ecx, [ebp - 0x4c]
00536390 e89b56f4ff               call       0x47ba30
00536395 6a00                     push       0
00536397 e894a4f2ff               call       0x460830
0053639c 83c404                   add        esp, 4
0053639f 8bc8                     mov        ecx, eax
005363a1 e8aaa4f2ff               call       0x460850
005363a6 8b10                     mov        edx, dword ptr [eax]
005363a8 8955b4                   mov        dword ptr [ebp - 0x4c], edx
005363ab 8b4804                   mov        ecx, dword ptr [eax + 4]
005363ae 894db8                   mov        dword ptr [ebp - 0x48], ecx
005363b1 8b5008                   mov        edx, dword ptr [eax + 8]
005363b4 8955bc                   mov        dword ptr [ebp - 0x44], edx
005363b7 6a00                     push       0
005363b9 e86211f0ff               call       0x437520
005363be 83c404                   add        esp, 4
005363c1 894594                   mov        dword ptr [ebp - 0x6c], eax
005363c4 6a00                     push       0
005363c6 8d45b4                   lea        eax, [ebp - 0x4c]
005363c9 50                       push       eax
005363ca 6a08                     push       8
005363cc 8d8d6cffffff             lea        ecx, [ebp - 0x94]
005363d2 51                       push       ecx
005363d3 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
005363d6 e89577f6ff               call       0x49db70
005363db 8b10                     mov        edx, dword ptr [eax]
005363dd 895590                   mov        dword ptr [ebp - 0x70], edx
005363e0 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
005363e3 50                       push       eax
005363e4 b984695c00               mov        ecx, 0x5c6984
005363e9 e892e1edff               call       0x414580
005363ee 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
005363f1 8908                     mov        dword ptr [eax], ecx
005363f3 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
005363f6 52                       push       edx
005363f7 b984695c00               mov        ecx, 0x5c6984
005363fc e87fe1edff               call       0x414580
00536401 89458c                   mov        dword ptr [ebp - 0x74], eax
00536404 51                       push       ecx
00536405 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
0053640d f30f110424               movss      dword ptr [esp], xmm0
00536412 51                       push       ecx
00536413 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
0053641b f30f110424               movss      dword ptr [esp], xmm0
00536420 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00536423 e888a2f1ff               call       0x4506b0
00536428 90                       nop        
00536429 8b45ac                   mov        eax, dword ptr [ebp - 0x54]
0053642c 50                       push       eax
0053642d b984695c00               mov        ecx, 0x5c6984
00536432 e849e1edff               call       0x414580
00536437 894588                   mov        dword ptr [ebp - 0x78], eax
0053643a 8d4dec                   lea        ecx, [ebp - 0x14]
0053643d 51                       push       ecx
0053643e 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00536441 e89ae1f2ff               call       0x4645e0
00536446 6a04                     push       4
00536448 8d4df8                   lea        ecx, [ebp - 8]
0053644b e8305cedff               call       0x40c080
00536450 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
00536453 52                       push       edx
00536454 8d4df8                   lea        ecx, [ebp - 8]
00536457 e844deedff               call       0x4142a0
0053645c 90                       nop        
0053645d 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00536460 83785800                 cmp        dword ptr [eax + 0x58], 0
00536464 740f                     je         0x536475
00536466 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00536469 8b5158                   mov        edx, dword ptr [ecx + 0x58]
0053646c 83ea01                   sub        edx, 1
0053646f 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00536472 895058                   mov        dword ptr [eax + 0x58], edx
00536475 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00536478 83795800                 cmp        dword ptr [ecx + 0x58], 0
0053647c 0f85a5000000             jne        0x536527
00536482 6a00                     push       0
00536484 e8f729f4ff               call       0x478e80
00536489 83c404                   add        esp, 4
0053648c 894584                   mov        dword ptr [ebp - 0x7c], eax
0053648f 83ec28                   sub        esp, 0x28
00536492 8bcc                     mov        ecx, esp
00536494 89a568ffffff             mov        dword ptr [ebp - 0x98], esp
0053649a 8d55f8                   lea        edx, [ebp - 8]
0053649d 52                       push       edx
0053649e e85df4ffff               call       0x535900
005364a3 51                       push       ecx
005364a4 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
005364ac f30f110424               movss      dword ptr [esp], xmm0
005364b1 8d45ec                   lea        eax, [ebp - 0x14]
005364b4 50                       push       eax
005364b5 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
005364b8 e8836df4ff               call       0x47d240
005364bd 894580                   mov        dword ptr [ebp - 0x80], eax
005364c0 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
005364c3 898d78ffffff             mov        dword ptr [ebp - 0x88], ecx
005364c9 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005364cc 8b4258                   mov        eax, dword ptr [edx + 0x58]
005364cf 89857cffffff             mov        dword ptr [ebp - 0x84], eax
005364d5 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
005364db 038d78ffffff             add        ecx, dword ptr [ebp - 0x88]
005364e1 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005364e4 894a58                   mov        dword ptr [edx + 0x58], ecx
005364e7 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
005364ea 8b4858                   mov        ecx, dword ptr [eax + 0x58]
005364ed d1e1                     shl        ecx, 1
005364ef 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
005364f2 894a58                   mov        dword ptr [edx + 0x58], ecx
005364f5 6a00                     push       0
005364f7 e8e429f4ff               call       0x478ee0
005364fc 83c404                   add        esp, 4
005364ff 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
00536505 6a01                     push       1
00536507 6a00                     push       0
00536509 51                       push       ecx
0053650a f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
00536512 f30f110424               movss      dword ptr [esp], xmm0
00536517 8d45ec                   lea        eax, [ebp - 0x14]
0053651a 50                       push       eax
0053651b 8b8d74ffffff             mov        ecx, dword ptr [ebp - 0x8c]
00536521 e8aa45f9ff               call       0x4caad0
00536526 90                       nop        
00536527 e9e7fdffff               jmp        0x536313
0053652c 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
0053652f 83c114                   add        ecx, 0x14
00536532 898d70ffffff             mov        dword ptr [ebp - 0x90], ecx
00536538 6a00                     push       0
0053653a 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
00536540 e8db2eefff               call       0x429420
00536545 33c0                     xor        eax, eax
00536547 5e                       pop        esi
00536548 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053654b 33cd                     xor        ecx, ebp
0053654d e89cc40000               call       0x5429ee
00536552 8be5                     mov        esp, ebp
00536554 5d                       pop        ebp
00536555 c3                       ret        
00536556 cc                       int3       
00536557 cc                       int3       
00536558 cc                       int3       
00536559 cc                       int3       
0053655a cc                       int3       
0053655b cc                       int3       
0053655c cc                       int3       
0053655d cc                       int3       
0053655e cc                       int3       
0053655f cc                       int3       
