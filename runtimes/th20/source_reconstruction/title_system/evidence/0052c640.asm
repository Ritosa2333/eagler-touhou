0052c640 55                       push       ebp
0052c641 8bec                     mov        ebp, esp
0052c643 6aff                     push       -1
0052c645 688d805600               push       0x56808d
0052c64a 64a100000000             mov        eax, dword ptr fs:[0]
0052c650 50                       push       eax
0052c651 83ec14                   sub        esp, 0x14
0052c654 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052c659 33c5                     xor        eax, ebp
0052c65b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0052c65e 50                       push       eax
0052c65f 8d45f4                   lea        eax, [ebp - 0xc]
0052c662 64a300000000             mov        dword ptr fs:[0], eax
0052c668 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0052c66b 6a14                     push       0x14
0052c66d b940025c00               mov        ecx, 0x5c0240
0052c672 e899fcedff               call       0x40c310
0052c677 50                       push       eax
0052c678 8d4dec                   lea        ecx, [ebp - 0x14]
0052c67b e8b0f0edff               call       0x40b730
0052c680 c745fc00000000           mov        dword ptr [ebp - 4], 0
0052c687 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c68a e82179f3ff               call       0x463fb0
0052c68f 90                       nop        
0052c690 6a00                     push       0
0052c692 8b4508                   mov        eax, dword ptr [ebp + 8]
0052c695 50                       push       eax
0052c696 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c699 e852feffff               call       0x52c4f0
0052c69e 85c0                     test       eax, eax
0052c6a0 741b                     je         0x52c6bd
0052c6a2 6a01                     push       1
0052c6a4 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052c6a7 51                       push       ecx
0052c6a8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052c6ab e840feffff               call       0x52c4f0
0052c6b0 85c0                     test       eax, eax
0052c6b2 7409                     je         0x52c6bd
0052c6b4 c745e001000000           mov        dword ptr [ebp - 0x20], 1
0052c6bb eb07                     jmp        0x52c6c4
0052c6bd c745e000000000           mov        dword ptr [ebp - 0x20], 0
0052c6c4 8a55e0                   mov        dl, byte ptr [ebp - 0x20]
0052c6c7 8855eb                   mov        byte ptr [ebp - 0x15], dl
0052c6ca c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0052c6d1 8d4dec                   lea        ecx, [ebp - 0x14]
0052c6d4 e827f2edff               call       0x40b900
0052c6d9 0fb645eb                 movzx      eax, byte ptr [ebp - 0x15]
0052c6dd 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052c6e0 64890d00000000           mov        dword ptr fs:[0], ecx
0052c6e7 59                       pop        ecx
0052c6e8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052c6eb 33cd                     xor        ecx, ebp
0052c6ed e8fc620100               call       0x5429ee
0052c6f2 8be5                     mov        esp, ebp
0052c6f4 5d                       pop        ebp
0052c6f5 c20400                   ret        4
0052c6f8 cc                       int3       
0052c6f9 cc                       int3       
0052c6fa cc                       int3       
0052c6fb cc                       int3       
0052c6fc cc                       int3       
0052c6fd cc                       int3       
0052c6fe cc                       int3       
0052c6ff cc                       int3       
