00486bb0 55                       push       ebp
00486bb1 8bec                     mov        ebp, esp
00486bb3 6aff                     push       -1
00486bb5 6800765600               push       0x567600
00486bba 64a100000000             mov        eax, dword ptr fs:[0]
00486bc0 50                       push       eax
00486bc1 51                       push       ecx
00486bc2 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00486bc7 33c5                     xor        eax, ebp
00486bc9 50                       push       eax
00486bca 8d45f4                   lea        eax, [ebp - 0xc]
00486bcd 64a300000000             mov        dword ptr fs:[0], eax
00486bd3 894df0                   mov        dword ptr [ebp - 0x10], ecx
00486bd6 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486bd9 e84291f9ff               call       0x41fd20
00486bde 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486be1 c70088fd5600             mov        dword ptr [eax], 0x56fd88
00486be7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486bea 83c110                   add        ecx, 0x10
00486bed e8cef0f9ff               call       0x425cc0
00486bf2 68c05c4200               push       0x425cc0
00486bf7 6a03                     push       3
00486bf9 6a04                     push       4
00486bfb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486bfe 83c114                   add        ecx, 0x14
00486c01 51                       push       ecx
00486c02 e81950f8ff               call       0x40bc20
00486c07 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486c0a 83c120                   add        ecx, 0x20
00486c0d e8aef0f9ff               call       0x425cc0
00486c12 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486c15 83c124                   add        ecx, 0x24
00486c18 e873c1f9ff               call       0x422d90
00486c1d 6a40                     push       0x40
00486c1f 6a00                     push       0
00486c21 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486c24 83c234                   add        edx, 0x34
00486c27 52                       push       edx
00486c28 e833db0b00               call       0x544760
00486c2d 83c40c                   add        esp, 0xc
00486c30 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486c33 c7407400000000           mov        dword ptr [eax + 0x74], 0
00486c3a 33c9                     xor        ecx, ecx
00486c3c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486c3f 83c278                   add        edx, 0x78
00486c42 890a                     mov        dword ptr [edx], ecx
00486c44 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486c47 c7407c00000000           mov        dword ptr [eax + 0x7c], 0
00486c4e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486c51 c7818000000000000000     mov        dword ptr [ecx + 0x80], 0
00486c5b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486c5e c7828400000000000000     mov        dword ptr [edx + 0x84], 0
00486c68 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486c6b c7808800000000000000     mov        dword ptr [eax + 0x88], 0
00486c75 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486c78 c7818c00000000000000     mov        dword ptr [ecx + 0x8c], 0
00486c82 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486c85 c7829000000000000000     mov        dword ptr [edx + 0x90], 0
00486c8f 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486c92 0f57c0                   xorps      xmm0, xmm0
00486c95 f20f118098000000         movsd      qword ptr [eax + 0x98], xmm0
00486c9d 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486ca0 0f57c0                   xorps      xmm0, xmm0
00486ca3 f20f1181a0000000         movsd      qword ptr [ecx + 0xa0], xmm0
00486cab 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486cae c782a800000000000000     mov        dword ptr [edx + 0xa8], 0
00486cb8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486cbb 81c1ac000000             add        ecx, 0xac
00486cc1 e84ac1f9ff               call       0x422e10
00486cc6 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486cc9 c780b800000000000000     mov        dword ptr [eax + 0xb8], 0
00486cd3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486cd6 c781bc00000000000000     mov        dword ptr [ecx + 0xbc], 0
00486ce0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00486ce3 c782c000000000000000     mov        dword ptr [edx + 0xc0], 0
00486ced 6894fd5600               push       0x56fd94
00486cf2 e8b959f8ff               call       0x40c6b0
00486cf7 83c404                   add        esp, 4
00486cfa 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486cfd 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00486d00 64890d00000000           mov        dword ptr fs:[0], ecx
00486d07 59                       pop        ecx
00486d08 8be5                     mov        esp, ebp
00486d0a 5d                       pop        ebp
00486d0b c3                       ret        
00486d0c cc                       int3       
00486d0d cc                       int3       
00486d0e cc                       int3       
00486d0f cc                       int3       
00486d10 cc                       int3       
00486d11 cc                       int3       
00486d12 cc                       int3       
00486d13 cc                       int3       
00486d14 cc                       int3       
00486d15 cc                       int3       
00486d16 cc                       int3       
00486d17 cc                       int3       
00486d18 cc                       int3       
00486d19 cc                       int3       
00486d1a cc                       int3       
00486d1b cc                       int3       
00486d1c cc                       int3       
00486d1d cc                       int3       
00486d1e cc                       int3       
00486d1f cc                       int3       
