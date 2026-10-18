004c02d0 55                       push       ebp
004c02d1 8bec                     mov        ebp, esp
004c02d3 6aff                     push       -1
004c02d5 683d865600               push       0x56863d
004c02da 64a100000000             mov        eax, dword ptr fs:[0]
004c02e0 50                       push       eax
004c02e1 83ec24                   sub        esp, 0x24
004c02e4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c02e9 33c5                     xor        eax, ebp
004c02eb 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c02ee 50                       push       eax
004c02ef 8d45f4                   lea        eax, [ebp - 0xc]
004c02f2 64a300000000             mov        dword ptr fs:[0], eax
004c02f8 894de0                   mov        dword ptr [ebp - 0x20], ecx
004c02fb c745e400000000           mov        dword ptr [ebp - 0x1c], 0
004c0302 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004c0305 0514c40000               add        eax, 0xc414
004c030a 8945dc                   mov        dword ptr [ebp - 0x24], eax
004c030d 6a08                     push       8
004c030f 8d4de8                   lea        ecx, [ebp - 0x18]
004c0312 e869bdf4ff               call       0x40c080
004c0317 8d4de8                   lea        ecx, [ebp - 0x18]
004c031a 51                       push       ecx
004c031b 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c031e e85d1ff5ff               call       0x412280
004c0323 c745fc00000000           mov        dword ptr [ebp - 4], 0
004c032a 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004c032d e80e22f5ff               call       0x412540
004c0332 8945d8                   mov        dword ptr [ebp - 0x28], eax
004c0335 eb09                     jmp        0x4c0340
004c0337 8d4de8                   lea        ecx, [ebp - 0x18]
004c033a e8f118f5ff               call       0x411c30
004c033f 90                       nop        
004c0340 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004c0343 52                       push       edx
004c0344 8d4de8                   lea        ecx, [ebp - 0x18]
004c0347 e86418f5ff               call       0x411bb0
004c034c 0fb6c0                   movzx      eax, al
004c034f 85c0                     test       eax, eax
004c0351 7425                     je         0x4c0378
004c0353 8d4de8                   lea        ecx, [ebp - 0x18]
004c0356 e8a5bff4ff               call       0x40c300
004c035b 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004c035e 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c0361 e8faedf4ff               call       0x40f160
004c0366 8b08                     mov        ecx, dword ptr [eax]
004c0368 e863000000               call       0x4c03d0
004c036d 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0370 83c101                   add        ecx, 1
004c0373 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004c0376 ebbf                     jmp        0x4c0337
004c0378 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c037f 8d4de8                   lea        ecx, [ebp - 0x18]
004c0382 e87917f5ff               call       0x411b00
004c0387 90                       nop        
004c0388 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004c038b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004c038e 898244c40000             mov        dword ptr [edx + 0xc444], eax
004c0394 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c0397 81c148c40000             add        ecx, 0xc448
004c039d 894dd0                   mov        dword ptr [ebp - 0x30], ecx
004c03a0 6a00                     push       0
004c03a2 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004c03a5 e89631f6ff               call       0x423540
004c03aa b801000000               mov        eax, 1
004c03af 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c03b2 64890d00000000           mov        dword ptr fs:[0], ecx
004c03b9 59                       pop        ecx
004c03ba 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c03bd 33cd                     xor        ecx, ebp
004c03bf e82a260800               call       0x5429ee
004c03c4 8be5                     mov        esp, ebp
004c03c6 5d                       pop        ebp
004c03c7 c3                       ret        
004c03c8 cc                       int3       
004c03c9 cc                       int3       
004c03ca cc                       int3       
004c03cb cc                       int3       
004c03cc cc                       int3       
004c03cd cc                       int3       
004c03ce cc                       int3       
004c03cf cc                       int3       
