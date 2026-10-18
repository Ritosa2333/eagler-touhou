0044fb80 55                       push       ebp
0044fb81 8bec                     mov        ebp, esp
0044fb83 6aff                     push       -1
0044fb85 683d865600               push       0x56863d
0044fb8a 64a100000000             mov        eax, dword ptr fs:[0]
0044fb90 50                       push       eax
0044fb91 83ec24                   sub        esp, 0x24
0044fb94 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044fb99 33c5                     xor        eax, ebp
0044fb9b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044fb9e 50                       push       eax
0044fb9f 8d45f4                   lea        eax, [ebp - 0xc]
0044fba2 64a300000000             mov        dword ptr fs:[0], eax
0044fba8 894dd0                   mov        dword ptr [ebp - 0x30], ecx
0044fbab 837d0800                 cmp        dword ptr [ebp + 8], 0
0044fbaf 7505                     jne        0x44fbb6
0044fbb1 e9c9000000               jmp        0x44fc7f
0044fbb6 8b4508                   mov        eax, dword ptr [ebp + 8]
0044fbb9 c7807005000001000000     mov        dword ptr [eax + 0x570], 1
0044fbc3 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044fbc6 81c114050000             add        ecx, 0x514
0044fbcc 894de0                   mov        dword ptr [ebp - 0x20], ecx
0044fbcf 6a01                     push       1
0044fbd1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044fbd4 e8d798ffff               call       0x4494b0
0044fbd9 8bc8                     mov        ecx, eax
0044fbdb e8b003fcff               call       0x40ff90
0044fbe0 85c0                     test       eax, eax
0044fbe2 0f8497000000             je         0x44fc7f
0044fbe8 8b5508                   mov        edx, dword ptr [ebp + 8]
0044fbeb 81c214050000             add        edx, 0x514
0044fbf1 8955dc                   mov        dword ptr [ebp - 0x24], edx
0044fbf4 6a01                     push       1
0044fbf6 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044fbf9 e8b298ffff               call       0x4494b0
0044fbfe 8bc8                     mov        ecx, eax
0044fc00 e88b03fcff               call       0x40ff90
0044fc05 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0044fc08 6a08                     push       8
0044fc0a 8d4de8                   lea        ecx, [ebp - 0x18]
0044fc0d e86ec4fbff               call       0x40c080
0044fc12 8d45e8                   lea        eax, [ebp - 0x18]
0044fc15 50                       push       eax
0044fc16 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044fc19 e8d2bdffff               call       0x44b9f0
0044fc1e c745fc00000000           mov        dword ptr [ebp - 4], 0
0044fc25 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044fc28 e81329fcff               call       0x412540
0044fc2d 8945d8                   mov        dword ptr [ebp - 0x28], eax
0044fc30 eb09                     jmp        0x44fc3b
0044fc32 8d4de8                   lea        ecx, [ebp - 0x18]
0044fc35 e8f61ffcff               call       0x411c30
0044fc3a 90                       nop        
0044fc3b 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0044fc3e 51                       push       ecx
0044fc3f 8d4de8                   lea        ecx, [ebp - 0x18]
0044fc42 e8691ffcff               call       0x411bb0
0044fc47 0fb6d0                   movzx      edx, al
0044fc4a 85d2                     test       edx, edx
0044fc4c 7421                     je         0x44fc6f
0044fc4e 8d4de8                   lea        ecx, [ebp - 0x18]
0044fc51 e8aac6fbff               call       0x40c300
0044fc56 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0044fc59 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0044fc5c e8fff4fbff               call       0x40f160
0044fc61 8b00                     mov        eax, dword ptr [eax]
0044fc63 50                       push       eax
0044fc64 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0044fc67 e814ffffff               call       0x44fb80
0044fc6c 90                       nop        
0044fc6d ebc3                     jmp        0x44fc32
0044fc6f c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044fc76 8d4de8                   lea        ecx, [ebp - 0x18]
0044fc79 e8821efcff               call       0x411b00
0044fc7e 90                       nop        
0044fc7f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044fc82 64890d00000000           mov        dword ptr fs:[0], ecx
0044fc89 59                       pop        ecx
0044fc8a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044fc8d 33cd                     xor        ecx, ebp
0044fc8f e85a2d0f00               call       0x5429ee
0044fc94 8be5                     mov        esp, ebp
0044fc96 5d                       pop        ebp
0044fc97 c20400                   ret        4
0044fc9a cc                       int3       
0044fc9b cc                       int3       
0044fc9c cc                       int3       
0044fc9d cc                       int3       
0044fc9e cc                       int3       
0044fc9f cc                       int3       
