0052e860 55                       push       ebp
0052e861 8bec                     mov        ebp, esp
0052e863 83ec60                   sub        esp, 0x60
0052e866 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052e86b 33c5                     xor        eax, ebp
0052e86d 8945fc                   mov        dword ptr [ebp - 4], eax
0052e870 56                       push       esi
0052e871 894df0                   mov        dword ptr [ebp - 0x10], ecx
0052e874 6a04                     push       4
0052e876 8d4df8                   lea        ecx, [ebp - 8]
0052e879 e802d8edff               call       0x40c080
0052e87e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e881 e8da08eeff               call       0x40f160
0052e886 50                       push       eax
0052e887 8d4df8                   lea        ecx, [ebp - 8]
0052e88a e8e1cdedff               call       0x40b670
0052e88f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e892 e809030000               call       0x52eba0
0052e897 833800                   cmp        dword ptr [eax], 0
0052e89a 760f                     jbe        0x52e8ab
0052e89c 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e89f e8fc020000               call       0x52eba0
0052e8a4 8b00                     mov        eax, dword ptr [eax]
0052e8a6 8945d8                   mov        dword ptr [ebp - 0x28], eax
0052e8a9 eb07                     jmp        0x52e8b2
0052e8ab c745d801000000           mov        dword ptr [ebp - 0x28], 1
0052e8b2 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
0052e8b5 894dec                   mov        dword ptr [ebp - 0x14], ecx
0052e8b8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e8bb e8e0020000               call       0x52eba0
0052e8c0 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0052e8c3 2b10                     sub        edx, dword ptr [eax]
0052e8c5 3b5508                   cmp        edx, dword ptr [ebp + 8]
0052e8c8 7206                     jb         0x52e8d0
0052e8ca 837dec08                 cmp        dword ptr [ebp - 0x14], 8
0052e8ce 7323                     jae        0x52e8f3
0052e8d0 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e8d3 e888d8f6ff               call       0x49c160
0052e8d8 c1e802                   shr        eax, 2
0052e8db 2b45ec                   sub        eax, dword ptr [ebp - 0x14]
0052e8de 3b45ec                   cmp        eax, dword ptr [ebp - 0x14]
0052e8e1 7306                     jae        0x52e8e9
0052e8e3 e81874f9ff               call       0x4c5d00
0052e8e8 90                       nop        
0052e8e9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0052e8ec d1e0                     shl        eax, 1
0052e8ee 8945ec                   mov        dword ptr [ebp - 0x14], eax
0052e8f1 ebc5                     jmp        0x52e8b8
0052e8f3 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052e8f6 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052e8f9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e8fc e89f030000               call       0x52eca0
0052e901 8b10                     mov        edx, dword ptr [eax]
0052e903 c1ea02                   shr        edx, 2
0052e906 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0052e909 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0052e90c 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052e90f 8d4df4                   lea        ecx, [ebp - 0xc]
0052e912 51                       push       ecx
0052e913 8d55f8                   lea        edx, [ebp - 8]
0052e916 52                       push       edx
0052e917 e874aaf5ff               call       0x489390
0052e91c 83c408                   add        esp, 8
0052e91f 8945dc                   mov        dword ptr [ebp - 0x24], eax
0052e922 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0052e925 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052e928 8d1481                   lea        edx, [ecx + eax*4]
0052e92b 8955e8                   mov        dword ptr [ebp - 0x18], edx
0052e92e 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
0052e931 d1e8                     shr        eax, 1
0052e933 3945ec                   cmp        dword ptr [ebp - 0x14], eax
0052e936 770a                     ja         0x52e942
0052e938 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052e93b d1e1                     shl        ecx, 1
0052e93d 894dec                   mov        dword ptr [ebp - 0x14], ecx
0052e940 ebec                     jmp        0x52e92e
0052e942 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e945 e856020000               call       0x52eba0
0052e94a 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
0052e94d 2b10                     sub        edx, dword ptr [eax]
0052e94f 895508                   mov        dword ptr [ebp + 8], edx
0052e952 8b4508                   mov        eax, dword ptr [ebp + 8]
0052e955 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052e958 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e95b e800020000               call       0x52eb60
0052e960 8bf0                     mov        esi, eax
0052e962 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e965 e816020000               call       0x52eb80
0052e96a 8b0e                     mov        ecx, dword ptr [esi]
0052e96c 8d1481                   lea        edx, [ecx + eax*4]
0052e96f 8955cc                   mov        dword ptr [ebp - 0x34], edx
0052e972 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e975 e8e6010000               call       0x52eb60
0052e97a 8b00                     mov        eax, dword ptr [eax]
0052e97c 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0052e97f 8d1488                   lea        edx, [eax + ecx*4]
0052e982 8955c8                   mov        dword ptr [ebp - 0x38], edx
0052e985 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052e988 50                       push       eax
0052e989 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0052e98c 51                       push       ecx
0052e98d 8b55c8                   mov        edx, dword ptr [ebp - 0x38]
0052e990 52                       push       edx
0052e991 e8ca6af9ff               call       0x4c5460
0052e996 83c40c                   add        esp, 0xc
0052e999 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052e99c 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
0052e99f 3b4508                   cmp        eax, dword ptr [ebp + 8]
0052e9a2 775f                     ja         0x52ea03
0052e9a4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e9a7 e8b4010000               call       0x52eb60
0052e9ac 8b08                     mov        ecx, dword ptr [eax]
0052e9ae 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052e9b1 8d0491                   lea        eax, [ecx + edx*4]
0052e9b4 8945c4                   mov        dword ptr [ebp - 0x3c], eax
0052e9b7 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052e9ba e8a1010000               call       0x52eb60
0052e9bf 8b08                     mov        ecx, dword ptr [eax]
0052e9c1 894dc0                   mov        dword ptr [ebp - 0x40], ecx
0052e9c4 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052e9c7 52                       push       edx
0052e9c8 8b45c4                   mov        eax, dword ptr [ebp - 0x3c]
0052e9cb 50                       push       eax
0052e9cc 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0052e9cf 51                       push       ecx
0052e9d0 e88b6af9ff               call       0x4c5460
0052e9d5 83c40c                   add        esp, 0xc
0052e9d8 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052e9db 8b5508                   mov        edx, dword ptr [ebp + 8]
0052e9de 2b55e4                   sub        edx, dword ptr [ebp - 0x1c]
0052e9e1 52                       push       edx
0052e9e2 8b45e8                   mov        eax, dword ptr [ebp - 0x18]
0052e9e5 50                       push       eax
0052e9e6 e8a569f9ff               call       0x4c5390
0052e9eb 83c408                   add        esp, 8
0052e9ee 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052e9f1 51                       push       ecx
0052e9f2 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
0052e9f5 52                       push       edx
0052e9f6 e89569f9ff               call       0x4c5390
0052e9fb 83c408                   add        esp, 8
0052e9fe e981000000               jmp        0x52ea84
0052ea03 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea06 e855010000               call       0x52eb60
0052ea0b 8b00                     mov        eax, dword ptr [eax]
0052ea0d 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052ea10 8d1488                   lea        edx, [eax + ecx*4]
0052ea13 8955bc                   mov        dword ptr [ebp - 0x44], edx
0052ea16 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea19 e842010000               call       0x52eb60
0052ea1e 8b00                     mov        eax, dword ptr [eax]
0052ea20 8945b8                   mov        dword ptr [ebp - 0x48], eax
0052ea23 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052ea26 51                       push       ecx
0052ea27 8b55bc                   mov        edx, dword ptr [ebp - 0x44]
0052ea2a 52                       push       edx
0052ea2b 8b45b8                   mov        eax, dword ptr [ebp - 0x48]
0052ea2e 50                       push       eax
0052ea2f e82c6af9ff               call       0x4c5460
0052ea34 83c40c                   add        esp, 0xc
0052ea37 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea3a e821010000               call       0x52eb60
0052ea3f 8b08                     mov        ecx, dword ptr [eax]
0052ea41 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0052ea44 8d0491                   lea        eax, [ecx + edx*4]
0052ea47 8945b4                   mov        dword ptr [ebp - 0x4c], eax
0052ea4a 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea4d e80e010000               call       0x52eb60
0052ea52 8b08                     mov        ecx, dword ptr [eax]
0052ea54 8b55d4                   mov        edx, dword ptr [ebp - 0x2c]
0052ea57 8d0491                   lea        eax, [ecx + edx*4]
0052ea5a 8945b0                   mov        dword ptr [ebp - 0x50], eax
0052ea5d 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052ea60 51                       push       ecx
0052ea61 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
0052ea64 52                       push       edx
0052ea65 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
0052ea68 50                       push       eax
0052ea69 e8f269f9ff               call       0x4c5460
0052ea6e 83c40c                   add        esp, 0xc
0052ea71 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052ea74 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052ea77 51                       push       ecx
0052ea78 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
0052ea7b 52                       push       edx
0052ea7c e80f69f9ff               call       0x4c5390
0052ea81 83c408                   add        esp, 8
0052ea84 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea87 e8d4000000               call       0x52eb60
0052ea8c 833800                   cmp        dword ptr [eax], 0
0052ea8f 7462                     je         0x52eaf3
0052ea91 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea94 e8c7000000               call       0x52eb60
0052ea99 8bf0                     mov        esi, eax
0052ea9b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ea9e e8dd000000               call       0x52eb80
0052eaa3 8b0e                     mov        ecx, dword ptr [esi]
0052eaa5 8d1481                   lea        edx, [ecx + eax*4]
0052eaa8 8955ac                   mov        dword ptr [ebp - 0x54], edx
0052eaab 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052eaae e8ad000000               call       0x52eb60
0052eab3 8b00                     mov        eax, dword ptr [eax]
0052eab5 8945a8                   mov        dword ptr [ebp - 0x58], eax
0052eab8 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
0052eabb 51                       push       ecx
0052eabc 8b55a8                   mov        edx, dword ptr [ebp - 0x58]
0052eabf 52                       push       edx
0052eac0 e8ebdbedff               call       0x40c6b0
0052eac5 83c408                   add        esp, 8
0052eac8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052eacb e8d0000000               call       0x52eba0
0052ead0 8b00                     mov        eax, dword ptr [eax]
0052ead2 8945a4                   mov        dword ptr [ebp - 0x5c], eax
0052ead5 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052ead8 e883000000               call       0x52eb60
0052eadd 8b08                     mov        ecx, dword ptr [eax]
0052eadf 894da0                   mov        dword ptr [ebp - 0x60], ecx
0052eae2 8b55a4                   mov        edx, dword ptr [ebp - 0x5c]
0052eae5 52                       push       edx
0052eae6 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
0052eae9 50                       push       eax
0052eaea 8d4df8                   lea        ecx, [ebp - 8]
0052eaed e8ce94f6ff               call       0x497fc0
0052eaf2 90                       nop        
0052eaf3 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052eaf6 e865000000               call       0x52eb60
0052eafb 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0052eafe 8908                     mov        dword ptr [eax], ecx
0052eb00 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052eb03 e898000000               call       0x52eba0
0052eb08 8945d0                   mov        dword ptr [ebp - 0x30], eax
0052eb0b 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
0052eb0e 8b02                     mov        eax, dword ptr [edx]
0052eb10 034508                   add        eax, dword ptr [ebp + 8]
0052eb13 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0052eb16 8901                     mov        dword ptr [ecx], eax
0052eb18 5e                       pop        esi
0052eb19 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052eb1c 33cd                     xor        ecx, ebp
0052eb1e e8cb3e0100               call       0x5429ee
0052eb23 8be5                     mov        esp, ebp
0052eb25 5d                       pop        ebp
0052eb26 c20400                   ret        4
0052eb29 cc                       int3       
0052eb2a cc                       int3       
0052eb2b cc                       int3       
0052eb2c cc                       int3       
0052eb2d cc                       int3       
0052eb2e cc                       int3       
0052eb2f cc                       int3       
