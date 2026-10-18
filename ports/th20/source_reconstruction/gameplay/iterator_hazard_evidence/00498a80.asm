00498a80 55                       push       ebp
00498a81 8bec                     mov        ebp, esp
00498a83 6aff                     push       -1
00498a85 688d835600               push       0x56838d
00498a8a 64a100000000             mov        eax, dword ptr fs:[0]
00498a90 50                       push       eax
00498a91 83ec20                   sub        esp, 0x20
00498a94 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00498a99 33c5                     xor        eax, ebp
00498a9b 8945f0                   mov        dword ptr [ebp - 0x10], eax
00498a9e 50                       push       eax
00498a9f 8d45f4                   lea        eax, [ebp - 0xc]
00498aa2 64a300000000             mov        dword ptr fs:[0], eax
00498aa8 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00498aab 837d0800                 cmp        dword ptr [ebp + 8], 0
00498aaf 7507                     jne        0x498ab8
00498ab1 33c0                     xor        eax, eax
00498ab3 e9a8000000               jmp        0x498b60
00498ab8 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
00498abb 0508010000               add        eax, 0x108
00498ac0 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00498ac3 6a08                     push       8
00498ac5 8d4de8                   lea        ecx, [ebp - 0x18]
00498ac8 e8b335f7ff               call       0x40c080
00498acd 8d4de8                   lea        ecx, [ebp - 0x18]
00498ad0 51                       push       ecx
00498ad1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00498ad4 e8a797f7ff               call       0x412280
00498ad9 c745fc00000000           mov        dword ptr [ebp - 4], 0
00498ae0 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00498ae3 e8589af7ff               call       0x412540
00498ae8 8945d8                   mov        dword ptr [ebp - 0x28], eax
00498aeb eb09                     jmp        0x498af6
00498aed 8d4de8                   lea        ecx, [ebp - 0x18]
00498af0 e83b91f7ff               call       0x411c30
00498af5 90                       nop        
00498af6 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
00498af9 52                       push       edx
00498afa 8d4de8                   lea        ecx, [ebp - 0x18]
00498afd e8ae90f7ff               call       0x411bb0
00498b02 0fb6c0                   movzx      eax, al
00498b05 85c0                     test       eax, eax
00498b07 7445                     je         0x498b4e
00498b09 8d4de8                   lea        ecx, [ebp - 0x18]
00498b0c e8ef37f7ff               call       0x40c300
00498b11 8945e0                   mov        dword ptr [ebp - 0x20], eax
00498b14 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00498b17 e8e437f7ff               call       0x40c300
00498b1c 0588000000               add        eax, 0x88
00498b21 8bc8                     mov        ecx, eax
00498b23 e8d837f7ff               call       0x40c300
00498b28 3b4508                   cmp        eax, dword ptr [ebp + 8]
00498b2b 751f                     jne        0x498b4c
00498b2d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00498b30 e8cb37f7ff               call       0x40c300
00498b35 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00498b38 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00498b3f 8d4de8                   lea        ecx, [ebp - 0x18]
00498b42 e8b98ff7ff               call       0x411b00
00498b47 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00498b4a eb14                     jmp        0x498b60
00498b4c eb9f                     jmp        0x498aed
00498b4e c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00498b55 8d4de8                   lea        ecx, [ebp - 0x18]
00498b58 e8a38ff7ff               call       0x411b00
00498b5d 90                       nop        
00498b5e 33c0                     xor        eax, eax
00498b60 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00498b63 64890d00000000           mov        dword ptr fs:[0], ecx
00498b6a 59                       pop        ecx
00498b6b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00498b6e 33cd                     xor        ecx, ebp
00498b70 e8799e0a00               call       0x5429ee
00498b75 8be5                     mov        esp, ebp
00498b77 5d                       pop        ebp
00498b78 c20400                   ret        4
00498b7b cc                       int3       
00498b7c cc                       int3       
00498b7d cc                       int3       
00498b7e cc                       int3       
00498b7f cc                       int3       
