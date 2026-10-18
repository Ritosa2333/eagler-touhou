0044efc0 55                       push       ebp
0044efc1 8bec                     mov        ebp, esp
0044efc3 6aff                     push       -1
0044efc5 68fd855600               push       0x5685fd
0044efca 64a100000000             mov        eax, dword ptr fs:[0]
0044efd0 50                       push       eax
0044efd1 83ec28                   sub        esp, 0x28
0044efd4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0044efd9 33c5                     xor        eax, ebp
0044efdb 8945f0                   mov        dword ptr [ebp - 0x10], eax
0044efde 50                       push       eax
0044efdf 8d45f4                   lea        eax, [ebp - 0xc]
0044efe2 64a300000000             mov        dword ptr fs:[0], eax
0044efe8 894dd8                   mov        dword ptr [ebp - 0x28], ecx
0044efeb 8b4508                   mov        eax, dword ptr [ebp + 8]
0044efee 50                       push       eax
0044efef 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0044eff2 e809ddffff               call       0x44cd00
0044eff7 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0044effa 837de400                 cmp        dword ptr [ebp - 0x1c], 0
0044effe 7505                     jne        0x44f005
0044f000 e9a5000000               jmp        0x44f0aa
0044f005 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0044f008 81c114050000             add        ecx, 0x514
0044f00e 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
0044f011 6a01                     push       1
0044f013 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0044f016 e895a4ffff               call       0x4494b0
0044f01b 8945e0                   mov        dword ptr [ebp - 0x20], eax
0044f01e 6a08                     push       8
0044f020 8d4de8                   lea        ecx, [ebp - 0x18]
0044f023 e858d0fbff               call       0x40c080
0044f028 8d55e8                   lea        edx, [ebp - 0x18]
0044f02b 52                       push       edx
0044f02c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044f02f e8bcc9ffff               call       0x44b9f0
0044f034 c745fc00000000           mov        dword ptr [ebp - 4], 0
0044f03b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0044f03e e8fd34fcff               call       0x412540
0044f043 8945d0                   mov        dword ptr [ebp - 0x30], eax
0044f046 eb09                     jmp        0x44f051
0044f048 8d4de8                   lea        ecx, [ebp - 0x18]
0044f04b e8e02bfcff               call       0x411c30
0044f050 90                       nop        
0044f051 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0044f054 50                       push       eax
0044f055 8d4de8                   lea        ecx, [ebp - 0x18]
0044f058 e8532bfcff               call       0x411bb0
0044f05d 0fb6c8                   movzx      ecx, al
0044f060 85c9                     test       ecx, ecx
0044f062 7436                     je         0x44f09a
0044f064 8d4de8                   lea        ecx, [ebp - 0x18]
0044f067 e894d2fbff               call       0x40c300
0044f06c 8945dc                   mov        dword ptr [ebp - 0x24], eax
0044f06f 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044f072 e8e900fcff               call       0x40f160
0044f077 8b10                     mov        edx, dword ptr [eax]
0044f079 8955cc                   mov        dword ptr [ebp - 0x34], edx
0044f07c 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0044f07f 50                       push       eax
0044f080 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0044f083 e86898feff               call       0x4388f0
0044f088 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0044f08b e8d000fcff               call       0x40f160
0044f090 8b08                     mov        ecx, dword ptr [eax]
0044f092 e839c5fdff               call       0x42b5d0
0044f097 90                       nop        
0044f098 ebae                     jmp        0x44f048
0044f09a c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0044f0a1 8d4de8                   lea        ecx, [ebp - 0x18]
0044f0a4 e8572afcff               call       0x411b00
0044f0a9 90                       nop        
0044f0aa 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0044f0ad 64890d00000000           mov        dword ptr fs:[0], ecx
0044f0b4 59                       pop        ecx
0044f0b5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0044f0b8 33cd                     xor        ecx, ebp
0044f0ba e82f390f00               call       0x5429ee
0044f0bf 8be5                     mov        esp, ebp
0044f0c1 5d                       pop        ebp
0044f0c2 c20800                   ret        8
0044f0c5 cc                       int3       
0044f0c6 cc                       int3       
0044f0c7 cc                       int3       
0044f0c8 cc                       int3       
0044f0c9 cc                       int3       
0044f0ca cc                       int3       
0044f0cb cc                       int3       
0044f0cc cc                       int3       
0044f0cd cc                       int3       
0044f0ce cc                       int3       
0044f0cf cc                       int3       
