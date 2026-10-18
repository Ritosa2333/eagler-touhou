0050a4d0 55                       push       ebp
0050a4d1 8bec                     mov        ebp, esp
0050a4d3 83ec38                   sub        esp, 0x38
0050a4d6 894dfc                   mov        dword ptr [ebp - 4], ecx
0050a4d9 c745f401000000           mov        dword ptr [ebp - 0xc], 1
0050a4e0 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a4e3 8b889c020000             mov        ecx, dword ptr [eax + 0x29c]
0050a4e9 894df0                   mov        dword ptr [ebp - 0x10], ecx
0050a4ec 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a4ef c782a402000000000000     mov        dword ptr [edx + 0x2a4], 0
0050a4f9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a4fc c780b402000000000000     mov        dword ptr [eax + 0x2b4], 0
0050a506 c745f800000000           mov        dword ptr [ebp - 8], 0
0050a50d eb19                     jmp        0x50a528
0050a50f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a512 83c101                   add        ecx, 1
0050a515 894df8                   mov        dword ptr [ebp - 8], ecx
0050a518 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0050a51b d1ea                     shr        edx, 1
0050a51d 8955f0                   mov        dword ptr [ebp - 0x10], edx
0050a520 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0050a523 d1e0                     shl        eax, 1
0050a525 8945f4                   mov        dword ptr [ebp - 0xc], eax
0050a528 837df820                 cmp        dword ptr [ebp - 8], 0x20
0050a52c 0f8329010000             jae        0x50a65b
0050a532 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0050a535 83e101                   and        ecx, 1
0050a538 0f84dd000000             je         0x50a61b
0050a53e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a541 81c218010000             add        edx, 0x118
0050a547 8955e0                   mov        dword ptr [ebp - 0x20], edx
0050a54a 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a54d 50                       push       eax
0050a54e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050a551 e82aa0f0ff               call       0x414580
0050a556 8945ec                   mov        dword ptr [ebp - 0x14], eax
0050a559 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0050a55c 8b11                     mov        edx, dword ptr [ecx]
0050a55e 83c201                   add        edx, 1
0050a561 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0050a564 8910                     mov        dword ptr [eax], edx
0050a566 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a569 81c118020000             add        ecx, 0x218
0050a56f 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0050a572 8b55f8                   mov        edx, dword ptr [ebp - 8]
0050a575 52                       push       edx
0050a576 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0050a579 e802a0f0ff               call       0x414580
0050a57e 8945e8                   mov        dword ptr [ebp - 0x18], eax
0050a581 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0050a584 8b08                     mov        ecx, dword ptr [eax]
0050a586 83c101                   add        ecx, 1
0050a589 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0050a58c 890a                     mov        dword ptr [edx], ecx
0050a58e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a591 0518010000               add        eax, 0x118
0050a596 8945d8                   mov        dword ptr [ebp - 0x28], eax
0050a599 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a59c 51                       push       ecx
0050a59d 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0050a5a0 e8db9ff0ff               call       0x414580
0050a5a5 833808                   cmp        dword ptr [eax], 8
0050a5a8 7215                     jb         0x50a5bf
0050a5aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a5ad 8b82b4020000             mov        eax, dword ptr [edx + 0x2b4]
0050a5b3 0b45f4                   or         eax, dword ptr [ebp - 0xc]
0050a5b6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a5b9 8981b4020000             mov        dword ptr [ecx + 0x2b4], eax
0050a5bf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a5c2 81c218010000             add        edx, 0x118
0050a5c8 8955d4                   mov        dword ptr [ebp - 0x2c], edx
0050a5cb 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a5ce 50                       push       eax
0050a5cf 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0050a5d2 e8a99ff0ff               call       0x414580
0050a5d7 83381a                   cmp        dword ptr [eax], 0x1a
0050a5da 723d                     jb         0x50a619
0050a5dc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a5df 8b91a4020000             mov        edx, dword ptr [ecx + 0x2a4]
0050a5e5 0b55f4                   or         edx, dword ptr [ebp - 0xc]
0050a5e8 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a5eb 8990a4020000             mov        dword ptr [eax + 0x2a4], edx
0050a5f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a5f4 81c118010000             add        ecx, 0x118
0050a5fa 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0050a5fd 8b55f8                   mov        edx, dword ptr [ebp - 8]
0050a600 52                       push       edx
0050a601 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0050a604 e8779ff0ff               call       0x414580
0050a609 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0050a60c 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0050a60f 8b08                     mov        ecx, dword ptr [eax]
0050a611 83e908                   sub        ecx, 8
0050a614 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0050a617 890a                     mov        dword ptr [edx], ecx
0050a619 eb3b                     jmp        0x50a656
0050a61b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a61e 0518010000               add        eax, 0x118
0050a623 8945cc                   mov        dword ptr [ebp - 0x34], eax
0050a626 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050a629 51                       push       ecx
0050a62a 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0050a62d e84e9ff0ff               call       0x414580
0050a632 c70000000000             mov        dword ptr [eax], 0
0050a638 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a63b 81c218020000             add        edx, 0x218
0050a641 8955c8                   mov        dword ptr [ebp - 0x38], edx
0050a644 8b45f8                   mov        eax, dword ptr [ebp - 8]
0050a647 50                       push       eax
0050a648 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0050a64b e8309ff0ff               call       0x414580
0050a650 c70000000000             mov        dword ptr [eax], 0
0050a656 e9b4feffff               jmp        0x50a50f
0050a65b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a65e 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a661 8b819c020000             mov        eax, dword ptr [ecx + 0x29c]
0050a667 3382a0020000             xor        eax, dword ptr [edx + 0x2a0]
0050a66d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a670 23819c020000             and        eax, dword ptr [ecx + 0x29c]
0050a676 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050a679 8982a8020000             mov        dword ptr [edx + 0x2a8], eax
0050a67f 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a682 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050a685 8b909c020000             mov        edx, dword ptr [eax + 0x29c]
0050a68b 3391a0020000             xor        edx, dword ptr [ecx + 0x2a0]
0050a691 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a694 8b889c020000             mov        ecx, dword ptr [eax + 0x29c]
0050a69a f7d1                     not        ecx
0050a69c 23d1                     and        edx, ecx
0050a69e 8b45fc                   mov        eax, dword ptr [ebp - 4]
0050a6a1 8990ac020000             mov        dword ptr [eax + 0x2ac], edx
0050a6a7 8be5                     mov        esp, ebp
0050a6a9 5d                       pop        ebp
0050a6aa c3                       ret        
0050a6ab cc                       int3       
0050a6ac cc                       int3       
0050a6ad cc                       int3       
0050a6ae cc                       int3       
0050a6af cc                       int3       
