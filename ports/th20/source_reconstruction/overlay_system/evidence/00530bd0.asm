00530bd0 55                       push       ebp
00530bd1 8bec                     mov        ebp, esp
00530bd3 83ec1c                   sub        esp, 0x1c
00530bd6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00530bdb 33c5                     xor        eax, ebp
00530bdd 8945fc                   mov        dword ptr [ebp - 4], eax
00530be0 894dec                   mov        dword ptr [ebp - 0x14], ecx
00530be3 e878360000               call       0x534260
00530be8 50                       push       eax
00530be9 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530bec 83c114                   add        ecx, 0x14
00530bef e82c29efff               call       0x423520
00530bf4 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530bf7 c6403401                 mov        byte ptr [eax + 0x34], 1
00530bfb 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530bfe 83c138                   add        ecx, 0x38
00530c01 e8caf0f1ff               call       0x44fcd0
00530c06 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530c09 f30f1005c0d75600         movss      xmm0, dword ptr [0x56d7c0]
00530c11 f30f11413c               movss      dword ptr [ecx + 0x3c], xmm0
00530c16 51                       push       ecx
00530c17 0f57c0                   xorps      xmm0, xmm0
00530c1a f30f110424               movss      dword ptr [esp], xmm0
00530c1f 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530c22 51                       push       ecx
00530c23 f30f10423c               movss      xmm0, dword ptr [edx + 0x3c]
00530c28 f30f110424               movss      dword ptr [esp], xmm0
00530c2d 51                       push       ecx
00530c2e 0f57c0                   xorps      xmm0, xmm0
00530c31 f30f110424               movss      dword ptr [esp], xmm0
00530c36 8d4df0                   lea        ecx, [ebp - 0x10]
00530c39 e89221efff               call       0x422dd0
00530c3e 90                       nop        
00530c3f 6a00                     push       0
00530c41 e8eafbf2ff               call       0x460830
00530c46 83c404                   add        esp, 4
00530c49 8bc8                     mov        ecx, eax
00530c4b e8c00befff               call       0x421810
00530c50 8945e8                   mov        dword ptr [ebp - 0x18], eax
00530c53 6a00                     push       0
00530c55 6aff                     push       -1
00530c57 51                       push       ecx
00530c58 0f57c0                   xorps      xmm0, xmm0
00530c5b f30f110424               movss      dword ptr [esp], xmm0
00530c60 8d45f0                   lea        eax, [ebp - 0x10]
00530c63 50                       push       eax
00530c64 6a18                     push       0x18
00530c66 6824fb5600               push       0x56fb24
00530c6b 8d4de4                   lea        ecx, [ebp - 0x1c]
00530c6e 51                       push       ecx
00530c6f 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00530c72 e86984f4ff               call       0x4790e0
00530c77 8b10                     mov        edx, dword ptr [eax]
00530c79 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00530c7c 895038                   mov        dword ptr [eax + 0x38], edx
00530c7f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530c82 33cd                     xor        ecx, ebp
00530c84 e8651d0100               call       0x5429ee
00530c89 8be5                     mov        esp, ebp
00530c8b 5d                       pop        ebp
00530c8c c3                       ret        
00530c8d cc                       int3       
00530c8e cc                       int3       
00530c8f cc                       int3       
