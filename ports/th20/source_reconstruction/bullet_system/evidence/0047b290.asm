0047b290 55                       push       ebp
0047b291 8bec                     mov        ebp, esp
0047b293 6aff                     push       -1
0047b295 684d905600               push       0x56904d
0047b29a 64a100000000             mov        eax, dword ptr fs:[0]
0047b2a0 50                       push       eax
0047b2a1 83ec1c                   sub        esp, 0x1c
0047b2a4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0047b2a9 33c5                     xor        eax, ebp
0047b2ab 8945f0                   mov        dword ptr [ebp - 0x10], eax
0047b2ae 50                       push       eax
0047b2af 8d45f4                   lea        eax, [ebp - 0xc]
0047b2b2 64a300000000             mov        dword ptr fs:[0], eax
0047b2b8 c745e000000000           mov        dword ptr [ebp - 0x20], 0
0047b2bf 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0047b2c2 50                       push       eax
0047b2c3 8d4def                   lea        ecx, [ebp - 0x11]
0047b2c6 e855fef8ff               call       0x40b120
0047b2cb 8d4def                   lea        ecx, [ebp - 0x11]
0047b2ce 51                       push       ecx
0047b2cf 8d4de4                   lea        ecx, [ebp - 0x1c]
0047b2d2 e8798cf9ff               call       0x413f50
0047b2d7 c745fc00000000           mov        dword ptr [ebp - 4], 0
0047b2de 8d4de4                   lea        ecx, [ebp - 0x1c]
0047b2e1 e80a900000               call       0x4842f0
0047b2e6 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0047b2e9 52                       push       edx
0047b2ea 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0047b2ed 50                       push       eax
0047b2ee e8bdf7ffff               call       0x47aab0
0047b2f3 83c408                   add        esp, 8
0047b2f6 6a08                     push       8
0047b2f8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047b2fb e8800df9ff               call       0x40c080
0047b300 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047b303 e878050000               call       0x47b880
0047b308 8945d8                   mov        dword ptr [ebp - 0x28], eax
0047b30b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047b30e 83c901                   or         ecx, 1
0047b311 894de0                   mov        dword ptr [ebp - 0x20], ecx
0047b314 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0047b317 83c20c                   add        edx, 0xc
0047b31a 52                       push       edx
0047b31b e860fff8ff               call       0x40b280
0047b320 83c404                   add        esp, 4
0047b323 8945dc                   mov        dword ptr [ebp - 0x24], eax
0047b326 8d4de4                   lea        ecx, [ebp - 0x1c]
0047b329 e8f2930000               call       0x484720
0047b32e 50                       push       eax
0047b32f e84cfff8ff               call       0x40b280
0047b334 83c404                   add        esp, 4
0047b337 50                       push       eax
0047b338 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0047b33b 50                       push       eax
0047b33c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047b33f e8ecfcffff               call       0x47b030
0047b344 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0047b34b 8d4de4                   lea        ecx, [ebp - 0x1c]
0047b34e e8cd0e0000               call       0x47c220
0047b353 8b4508                   mov        eax, dword ptr [ebp + 8]
0047b356 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047b359 64890d00000000           mov        dword ptr fs:[0], ecx
0047b360 59                       pop        ecx
0047b361 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0047b364 33cd                     xor        ecx, ebp
0047b366 e883760c00               call       0x5429ee
0047b36b 8be5                     mov        esp, ebp
0047b36d 5d                       pop        ebp
0047b36e c3                       ret        
0047b36f cc                       int3       
