0051d230 55                       push       ebp
0051d231 8bec                     mov        ebp, esp
0051d233 6aff                     push       -1
0051d235 685d755600               push       0x56755d
0051d23a 64a100000000             mov        eax, dword ptr fs:[0]
0051d240 50                       push       eax
0051d241 83ec1c                   sub        esp, 0x1c
0051d244 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051d249 33c5                     xor        eax, ebp
0051d24b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051d24e 50                       push       eax
0051d24f 8d45f4                   lea        eax, [ebp - 0xc]
0051d252 64a300000000             mov        dword ptr fs:[0], eax
0051d258 894de8                   mov        dword ptr [ebp - 0x18], ecx
0051d25b 6a06                     push       6
0051d25d b940025c00               mov        ecx, 0x5c0240
0051d262 e8a9f0eeff               call       0x40c310
0051d267 50                       push       eax
0051d268 8d4dec                   lea        ecx, [ebp - 0x14]
0051d26b e8c0e4eeff               call       0x40b730
0051d270 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051d277 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051d27a e8e1e9eeff               call       0x40bc60
0051d27f 6a00                     push       0
0051d281 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051d284 83c10c                   add        ecx, 0xc
0051d287 e814e8eeff               call       0x40baa0
0051d28c 90                       nop        
0051d28d 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0051d290 50                       push       eax
0051d291 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051d294 51                       push       ecx
0051d295 8d4dd8                   lea        ecx, [ebp - 0x28]
0051d298 e8f3feffff               call       0x51d190
0051d29d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0051d2a0 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
0051d2a3 52                       push       edx
0051d2a4 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0051d2a7 e814e8eeff               call       0x40bac0
0051d2ac 8d4dd8                   lea        ecx, [ebp - 0x28]
0051d2af e81ce7eeff               call       0x40b9d0
0051d2b4 90                       nop        
0051d2b5 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051d2bc 8d4dec                   lea        ecx, [ebp - 0x14]
0051d2bf e83ce6eeff               call       0x40b900
0051d2c4 90                       nop        
0051d2c5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051d2c8 64890d00000000           mov        dword ptr fs:[0], ecx
0051d2cf 59                       pop        ecx
0051d2d0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051d2d3 33cd                     xor        ecx, ebp
0051d2d5 e814570200               call       0x5429ee
0051d2da 8be5                     mov        esp, ebp
0051d2dc 5d                       pop        ebp
0051d2dd c20800                   ret        8
