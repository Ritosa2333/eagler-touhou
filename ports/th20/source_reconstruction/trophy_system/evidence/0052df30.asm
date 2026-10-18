0052df30 55                       push       ebp
0052df31 8bec                     mov        ebp, esp
0052df33 83ec5c                   sub        esp, 0x5c
0052df36 894df8                   mov        dword ptr [ebp - 8], ecx
0052df39 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052df3c 8b4830                   mov        ecx, dword ptr [eax + 0x30]
0052df3f 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052df42 837df400                 cmp        dword ptr [ebp - 0xc], 0
0052df46 7419                     je         0x52df61
0052df48 837df401                 cmp        dword ptr [ebp - 0xc], 1
0052df4c 0f847c010000             je         0x52e0ce
0052df52 837df402                 cmp        dword ptr [ebp - 0xc], 2
0052df56 0f84b8010000             je         0x52e114
0052df5c e9e7010000               jmp        0x52e148
0052df61 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052df64 83c114                   add        ecx, 0x14
0052df67 e854150000               call       0x52f4c0
0052df6c 85c0                     test       eax, eax
0052df6e 0f845a010000             je         0x52e0ce
0052df74 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052df77 83c214                   add        edx, 0x14
0052df7a 8955f0                   mov        dword ptr [ebp - 0x10], edx
0052df7d 6a00                     push       0
0052df7f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0052df82 e8b9170000               call       0x52f740
0052df87 8945e0                   mov        dword ptr [ebp - 0x20], eax
0052df8a 6a01                     push       1
0052df8c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052df8f e86c180000               call       0x52f800
0052df94 90                       nop        
0052df95 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052df98 8b4810                   mov        ecx, dword ptr [eax + 0x10]
0052df9b 894dec                   mov        dword ptr [ebp - 0x14], ecx
0052df9e 6a00                     push       0
0052dfa0 6a00                     push       0
0052dfa2 68c4575700               push       0x5757c4
0052dfa7 8d55c4                   lea        edx, [ebp - 0x3c]
0052dfaa 52                       push       edx
0052dfab 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052dfae e8cd00f7ff               call       0x49e080
0052dfb3 8b00                     mov        eax, dword ptr [eax]
0052dfb5 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052dfb8 894148                   mov        dword ptr [ecx + 0x48], eax
0052dfbb 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052dfbe 8b4210                   mov        eax, dword ptr [edx + 0x10]
0052dfc1 8945e8                   mov        dword ptr [ebp - 0x18], eax
0052dfc4 6a00                     push       0
0052dfc6 6a07                     push       7
0052dfc8 68c4575700               push       0x5757c4
0052dfcd 8d4dc0                   lea        ecx, [ebp - 0x40]
0052dfd0 51                       push       ecx
0052dfd1 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0052dfd4 e8a700f7ff               call       0x49e080
0052dfd9 8b10                     mov        edx, dword ptr [eax]
0052dfdb 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052dfde 89504c                   mov        dword ptr [eax + 0x4c], edx
0052dfe1 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052dfe4 8b5110                   mov        edx, dword ptr [ecx + 0x10]
0052dfe7 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0052dfea 6a00                     push       0
0052dfec 6a08                     push       8
0052dfee 68c4575700               push       0x5757c4
0052dff3 8d45bc                   lea        eax, [ebp - 0x44]
0052dff6 50                       push       eax
0052dff7 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0052dffa e88100f7ff               call       0x49e080
0052dfff 8b08                     mov        ecx, dword ptr [eax]
0052e001 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052e004 894a50                   mov        dword ptr [edx + 0x50], ecx
0052e007 6945e004070000           imul       eax, dword ptr [ebp - 0x20], 0x704
0052e00e 8b0d5c685c00             mov        ecx, dword ptr [0x5c685c]
0052e014 8d540104                 lea        edx, [ecx + eax + 4]
0052e018 52                       push       edx
0052e019 e842100000               call       0x52f060
0052e01e 83c404                   add        esp, 4
0052e021 8945dc                   mov        dword ptr [ebp - 0x24], eax
0052e024 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e027 83c14c                   add        ecx, 0x4c
0052e02a e8a1eef1ff               call       0x44ced0
0052e02f 8945d8                   mov        dword ptr [ebp - 0x28], eax
0052e032 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
0052e035 50                       push       eax
0052e036 68cc575700               push       0x5757cc
0052e03b 83ec28                   sub        esp, 0x28
0052e03e 8bcc                     mov        ecx, esp
0052e040 8965b8                   mov        dword ptr [ebp - 0x48], esp
0052e043 8d55ff                   lea        edx, [ebp - 1]
0052e046 52                       push       edx
0052e047 e884f0ffff               call       0x52d0d0
0052e04c 6a00                     push       0
0052e04e 6a00                     push       0
0052e050 6a02                     push       2
0052e052 6a00                     push       0
0052e054 68c0c08000               push       0x80c0c0
0052e059 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
0052e05c 50                       push       eax
0052e05d 8d4dac                   lea        ecx, [ebp - 0x54]
0052e060 51                       push       ecx
0052e061 8b1528005c00             mov        edx, dword ptr [0x5c0028]
0052e067 52                       push       edx
0052e068 e863cdf1ff               call       0x44add0
0052e06d 83c450                   add        esp, 0x50
0052e070 a1a80a5b00               mov        eax, dword ptr [0x5b0aa8]
0052e075 8945d4                   mov        dword ptr [ebp - 0x2c], eax
0052e078 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e07b 83c150                   add        ecx, 0x50
0052e07e e84deef1ff               call       0x44ced0
0052e083 8945d0                   mov        dword ptr [ebp - 0x30], eax
0052e086 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
0052e089 51                       push       ecx
0052e08a 83ec28                   sub        esp, 0x28
0052e08d 8bcc                     mov        ecx, esp
0052e08f 8965b4                   mov        dword ptr [ebp - 0x4c], esp
0052e092 8d55fe                   lea        edx, [ebp - 2]
0052e095 52                       push       edx
0052e096 e8a5f0ffff               call       0x52d140
0052e09b 6a00                     push       0
0052e09d 6a00                     push       0
0052e09f 6a02                     push       2
0052e0a1 6a00                     push       0
0052e0a3 68c0c08000               push       0x80c0c0
0052e0a8 8b45d0                   mov        eax, dword ptr [ebp - 0x30]
0052e0ab 50                       push       eax
0052e0ac 8d4da4                   lea        ecx, [ebp - 0x5c]
0052e0af 51                       push       ecx
0052e0b0 8b1528005c00             mov        edx, dword ptr [0x5c0028]
0052e0b6 52                       push       edx
0052e0b7 e814cdf1ff               call       0x44add0
0052e0bc 83c44c                   add        esp, 0x4c
0052e0bf 6a00                     push       0
0052e0c1 6a4f                     push       0x4f
0052e0c3 b930a85b00               mov        ecx, 0x5ba830
0052e0c8 e8a38cefff               call       0x426d70
0052e0cd 90                       nop        
0052e0ce 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052e0d1 83c038                   add        eax, 0x38
0052e0d4 8945cc                   mov        dword ptr [ebp - 0x34], eax
0052e0d7 68f0000000               push       0xf0
0052e0dc 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0052e0df e8dc54efff               call       0x4235c0
0052e0e4 0fb6c8                   movzx      ecx, al
0052e0e7 85c9                     test       ecx, ecx
0052e0e9 7427                     je         0x52e112
0052e0eb 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e0ee 83c114                   add        ecx, 0x14
0052e0f1 e8ca130000               call       0x52f4c0
0052e0f6 85c0                     test       eax, eax
0052e0f8 740d                     je         0x52e107
0052e0fa 6a00                     push       0
0052e0fc 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e0ff e8fc160000               call       0x52f800
0052e104 90                       nop        
0052e105 eb0b                     jmp        0x52e112
0052e107 6a02                     push       2
0052e109 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e10c e8ef160000               call       0x52f800
0052e111 90                       nop        
0052e112 eb34                     jmp        0x52e148
0052e114 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e117 83c148                   add        ecx, 0x48
0052e11a e8b11bf2ff               call       0x44fcd0
0052e11f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e122 83c14c                   add        ecx, 0x4c
0052e125 e8a61bf2ff               call       0x44fcd0
0052e12a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0052e12d 83c150                   add        ecx, 0x50
0052e130 e89b1bf2ff               call       0x44fcd0
0052e135 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052e138 52                       push       edx
0052e139 e88236efff               call       0x4217c0
0052e13e 83c404                   add        esp, 4
0052e141 b801000000               mov        eax, 1
0052e146 eb18                     jmp        0x52e160
0052e148 8b45f8                   mov        eax, dword ptr [ebp - 8]
0052e14b 83c038                   add        eax, 0x38
0052e14e 8945c8                   mov        dword ptr [ebp - 0x38], eax
0052e151 6a00                     push       0
0052e153 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
0052e156 e8e553efff               call       0x423540
0052e15b b801000000               mov        eax, 1
0052e160 8be5                     mov        esp, ebp
0052e162 5d                       pop        ebp
0052e163 c3                       ret        
0052e164 cc                       int3       
0052e165 cc                       int3       
0052e166 cc                       int3       
0052e167 cc                       int3       
0052e168 cc                       int3       
0052e169 cc                       int3       
0052e16a cc                       int3       
0052e16b cc                       int3       
0052e16c cc                       int3       
0052e16d cc                       int3       
0052e16e cc                       int3       
0052e16f cc                       int3       
