0049bf00 55                       push       ebp
0049bf01 8bec                     mov        ebp, esp
0049bf03 6aff                     push       -1
0049bf05 688d835600               push       0x56838d
0049bf0a 64a100000000             mov        eax, dword ptr fs:[0]
0049bf10 50                       push       eax
0049bf11 83ec20                   sub        esp, 0x20
0049bf14 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0049bf19 33c5                     xor        eax, ebp
0049bf1b 8945f0                   mov        dword ptr [ebp - 0x10], eax
0049bf1e 50                       push       eax
0049bf1f 8d45f4                   lea        eax, [ebp - 0xc]
0049bf22 64a300000000             mov        dword ptr fs:[0], eax
0049bf28 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0049bf2b 837d0800                 cmp        dword ptr [ebp + 8], 0
0049bf2f 7507                     jne        0x49bf38
0049bf31 32c0                     xor        al, al
0049bf33 e9a1000000               jmp        0x49bfd9
0049bf38 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0049bf3b 0508010000               add        eax, 0x108
0049bf40 8945e0                   mov        dword ptr [ebp - 0x20], eax
0049bf43 6a08                     push       8
0049bf45 8d4de8                   lea        ecx, [ebp - 0x18]
0049bf48 e83301f7ff               call       0x40c080
0049bf4d 8d4de8                   lea        ecx, [ebp - 0x18]
0049bf50 51                       push       ecx
0049bf51 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0049bf54 e82763f7ff               call       0x412280
0049bf59 c745fc00000000           mov        dword ptr [ebp - 4], 0
0049bf60 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0049bf63 e8d865f7ff               call       0x412540
0049bf68 8945d8                   mov        dword ptr [ebp - 0x28], eax
0049bf6b eb09                     jmp        0x49bf76
0049bf6d 8d4de8                   lea        ecx, [ebp - 0x18]
0049bf70 e8bb5cf7ff               call       0x411c30
0049bf75 90                       nop        
0049bf76 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
0049bf79 52                       push       edx
0049bf7a 8d4de8                   lea        ecx, [ebp - 0x18]
0049bf7d e82e5cf7ff               call       0x411bb0
0049bf82 0fb6c0                   movzx      eax, al
0049bf85 85c0                     test       eax, eax
0049bf87 743e                     je         0x49bfc7
0049bf89 8d4de8                   lea        ecx, [ebp - 0x18]
0049bf8c e86f03f7ff               call       0x40c300
0049bf91 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0049bf94 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0049bf97 e86403f7ff               call       0x40c300
0049bf9c 0588000000               add        eax, 0x88
0049bfa1 8bc8                     mov        ecx, eax
0049bfa3 e85803f7ff               call       0x40c300
0049bfa8 3b4508                   cmp        eax, dword ptr [ebp + 8]
0049bfab 7518                     jne        0x49bfc5
0049bfad c645e701                 mov        byte ptr [ebp - 0x19], 1
0049bfb1 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0049bfb8 8d4de8                   lea        ecx, [ebp - 0x18]
0049bfbb e8405bf7ff               call       0x411b00
0049bfc0 8a45e7                   mov        al, byte ptr [ebp - 0x19]
0049bfc3 eb14                     jmp        0x49bfd9
0049bfc5 eba6                     jmp        0x49bf6d
0049bfc7 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
0049bfce 8d4de8                   lea        ecx, [ebp - 0x18]
0049bfd1 e82a5bf7ff               call       0x411b00
0049bfd6 90                       nop        
0049bfd7 32c0                     xor        al, al
0049bfd9 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0049bfdc 64890d00000000           mov        dword ptr fs:[0], ecx
0049bfe3 59                       pop        ecx
0049bfe4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0049bfe7 33cd                     xor        ecx, ebp
0049bfe9 e8006a0a00               call       0x5429ee
0049bfee 8be5                     mov        esp, ebp
0049bff0 5d                       pop        ebp
0049bff1 c20400                   ret        4
0049bff4 cc                       int3       
0049bff5 cc                       int3       
0049bff6 cc                       int3       
0049bff7 cc                       int3       
0049bff8 cc                       int3       
0049bff9 cc                       int3       
0049bffa cc                       int3       
0049bffb cc                       int3       
0049bffc cc                       int3       
0049bffd cc                       int3       
0049bffe cc                       int3       
0049bfff cc                       int3       
