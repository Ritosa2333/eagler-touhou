0047d600 55                       push       ebp
0047d601 8bec                     mov        ebp, esp
0047d603 6aff                     push       -1
0047d605 68cd835600               push       0x5683cd
0047d60a 64a100000000             mov        eax, dword ptr fs:[0]
0047d610 50                       push       eax
0047d611 83ec34                   sub        esp, 0x34
0047d614 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047d619 33c5                     xor        eax, ebp
0047d61b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047d61e 50                       push       eax
0047d61f 8d45f4                   lea        eax, [ebp - 0xc]
0047d622 64a300000000             mov        dword ptr fs:[0], eax
0047d628 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0047d62b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d62e 8b889c6d2800             mov        ecx, dword ptr [eax + 0x286d9c]
0047d634 51                       push       ecx
0047d635 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0047d63b e860a0ffff               call       0x4776a0
0047d640 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d643 c7424400000000           mov        dword ptr [edx + 0x44], 0
0047d64a b804000000               mov        eax, 4
0047d64f 6bc805                   imul       ecx, eax, 5
0047d652 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d655 c7440a1400000000         mov        dword ptr [edx + ecx + 0x14], 0
0047d65d b804000000               mov        eax, 4
0047d662 c1e002                   shl        eax, 2
0047d665 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d668 c744011400000000         mov        dword ptr [ecx + eax + 0x14], 0
0047d670 ba04000000               mov        edx, 4
0047d675 6bc203                   imul       eax, edx, 3
0047d678 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d67b c744011400000000         mov        dword ptr [ecx + eax + 0x14], 0
0047d683 ba04000000               mov        edx, 4
0047d688 d1e2                     shl        edx, 1
0047d68a 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d68d c744101400000000         mov        dword ptr [eax + edx + 0x14], 0
0047d695 b904000000               mov        ecx, 4
0047d69a c1e100                   shl        ecx, 0
0047d69d 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d6a0 c7440a1400000000         mov        dword ptr [edx + ecx + 0x14], 0
0047d6a8 b804000000               mov        eax, 4
0047d6ad 6bc800                   imul       ecx, eax, 0
0047d6b0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d6b3 c7440a1400000000         mov        dword ptr [edx + ecx + 0x14], 0
0047d6bb b804000000               mov        eax, 4
0047d6c0 6bc805                   imul       ecx, eax, 5
0047d6c3 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d6c6 c7440a2c00000000         mov        dword ptr [edx + ecx + 0x2c], 0
0047d6ce b804000000               mov        eax, 4
0047d6d3 c1e002                   shl        eax, 2
0047d6d6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d6d9 c744012c00000000         mov        dword ptr [ecx + eax + 0x2c], 0
0047d6e1 ba04000000               mov        edx, 4
0047d6e6 6bc203                   imul       eax, edx, 3
0047d6e9 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d6ec c744012c00000000         mov        dword ptr [ecx + eax + 0x2c], 0
0047d6f4 ba04000000               mov        edx, 4
0047d6f9 d1e2                     shl        edx, 1
0047d6fb 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d6fe c744102c00000000         mov        dword ptr [eax + edx + 0x2c], 0
0047d706 b904000000               mov        ecx, 4
0047d70b c1e100                   shl        ecx, 0
0047d70e 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d711 c7440a2c00000000         mov        dword ptr [edx + ecx + 0x2c], 0
0047d719 b804000000               mov        eax, 4
0047d71e 6bc800                   imul       ecx, eax, 0
0047d721 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d724 c7440a2c00000000         mov        dword ptr [edx + ecx + 0x2c], 0
0047d72c 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d72f 05646d2800               add        eax, 0x286d64
0047d734 8945dc                   mov        dword ptr [ebp - 0x24], eax
0047d737 6a08                     push       8
0047d739 8d4de8                   lea        ecx, [ebp - 0x18]
0047d73c e83fe9f8ff               call       0x40c080
0047d741 8d4de8                   lea        ecx, [ebp - 0x18]
0047d744 51                       push       ecx
0047d745 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0047d748 e8334bf9ff               call       0x412280
0047d74d c745fc00000000           mov        dword ptr [ebp - 4], 0
0047d754 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0047d757 e8e44df9ff               call       0x412540
0047d75c 8945d8                   mov        dword ptr [ebp - 0x28], eax
0047d75f eb09                     jmp        0x47d76a
0047d761 8d4de8                   lea        ecx, [ebp - 0x18]
0047d764 e8c744f9ff               call       0x411c30
0047d769 90                       nop        
0047d76a 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0047d76d 52                       push       edx
0047d76e 8d4de8                   lea        ecx, [ebp - 0x18]
0047d771 e83a44f9ff               call       0x411bb0
0047d776 0fb6c0                   movzx      eax, al
0047d779 85c0                     test       eax, eax
0047d77b 0f84b0010000             je         0x47d931
0047d781 8d4de8                   lea        ecx, [ebp - 0x18]
0047d784 e877ebf8ff               call       0x40c300
0047d789 8945e0                   mov        dword ptr [ebp - 0x20], eax
0047d78c 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
0047d793 7419                     je         0x47d7ae
0047d795 8b0d28a85b00             mov        ecx, dword ptr [0x5ba828]
0047d79b 8b91e8000000             mov        edx, dword ptr [ecx + 0xe8]
0047d7a1 c1ea0b                   shr        edx, 0xb
0047d7a4 83e201                   and        edx, 1
0047d7a7 7405                     je         0x47d7ae
0047d7a9 e98a000000               jmp        0x47d838
0047d7ae 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d7b1 e8aa19f9ff               call       0x40f160
0047d7b6 8b00                     mov        eax, dword ptr [eax]
0047d7b8 8b4814                   mov        ecx, dword ptr [eax + 0x14]
0047d7bb c1e908                   shr        ecx, 8
0047d7be 83e101                   and        ecx, 1
0047d7c1 745d                     je         0x47d820
0047d7c3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d7c6 e89519f9ff               call       0x40f160
0047d7cb 8b10                     mov        edx, dword ptr [eax]
0047d7cd 837a5002                 cmp        dword ptr [edx + 0x50], 2
0047d7d1 7523                     jne        0x47d7f6
0047d7d3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d7d6 e88519f9ff               call       0x40f160
0047d7db 8b00                     mov        eax, dword ptr [eax]
0047d7dd 05f8040000               add        eax, 0x4f8
0047d7e2 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0047d7e5 6a08                     push       8
0047d7e7 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0047d7ea e8d15dfaff               call       0x4235c0
0047d7ef 0fb6c8                   movzx      ecx, al
0047d7f2 85c9                     test       ecx, ecx
0047d7f4 7510                     jne        0x47d806
0047d7f6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d7f9 e86219f9ff               call       0x40f160
0047d7fe 8b10                     mov        edx, dword ptr [eax]
0047d800 837a5001                 cmp        dword ptr [edx + 0x50], 1
0047d804 751a                     jne        0x47d820
0047d806 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d809 e85219f9ff               call       0x40f160
0047d80e 8b00                     mov        eax, dword ptr [eax]
0047d810 8945d0                   mov        dword ptr [ebp - 0x30], eax
0047d813 6a01                     push       1
0047d815 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0047d818 e803720000               call       0x484a20
0047d81d 90                       nop        
0047d81e eb18                     jmp        0x47d838
0047d820 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d823 e83819f9ff               call       0x40f160
0047d828 8b08                     mov        ecx, dword ptr [eax]
0047d82a e831010000               call       0x47d960
0047d82f 85c0                     test       eax, eax
0047d831 7405                     je         0x47d838
0047d833 e929ffffff               jmp        0x47d761
0047d838 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d83b e82019f9ff               call       0x40f160
0047d840 8b08                     mov        ecx, dword ptr [eax]
0047d842 8b5114                   mov        edx, dword ptr [ecx + 0x14]
0047d845 c1ea09                   shr        edx, 9
0047d848 83e201                   and        edx, 1
0047d84b 0f85ae000000             jne        0x47d8ff
0047d851 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d854 e80719f9ff               call       0x40f160
0047d859 8b00                     mov        eax, dword ptr [eax]
0047d85b 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047d85e 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d861 837c8a1400               cmp        dword ptr [edx + ecx*4 + 0x14], 0
0047d866 743e                     je         0x47d8a6
0047d868 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d86b e8f018f9ff               call       0x40f160
0047d870 8b00                     mov        eax, dword ptr [eax]
0047d872 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047d875 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d878 837c8a2c00               cmp        dword ptr [edx + ecx*4 + 0x2c], 0
0047d87d 7425                     je         0x47d8a4
0047d87f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d882 e879eaf8ff               call       0x40c300
0047d887 8945cc                   mov        dword ptr [ebp - 0x34], eax
0047d88a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d88d e8ce18f9ff               call       0x40f160
0047d892 8b00                     mov        eax, dword ptr [eax]
0047d894 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047d897 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d89a 8b448a2c                 mov        eax, dword ptr [edx + ecx*4 + 0x2c]
0047d89e 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0047d8a1 894860                   mov        dword ptr [eax + 0x60], ecx
0047d8a4 eb24                     jmp        0x47d8ca
0047d8a6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d8a9 e8b218f9ff               call       0x40f160
0047d8ae 8b10                     mov        edx, dword ptr [eax]
0047d8b0 8955c8                   mov        dword ptr [ebp - 0x38], edx
0047d8b3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d8b6 e8a518f9ff               call       0x40f160
0047d8bb 8b00                     mov        eax, dword ptr [eax]
0047d8bd 8b4844                   mov        ecx, dword ptr [eax + 0x44]
0047d8c0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0047d8c3 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
0047d8c6 89448a14                 mov        dword ptr [edx + ecx*4 + 0x14], eax
0047d8ca 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d8cd e88e18f9ff               call       0x40f160
0047d8d2 8b08                     mov        ecx, dword ptr [eax]
0047d8d4 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
0047d8d7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d8da e88118f9ff               call       0x40f160
0047d8df 8b10                     mov        edx, dword ptr [eax]
0047d8e1 8b4244                   mov        eax, dword ptr [edx + 0x44]
0047d8e4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d8e7 8b55c4                   mov        edx, dword ptr [ebp - 0x3c]
0047d8ea 8954812c                 mov        dword ptr [ecx + eax*4 + 0x2c], edx
0047d8ee 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d8f1 e86a18f9ff               call       0x40f160
0047d8f6 8b00                     mov        eax, dword ptr [eax]
0047d8f8 c7406000000000           mov        dword ptr [eax + 0x60], 0
0047d8ff 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0047d902 8b5144                   mov        edx, dword ptr [ecx + 0x44]
0047d905 83c201                   add        edx, 1
0047d908 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0047d90b 895044                   mov        dword ptr [eax + 0x44], edx
0047d90e 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047d911 e84a18f9ff               call       0x40f160
0047d916 8b08                     mov        ecx, dword ptr [eax]
0047d918 81c1f8040000             add        ecx, 0x4f8
0047d91e 894dc0                   mov        dword ptr [ebp - 0x40], ecx
0047d921 6a00                     push       0
0047d923 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0047d926 e8155cfaff               call       0x423540
0047d92b 90                       nop        
0047d92c e930feffff               jmp        0x47d761
0047d931 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047d938 8d4de8                   lea        ecx, [ebp - 0x18]
0047d93b e8c041f9ff               call       0x411b00
0047d940 90                       nop        
0047d941 b801000000               mov        eax, 1
0047d946 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047d949 64890d00000000           mov        dword ptr fs:[0], ecx
0047d950 59                       pop        ecx
0047d951 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047d954 33cd                     xor        ecx, ebp
0047d956 e893500c00               call       0x5429ee
0047d95b 8be5                     mov        esp, ebp
0047d95d 5d                       pop        ebp
0047d95e c3                       ret        
0047d95f cc                       int3       
