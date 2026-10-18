00479360 55                       push       ebp
00479361 8bec                     mov        ebp, esp
00479363 81ec98000000             sub        esp, 0x98
00479369 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047936e 33c5                     xor        eax, ebp
00479370 8945fc                   mov        dword ptr [ebp - 4], eax
00479373 894dec                   mov        dword ptr [ebp - 0x14], ecx
00479376 8d4df0                   lea        ecx, [ebp - 0x10]
00479379 e8929afaff               call       0x422e10
0047937e 90                       nop        
0047937f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479382 81c1a4000000             add        ecx, 0xa4
00479388 e8a33cfeff               call       0x45d030
0047938d 85c0                     test       eax, eax
0047938f 0f8429040000             je         0x4797be
00479395 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479398 05a4000000               add        eax, 0xa4
0047939d 89857cffffff             mov        dword ptr [ebp - 0x84], eax
004793a3 6a5a                     push       0x5a
004793a5 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004793ab e8b0a1faff               call       0x423560
004793b0 0fb6c8                   movzx      ecx, al
004793b3 85c9                     test       ecx, ecx
004793b5 746a                     je         0x479421
004793b7 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004793ba 83c204                   add        edx, 4
004793bd 8955e0                   mov        dword ptr [ebp - 0x20], edx
004793c0 6a00                     push       0
004793c2 e86974feff               call       0x460830
004793c7 83c404                   add        esp, 4
004793ca 8bc8                     mov        ecx, eax
004793cc e87f74feff               call       0x460850
004793d1 50                       push       eax
004793d2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004793d5 e806120000               call       0x47a5e0
004793da 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004793dd 83c004                   add        eax, 4
004793e0 8945dc                   mov        dword ptr [ebp - 0x24], eax
004793e3 51                       push       ecx
004793e4 f30f100568cd5600         movss      xmm0, dword ptr [0x56cd68]
004793ec f30f110424               movss      dword ptr [esp], xmm0
004793f1 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004793f4 e8170e0000               call       0x47a210
004793f9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004793fc 83c104                   add        ecx, 4
004793ff 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00479402 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00479405 51                       push       ecx
00479406 f30f1082d4000000         movss      xmm0, dword ptr [edx + 0xd4]
0047940e f30f110424               movss      dword ptr [esp], xmm0
00479413 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00479416 e8250e0000               call       0x47a240
0047941b 90                       nop        
0047941c e99d030000               jmp        0x4797be
00479421 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479424 05a4000000               add        eax, 0xa4
00479429 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0047942c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047942f 6b91c80000000a           imul       edx, dword ptr [ecx + 0xc8], 0xa
00479436 83c25a                   add        edx, 0x5a
00479439 52                       push       edx
0047943a 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0047943d e81ea1faff               call       0x423560
00479442 0fb6c0                   movzx      eax, al
00479445 85c0                     test       eax, eax
00479447 744b                     je         0x479494
00479449 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047944c 83c104                   add        ecx, 4
0047944f 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00479452 6a00                     push       0
00479454 e8d773feff               call       0x460830
00479459 83c404                   add        esp, 4
0047945c 8bc8                     mov        ecx, eax
0047945e e8ed73feff               call       0x460850
00479463 50                       push       eax
00479464 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00479467 e874110000               call       0x47a5e0
0047946c 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0047946f 83c204                   add        edx, 4
00479472 8955cc                   mov        dword ptr [ebp - 0x34], edx
00479475 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479478 51                       push       ecx
00479479 f30f1080d4000000         movss      xmm0, dword ptr [eax + 0xd4]
00479481 f30f110424               movss      dword ptr [esp], xmm0
00479486 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00479489 e8b20d0000               call       0x47a240
0047948e 90                       nop        
0047948f e92a030000               jmp        0x4797be
00479494 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479497 81c1a4000000             add        ecx, 0xa4
0047949d 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004794a0 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004794a3 6b82c80000000a           imul       eax, dword ptr [edx + 0xc8], 0xa
004794aa 83c05a                   add        eax, 0x5a
004794ad 50                       push       eax
004794ae 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004794b1 e82af1ffff               call       0x4785e0
004794b6 0fb6c8                   movzx      ecx, al
004794b9 85c9                     test       ecx, ecx
004794bb 7456                     je         0x479513
004794bd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004794c0 83c104                   add        ecx, 4
004794c3 e838100000               call       0x47a500
004794c8 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004794cb 83c204                   add        edx, 4
004794ce 8955c4                   mov        dword ptr [ebp - 0x3c], edx
004794d1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004794d4 05b4000000               add        eax, 0xb4
004794d9 50                       push       eax
004794da e831cdfdff               call       0x456210
004794df d91c24                   fstp       dword ptr [esp]
004794e2 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004794e5 e876100000               call       0x47a560
004794ea 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004794ed 83c104                   add        ecx, 4
004794f0 894dc0                   mov        dword ptr [ebp - 0x40], ecx
004794f3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004794f6 81c2b4000000             add        edx, 0xb4
004794fc 52                       push       edx
004794fd e89e0d0000               call       0x47a2a0
00479502 d91c24                   fstp       dword ptr [esp]
00479505 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00479508 e8b3100000               call       0x47a5c0
0047950d 90                       nop        
0047950e e9ab020000               jmp        0x4797be
00479513 6a00                     push       0
00479515 e846ebffff               call       0x478060
0047951a 83c404                   add        esp, 4
0047951d 85c0                     test       eax, eax
0047951f 7447                     je         0x479568
00479521 6a00                     push       0
00479523 e838ebffff               call       0x478060
00479528 83c404                   add        esp, 4
0047952b 8945b8                   mov        dword ptr [ebp - 0x48], eax
0047952e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479531 83c104                   add        ecx, 4
00479534 e86728f9ff               call       0x40bda0
00479539 8945bc                   mov        dword ptr [ebp - 0x44], eax
0047953c 51                       push       ecx
0047953d f30f100514f15600         movss      xmm0, dword ptr [0x56f114]
00479545 f30f110424               movss      dword ptr [esp], xmm0
0047954a 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
0047954d 50                       push       eax
0047954e 8d8d74ffffff             lea        ecx, [ebp - 0x8c]
00479554 51                       push       ecx
00479555 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00479558 e8a3160300               call       0x4aac00
0047955d 8b10                     mov        edx, dword ptr [eax]
0047955f 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479562 8990c0000000             mov        dword ptr [eax + 0xc0], edx
00479568 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047956b 81c1c0000000             add        ecx, 0xc0
00479571 e88a2df9ff               call       0x40c300
00479576 85c0                     test       eax, eax
00479578 0f84c5010000             je         0x479743
0047957e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479581 81c1c0000000             add        ecx, 0xc0
00479587 e834150300               call       0x4aaac0
0047958c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047958f 8981c4000000             mov        dword ptr [ecx + 0xc4], eax
00479595 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00479598 8b8ac4000000             mov        ecx, dword ptr [edx + 0xc4]
0047959e e83d0e0000               call       0x47a3e0
004795a3 85c0                     test       eax, eax
004795a5 0f8596010000             jne        0x479741
004795ab 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004795ae 8b88c4000000             mov        ecx, dword ptr [eax + 0xc4]
004795b4 e8070d0000               call       0x47a2c0
004795b9 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004795bc 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004795bf 83c104                   add        ecx, 4
004795c2 e8d927f9ff               call       0x40bda0
004795c7 8945b0                   mov        dword ptr [ebp - 0x50], eax
004795ca 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004795cd 51                       push       ecx
004795ce 8b55b0                   mov        edx, dword ptr [ebp - 0x50]
004795d1 52                       push       edx
004795d2 e8090d0000               call       0x47a2e0
004795d7 83c408                   add        esp, 8
004795da d95dac                   fstp       dword ptr [ebp - 0x54]
004795dd 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004795e0 83c104                   add        ecx, 4
004795e3 e84882faff               call       0x421830
004795e8 8bc8                     mov        ecx, eax
004795ea e8f1fcfaff               call       0x4292e0
004795ef 51                       push       ecx
004795f0 d91c24                   fstp       dword ptr [esp]
004795f3 51                       push       ecx
004795f4 f30f1045ac               movss      xmm0, dword ptr [ebp - 0x54]
004795f9 f30f110424               movss      dword ptr [esp], xmm0
004795fe e8bd00fcff               call       0x4396c0
00479603 83c408                   add        esp, 8
00479606 d95de4                   fstp       dword ptr [ebp - 0x1c]
00479609 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047960c 83c104                   add        ecx, 4
0047960f e80c0d0000               call       0x47a320
00479614 d95de8                   fstp       dword ptr [ebp - 0x18]
00479617 51                       push       ecx
00479618 f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
0047961d f30f110424               movss      dword ptr [esp], xmm0
00479622 e809100000               call       0x47a630
00479627 83c404                   add        esp, 4
0047962a d95da8                   fstp       dword ptr [ebp - 0x58]
0047962d f30f1045a8               movss      xmm0, dword ptr [ebp - 0x58]
00479632 f30f100df0e05600         movss      xmm1, dword ptr [0x56e0f0]
0047963a f30f590dccc85600         mulss      xmm1, dword ptr [0x56c8cc]
00479642 f30f5e0de0c85600         divss      xmm1, dword ptr [0x56c8e0]
0047964a 0f2fc1                   comiss     xmm0, xmm1
0047964d 722f                     jb         0x47967e
0047964f f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00479654 f30f5c05fcfb5600         subss      xmm0, dword ptr [0x56fbfc]
0047965c f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00479661 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00479669 0f2f45e8                 comiss     xmm0, dword ptr [ebp - 0x18]
0047966d 760d                     jbe        0x47967c
0047966f f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
00479677 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
0047967c eb59                     jmp        0x4796d7
0047967e 51                       push       ecx
0047967f f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
00479684 f30f110424               movss      dword ptr [esp], xmm0
00479689 e8a20f0000               call       0x47a630
0047968e 83c404                   add        esp, 4
00479691 d95da4                   fstp       dword ptr [ebp - 0x5c]
00479694 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0047969c f30f5e05a8f75600         divss      xmm0, dword ptr [0x56f7a8]
004796a4 0f2f45a4                 comiss     xmm0, dword ptr [ebp - 0x5c]
004796a8 762d                     jbe        0x4796d7
004796aa f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004796af f30f5805f8fb5600         addss      xmm0, dword ptr [0x56fbf8]
004796b7 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004796bc f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
004796c1 0f2f05bcd75600           comiss     xmm0, dword ptr [0x56d7bc]
004796c8 760d                     jbe        0x4796d7
004796ca f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
004796d2 f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
004796d7 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004796da 83c004                   add        eax, 4
004796dd 89459c                   mov        dword ptr [ebp - 0x64], eax
004796e0 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004796e3 83c104                   add        ecx, 4
004796e6 e84581faff               call       0x421830
004796eb 8945a0                   mov        dword ptr [ebp - 0x60], eax
004796ee f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
004796f3 f30f590528ef5600         mulss      xmm0, dword ptr [0x56ef28]
004796fb 51                       push       ecx
004796fc f30f110424               movss      dword ptr [esp], xmm0
00479701 8d8d78ffffff             lea        ecx, [ebp - 0x88]
00479707 51                       push       ecx
00479708 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0047970b e8b098fdff               call       0x452fc0
00479710 8bc8                     mov        ecx, eax
00479712 e8c9fbfaff               call       0x4292e0
00479717 51                       push       ecx
00479718 d91c24                   fstp       dword ptr [esp]
0047971b 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0047971e e83d0e0000               call       0x47a560
00479723 90                       nop        
00479724 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00479727 83c204                   add        edx, 4
0047972a 895598                   mov        dword ptr [ebp - 0x68], edx
0047972d 51                       push       ecx
0047972e f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00479733 f30f110424               movss      dword ptr [esp], xmm0
00479738 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0047973b e8800e0000               call       0x47a5c0
00479740 90                       nop        
00479741 eb7b                     jmp        0x4797be
00479743 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00479746 83c004                   add        eax, 4
00479749 894594                   mov        dword ptr [ebp - 0x6c], eax
0047974c 51                       push       ecx
0047974d f30f1005a4cd5600         movss      xmm0, dword ptr [0x56cda4]
00479755 f30f110424               movss      dword ptr [esp], xmm0
0047975a 51                       push       ecx
0047975b f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00479763 f30f110424               movss      dword ptr [esp], xmm0
00479768 51                       push       ecx
00479769 f30f100514ed5600         movss      xmm0, dword ptr [0x56ed14]
00479771 f30f110424               movss      dword ptr [esp], xmm0
00479776 51                       push       ecx
00479777 0f57c0                   xorps      xmm0, xmm0
0047977a f30f110424               movss      dword ptr [esp], xmm0
0047977f 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
00479782 e8790c0000               call       0x47a400
00479787 85c0                     test       eax, eax
00479789 7433                     je         0x4797be
0047978b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047978e 83c104                   add        ecx, 4
00479791 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00479794 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479797 83c104                   add        ecx, 4
0047979a e8810b0000               call       0x47a320
0047979f d95d90                   fstp       dword ptr [ebp - 0x70]
004797a2 f30f104590               movss      xmm0, dword ptr [ebp - 0x70]
004797a7 f30f590500fc5600         mulss      xmm0, dword ptr [0x56fc00]
004797af 51                       push       ecx
004797b0 f30f110424               movss      dword ptr [esp], xmm0
004797b5 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
004797b8 e8030e0000               call       0x47a5c0
004797bd 90                       nop        
004797be 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004797c1 83c104                   add        ecx, 4
004797c4 e8d725f9ff               call       0x40bda0
004797c9 8b10                     mov        edx, dword ptr [eax]
004797cb 8955f0                   mov        dword ptr [ebp - 0x10], edx
004797ce 8b4804                   mov        ecx, dword ptr [eax + 4]
004797d1 894df4                   mov        dword ptr [ebp - 0xc], ecx
004797d4 8b5008                   mov        edx, dword ptr [eax + 8]
004797d7 8955f8                   mov        dword ptr [ebp - 8], edx
004797da 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004797dd 83c104                   add        ecx, 4
004797e0 e80b0a0000               call       0x47a1f0
004797e5 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
004797e8 894588                   mov        dword ptr [ebp - 0x78], eax
004797eb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004797ee 83c104                   add        ecx, 4
004797f1 e8aa25f9ff               call       0x40bda0
004797f6 50                       push       eax
004797f7 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004797fa e8e1adfeff               call       0x4645e0
004797ff 90                       nop        
00479800 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479803 83c104                   add        ecx, 4
00479806 e89525f9ff               call       0x40bda0
0047980b 894584                   mov        dword ptr [ebp - 0x7c], eax
0047980e 8d4df0                   lea        ecx, [ebp - 0x10]
00479811 51                       push       ecx
00479812 8d9568ffffff             lea        edx, [ebp - 0x98]
00479818 52                       push       edx
00479819 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0047981c e81ffcfaff               call       0x429440
00479821 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00479824 81c1b4000000             add        ecx, 0xb4
0047982a 8b10                     mov        edx, dword ptr [eax]
0047982c 8911                     mov        dword ptr [ecx], edx
0047982e 8b5004                   mov        edx, dword ptr [eax + 4]
00479831 895104                   mov        dword ptr [ecx + 4], edx
00479834 8b4008                   mov        eax, dword ptr [eax + 8]
00479837 894108                   mov        dword ptr [ecx + 8], eax
0047983a 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047983d 81c1a4000000             add        ecx, 0xa4
00479843 894d80                   mov        dword ptr [ebp - 0x80], ecx
00479846 6a00                     push       0
00479848 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
0047984b e8f09cfaff               call       0x423540
00479850 90                       nop        
00479851 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00479854 33cd                     xor        ecx, ebp
00479856 e893910c00               call       0x5429ee
0047985b 8be5                     mov        esp, ebp
0047985d 5d                       pop        ebp
0047985e c3                       ret        
0047985f cc                       int3       
