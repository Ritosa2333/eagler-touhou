0049f3d0 55                       push       ebp
0049f3d1 8bec                     mov        ebp, esp
0049f3d3 6aff                     push       -1
0049f3d5 68bb955600               push       0x5695bb
0049f3da 64a100000000             mov        eax, dword ptr fs:[0]
0049f3e0 50                       push       eax
0049f3e1 81ecc0010000             sub        esp, 0x1c0
0049f3e7 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049f3ec 33c5                     xor        eax, ebp
0049f3ee 8945f0                   mov        dword ptr [ebp - 0x10], eax
0049f3f1 56                       push       esi
0049f3f2 50                       push       eax
0049f3f3 8d45f4                   lea        eax, [ebp - 0xc]
0049f3f6 64a300000000             mov        dword ptr fs:[0], eax
0049f3fc 894da8                   mov        dword ptr [ebp - 0x58], ecx
0049f3ff 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f402 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
0049f405 c1e902                   shr        ecx, 2
0049f408 83e101                   and        ecx, 1
0049f40b 7407                     je         0x49f414
0049f40d 33c0                     xor        eax, eax
0049f40f e9ad0f0000               jmp        0x4a03c1
0049f414 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f417 83c114                   add        ecx, 0x14
0049f41a e8710bf7ff               call       0x40ff90
0049f41f 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f422 8b4a5c                   mov        ecx, dword ptr [edx + 0x5c]
0049f425 0fb711                   movzx      edx, word ptr [ecx]
0049f428 3bc2                     cmp        eax, edx
0049f42a 0f8c750f0000             jl         0x4a03a5
0049f430 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f433 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049f436 0fb65102                 movzx      edx, byte ptr [ecx + 2]
0049f43a 89558c                   mov        dword ptr [ebp - 0x74], edx
0049f43d 837d8c11                 cmp        dword ptr [ebp - 0x74], 0x11
0049f441 770a                     ja         0x49f44d
0049f443 8b458c                   mov        eax, dword ptr [ebp - 0x74]
0049f446 ff2485dc034a00           jmp        dword ptr [eax*4 + 0x4a03dc]
0049f44d 83c8ff                   or         eax, 0xffffffff
0049f450 e96c0f0000               jmp        0x4a03c1
0049f455 6a1c                     push       0x1c
0049f457 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f45a e821ccf6ff               call       0x40c080
0049f45f 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f462 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049f465 83c204                   add        edx, 4
0049f468 52                       push       edx
0049f469 e822880100               call       0x4b7c90
0049f46e 83c404                   add        esp, 4
0049f471 50                       push       eax
0049f472 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f475 e8b6b3fcff               call       0x46a830
0049f47a c745fc00000000           mov        dword ptr [ebp - 4], 0
0049f481 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f484 83b88000000000           cmp        dword ptr [eax + 0x80], 0
0049f48b 755f                     jne        0x49f4ec
0049f48d c745a000000000           mov        dword ptr [ebp - 0x60], 0
0049f494 eb09                     jmp        0x49f49f
0049f496 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
0049f499 83c101                   add        ecx, 1
0049f49c 894da0                   mov        dword ptr [ebp - 0x60], ecx
0049f49f 837da005                 cmp        dword ptr [ebp - 0x60], 5
0049f4a3 7d47                     jge        0x49f4ec
0049f4a5 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f4a8 83c234                   add        edx, 0x34
0049f4ab 8995a8feffff             mov        dword ptr [ebp - 0x158], edx
0049f4b1 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
0049f4b4 50                       push       eax
0049f4b5 8b8da8feffff             mov        ecx, dword ptr [ebp - 0x158]
0049f4bb e8c050f7ff               call       0x414580
0049f4c0 8bc8                     mov        ecx, eax
0049f4c2 e8c91a0000               call       0x4a0f90
0049f4c7 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f4ca 83c148                   add        ecx, 0x48
0049f4cd 898da0feffff             mov        dword ptr [ebp - 0x160], ecx
0049f4d3 8b55a0                   mov        edx, dword ptr [ebp - 0x60]
0049f4d6 52                       push       edx
0049f4d7 8b8da0feffff             mov        ecx, dword ptr [ebp - 0x160]
0049f4dd e89e50f7ff               call       0x414580
0049f4e2 8bc8                     mov        ecx, eax
0049f4e4 e8a71a0000               call       0x4a0f90
0049f4e9 90                       nop        
0049f4ea ebaa                     jmp        0x49f496
0049f4ec 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f4ef 83c034                   add        eax, 0x34
0049f4f2 89859cfeffff             mov        dword ptr [ebp - 0x164], eax
0049f4f8 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f4fb 8b9180000000             mov        edx, dword ptr [ecx + 0x80]
0049f501 52                       push       edx
0049f502 8b8d9cfeffff             mov        ecx, dword ptr [ebp - 0x164]
0049f508 e87350f7ff               call       0x414580
0049f50d 8bc8                     mov        ecx, eax
0049f50f e87c1a0000               call       0x4a0f90
0049f514 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f517 83c048                   add        eax, 0x48
0049f51a 898568ffffff             mov        dword ptr [ebp - 0x98], eax
0049f520 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f523 8b9180000000             mov        edx, dword ptr [ecx + 0x80]
0049f529 52                       push       edx
0049f52a 8b8d68ffffff             mov        ecx, dword ptr [ebp - 0x98]
0049f530 e84b50f7ff               call       0x414580
0049f535 8bc8                     mov        ecx, eax
0049f537 e8541a0000               call       0x4a0f90
0049f53c 90                       nop        
0049f53d 6a00                     push       0
0049f53f 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f542 e879fcffff               call       0x49f1c0
0049f547 0fbe00                   movsx      eax, byte ptr [eax]
0049f54a 83f87c                   cmp        eax, 0x7c
0049f54d 0f8520010000             jne        0x49f673
0049f553 6a01                     push       1
0049f555 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f558 e863fcffff               call       0x49f1c0
0049f55d 50                       push       eax
0049f55e e8691b0b00               call       0x5510cc
0049f563 83c404                   add        esp, 4
0049f566 8945ec                   mov        dword ptr [ebp - 0x14], eax
0049f569 6a2c                     push       0x2c
0049f56b 6a01                     push       1
0049f56d 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f570 e84bfcffff               call       0x49f1c0
0049f575 50                       push       eax
0049f576 e8f51a0000               call       0x4a1070
0049f57b 83c408                   add        esp, 8
0049f57e 894594                   mov        dword ptr [ebp - 0x6c], eax
0049f581 b901000000               mov        ecx, 1
0049f586 c1e100                   shl        ecx, 0
0049f589 034d94                   add        ecx, dword ptr [ebp - 0x6c]
0049f58c 51                       push       ecx
0049f58d e83a1b0b00               call       0x5510cc
0049f592 83c404                   add        esp, 4
0049f595 8945e8                   mov        dword ptr [ebp - 0x18], eax
0049f598 6a2c                     push       0x2c
0049f59a ba01000000               mov        edx, 1
0049f59f c1e200                   shl        edx, 0
0049f5a2 035594                   add        edx, dword ptr [ebp - 0x6c]
0049f5a5 52                       push       edx
0049f5a6 e8c51a0000               call       0x4a1070
0049f5ab 83c408                   add        esp, 8
0049f5ae 894594                   mov        dword ptr [ebp - 0x6c], eax
0049f5b1 6a1c                     push       0x1c
0049f5b3 8d4dac                   lea        ecx, [ebp - 0x54]
0049f5b6 e8c5caf6ff               call       0x40c080
0049f5bb b801000000               mov        eax, 1
0049f5c0 c1e000                   shl        eax, 0
0049f5c3 034594                   add        eax, dword ptr [ebp - 0x6c]
0049f5c6 50                       push       eax
0049f5c7 8d4dac                   lea        ecx, [ebp - 0x54]
0049f5ca e861b2fcff               call       0x46a830
0049f5cf c645fc01                 mov        byte ptr [ebp - 4], 1
0049f5d3 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f5d6 83c148                   add        ecx, 0x48
0049f5d9 898d64ffffff             mov        dword ptr [ebp - 0x9c], ecx
0049f5df 83ec28                   sub        esp, 0x28
0049f5e2 8bf4                     mov        esi, esp
0049f5e4 89a588feffff             mov        dword ptr [ebp - 0x178], esp
0049f5ea 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f5ed 8b8280000000             mov        eax, dword ptr [edx + 0x80]
0049f5f3 50                       push       eax
0049f5f4 8b8d64ffffff             mov        ecx, dword ptr [ebp - 0x9c]
0049f5fa e8814ff7ff               call       0x414580
0049f5ff 8bc8                     mov        ecx, eax
0049f601 e8cad8faff               call       0x44ced0
0049f606 50                       push       eax
0049f607 8d4de8                   lea        ecx, [ebp - 0x18]
0049f60a 51                       push       ecx
0049f60b 8d55ec                   lea        edx, [ebp - 0x14]
0049f60e 52                       push       edx
0049f60f 8d45ac                   lea        eax, [ebp - 0x54]
0049f612 50                       push       eax
0049f613 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f616 51                       push       ecx
0049f617 8d8d34feffff             lea        ecx, [ebp - 0x1cc]
0049f61d e81ef5ffff               call       0x49eb40
0049f622 898560ffffff             mov        dword ptr [ebp - 0xa0], eax
0049f628 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
0049f62e 89955cffffff             mov        dword ptr [ebp - 0xa4], edx
0049f634 c645fc02                 mov        byte ptr [ebp - 4], 2
0049f638 8b855cffffff             mov        eax, dword ptr [ebp - 0xa4]
0049f63e 50                       push       eax
0049f63f 8bce                     mov        ecx, esi
0049f641 e82aedffff               call       0x49e370
0049f646 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0049f64c e82f0bfdff               call       0x470180
0049f651 c645fc01                 mov        byte ptr [ebp - 4], 1
0049f655 8d8d34feffff             lea        ecx, [ebp - 0x1cc]
0049f65b e880b7fcff               call       0x46ade0
0049f660 90                       nop        
0049f661 c645fc00                 mov        byte ptr [ebp - 4], 0
0049f665 8d4dac                   lea        ecx, [ebp - 0x54]
0049f668 e8d39bfaff               call       0x449240
0049f66d 90                       nop        
0049f66e e9b4000000               jmp        0x49f727
0049f673 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f676 83c134                   add        ecx, 0x34
0049f679 898d58ffffff             mov        dword ptr [ebp - 0xa8], ecx
0049f67f 83ec28                   sub        esp, 0x28
0049f682 8bf4                     mov        esi, esp
0049f684 89a584feffff             mov        dword ptr [ebp - 0x17c], esp
0049f68a 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f68d 8b8280000000             mov        eax, dword ptr [edx + 0x80]
0049f693 50                       push       eax
0049f694 8b8d58ffffff             mov        ecx, dword ptr [ebp - 0xa8]
0049f69a e8e14ef7ff               call       0x414580
0049f69f 8bc8                     mov        ecx, eax
0049f6a1 e82ad8faff               call       0x44ced0
0049f6a6 50                       push       eax
0049f6a7 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f6aa 51                       push       ecx
0049f6ab 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f6ae 52                       push       edx
0049f6af 8d8d60feffff             lea        ecx, [ebp - 0x1a0]
0049f6b5 e8b674feff               call       0x486b70
0049f6ba 898554ffffff             mov        dword ptr [ebp - 0xac], eax
0049f6c0 8b8554ffffff             mov        eax, dword ptr [ebp - 0xac]
0049f6c6 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
0049f6cc c645fc03                 mov        byte ptr [ebp - 4], 3
0049f6d0 8b8d50ffffff             mov        ecx, dword ptr [ebp - 0xb0]
0049f6d6 51                       push       ecx
0049f6d7 8bce                     mov        ecx, esi
0049f6d9 e852edffff               call       0x49e430
0049f6de 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0049f6e4 e8970afdff               call       0x470180
0049f6e9 c645fc00                 mov        byte ptr [ebp - 4], 0
0049f6ed 8d8d60feffff             lea        ecx, [ebp - 0x1a0]
0049f6f3 e8e8b6fcff               call       0x46ade0
0049f6f8 90                       nop        
0049f6f9 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f6fc 8b8280000000             mov        eax, dword ptr [edx + 0x80]
0049f702 83c001                   add        eax, 1
0049f705 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f708 898180000000             mov        dword ptr [ecx + 0x80], eax
0049f70e 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f711 83ba8000000005           cmp        dword ptr [edx + 0x80], 5
0049f718 7c0d                     jl         0x49f727
0049f71a 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f71d c7808000000000000000     mov        dword ptr [eax + 0x80], 0
0049f727 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0049f72e 8d4dc8                   lea        ecx, [ebp - 0x38]
0049f731 e80a9bfaff               call       0x449240
0049f736 90                       nop        
0049f737 e9400c0000               jmp        0x4a037c
0049f73c c7459c00000000           mov        dword ptr [ebp - 0x64], 0
0049f743 eb09                     jmp        0x49f74e
0049f745 8b4d9c                   mov        ecx, dword ptr [ebp - 0x64]
0049f748 83c101                   add        ecx, 1
0049f74b 894d9c                   mov        dword ptr [ebp - 0x64], ecx
0049f74e 837d9c05                 cmp        dword ptr [ebp - 0x64], 5
0049f752 7d47                     jge        0x49f79b
0049f754 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f757 83c234                   add        edx, 0x34
0049f75a 89954cffffff             mov        dword ptr [ebp - 0xb4], edx
0049f760 8b459c                   mov        eax, dword ptr [ebp - 0x64]
0049f763 50                       push       eax
0049f764 8b8d4cffffff             mov        ecx, dword ptr [ebp - 0xb4]
0049f76a e8114ef7ff               call       0x414580
0049f76f 8bc8                     mov        ecx, eax
0049f771 e8da6bfeff               call       0x486350
0049f776 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f779 83c148                   add        ecx, 0x48
0049f77c 898d48ffffff             mov        dword ptr [ebp - 0xb8], ecx
0049f782 8b559c                   mov        edx, dword ptr [ebp - 0x64]
0049f785 52                       push       edx
0049f786 8b8d48ffffff             mov        ecx, dword ptr [ebp - 0xb8]
0049f78c e8ef4df7ff               call       0x414580
0049f791 8bc8                     mov        ecx, eax
0049f793 e8b86bfeff               call       0x486350
0049f798 90                       nop        
0049f799 ebaa                     jmp        0x49f745
0049f79b e9dc0b0000               jmp        0x4a037c
0049f7a0 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f7a3 83c124                   add        ecx, 0x24
0049f7a6 e8e507f7ff               call       0x40ff90
0049f7ab 85c0                     test       eax, eax
0049f7ad 7f1f                     jg         0x49f7ce
0049f7af 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f7b2 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049f7b5 ba04000000               mov        edx, 4
0049f7ba 6bc200                   imul       eax, edx, 0
0049f7bd 8b4c0104                 mov        ecx, dword ptr [ecx + eax + 4]
0049f7c1 51                       push       ecx
0049f7c2 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f7c5 83c124                   add        ecx, 0x24
0049f7c8 e8533df8ff               call       0x423520
0049f7cd 90                       nop        
0049f7ce 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f7d1 83c224                   add        edx, 0x24
0049f7d4 899544ffffff             mov        dword ptr [ebp - 0xbc], edx
0049f7da 6a00                     push       0
0049f7dc 8b8d44ffffff             mov        ecx, dword ptr [ebp - 0xbc]
0049f7e2 e8399cf8ff               call       0x429420
0049f7e7 90                       nop        
0049f7e8 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049f7eb 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049f7ee ba04000000               mov        edx, 4
0049f7f3 6bc200                   imul       eax, edx, 0
0049f7f6 837c010400               cmp        dword ptr [ecx + eax + 4], 0
0049f7fb 7d11                     jge        0x49f80e
0049f7fd 68e7030000               push       0x3e7
0049f802 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f805 83c124                   add        ecx, 0x24
0049f808 e8133df8ff               call       0x423520
0049f80d 90                       nop        
0049f80e 33c9                     xor        ecx, ecx
0049f810 753a                     jne        0x49f84c
0049f812 ba04000000               mov        edx, 4
0049f817 6bc200                   imul       eax, edx, 0
0049f81a 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0049f821 7429                     je         0x49f84c
0049f823 b904000000               mov        ecx, 4
0049f828 6bd100                   imul       edx, ecx, 0
0049f82b 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0049f831 898540ffffff             mov        dword ptr [ebp - 0xc0], eax
0049f837 6801000800               push       0x80001
0049f83c 8b8d40ffffff             mov        ecx, dword ptr [ebp - 0xc0]
0049f842 e8b9a3f7ff               call       0x419c00
0049f847 894588                   mov        dword ptr [ebp - 0x78], eax
0049f84a eb07                     jmp        0x49f853
0049f84c c7458800000000           mov        dword ptr [ebp - 0x78], 0
0049f853 837d8800                 cmp        dword ptr [ebp - 0x78], 0
0049f857 750f                     jne        0x49f868
0049f859 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f85c 83c124                   add        ecx, 0x24
0049f85f e82c07f7ff               call       0x40ff90
0049f864 85c0                     test       eax, eax
0049f866 7f21                     jg         0x49f889
0049f868 6a00                     push       0
0049f86a 6a00                     push       0
0049f86c b930a85b00               mov        ecx, 0x5ba830
0049f871 e8fa74f8ff               call       0x426d70
0049f876 6a00                     push       0
0049f878 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f87b 83c124                   add        ecx, 0x24
0049f87e e89d3cf8ff               call       0x423520
0049f883 90                       nop        
0049f884 e9f30a0000               jmp        0x4a037c
0049f889 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
0049f88f e8fc150000               call       0x4a0e90
0049f894 85c0                     test       eax, eax
0049f896 0f85c3000000             jne        0x49f95f
0049f89c 33c9                     xor        ecx, ecx
0049f89e 753a                     jne        0x49f8da
0049f8a0 ba04000000               mov        edx, 4
0049f8a5 6bc200                   imul       eax, edx, 0
0049f8a8 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0049f8af 7429                     je         0x49f8da
0049f8b1 b904000000               mov        ecx, 4
0049f8b6 6bd100                   imul       edx, ecx, 0
0049f8b9 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0049f8bf 89853cffffff             mov        dword ptr [ebp - 0xc4], eax
0049f8c5 6800020000               push       0x200
0049f8ca 8b8d3cffffff             mov        ecx, dword ptr [ebp - 0xc4]
0049f8d0 e8cb9cfaff               call       0x4495a0
0049f8d5 894584                   mov        dword ptr [ebp - 0x7c], eax
0049f8d8 eb07                     jmp        0x49f8e1
0049f8da c7458400000000           mov        dword ptr [ebp - 0x7c], 0
0049f8e1 837d8400                 cmp        dword ptr [ebp - 0x7c], 0
0049f8e5 7548                     jne        0x49f92f
0049f8e7 33c9                     xor        ecx, ecx
0049f8e9 7537                     jne        0x49f922
0049f8eb ba04000000               mov        edx, 4
0049f8f0 6bc200                   imul       eax, edx, 0
0049f8f3 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0049f8fa 7426                     je         0x49f922
0049f8fc b904000000               mov        ecx, 4
0049f901 6bd100                   imul       edx, ecx, 0
0049f904 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0049f90a 898538ffffff             mov        dword ptr [ebp - 0xc8], eax
0049f910 6a00                     push       0
0049f912 8b8d38ffffff             mov        ecx, dword ptr [ebp - 0xc8]
0049f918 e803110000               call       0x4a0a20
0049f91d 894580                   mov        dword ptr [ebp - 0x80], eax
0049f920 eb07                     jmp        0x49f929
0049f922 c7458000000000           mov        dword ptr [ebp - 0x80], 0
0049f929 837d8014                 cmp        dword ptr [ebp - 0x80], 0x14
0049f92d 7230                     jb         0x49f95f
0049f92f 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f932 83c124                   add        ecx, 0x24
0049f935 898d34ffffff             mov        dword ptr [ebp - 0xcc], ecx
0049f93b 6a06                     push       6
0049f93d 8b8d34ffffff             mov        ecx, dword ptr [ebp - 0xcc]
0049f943 e8f826fdff               call       0x472040
0049f948 85c0                     test       eax, eax
0049f94a 7513                     jne        0x49f95f
0049f94c 6a00                     push       0
0049f94e 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f951 83c124                   add        ecx, 0x24
0049f954 e8c73bf8ff               call       0x423520
0049f959 90                       nop        
0049f95a e91d0a0000               jmp        0x4a037c
0049f95f e95b0a0000               jmp        0x4a03bf
0049f964 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f967 83c124                   add        ecx, 0x24
0049f96a e82106f7ff               call       0x40ff90
0049f96f 85c0                     test       eax, eax
0049f971 7f1f                     jg         0x49f992
0049f973 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049f976 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049f979 b904000000               mov        ecx, 4
0049f97e 6bd100                   imul       edx, ecx, 0
0049f981 8b441004                 mov        eax, dword ptr [eax + edx + 4]
0049f985 50                       push       eax
0049f986 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f989 83c124                   add        ecx, 0x24
0049f98c e88f3bf8ff               call       0x423520
0049f991 90                       nop        
0049f992 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049f995 83c124                   add        ecx, 0x24
0049f998 898d30ffffff             mov        dword ptr [ebp - 0xd0], ecx
0049f99e 6a00                     push       0
0049f9a0 8b8d30ffffff             mov        ecx, dword ptr [ebp - 0xd0]
0049f9a6 e8759af8ff               call       0x429420
0049f9ab 90                       nop        
0049f9ac 33d2                     xor        edx, edx
0049f9ae 753d                     jne        0x49f9ed
0049f9b0 b804000000               mov        eax, 4
0049f9b5 6bc800                   imul       ecx, eax, 0
0049f9b8 83b99c885b0000           cmp        dword ptr [ecx + 0x5b889c], 0
0049f9bf 742c                     je         0x49f9ed
0049f9c1 ba04000000               mov        edx, 4
0049f9c6 6bc200                   imul       eax, edx, 0
0049f9c9 8b889c885b00             mov        ecx, dword ptr [eax + 0x5b889c]
0049f9cf 898d2cffffff             mov        dword ptr [ebp - 0xd4], ecx
0049f9d5 6801000800               push       0x80001
0049f9da 8b8d2cffffff             mov        ecx, dword ptr [ebp - 0xd4]
0049f9e0 e81ba2f7ff               call       0x419c00
0049f9e5 89857cffffff             mov        dword ptr [ebp - 0x84], eax
0049f9eb eb0a                     jmp        0x49f9f7
0049f9ed c7857cffffff00000000     mov        dword ptr [ebp - 0x84], 0
0049f9f7 83bd7cffffff00           cmp        dword ptr [ebp - 0x84], 0
0049f9fe 750f                     jne        0x49fa0f
0049fa00 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fa03 83c124                   add        ecx, 0x24
0049fa06 e88505f7ff               call       0x40ff90
0049fa0b 85c0                     test       eax, eax
0049fa0d 7f37                     jg         0x49fa46
0049fa0f 6a00                     push       0
0049fa11 6a00                     push       0
0049fa13 b930a85b00               mov        ecx, 0x5ba830
0049fa18 e85373f8ff               call       0x426d70
0049fa1d 6a00                     push       0
0049fa1f 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fa22 83c124                   add        ecx, 0x24
0049fa25 e8f63af8ff               call       0x423520
0049fa2a 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fa2d c7828000000000000000     mov        dword ptr [edx + 0x80], 0
0049fa37 c70518a55b0000000000     mov        dword ptr [0x5ba518], 0
0049fa41 e936090000               jmp        0x4a037c
0049fa46 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
0049fa4c e83f140000               call       0x4a0e90
0049fa51 85c0                     test       eax, eax
0049fa53 0f85eb000000             jne        0x49fb44
0049fa59 33c0                     xor        eax, eax
0049fa5b 753d                     jne        0x49fa9a
0049fa5d b904000000               mov        ecx, 4
0049fa62 6bd100                   imul       edx, ecx, 0
0049fa65 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0049fa6c 742c                     je         0x49fa9a
0049fa6e b804000000               mov        eax, 4
0049fa73 6bc800                   imul       ecx, eax, 0
0049fa76 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0049fa7c 899528ffffff             mov        dword ptr [ebp - 0xd8], edx
0049fa82 6800020000               push       0x200
0049fa87 8b8d28ffffff             mov        ecx, dword ptr [ebp - 0xd8]
0049fa8d e80e9bfaff               call       0x4495a0
0049fa92 898578ffffff             mov        dword ptr [ebp - 0x88], eax
0049fa98 eb0a                     jmp        0x49faa4
0049fa9a c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
0049faa4 83bd78ffffff00           cmp        dword ptr [ebp - 0x88], 0
0049faab 7551                     jne        0x49fafe
0049faad 33c0                     xor        eax, eax
0049faaf 753a                     jne        0x49faeb
0049fab1 b904000000               mov        ecx, 4
0049fab6 6bd100                   imul       edx, ecx, 0
0049fab9 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
0049fac0 7429                     je         0x49faeb
0049fac2 b804000000               mov        eax, 4
0049fac7 6bc800                   imul       ecx, eax, 0
0049faca 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
0049fad0 899524ffffff             mov        dword ptr [ebp - 0xdc], edx
0049fad6 6a00                     push       0
0049fad8 8b8d24ffffff             mov        ecx, dword ptr [ebp - 0xdc]
0049fade e83d0f0000               call       0x4a0a20
0049fae3 898574ffffff             mov        dword ptr [ebp - 0x8c], eax
0049fae9 eb0a                     jmp        0x49faf5
0049faeb c78574ffffff00000000     mov        dword ptr [ebp - 0x8c], 0
0049faf5 83bd74ffffff14           cmp        dword ptr [ebp - 0x8c], 0x14
0049fafc 7246                     jb         0x49fb44
0049fafe 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fb01 83c024                   add        eax, 0x24
0049fb04 898520ffffff             mov        dword ptr [ebp - 0xe0], eax
0049fb0a 6a06                     push       6
0049fb0c 8b8d20ffffff             mov        ecx, dword ptr [ebp - 0xe0]
0049fb12 e82925fdff               call       0x472040
0049fb17 85c0                     test       eax, eax
0049fb19 7529                     jne        0x49fb44
0049fb1b 6a00                     push       0
0049fb1d 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fb20 83c124                   add        ecx, 0x24
0049fb23 e8f839f8ff               call       0x423520
0049fb28 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fb2b c7818000000000000000     mov        dword ptr [ecx + 0x80], 0
0049fb35 c70518a55b0000000000     mov        dword ptr [0x5ba518], 0
0049fb3f e938080000               jmp        0x4a037c
0049fb44 e976080000               jmp        0x4a03bf
0049fb49 51                       push       ecx
0049fb4a f30f100588035700         movss      xmm0, dword ptr [0x570388]
0049fb52 f30f110424               movss      dword ptr [esp], xmm0
0049fb57 51                       push       ecx
0049fb58 f30f1005a8cd5600         movss      xmm0, dword ptr [0x56cda8]
0049fb60 f30f110424               movss      dword ptr [esp], xmm0
0049fb65 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
0049fb6b e8300f0000               call       0x4a0aa0
0049fb70 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fb73 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049fb76 b904000000               mov        ecx, 4
0049fb7b 6bd100                   imul       edx, ecx, 0
0049fb7e 8b441004                 mov        eax, dword ptr [eax + edx + 4]
0049fb82 83c00f                   add        eax, 0xf
0049fb85 50                       push       eax
0049fb86 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0049fb8c e89fc8faff               call       0x44c430
0049fb91 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fb94 8b517c                   mov        edx, dword ptr [ecx + 0x7c]
0049fb97 83ca04                   or         edx, 4
0049fb9a 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fb9d 89507c                   mov        dword ptr [eax + 0x7c], edx
0049fba0 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fba3 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049fba6 b801000000               mov        eax, 1
0049fbab c1e002                   shl        eax, 2
0049fbae 8d4c0204                 lea        ecx, [edx + eax + 4]
0049fbb2 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fbb5 894a78                   mov        dword ptr [edx + 0x78], ecx
0049fbb8 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fbbb 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fbbe ba04000000               mov        edx, 4
0049fbc3 6bc200                   imul       eax, edx, 0
0049fbc6 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fbc9 8b440104                 mov        eax, dword ptr [ecx + eax + 4]
0049fbcd 8982ec000000             mov        dword ptr [edx + 0xec], eax
0049fbd3 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fbd6 81c1dc000000             add        ecx, 0xdc
0049fbdc 898d18ffffff             mov        dword ptr [ebp - 0xe8], ecx
0049fbe2 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fbe5 89951cffffff             mov        dword ptr [ebp - 0xe4], edx
0049fbeb 8d851cffffff             lea        eax, [ebp - 0xe4]
0049fbf1 50                       push       eax
0049fbf2 68000a4a00               push       0x4a0a00
0049fbf7 8b8d18ffffff             mov        ecx, dword ptr [ebp - 0xe8]
0049fbfd e8ceb5f6ff               call       0x40b1d0
0049fc02 90                       nop        
0049fc03 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fc06 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049fc09 b804000000               mov        eax, 4
0049fc0e 6bc800                   imul       ecx, eax, 0
0049fc11 8d540a04                 lea        edx, [edx + ecx + 4]
0049fc15 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fc18 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fc1b 0fb64103                 movzx      eax, byte ptr [ecx + 3]
0049fc1f 03d0                     add        edx, eax
0049fc21 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fc24 89515c                   mov        dword ptr [ecx + 0x5c], edx
0049fc27 33c0                     xor        eax, eax
0049fc29 e993070000               jmp        0x4a03c1
0049fc2e 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fc31 81c29c000000             add        edx, 0x9c
0049fc37 899514ffffff             mov        dword ptr [ebp - 0xec], edx
0049fc3d 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fc40 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fc43 ba04000000               mov        edx, 4
0049fc48 6bc200                   imul       eax, edx, 0
0049fc4b 8b4c0104                 mov        ecx, dword ptr [ecx + eax + 4]
0049fc4f 51                       push       ecx
0049fc50 8b8d14ffffff             mov        ecx, dword ptr [ebp - 0xec]
0049fc56 e82549f7ff               call       0x414580
0049fc5b 8bc8                     mov        ecx, eax
0049fc5d e86e00fbff               call       0x44fcd0
0049fc62 90                       nop        
0049fc63 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fc66 81c28c000000             add        edx, 0x8c
0049fc6c 899510ffffff             mov        dword ptr [ebp - 0xf0], edx
0049fc72 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fc75 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fc78 ba04000000               mov        edx, 4
0049fc7d c1e200                   shl        edx, 0
0049fc80 8b441104                 mov        eax, dword ptr [ecx + edx + 4]
0049fc84 50                       push       eax
0049fc85 8b8d10ffffff             mov        ecx, dword ptr [ebp - 0xf0]
0049fc8b e8f048f7ff               call       0x414580
0049fc90 8b08                     mov        ecx, dword ptr [eax]
0049fc92 898d08ffffff             mov        dword ptr [ebp - 0xf8], ecx
0049fc98 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fc9b 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049fc9e b904000000               mov        ecx, 4
0049fca3 d1e1                     shl        ecx, 1
0049fca5 8b540804                 mov        edx, dword ptr [eax + ecx + 4]
0049fca9 89950cffffff             mov        dword ptr [ebp - 0xf4], edx
0049fcaf 6a00                     push       0
0049fcb1 6aff                     push       -1
0049fcb3 8b850cffffff             mov        eax, dword ptr [ebp - 0xf4]
0049fcb9 50                       push       eax
0049fcba 6a00                     push       0
0049fcbc 8d8d98feffff             lea        ecx, [ebp - 0x168]
0049fcc2 51                       push       ecx
0049fcc3 8b8d08ffffff             mov        ecx, dword ptr [ebp - 0xf8]
0049fcc9 e8a20ffbff               call       0x450c70
0049fcce 8b10                     mov        edx, dword ptr [eax]
0049fcd0 899500ffffff             mov        dword ptr [ebp - 0x100], edx
0049fcd6 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fcd9 059c000000               add        eax, 0x9c
0049fcde 898504ffffff             mov        dword ptr [ebp - 0xfc], eax
0049fce4 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fce7 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049fcea b804000000               mov        eax, 4
0049fcef 6bc800                   imul       ecx, eax, 0
0049fcf2 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
0049fcf6 52                       push       edx
0049fcf7 8b8d04ffffff             mov        ecx, dword ptr [ebp - 0xfc]
0049fcfd e87e48f7ff               call       0x414580
0049fd02 8b8d00ffffff             mov        ecx, dword ptr [ebp - 0x100]
0049fd08 8908                     mov        dword ptr [eax], ecx
0049fd0a e96d060000               jmp        0x4a037c
0049fd0f b968a55b00               mov        ecx, 0x5ba568
0049fd14 e88743fcff               call       0x4640a0
0049fd19 83f801                   cmp        eax, 1
0049fd1c 0f85dc000000             jne        0x49fdfe
0049fd22 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fd25 81c29c000000             add        edx, 0x9c
0049fd2b 8995fcfeffff             mov        dword ptr [ebp - 0x104], edx
0049fd31 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fd34 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fd37 ba04000000               mov        edx, 4
0049fd3c 6bc200                   imul       eax, edx, 0
0049fd3f 8b4c0104                 mov        ecx, dword ptr [ecx + eax + 4]
0049fd43 51                       push       ecx
0049fd44 8b8dfcfeffff             mov        ecx, dword ptr [ebp - 0x104]
0049fd4a e83148f7ff               call       0x414580
0049fd4f 8bc8                     mov        ecx, eax
0049fd51 e87afffaff               call       0x44fcd0
0049fd56 90                       nop        
0049fd57 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fd5a 81c28c000000             add        edx, 0x8c
0049fd60 8995f8feffff             mov        dword ptr [ebp - 0x108], edx
0049fd66 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fd69 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fd6c ba04000000               mov        edx, 4
0049fd71 c1e200                   shl        edx, 0
0049fd74 8b441104                 mov        eax, dword ptr [ecx + edx + 4]
0049fd78 50                       push       eax
0049fd79 8b8df8feffff             mov        ecx, dword ptr [ebp - 0x108]
0049fd7f e8fc47f7ff               call       0x414580
0049fd84 8b08                     mov        ecx, dword ptr [eax]
0049fd86 898df0feffff             mov        dword ptr [ebp - 0x110], ecx
0049fd8c 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fd8f 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049fd92 b904000000               mov        ecx, 4
0049fd97 d1e1                     shl        ecx, 1
0049fd99 8b540804                 mov        edx, dword ptr [eax + ecx + 4]
0049fd9d 8995f4feffff             mov        dword ptr [ebp - 0x10c], edx
0049fda3 6a00                     push       0
0049fda5 6aff                     push       -1
0049fda7 8b85f4feffff             mov        eax, dword ptr [ebp - 0x10c]
0049fdad 50                       push       eax
0049fdae 6a00                     push       0
0049fdb0 8d8d94feffff             lea        ecx, [ebp - 0x16c]
0049fdb6 51                       push       ecx
0049fdb7 8b8df0feffff             mov        ecx, dword ptr [ebp - 0x110]
0049fdbd e8ae0efbff               call       0x450c70
0049fdc2 8b10                     mov        edx, dword ptr [eax]
0049fdc4 8995e8feffff             mov        dword ptr [ebp - 0x118], edx
0049fdca 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fdcd 059c000000               add        eax, 0x9c
0049fdd2 8985ecfeffff             mov        dword ptr [ebp - 0x114], eax
0049fdd8 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fddb 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049fdde b804000000               mov        eax, 4
0049fde3 6bc800                   imul       ecx, eax, 0
0049fde6 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
0049fdea 52                       push       edx
0049fdeb 8b8decfeffff             mov        ecx, dword ptr [ebp - 0x114]
0049fdf1 e88a47f7ff               call       0x414580
0049fdf6 8b8de8feffff             mov        ecx, dword ptr [ebp - 0x118]
0049fdfc 8908                     mov        dword ptr [eax], ecx
0049fdfe e979050000               jmp        0x4a037c
0049fe03 b968a55b00               mov        ecx, 0x5ba568
0049fe08 e89342fcff               call       0x4640a0
0049fe0d 83f802                   cmp        eax, 2
0049fe10 0f85dc000000             jne        0x49fef2
0049fe16 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fe19 81c29c000000             add        edx, 0x9c
0049fe1f 8995e4feffff             mov        dword ptr [ebp - 0x11c], edx
0049fe25 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fe28 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fe2b ba04000000               mov        edx, 4
0049fe30 6bc200                   imul       eax, edx, 0
0049fe33 8b4c0104                 mov        ecx, dword ptr [ecx + eax + 4]
0049fe37 51                       push       ecx
0049fe38 8b8de4feffff             mov        ecx, dword ptr [ebp - 0x11c]
0049fe3e e83d47f7ff               call       0x414580
0049fe43 8bc8                     mov        ecx, eax
0049fe45 e886fefaff               call       0x44fcd0
0049fe4a 90                       nop        
0049fe4b 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fe4e 81c28c000000             add        edx, 0x8c
0049fe54 8995e0feffff             mov        dword ptr [ebp - 0x120], edx
0049fe5a 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fe5d 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049fe60 ba04000000               mov        edx, 4
0049fe65 c1e200                   shl        edx, 0
0049fe68 8b441104                 mov        eax, dword ptr [ecx + edx + 4]
0049fe6c 50                       push       eax
0049fe6d 8b8de0feffff             mov        ecx, dword ptr [ebp - 0x120]
0049fe73 e80847f7ff               call       0x414580
0049fe78 8b08                     mov        ecx, dword ptr [eax]
0049fe7a 898dd8feffff             mov        dword ptr [ebp - 0x128], ecx
0049fe80 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049fe83 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049fe86 b904000000               mov        ecx, 4
0049fe8b d1e1                     shl        ecx, 1
0049fe8d 8b540804                 mov        edx, dword ptr [eax + ecx + 4]
0049fe91 8995dcfeffff             mov        dword ptr [ebp - 0x124], edx
0049fe97 6a00                     push       0
0049fe99 6aff                     push       -1
0049fe9b 8b85dcfeffff             mov        eax, dword ptr [ebp - 0x124]
0049fea1 50                       push       eax
0049fea2 6a00                     push       0
0049fea4 8d8d90feffff             lea        ecx, [ebp - 0x170]
0049feaa 51                       push       ecx
0049feab 8b8dd8feffff             mov        ecx, dword ptr [ebp - 0x128]
0049feb1 e8ba0dfbff               call       0x450c70
0049feb6 8b10                     mov        edx, dword ptr [eax]
0049feb8 8995d0feffff             mov        dword ptr [ebp - 0x130], edx
0049febe 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049fec1 059c000000               add        eax, 0x9c
0049fec6 8985d4feffff             mov        dword ptr [ebp - 0x12c], eax
0049fecc 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fecf 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049fed2 b804000000               mov        eax, 4
0049fed7 6bc800                   imul       ecx, eax, 0
0049feda 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
0049fede 52                       push       edx
0049fedf 8b8dd4feffff             mov        ecx, dword ptr [ebp - 0x12c]
0049fee5 e89646f7ff               call       0x414580
0049feea 8b8dd0feffff             mov        ecx, dword ptr [ebp - 0x130]
0049fef0 8908                     mov        dword ptr [eax], ecx
0049fef2 e985040000               jmp        0x4a037c
0049fef7 b968a55b00               mov        ecx, 0x5ba568
0049fefc e89f41fcff               call       0x4640a0
0049ff01 83f803                   cmp        eax, 3
0049ff04 0f85dc000000             jne        0x49ffe6
0049ff0a 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049ff0d 81c29c000000             add        edx, 0x9c
0049ff13 8995ccfeffff             mov        dword ptr [ebp - 0x134], edx
0049ff19 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049ff1c 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049ff1f ba04000000               mov        edx, 4
0049ff24 6bc200                   imul       eax, edx, 0
0049ff27 8b4c0104                 mov        ecx, dword ptr [ecx + eax + 4]
0049ff2b 51                       push       ecx
0049ff2c 8b8dccfeffff             mov        ecx, dword ptr [ebp - 0x134]
0049ff32 e84946f7ff               call       0x414580
0049ff37 8bc8                     mov        ecx, eax
0049ff39 e892fdfaff               call       0x44fcd0
0049ff3e 90                       nop        
0049ff3f 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049ff42 81c28c000000             add        edx, 0x8c
0049ff48 8995c8feffff             mov        dword ptr [ebp - 0x138], edx
0049ff4e 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049ff51 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
0049ff54 ba04000000               mov        edx, 4
0049ff59 c1e200                   shl        edx, 0
0049ff5c 8b441104                 mov        eax, dword ptr [ecx + edx + 4]
0049ff60 50                       push       eax
0049ff61 8b8dc8feffff             mov        ecx, dword ptr [ebp - 0x138]
0049ff67 e81446f7ff               call       0x414580
0049ff6c 8b08                     mov        ecx, dword ptr [eax]
0049ff6e 898dc0feffff             mov        dword ptr [ebp - 0x140], ecx
0049ff74 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049ff77 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049ff7a b904000000               mov        ecx, 4
0049ff7f d1e1                     shl        ecx, 1
0049ff81 8b540804                 mov        edx, dword ptr [eax + ecx + 4]
0049ff85 8995c4feffff             mov        dword ptr [ebp - 0x13c], edx
0049ff8b 6a00                     push       0
0049ff8d 6aff                     push       -1
0049ff8f 8b85c4feffff             mov        eax, dword ptr [ebp - 0x13c]
0049ff95 50                       push       eax
0049ff96 6a00                     push       0
0049ff98 8d8d8cfeffff             lea        ecx, [ebp - 0x174]
0049ff9e 51                       push       ecx
0049ff9f 8b8dc0feffff             mov        ecx, dword ptr [ebp - 0x140]
0049ffa5 e8c60cfbff               call       0x450c70
0049ffaa 8b10                     mov        edx, dword ptr [eax]
0049ffac 8995b8feffff             mov        dword ptr [ebp - 0x148], edx
0049ffb2 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
0049ffb5 059c000000               add        eax, 0x9c
0049ffba 8985bcfeffff             mov        dword ptr [ebp - 0x144], eax
0049ffc0 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049ffc3 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
0049ffc6 b804000000               mov        eax, 4
0049ffcb 6bc800                   imul       ecx, eax, 0
0049ffce 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
0049ffd2 52                       push       edx
0049ffd3 8b8dbcfeffff             mov        ecx, dword ptr [ebp - 0x144]
0049ffd9 e8a245f7ff               call       0x414580
0049ffde 8b8db8feffff             mov        ecx, dword ptr [ebp - 0x148]
0049ffe4 8908                     mov        dword ptr [eax], ecx
0049ffe6 e991030000               jmp        0x4a037c
0049ffeb 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0049ffee 8b425c                   mov        eax, dword ptr [edx + 0x5c]
0049fff1 b904000000               mov        ecx, 4
0049fff6 6bd100                   imul       edx, ecx, 0
0049fff9 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
0049fffc 8b541004                 mov        edx, dword ptr [eax + edx + 4]
004a0000 899184000000             mov        dword ptr [ecx + 0x84], edx
004a0006 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a0009 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
004a000c ba04000000               mov        edx, 4
004a0011 c1e200                   shl        edx, 0
004a0014 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a0017 8b4c1104                 mov        ecx, dword ptr [ecx + edx + 4]
004a001b 898888000000             mov        dword ptr [eax + 0x88], ecx
004a0021 e956030000               jmp        0x4a037c
004a0026 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a0029 8b425c                   mov        eax, dword ptr [edx + 0x5c]
004a002c 83c004                   add        eax, 4
004a002f 50                       push       eax
004a0030 6a00                     push       0
004a0032 b9404d5c00               mov        ecx, 0x5c4d40
004a0037 e8349a0300               call       0x4d9a70
004a003c 90                       nop        
004a003d 6800035700               push       0x570300
004a0042 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a0045 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
004a0048 83c204                   add        edx, 4
004a004b 52                       push       edx
004a004c e8bffc0a00               call       0x54fd10
004a0051 83c408                   add        esp, 8
004a0054 85c0                     test       eax, eax
004a0056 7511                     jne        0x4a0069
004a0058 6a0f                     push       0xf
004a005a 6a00                     push       0
004a005c b9404d5c00               mov        ecx, 0x5c4d40
004a0061 e8ea9a0300               call       0x4d9b50
004a0066 90                       nop        
004a0067 eb3b                     jmp        0x4a00a4
004a0069 680c035700               push       0x57030c
004a006e 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a0071 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
004a0074 83c104                   add        ecx, 4
004a0077 51                       push       ecx
004a0078 e893fc0a00               call       0x54fd10
004a007d 83c408                   add        esp, 8
004a0080 85c0                     test       eax, eax
004a0082 7511                     jne        0x4a0095
004a0084 6a10                     push       0x10
004a0086 6a00                     push       0
004a0088 b9404d5c00               mov        ecx, 0x5c4d40
004a008d e8be9a0300               call       0x4d9b50
004a0092 90                       nop        
004a0093 eb0f                     jmp        0x4a00a4
004a0095 6a11                     push       0x11
004a0097 6a00                     push       0
004a0099 b9404d5c00               mov        ecx, 0x5c4d40
004a009e e8ad9a0300               call       0x4d9b50
004a00a3 90                       nop        
004a00a4 e9d3020000               jmp        0x4a037c
004a00a9 51                       push       ecx
004a00aa f30f1005d8c85600         movss      xmm0, dword ptr [0x56c8d8]
004a00b2 f30f110424               movss      dword ptr [esp], xmm0
004a00b7 b9404d5c00               mov        ecx, 0x5c4d40
004a00bc e80f990300               call       0x4d99d0
004a00c1 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a00c4 8b427c                   mov        eax, dword ptr [edx + 0x7c]
004a00c7 83e0fe                   and        eax, 0xfffffffe
004a00ca 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a00cd 89417c                   mov        dword ptr [ecx + 0x7c], eax
004a00d0 e9a7020000               jmp        0x4a037c
004a00d5 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a00db e8300e0000               call       0x4a0f10
004a00e0 85c0                     test       eax, eax
004a00e2 0f853b020000             jne        0x4a0323
004a00e8 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a00eb 8b02                     mov        eax, dword ptr [edx]
004a00ed 50                       push       eax
004a00ee e80df80800               call       0x52f900
004a00f3 83c404                   add        esp, 4
004a00f6 c7459800000000           mov        dword ptr [ebp - 0x68], 0
004a00fd eb09                     jmp        0x4a0108
004a00ff 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004a0102 83c101                   add        ecx, 1
004a0105 894d98                   mov        dword ptr [ebp - 0x68], ecx
004a0108 837d9805                 cmp        dword ptr [ebp - 0x68], 5
004a010c 7d47                     jge        0x4a0155
004a010e 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a0111 83c234                   add        edx, 0x34
004a0114 8995b4feffff             mov        dword ptr [ebp - 0x14c], edx
004a011a 8b4598                   mov        eax, dword ptr [ebp - 0x68]
004a011d 50                       push       eax
004a011e 8b8db4feffff             mov        ecx, dword ptr [ebp - 0x14c]
004a0124 e85744f7ff               call       0x414580
004a0129 8bc8                     mov        ecx, eax
004a012b e8a0fbfaff               call       0x44fcd0
004a0130 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a0133 83c148                   add        ecx, 0x48
004a0136 898db0feffff             mov        dword ptr [ebp - 0x150], ecx
004a013c 8b5598                   mov        edx, dword ptr [ebp - 0x68]
004a013f 52                       push       edx
004a0140 8b8db0feffff             mov        ecx, dword ptr [ebp - 0x150]
004a0146 e83544f7ff               call       0x414580
004a014b 8bc8                     mov        ecx, eax
004a014d e87efbfaff               call       0x44fcd0
004a0152 90                       nop        
004a0153 ebaa                     jmp        0x4a00ff
004a0155 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a0158 059c000000               add        eax, 0x9c
004a015d 898570ffffff             mov        dword ptr [ebp - 0x90], eax
004a0163 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004a0169 e832bcf6ff               call       0x40bda0
004a016e 894590                   mov        dword ptr [ebp - 0x70], eax
004a0171 8b8d70ffffff             mov        ecx, dword ptr [ebp - 0x90]
004a0177 e8847affff               call       0x497c00
004a017c 8985acfeffff             mov        dword ptr [ebp - 0x154], eax
004a0182 eb09                     jmp        0x4a018d
004a0184 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
004a0187 83c104                   add        ecx, 4
004a018a 894d90                   mov        dword ptr [ebp - 0x70], ecx
004a018d 8b5590                   mov        edx, dword ptr [ebp - 0x70]
004a0190 3b95acfeffff             cmp        edx, dword ptr [ebp - 0x154]
004a0196 7413                     je         0x4a01ab
004a0198 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004a019b 8b08                     mov        ecx, dword ptr [eax]
004a019d 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004a01a0 8d4de4                   lea        ecx, [ebp - 0x1c]
004a01a3 e828fbfaff               call       0x44fcd0
004a01a8 90                       nop        
004a01a9 ebd9                     jmp        0x4a0184
004a01ab 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a01ae 833a10                   cmp        dword ptr [edx], 0x10
004a01b1 7408                     je         0x4a01bb
004a01b3 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a01b6 833811                   cmp        dword ptr [eax], 0x11
004a01b9 7530                     jne        0x4a01eb
004a01bb b904000000               mov        ecx, 4
004a01c0 6bd116                   imul       edx, ecx, 0x16
004a01c3 8b82a0fc5a00             mov        eax, dword ptr [edx + 0x5afca0]
004a01c9 50                       push       eax
004a01ca 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a01d0 e85b0d0000               call       0x4a0f30
004a01d5 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a01d8 837da400                 cmp        dword ptr [ebp - 0x5c], 0
004a01dc 7508                     jne        0x4a01e6
004a01de 83c8ff                   or         eax, 0xffffffff
004a01e1 e9db010000               jmp        0x4a03c1
004a01e6 e9db000000               jmp        0x4a02c6
004a01eb b968a55b00               mov        ecx, 0x5ba568
004a01f0 e8ab3efcff               call       0x4640a0
004a01f5 89856cffffff             mov        dword ptr [ebp - 0x94], eax
004a01fb 83bd6cffffff03           cmp        dword ptr [ebp - 0x94], 3
004a0202 770d                     ja         0x4a0211
004a0204 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
004a020a ff248d24044a00           jmp        dword ptr [ecx*4 + 0x4a0424]
004a0211 ba04000000               mov        edx, 4
004a0216 6bc212                   imul       eax, edx, 0x12
004a0219 8b88a0fc5a00             mov        ecx, dword ptr [eax + 0x5afca0]
004a021f 51                       push       ecx
004a0220 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a0226 e8050d0000               call       0x4a0f30
004a022b 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a022e 837da400                 cmp        dword ptr [ebp - 0x5c], 0
004a0232 7508                     jne        0x4a023c
004a0234 83c8ff                   or         eax, 0xffffffff
004a0237 e985010000               jmp        0x4a03c1
004a023c e985000000               jmp        0x4a02c6
004a0241 ba04000000               mov        edx, 4
004a0246 6bc213                   imul       eax, edx, 0x13
004a0249 8b88a0fc5a00             mov        ecx, dword ptr [eax + 0x5afca0]
004a024f 51                       push       ecx
004a0250 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a0256 e8d50c0000               call       0x4a0f30
004a025b 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a025e 837da400                 cmp        dword ptr [ebp - 0x5c], 0
004a0262 7508                     jne        0x4a026c
004a0264 83c8ff                   or         eax, 0xffffffff
004a0267 e955010000               jmp        0x4a03c1
004a026c eb58                     jmp        0x4a02c6
004a026e ba04000000               mov        edx, 4
004a0273 6bc214                   imul       eax, edx, 0x14
004a0276 8b88a0fc5a00             mov        ecx, dword ptr [eax + 0x5afca0]
004a027c 51                       push       ecx
004a027d 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a0283 e8a80c0000               call       0x4a0f30
004a0288 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a028b 837da400                 cmp        dword ptr [ebp - 0x5c], 0
004a028f 7508                     jne        0x4a0299
004a0291 83c8ff                   or         eax, 0xffffffff
004a0294 e928010000               jmp        0x4a03c1
004a0299 eb2b                     jmp        0x4a02c6
004a029b ba04000000               mov        edx, 4
004a02a0 6bc215                   imul       eax, edx, 0x15
004a02a3 8b88a0fc5a00             mov        ecx, dword ptr [eax + 0x5afca0]
004a02a9 51                       push       ecx
004a02aa 8b0de8495c00             mov        ecx, dword ptr [0x5c49e8]
004a02b0 e87b0c0000               call       0x4a0f30
004a02b5 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004a02b8 837da400                 cmp        dword ptr [ebp - 0x5c], 0
004a02bc 7508                     jne        0x4a02c6
004a02be 83c8ff                   or         eax, 0xffffffff
004a02c1 e9fb000000               jmp        0x4a03c1
004a02c6 ba08000000               mov        edx, 8
004a02cb 6bc200                   imul       eax, edx, 0
004a02ce 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004a02d1 8b540104                 mov        edx, dword ptr [ecx + eax + 4]
004a02d5 0355a4                   add        edx, dword ptr [ebp - 0x5c]
004a02d8 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a02db 89505c                   mov        dword ptr [eax + 0x5c], edx
004a02de 6a00                     push       0
004a02e0 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a02e3 83c104                   add        ecx, 4
004a02e6 e83532f8ff               call       0x423520
004a02eb 6a00                     push       0
004a02ed 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a02f0 83c114                   add        ecx, 0x14
004a02f3 e82832f8ff               call       0x423520
004a02f8 6a00                     push       0
004a02fa 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a02fd 83c124                   add        ecx, 0x24
004a0300 e81b32f8ff               call       0x423520
004a0305 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a0308 c78184000000ffffff00     mov        dword ptr [ecx + 0x84], 0xffffff
004a0312 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a0315 8b427c                   mov        eax, dword ptr [edx + 0x7c]
004a0318 83c802                   or         eax, 2
004a031b 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a031e 89417c                   mov        dword ptr [ecx + 0x7c], eax
004a0321 eb08                     jmp        0x4a032b
004a0323 83c8ff                   or         eax, 0xffffffff
004a0326 e996000000               jmp        0x4a03c1
004a032b e9e4f0ffff               jmp        0x49f414
004a0330 6a6d                     push       0x6d
004a0332 6a00                     push       0
004a0334 6a00                     push       0
004a0336 6a00                     push       0
004a0338 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a033b 8b425c                   mov        eax, dword ptr [edx + 0x5c]
004a033e b904000000               mov        ecx, 4
004a0343 6bd100                   imul       edx, ecx, 0
004a0346 8b441004                 mov        eax, dword ptr [eax + edx + 4]
004a034a 50                       push       eax
004a034b 6a00                     push       0
004a034d e86e4df8ff               call       0x4250c0
004a0352 83c418                   add        esp, 0x18
004a0355 eb25                     jmp        0x4a037c
004a0357 6a6d                     push       0x6d
004a0359 6a00                     push       0
004a035b 6a00                     push       0
004a035d 6a00                     push       0
004a035f 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a0362 8b515c                   mov        edx, dword ptr [ecx + 0x5c]
004a0365 b804000000               mov        eax, 4
004a036a 6bc800                   imul       ecx, eax, 0
004a036d 8b540a04                 mov        edx, dword ptr [edx + ecx + 4]
004a0371 52                       push       edx
004a0372 6a05                     push       5
004a0374 e8474df8ff               call       0x4250c0
004a0379 83c418                   add        esp, 0x18
004a037c 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a037f 8b485c                   mov        ecx, dword ptr [eax + 0x5c]
004a0382 ba04000000               mov        edx, 4
004a0387 6bc200                   imul       eax, edx, 0
004a038a 8d4c0104                 lea        ecx, [ecx + eax + 4]
004a038e 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
004a0391 8b425c                   mov        eax, dword ptr [edx + 0x5c]
004a0394 0fb65003                 movzx      edx, byte ptr [eax + 3]
004a0398 03ca                     add        ecx, edx
004a039a 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004a039d 89485c                   mov        dword ptr [eax + 0x5c], ecx
004a03a0 e96ff0ffff               jmp        0x49f414
004a03a5 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004a03a8 83c114                   add        ecx, 0x14
004a03ab 898da4feffff             mov        dword ptr [ebp - 0x15c], ecx
004a03b1 6a00                     push       0
004a03b3 8b8da4feffff             mov        ecx, dword ptr [ebp - 0x15c]
004a03b9 e88231f8ff               call       0x423540
004a03be 90                       nop        
004a03bf 33c0                     xor        eax, eax
004a03c1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a03c4 64890d00000000           mov        dword ptr fs:[0], ecx
004a03cb 59                       pop        ecx
004a03cc 5e                       pop        esi
004a03cd 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a03d0 33cd                     xor        ecx, ebp
004a03d2 e817260a00               call       0x5429ee
004a03d7 8be5                     mov        esp, ebp
004a03d9 5d                       pop        ebp
004a03da c3                       ret        
004a03db 90                       nop        
004a03dc 4d                       dec        ebp
004a03dd f4                       hlt        
004a03de 49                       dec        ecx
004a03df 004df4                   add        byte ptr [ebp - 0xc], cl
004a03e2 49                       dec        ecx
004a03e3 004df4                   add        byte ptr [ebp - 0xc], cl
004a03e6 49                       dec        ecx
004a03e7 0055f4                   add        byte ptr [ebp - 0xc], dl
004a03ea 49                       dec        ecx
004a03eb 003cf7                   add        byte ptr [edi + esi*8], bh
004a03ee 49                       dec        ecx
004a03ef 00a0f7490064             add        byte ptr [eax + 0x640049f7], ah
004a03f5 f9                       stc        
004a03f6 49                       dec        ecx
004a03f7 0049fb                   add        byte ptr [ecx - 5], cl
004a03fa 49                       dec        ecx
004a03fb 002e                     add        byte ptr [esi], ch
004a03fd fc                       cld        
004a03fe 49                       dec        ecx
004a03ff 00eb                     add        bl, ch
004a0401 ff4900                   dec        dword ptr [ecx]
004a0404 26004a00                 add        byte ptr es:[edx], cl
004a0408 a9004a00d5               test       eax, 0xd5004a00
004a040d 004a00                   add        byte ptr [edx], cl
004a0410 3003                     xor        byte ptr [ebx], al
004a0412 4a                       dec        edx
004a0413 005703                   add        byte ptr [edi + 3], dl
004a0416 4a                       dec        edx
004a0417 000f                     add        byte ptr [edi], cl
004a0419 fd                       std        
004a041a 49                       dec        ecx
004a041b 0003                     add        byte ptr [ebx], al
004a041d fe4900                   dec        byte ptr [ecx]
004a0420 f7fe                     idiv       esi
004a0422 49                       dec        ecx
004a0423 0011                     add        byte ptr [ecx], dl
004a0425 024a00                   add        cl, byte ptr [edx]
004a0428 41                       inc        ecx
004a0429 024a00                   add        cl, byte ptr [edx]
004a042c 6e                       outsb      dx, byte ptr [esi]
004a042d 024a00                   add        cl, byte ptr [edx]
004a0430 9b                       wait       
004a0431 024a00                   add        cl, byte ptr [edx]
004a0434 cc                       int3       
004a0435 cc                       int3       
004a0436 cc                       int3       
004a0437 cc                       int3       
004a0438 cc                       int3       
004a0439 cc                       int3       
004a043a cc                       int3       
004a043b cc                       int3       
004a043c cc                       int3       
004a043d cc                       int3       
004a043e cc                       int3       
004a043f cc                       int3       
