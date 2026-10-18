004119b0 55                       push       ebp
004119b1 8bec                     mov        ebp, esp
004119b3 6aff                     push       -1
004119b5 68f0745600               push       0x5674f0
004119ba 64a100000000             mov        eax, dword ptr fs:[0]
004119c0 50                       push       eax
004119c1 83ec10                   sub        esp, 0x10
004119c4 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004119c9 33c5                     xor        eax, ebp
004119cb 50                       push       eax
004119cc 8d45f4                   lea        eax, [ebp - 0xc]
004119cf 64a300000000             mov        dword ptr fs:[0], eax
004119d5 894df0                   mov        dword ptr [ebp - 0x10], ecx
004119d8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004119db 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004119de 8908                     mov        dword ptr [eax], ecx
004119e0 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004119e3 8b450c                   mov        eax, dword ptr [ebp + 0xc]
004119e6 894204                   mov        dword ptr [edx + 4], eax
004119e9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004119ec 833900                   cmp        dword ptr [ecx], 0
004119ef 7415                     je         0x411a06
004119f1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004119f4 8b02                     mov        eax, dword ptr [edx]
004119f6 8945e8                   mov        dword ptr [ebp - 0x18], eax
004119f9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004119fc 51                       push       ecx
004119fd 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00411a00 e89b130000               call       0x412da0
00411a05 90                       nop        
00411a06 837d0800                 cmp        dword ptr [ebp + 8], 0
00411a0a 740d                     je         0x411a19
00411a0c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00411a0f e87ce5ffff               call       0x40ff90
00411a14 8945ec                   mov        dword ptr [ebp - 0x14], eax
00411a17 eb07                     jmp        0x411a20
00411a19 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00411a20 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00411a23 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00411a26 894204                   mov        dword ptr [edx + 4], eax
00411a29 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00411a2c 83790400                 cmp        dword ptr [ecx + 4], 0
00411a30 7416                     je         0x411a48
00411a32 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00411a35 8b4204                   mov        eax, dword ptr [edx + 4]
00411a38 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00411a3b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00411a3e 51                       push       ecx
00411a3f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00411a42 e859130000               call       0x412da0
00411a47 90                       nop        
00411a48 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00411a4b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00411a4e 64890d00000000           mov        dword ptr fs:[0], ecx
00411a55 59                       pop        ecx
00411a56 8be5                     mov        esp, ebp
00411a58 5d                       pop        ebp
00411a59 c20800                   ret        8
00411a5c cc                       int3       
00411a5d cc                       int3       
00411a5e cc                       int3       
00411a5f cc                       int3       
00411a60 cc                       int3       
00411a61 cc                       int3       
00411a62 cc                       int3       
00411a63 cc                       int3       
00411a64 cc                       int3       
00411a65 cc                       int3       
00411a66 cc                       int3       
00411a67 cc                       int3       
00411a68 cc                       int3       
00411a69 cc                       int3       
00411a6a cc                       int3       
00411a6b cc                       int3       
00411a6c cc                       int3       
00411a6d cc                       int3       
00411a6e cc                       int3       
00411a6f cc                       int3       
