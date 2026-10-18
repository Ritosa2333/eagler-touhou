0044fa70 55                       push       ebp
0044fa71 8bec                     mov        ebp, esp
0044fa73 6aff                     push       -1
0044fa75 683d865600               push       0x56863d
0044fa7a 64a100000000             mov        eax, dword ptr fs:[0]
0044fa80 50                       push       eax
0044fa81 83ec24                   sub        esp, 0x24
0044fa84 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044fa89 33c5                     xor        eax, ebp
0044fa8b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044fa8e 50                       push       eax
0044fa8f 8d45f4                   lea        eax, [ebp - 0xc]
0044fa92 64a300000000             mov        dword ptr fs:[0], eax
0044fa98 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0044fa9b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0044fa9e 8b889c040000             mov        ecx, dword ptr [eax + 0x49c]
0044faa4 83c901                   or         ecx, 1
0044faa7 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0044faaa 898a9c040000             mov        dword ptr [edx + 0x49c], ecx
0044fab0 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0044fab3 0514050000               add        eax, 0x514
0044fab8 8945dc                   mov        dword ptr [ebp - 0x24], eax
0044fabb 6a01                     push       1
0044fabd 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044fac0 e8eb99ffff               call       0x4494b0
0044fac5 8bc8                     mov        ecx, eax
0044fac7 e8c404fcff               call       0x40ff90
0044facc 85c0                     test       eax, eax
0044face 0f8493000000             je         0x44fb67
0044fad4 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044fad7 81c114050000             add        ecx, 0x514
0044fadd 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0044fae0 6a01                     push       1
0044fae2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0044fae5 e8c699ffff               call       0x4494b0
0044faea 8bc8                     mov        ecx, eax
0044faec e89f04fcff               call       0x40ff90
0044faf1 8945e0                   mov        dword ptr [ebp - 0x20], eax
0044faf4 6a08                     push       8
0044faf6 8d4de8                   lea        ecx, [ebp - 0x18]
0044faf9 e882c5fbff               call       0x40c080
0044fafe 8d55e8                   lea        edx, [ebp - 0x18]
0044fb01 52                       push       edx
0044fb02 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044fb05 e8e6beffff               call       0x44b9f0
0044fb0a c745fc00000000           mov        dword ptr [ebp - 4], 0
0044fb11 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044fb14 e8272afcff               call       0x412540
0044fb19 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0044fb1c eb09                     jmp        0x44fb27
0044fb1e 8d4de8                   lea        ecx, [ebp - 0x18]
0044fb21 e80a21fcff               call       0x411c30
0044fb26 90                       nop        
0044fb27 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
0044fb2a 50                       push       eax
0044fb2b 8d4de8                   lea        ecx, [ebp - 0x18]
0044fb2e e87d20fcff               call       0x411bb0
0044fb33 0fb6c8                   movzx      ecx, al
0044fb36 85c9                     test       ecx, ecx
0044fb38 741d                     je         0x44fb57
0044fb3a 8d4de8                   lea        ecx, [ebp - 0x18]
0044fb3d e8bec7fbff               call       0x40c300
0044fb42 8945d0                   mov        dword ptr [ebp - 0x30], eax
0044fb45 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0044fb48 e813f6fbff               call       0x40f160
0044fb4d 8b08                     mov        ecx, dword ptr [eax]
0044fb4f e81cffffff               call       0x44fa70
0044fb54 90                       nop        
0044fb55 ebc7                     jmp        0x44fb1e
0044fb57 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044fb5e 8d4de8                   lea        ecx, [ebp - 0x18]
0044fb61 e89a1ffcff               call       0x411b00
0044fb66 90                       nop        
0044fb67 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044fb6a 64890d00000000           mov        dword ptr fs:[0], ecx
0044fb71 59                       pop        ecx
0044fb72 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044fb75 33cd                     xor        ecx, ebp
0044fb77 e8722e0f00               call       0x5429ee
0044fb7c 8be5                     mov        esp, ebp
0044fb7e 5d                       pop        ebp
0044fb7f c3                       ret        
