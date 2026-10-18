00511f00 55                       push       ebp
00511f01 8bec                     mov        ebp, esp
00511f03 83ec34                   sub        esp, 0x34
00511f06 56                       push       esi
00511f07 894de8                   mov        dword ptr [ebp - 0x18], ecx
00511f0a 6a00                     push       0
00511f0c e86f6ff6ff               call       0x478e80
00511f11 83c404                   add        esp, 4
00511f14 8945f4                   mov        dword ptr [ebp - 0xc], eax
00511f17 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00511f1a e8a183f6ff               call       0x47a2c0
00511f1f 8945f8                   mov        dword ptr [ebp - 8], eax
00511f22 6a00                     push       0
00511f24 689f860100               push       0x1869f
00511f29 6a00                     push       0
00511f2b 51                       push       ecx
00511f2c f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00511f34 f30f110424               movss      dword ptr [esp], xmm0
00511f39 8b45f8                   mov        eax, dword ptr [ebp - 8]
00511f3c 50                       push       eax
00511f3d 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00511f40 e81bb0f6ff               call       0x47cf60
00511f45 6a00                     push       0
00511f47 e8946ff6ff               call       0x478ee0
00511f4c 83c404                   add        esp, 4
00511f4f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00511f52 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00511f55 e86683f6ff               call       0x47a2c0
00511f5a 8945f0                   mov        dword ptr [ebp - 0x10], eax
00511f5d 6a01                     push       1
00511f5f 6a00                     push       0
00511f61 51                       push       ecx
00511f62 f30f10050cf15600         movss      xmm0, dword ptr [0x56f10c]
00511f6a f30f110424               movss      dword ptr [esp], xmm0
00511f6f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00511f72 51                       push       ecx
00511f73 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00511f76 e8558bfbff               call       0x4caad0
00511f7b 90                       nop        
00511f7c 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00511f7f 8b02                     mov        eax, dword ptr [edx]
00511f81 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
00511f84 894dfc                   mov        dword ptr [ebp - 4], ecx
00511f87 837dfc03                 cmp        dword ptr [ebp - 4], 3
00511f8b 7770                     ja         0x511ffd
00511f8d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00511f90 ff249580205100           jmp        dword ptr [edx*4 + 0x512080]
00511f97 6a00                     push       0
00511f99 e8e220f5ff               call       0x464080
00511f9e 83c404                   add        esp, 4
00511fa1 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00511fa4 6a01                     push       1
00511fa6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00511fa9 e8b2cafeff               call       0x4fea60
00511fae 90                       nop        
00511faf eb4c                     jmp        0x511ffd
00511fb1 6a00                     push       0
00511fb3 e8c820f5ff               call       0x464080
00511fb8 83c404                   add        esp, 4
00511fbb 8945e0                   mov        dword ptr [ebp - 0x20], eax
00511fbe 6a01                     push       1
00511fc0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00511fc3 e8e8c9feff               call       0x4fe9b0
00511fc8 90                       nop        
00511fc9 eb32                     jmp        0x511ffd
00511fcb 6a00                     push       0
00511fcd e8ae20f5ff               call       0x464080
00511fd2 83c404                   add        esp, 4
00511fd5 8945dc                   mov        dword ptr [ebp - 0x24], eax
00511fd8 6a01                     push       1
00511fda 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
00511fdd e8cecafeff               call       0x4feab0
00511fe2 90                       nop        
00511fe3 eb18                     jmp        0x511ffd
00511fe5 6a00                     push       0
00511fe7 e89420f5ff               call       0x464080
00511fec 83c404                   add        esp, 4
00511fef 8945d8                   mov        dword ptr [ebp - 0x28], eax
00511ff2 6a01                     push       1
00511ff4 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00511ff7 e804cafeff               call       0x4fea00
00511ffc 90                       nop        
00511ffd 6a00                     push       0
00511fff e87c20f5ff               call       0x464080
00512004 83c404                   add        esp, 4
00512007 8bc8                     mov        ecx, eax
00512009 e81275f8ff               call       0x499520
0051200e 8bf0                     mov        esi, eax
00512010 6a00                     push       0
00512012 e86920f5ff               call       0x464080
00512017 83c404                   add        esp, 4
0051201a 8bc8                     mov        ecx, eax
0051201c e83f1e0000               call       0x513e60
00512021 99                       cdq        
00512022 2bc2                     sub        eax, edx
00512024 d1f8                     sar        eax, 1
00512026 3bf0                     cmp        esi, eax
00512028 7c33                     jl         0x51205d
0051202a 6a00                     push       0
0051202c e84f20f5ff               call       0x464080
00512031 83c404                   add        esp, 4
00512034 8945d0                   mov        dword ptr [ebp - 0x30], eax
00512037 6a00                     push       0
00512039 e84220f5ff               call       0x464080
0051203e 83c404                   add        esp, 4
00512041 8bc8                     mov        ecx, eax
00512043 e8181e0000               call       0x513e60
00512048 99                       cdq        
00512049 2bc2                     sub        eax, edx
0051204b d1f8                     sar        eax, 1
0051204d 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00512050 8b45d4                   mov        eax, dword ptr [ebp - 0x2c]
00512053 50                       push       eax
00512054 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
00512057 e8c4c5faff               call       0x4be620
0051205c 90                       nop        
0051205d 6a00                     push       0
0051205f e81c20f5ff               call       0x464080
00512064 83c404                   add        esp, 4
00512067 8945cc                   mov        dword ptr [ebp - 0x34], eax
0051206a 6896000000               push       0x96
0051206f 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00512072 e8f91b0000               call       0x513c70
00512077 33c0                     xor        eax, eax
00512079 5e                       pop        esi
0051207a 8be5                     mov        esp, ebp
0051207c 5d                       pop        ebp
0051207d c20400                   ret        4
00512080 97                       xchg       edi, eax
00512081 1f                       pop        ds
00512082 51                       push       ecx
00512083 00b11f5100cb             add        byte ptr [ecx - 0x34ffaee1], dh
00512089 1f                       pop        ds
0051208a 51                       push       ecx
0051208b 00e5                     add        ch, ah
0051208d 1f                       pop        ds
0051208e 51                       push       ecx
