0051bc10 55                       push       ebp
0051bc11 8bec                     mov        ebp, esp
0051bc13 6aff                     push       -1
0051bc15 68cd795600               push       0x5679cd
0051bc1a 64a100000000             mov        eax, dword ptr fs:[0]
0051bc20 50                       push       eax
0051bc21 83ec10                   sub        esp, 0x10
0051bc24 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0051bc29 33c5                     xor        eax, ebp
0051bc2b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051bc2e 50                       push       eax
0051bc2f 8d45f4                   lea        eax, [ebp - 0xc]
0051bc32 64a300000000             mov        dword ptr fs:[0], eax
0051bc38 894de4                   mov        dword ptr [ebp - 0x1c], ecx
0051bc3b 6a14                     push       0x14
0051bc3d b940025c00               mov        ecx, 0x5c0240
0051bc42 e8c906efff               call       0x40c310
0051bc47 50                       push       eax
0051bc48 8d4dec                   lea        ecx, [ebp - 0x14]
0051bc4b e8e0faeeff               call       0x40b730
0051bc50 c745fc00000000           mov        dword ptr [ebp - 4], 0
0051bc57 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0051bc5a e85183f4ff               call       0x463fb0
0051bc5f 90                       nop        
0051bc60 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0051bc63 50                       push       eax
0051bc64 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0051bc67 51                       push       ecx
0051bc68 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0051bc6b e8f017faff               call       0x4bd460
0051bc70 83b8f076000000           cmp        dword ptr [eax + 0x76f0], 0
0051bc77 7406                     je         0x51bc7f
0051bc79 c645eb01                 mov        byte ptr [ebp - 0x15], 1
0051bc7d eb04                     jmp        0x51bc83
0051bc7f c645eb00                 mov        byte ptr [ebp - 0x15], 0
0051bc83 8a55eb                   mov        dl, byte ptr [ebp - 0x15]
0051bc86 8855ea                   mov        byte ptr [ebp - 0x16], dl
0051bc89 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0051bc90 8d4dec                   lea        ecx, [ebp - 0x14]
0051bc93 e868fceeff               call       0x40b900
0051bc98 0fb645ea                 movzx      eax, byte ptr [ebp - 0x16]
0051bc9c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0051bc9f 64890d00000000           mov        dword ptr fs:[0], ecx
0051bca6 59                       pop        ecx
0051bca7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051bcaa 33cd                     xor        ecx, ebp
0051bcac e83d6d0200               call       0x5429ee
0051bcb1 8be5                     mov        esp, ebp
0051bcb3 5d                       pop        ebp
0051bcb4 c20800                   ret        8
0051bcb7 cc                       int3       
0051bcb8 cc                       int3       
0051bcb9 cc                       int3       
0051bcba cc                       int3       
0051bcbb cc                       int3       
0051bcbc cc                       int3       
0051bcbd cc                       int3       
0051bcbe cc                       int3       
0051bcbf cc                       int3       
