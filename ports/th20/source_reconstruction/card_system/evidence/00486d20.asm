00486d20 55                       push       ebp
00486d21 8bec                     mov        ebp, esp
00486d23 6aff                     push       -1
00486d25 6800765600               push       0x567600
00486d2a 64a100000000             mov        eax, dword ptr fs:[0]
00486d30 50                       push       eax
00486d31 51                       push       ecx
00486d32 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00486d37 33c5                     xor        eax, ebp
00486d39 50                       push       eax
00486d3a 8d45f4                   lea        eax, [ebp - 0xc]
00486d3d 64a300000000             mov        dword ptr fs:[0], eax
00486d43 894df0                   mov        dword ptr [ebp - 0x10], ecx
00486d46 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486d49 c70088fd5600             mov        dword ptr [eax], 0x56fd88
00486d4f b904000000               mov        ecx, 4
00486d54 6bd100                   imul       edx, ecx, 0
00486d57 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486d5a 8d4c1014                 lea        ecx, [eax + edx + 0x14]
00486d5e e86d8ffcff               call       0x44fcd0
00486d63 b904000000               mov        ecx, 4
00486d68 c1e100                   shl        ecx, 0
00486d6b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486d6e 8d4c0a14                 lea        ecx, [edx + ecx + 0x14]
00486d72 e8598ffcff               call       0x44fcd0
00486d77 b804000000               mov        eax, 4
00486d7c d1e0                     shl        eax, 1
00486d7e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486d81 8d4c0114                 lea        ecx, [ecx + eax + 0x14]
00486d85 e8468ffcff               call       0x44fcd0
00486d8a 68a8fd5600               push       0x56fda8
00486d8f e81c59f8ff               call       0x40c6b0
00486d94 83c404                   add        esp, 4
00486d97 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486d9a 8b4208                   mov        eax, dword ptr [edx + 8]
00486d9d 50                       push       eax
00486d9e 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00486da4 e807b7f8ff               call       0x4124b0
00486da9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486dac 8b510c                   mov        edx, dword ptr [ecx + 0xc]
00486daf 52                       push       edx
00486db0 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
00486db6 e8f5b6f8ff               call       0x4124b0
00486dbb 90                       nop        
00486dbc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486dbf e82c90f9ff               call       0x41fdf0
00486dc4 90                       nop        
00486dc5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00486dc8 64890d00000000           mov        dword ptr fs:[0], ecx
00486dcf 59                       pop        ecx
00486dd0 8be5                     mov        esp, ebp
00486dd2 5d                       pop        ebp
00486dd3 c3                       ret        
00486dd4 cc                       int3       
00486dd5 cc                       int3       
00486dd6 cc                       int3       
00486dd7 cc                       int3       
00486dd8 cc                       int3       
00486dd9 cc                       int3       
00486dda cc                       int3       
00486ddb cc                       int3       
00486ddc cc                       int3       
00486ddd cc                       int3       
00486dde cc                       int3       
00486ddf cc                       int3       
