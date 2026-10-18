004a3b10 55                       push       ebp
004a3b11 8bec                     mov        ebp, esp
004a3b13 6aff                     push       -1
004a3b15 6810975600               push       0x569710
004a3b1a 64a100000000             mov        eax, dword ptr fs:[0]
004a3b20 50                       push       eax
004a3b21 83ec38                   sub        esp, 0x38
004a3b24 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004a3b29 33c5                     xor        eax, ebp
004a3b2b 8945f0                   mov        dword ptr [ebp - 0x10], eax
004a3b2e 50                       push       eax
004a3b2f 8d45f4                   lea        eax, [ebp - 0xc]
004a3b32 64a300000000             mov        dword ptr fs:[0], eax
004a3b38 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004a3b3b 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3b3e c700cc035700             mov        dword ptr [eax], 0x5703cc
004a3b44 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3b47 81c1cc030000             add        ecx, 0x3cc
004a3b4d 894ddc                   mov        dword ptr [ebp - 0x24], ecx
004a3b50 6a08                     push       8
004a3b52 8d4de8                   lea        ecx, [ebp - 0x18]
004a3b55 e82685f6ff               call       0x40c080
004a3b5a 8d55e8                   lea        edx, [ebp - 0x18]
004a3b5d 52                       push       edx
004a3b5e 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a3b61 e81ae7f6ff               call       0x412280
004a3b66 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
004a3b69 e8d2e9f6ff               call       0x412540
004a3b6e 8945d4                   mov        dword ptr [ebp - 0x2c], eax
004a3b71 eb09                     jmp        0x4a3b7c
004a3b73 8d4de8                   lea        ecx, [ebp - 0x18]
004a3b76 e8b5e0f6ff               call       0x411c30
004a3b7b 90                       nop        
004a3b7c 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
004a3b7f 50                       push       eax
004a3b80 8d4de8                   lea        ecx, [ebp - 0x18]
004a3b83 e828e0f6ff               call       0x411bb0
004a3b88 0fb6c8                   movzx      ecx, al
004a3b8b 85c9                     test       ecx, ecx
004a3b8d 741d                     je         0x4a3bac
004a3b8f 8d4de8                   lea        ecx, [ebp - 0x18]
004a3b92 e86987f6ff               call       0x40c300
004a3b97 8945d0                   mov        dword ptr [ebp - 0x30], eax
004a3b9a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
004a3b9d e85e87f6ff               call       0x40c300
004a3ba2 8bc8                     mov        ecx, eax
004a3ba4 e89736ffff               call       0x497240
004a3ba9 90                       nop        
004a3baa ebc7                     jmp        0x4a3b73
004a3bac 8d4de8                   lea        ecx, [ebp - 0x18]
004a3baf e84cdff6ff               call       0x411b00
004a3bb4 90                       nop        
004a3bb5 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3bb8 81c1e4030000             add        ecx, 0x3e4
004a3bbe e81de1f6ff               call       0x411ce0
004a3bc3 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a3bc6 8b8a24040000             mov        ecx, dword ptr [edx + 0x424]
004a3bcc e85febf6ff               call       0x412730
004a3bd1 8945cc                   mov        dword ptr [ebp - 0x34], eax
004a3bd4 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3bd7 50                       push       eax
004a3bd8 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
004a3bdb e8f0190000               call       0x4a55d0
004a3be0 90                       nop        
004a3be1 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3be4 8b9154030000             mov        edx, dword ptr [ecx + 0x354]
004a3bea c1ea12                   shr        edx, 0x12
004a3bed 83e201                   and        edx, 1
004a3bf0 0f8582000000             jne        0x4a3c78
004a3bf6 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3bf9 8b8854030000             mov        ecx, dword ptr [eax + 0x354]
004a3bff c1e907                   shr        ecx, 7
004a3c02 83e101                   and        ecx, 1
004a3c05 742c                     je         0x4a3c33
004a3c07 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
004a3c0a 8b8a24040000             mov        ecx, dword ptr [edx + 0x424]
004a3c10 e81bebf6ff               call       0x412730
004a3c15 8945c4                   mov        dword ptr [ebp - 0x3c], eax
004a3c18 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3c1b 8b88f8020000             mov        ecx, dword ptr [eax + 0x2f8]
004a3c21 894dc8                   mov        dword ptr [ebp - 0x38], ecx
004a3c24 6a00                     push       0
004a3c26 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
004a3c29 52                       push       edx
004a3c2a 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
004a3c2d e8ae86ffff               call       0x49c2e0
004a3c32 90                       nop        
004a3c33 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3c36 0594000000               add        eax, 0x94
004a3c3b 8945d8                   mov        dword ptr [ebp - 0x28], eax
004a3c3e 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a3c41 e8ea12f7ff               call       0x414f30
004a3c46 8945e0                   mov        dword ptr [ebp - 0x20], eax
004a3c49 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004a3c4c e81f13f7ff               call       0x414f70
004a3c51 8945c0                   mov        dword ptr [ebp - 0x40], eax
004a3c54 eb09                     jmp        0x4a3c5f
004a3c56 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004a3c59 83c114                   add        ecx, 0x14
004a3c5c 894de0                   mov        dword ptr [ebp - 0x20], ecx
004a3c5f 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004a3c62 3b55c0                   cmp        edx, dword ptr [ebp - 0x40]
004a3c65 7411                     je         0x4a3c78
004a3c67 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004a3c6a 8945bc                   mov        dword ptr [ebp - 0x44], eax
004a3c6d 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004a3c70 e85bc0faff               call       0x44fcd0
004a3c75 90                       nop        
004a3c76 ebde                     jmp        0x4a3c56
004a3c78 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3c7b 8b915c030000             mov        edx, dword ptr [ecx + 0x35c]
004a3c81 52                       push       edx
004a3c82 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004a3c88 e8a372feff               call       0x48af30
004a3c8d 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004a3c90 c7805c03000000000000     mov        dword ptr [eax + 0x35c], 0
004a3c9a 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3c9d 81c1f8030000             add        ecx, 0x3f8
004a3ca3 e85807f7ff               call       0x414400
004a3ca8 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3cab 81c188000000             add        ecx, 0x88
004a3cb1 e80afeffff               call       0x4a3ac0
004a3cb6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004a3cb9 e822000000               call       0x4a3ce0
004a3cbe 90                       nop        
004a3cbf 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004a3cc2 64890d00000000           mov        dword ptr fs:[0], ecx
004a3cc9 59                       pop        ecx
004a3cca 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004a3ccd 33cd                     xor        ecx, ebp
004a3ccf e81aed0900               call       0x5429ee
004a3cd4 8be5                     mov        esp, ebp
004a3cd6 5d                       pop        ebp
004a3cd7 c3                       ret        
004a3cd8 cc                       int3       
004a3cd9 cc                       int3       
004a3cda cc                       int3       
004a3cdb cc                       int3       
004a3cdc cc                       int3       
004a3cdd cc                       int3       
004a3cde cc                       int3       
004a3cdf cc                       int3       
