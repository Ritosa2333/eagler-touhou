005348d0 55                       push       ebp
005348d1 8bec                     mov        ebp, esp
005348d3 83ec60                   sub        esp, 0x60
005348d6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005348db 33c5                     xor        eax, ebp
005348dd 8945fc                   mov        dword ptr [ebp - 4], eax
005348e0 894dbc                   mov        dword ptr [ebp - 0x44], ecx
005348e3 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
005348e6 8b4840                   mov        ecx, dword ptr [eax + 0x40]
005348e9 51                       push       ecx
005348ea 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
005348f0 e80bdff6ff               call       0x4a2800
005348f5 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
005348f8 c7424000000000           mov        dword ptr [edx + 0x40], 0
005348ff 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
00534902 8b487c                   mov        ecx, dword ptr [eax + 0x7c]
00534905 51                       push       ecx
00534906 6a00                     push       0
00534908 6a0c                     push       0xc
0053490a 6a0c                     push       0xc
0053490c 68d0625700               push       0x5762d0
00534911 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00534917 e8b4c9f3ff               call       0x4712d0
0053491c 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0053491f 894240                   mov        dword ptr [edx + 0x40], eax
00534922 6a00                     push       0
00534924 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
00534927 83c144                   add        ecx, 0x44
0053492a e8f1ebeeff               call       0x423520
0053492f 8b45bc                   mov        eax, dword ptr [ebp - 0x44]
00534932 8b4840                   mov        ecx, dword ptr [eax + 0x40]
00534935 894db0                   mov        dword ptr [ebp - 0x50], ecx
00534938 6820275300               push       0x532720
0053493d 8b4db0                   mov        ecx, dword ptr [ebp - 0x50]
00534940 e80b84ffff               call       0x52cd50
00534945 90                       nop        
00534946 8d4dc0                   lea        ecx, [ebp - 0x40]
00534949 e8e270f4ff               call       0x47ba30
0053494e 51                       push       ecx
0053494f 0f57c0                   xorps      xmm0, xmm0
00534952 f30f110424               movss      dword ptr [esp], xmm0
00534957 8d4df8                   lea        ecx, [ebp - 8]
0053495a e8b148efff               call       0x429210
0053495f 6a00                     push       0
00534961 e8cabef2ff               call       0x460830
00534966 83c404                   add        esp, 4
00534969 8bc8                     mov        ecx, eax
0053496b e8e0bef2ff               call       0x460850
00534970 8b10                     mov        edx, dword ptr [eax]
00534972 8955c0                   mov        dword ptr [ebp - 0x40], edx
00534975 8b4804                   mov        ecx, dword ptr [eax + 4]
00534978 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
0053497b 8b5008                   mov        edx, dword ptr [eax + 8]
0053497e 8955c8                   mov        dword ptr [ebp - 0x38], edx
00534981 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00534989 f30f5e058cec5600         divss      xmm0, dword ptr [0x56ec8c]
00534991 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00534996 6880c0ffff               push       0xffffc080
0053499b 8d4de0                   lea        ecx, [ebp - 0x20]
0053499e e8ad8cf2ff               call       0x45d650
005349a3 90                       nop        
005349a4 c745b800000000           mov        dword ptr [ebp - 0x48], 0
005349ab eb09                     jmp        0x5349b6
005349ad 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
005349b0 83c001                   add        eax, 1
005349b3 8945b8                   mov        dword ptr [ebp - 0x48], eax
005349b6 837db818                 cmp        dword ptr [ebp - 0x48], 0x18
005349ba 7d56                     jge        0x534a12
005349bc 8d4df8                   lea        ecx, [ebp - 8]
005349bf e81c49efff               call       0x4292e0
005349c4 d95dd8                   fstp       dword ptr [ebp - 0x28]
005349c7 6a00                     push       0
005349c9 e8522bf0ff               call       0x437520
005349ce 83c404                   add        esp, 4
005349d1 8945ac                   mov        dword ptr [ebp - 0x54], eax
005349d4 6a00                     push       0
005349d6 8d4dc0                   lea        ecx, [ebp - 0x40]
005349d9 51                       push       ecx
005349da 6a05                     push       5
005349dc 8d55a4                   lea        edx, [ebp - 0x5c]
005349df 52                       push       edx
005349e0 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
005349e3 e88891f6ff               call       0x49db70
005349e8 90                       nop        
005349e9 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
005349f1 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
005349f9 f30f5e05b4f75600         divss      xmm0, dword ptr [0x56f7b4]
00534a01 51                       push       ecx
00534a02 f30f110424               movss      dword ptr [esp], xmm0
00534a07 8d4df8                   lea        ecx, [ebp - 8]
00534a0a e8b1e6f1ff               call       0x4530c0
00534a0f 90                       nop        
00534a10 eb9b                     jmp        0x5349ad
00534a12 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
00534a1a f30f5e058cec5600         divss      xmm0, dword ptr [0x56ec8c]
00534a22 f30f1145dc               movss      dword ptr [ebp - 0x24], xmm0
00534a27 c745b400000000           mov        dword ptr [ebp - 0x4c], 0
00534a2e eb09                     jmp        0x534a39
00534a30 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00534a33 83c001                   add        eax, 1
00534a36 8945b4                   mov        dword ptr [ebp - 0x4c], eax
00534a39 837db418                 cmp        dword ptr [ebp - 0x4c], 0x18
00534a3d 7d56                     jge        0x534a95
00534a3f 8d4df8                   lea        ecx, [ebp - 8]
00534a42 e89948efff               call       0x4292e0
00534a47 d95dd8                   fstp       dword ptr [ebp - 0x28]
00534a4a 6a00                     push       0
00534a4c e8cf2af0ff               call       0x437520
00534a51 83c404                   add        esp, 4
00534a54 8945a8                   mov        dword ptr [ebp - 0x58], eax
00534a57 6a00                     push       0
00534a59 8d4dc0                   lea        ecx, [ebp - 0x40]
00534a5c 51                       push       ecx
00534a5d 6a05                     push       5
00534a5f 8d55a0                   lea        edx, [ebp - 0x60]
00534a62 52                       push       edx
00534a63 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00534a66 e80591f6ff               call       0x49db70
00534a6b 90                       nop        
00534a6c f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00534a74 f30f5905d0c85600         mulss      xmm0, dword ptr [0x56c8d0]
00534a7c f30f5e05b4f75600         divss      xmm0, dword ptr [0x56f7b4]
00534a84 51                       push       ecx
00534a85 f30f110424               movss      dword ptr [esp], xmm0
00534a8a 8d4df8                   lea        ecx, [ebp - 8]
00534a8d e8ced5f3ff               call       0x472060
00534a92 90                       nop        
00534a93 eb9b                     jmp        0x534a30
00534a95 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00534a98 33cd                     xor        ecx, ebp
00534a9a e84fdf0000               call       0x5429ee
00534a9f 8be5                     mov        esp, ebp
00534aa1 5d                       pop        ebp
00534aa2 c3                       ret        
00534aa3 cc                       int3       
00534aa4 cc                       int3       
00534aa5 cc                       int3       
00534aa6 cc                       int3       
00534aa7 cc                       int3       
00534aa8 cc                       int3       
00534aa9 cc                       int3       
00534aaa cc                       int3       
00534aab cc                       int3       
00534aac cc                       int3       
00534aad cc                       int3       
00534aae cc                       int3       
00534aaf cc                       int3       
