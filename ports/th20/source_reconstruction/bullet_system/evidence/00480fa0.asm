00480fa0 53                       push       ebx
00480fa1 8bdc                     mov        ebx, esp
00480fa3 83ec08                   sub        esp, 8
00480fa6 83e4f8                   and        esp, 0xfffffff8
00480fa9 83c404                   add        esp, 4
00480fac 55                       push       ebp
00480fad 8b6b04                   mov        ebp, dword ptr [ebx + 4]
00480fb0 896c2404                 mov        dword ptr [esp + 4], ebp
00480fb4 8bec                     mov        ebp, esp
00480fb6 83ec70                   sub        esp, 0x70
00480fb9 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00480fbe 33c5                     xor        eax, ebp
00480fc0 8945fc                   mov        dword ptr [ebp - 4], eax
00480fc3 894de0                   mov        dword ptr [ebp - 0x20], ecx
00480fc6 6880fc5600               push       0x56fc80
00480fcb 6a07                     push       7
00480fcd 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00480fd3 e858dbfcff               call       0x44eb30
00480fd8 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00480fdb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00480fde 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00480fe1 89888c6d2800             mov        dword ptr [eax + 0x286d8c], ecx
00480fe7 837dd400                 cmp        dword ptr [ebp - 0x2c], 0
00480feb 751a                     jne        0x481007
00480fed 688cfc5600               push       0x56fc8c
00480ff2 6878065c00               push       0x5c0678
00480ff7 e85431fdff               call       0x454150
00480ffc 83c408                   add        esp, 8
00480fff 83c8ff                   or         eax, 0xffffffff
00481002 e9e0010000               jmp        0x4811e7
00481007 8b5308                   mov        edx, dword ptr [ebx + 8]
0048100a 52                       push       edx
0048100b b968a55b00               mov        ecx, 0x5ba568
00481010 e8ababf8ff               call       0x40bbc0
00481015 8945c4                   mov        dword ptr [ebp - 0x3c], eax
00481018 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
0048101b 50                       push       eax
0048101c 8b4dc4                   mov        ecx, dword ptr [ebp - 0x3c]
0048101f e87c07f9ff               call       0x4117a0
00481024 8b4b08                   mov        ecx, dword ptr [ebx + 8]
00481027 51                       push       ecx
00481028 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048102b e800550000               call       0x486530
00481030 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00481033 8b829c6d2800             mov        eax, dword ptr [edx + 0x286d9c]
00481039 50                       push       eax
0048103a 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00481040 e85b66ffff               call       0x4776a0
00481045 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481048 83c160                   add        ecx, 0x60
0048104b e850adf8ff               call       0x40bda0
00481050 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481053 894110                   mov        dword ptr [ecx + 0x10], eax
00481056 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
00481059 83c260                   add        edx, 0x60
0048105c 8955c0                   mov        dword ptr [ebp - 0x40], edx
0048105f 68d0070000               push       0x7d0
00481064 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00481067 e834b6ffff               call       0x47c6a0
0048106c c7405005000000           mov        dword ptr [eax + 0x50], 5
00481073 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00481076 50                       push       eax
00481077 68d0494800               push       0x4849d0
0048107c 6a26                     push       0x26
0048107e e88d12f9ff               call       0x412310
00481083 83c40c                   add        esp, 0xc
00481086 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00481089 894108                   mov        dword ptr [ecx + 8], eax
0048108c 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
0048108f 52                       push       edx
00481090 6810534800               push       0x485310
00481095 6a29                     push       0x29
00481097 e81413f9ff               call       0x4123b0
0048109c 83c40c                   add        esp, 0xc
0048109f 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004810a2 89410c                   mov        dword ptr [ecx + 0xc], eax
004810a5 8b55e0                   mov        edx, dword ptr [ebp - 0x20]
004810a8 81c24c6d2800             add        edx, 0x286d4c
004810ae 8955bc                   mov        dword ptr [ebp - 0x44], edx
004810b1 6a00                     push       0
004810b3 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004810b6 e88595fcff               call       0x44a640
004810bb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004810be 05646d2800               add        eax, 0x286d64
004810c3 8945b8                   mov        dword ptr [ebp - 0x48], eax
004810c6 6a00                     push       0
004810c8 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004810cb e87095fcff               call       0x44a640
004810d0 c745d800000000           mov        dword ptr [ebp - 0x28], 0
004810d7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004810da 83c160                   add        ecx, 0x60
004810dd 51                       push       ecx
004810de 8d559c                   lea        edx, [ebp - 0x64]
004810e1 52                       push       edx
004810e2 e83997ffff               call       0x47a820
004810e7 83c408                   add        esp, 8
004810ea 8b00                     mov        eax, dword ptr [eax]
004810ec 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004810ef 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004810f2 894db0                   mov        dword ptr [ebp - 0x50], ecx
004810f5 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004810f8 83c160                   add        ecx, 0x60
004810fb e8f0540000               call       0x4865f0
00481100 3dd0070000               cmp        eax, 0x7d0
00481105 7d10                     jge        0x481117
00481107 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0048110a 83c160                   add        ecx, 0x60
0048110d e8de540000               call       0x4865f0
00481112 8945d0                   mov        dword ptr [ebp - 0x30], eax
00481115 eb07                     jmp        0x48111e
00481117 c745d0d0070000           mov        dword ptr [ebp - 0x30], 0x7d0
0048111e 8b55d0                   mov        edx, dword ptr [ebp - 0x30]
00481121 52                       push       edx
00481122 8b45b0                   mov        eax, dword ptr [ebp - 0x50]
00481125 50                       push       eax
00481126 8d4d94                   lea        ecx, [ebp - 0x6c]
00481129 e8e2e1fdff               call       0x45f310
0048112e 8b08                     mov        ecx, dword ptr [eax]
00481130 8b5004                   mov        edx, dword ptr [eax + 4]
00481133 894da0                   mov        dword ptr [ebp - 0x60], ecx
00481136 8955a4                   mov        dword ptr [ebp - 0x5c], edx
00481139 a048f35600               mov        al, byte ptr [0x56f348]
0048113e 8845df                   mov        byte ptr [ebp - 0x21], al
00481141 8a4ddf                   mov        cl, byte ptr [ebp - 0x21]
00481144 884dde                   mov        byte ptr [ebp - 0x22], cl
00481147 0fb655de                 movzx      edx, byte ptr [ebp - 0x22]
0048114b 52                       push       edx
0048114c 8b45a4                   mov        eax, dword ptr [ebp - 0x5c]
0048114f 50                       push       eax
00481150 8b4da0                   mov        ecx, dword ptr [ebp - 0x60]
00481153 51                       push       ecx
00481154 8d4de4                   lea        ecx, [ebp - 0x1c]
00481157 e8f46dfeff               call       0x467f50
0048115c 8d55e4                   lea        edx, [ebp - 0x1c]
0048115f 8955cc                   mov        dword ptr [ebp - 0x34], edx
00481162 6a08                     push       8
00481164 8d4df0                   lea        ecx, [ebp - 0x10]
00481167 e814aff8ff               call       0x40c080
0048116c 8d45f0                   lea        eax, [ebp - 0x10]
0048116f 50                       push       eax
00481170 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
00481173 e8a804fdff               call       0x451620
00481178 8d4dfb                   lea        ecx, [ebp - 5]
0048117b 51                       push       ecx
0048117c 8b4dcc                   mov        ecx, dword ptr [ebp - 0x34]
0048117f e8bcf3feff               call       0x470540
00481184 90                       nop        
00481185 eb09                     jmp        0x481190
00481187 8d4df0                   lea        ecx, [ebp - 0x10]
0048118a e8a1b5ffff               call       0x47c730
0048118f 90                       nop        
00481190 0fb655fb                 movzx      edx, byte ptr [ebp - 5]
00481194 52                       push       edx
00481195 8d45f0                   lea        eax, [ebp - 0x10]
00481198 50                       push       eax
00481199 e842a0feff               call       0x46b1e0
0048119e 83c408                   add        esp, 8
004811a1 0fb6c8                   movzx      ecx, al
004811a4 85c9                     test       ecx, ecx
004811a6 753d                     jne        0x4811e5
004811a8 8d4df0                   lea        ecx, [ebp - 0x10]
004811ab e8a0a0feff               call       0x46b250
004811b0 8945c8                   mov        dword ptr [ebp - 0x38], eax
004811b3 8b55d8                   mov        edx, dword ptr [ebp - 0x28]
004811b6 8955ac                   mov        dword ptr [ebp - 0x54], edx
004811b9 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004811bc 83c001                   add        eax, 1
004811bf 8945d8                   mov        dword ptr [ebp - 0x28], eax
004811c2 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004811c5 8b55ac                   mov        edx, dword ptr [ebp - 0x54]
004811c8 895128                   mov        dword ptr [ecx + 0x28], edx
004811cb 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
004811ce 054c6d2800               add        eax, 0x286d4c
004811d3 8945a8                   mov        dword ptr [ebp - 0x58], eax
004811d6 8b4dc8                   mov        ecx, dword ptr [ebp - 0x38]
004811d9 51                       push       ecx
004811da 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
004811dd e8ce94fcff               call       0x44a6b0
004811e2 90                       nop        
004811e3 eba2                     jmp        0x481187
004811e5 33c0                     xor        eax, eax
004811e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004811ea 33cd                     xor        ecx, ebp
004811ec e8fd170c00               call       0x5429ee
004811f1 8be5                     mov        esp, ebp
004811f3 5d                       pop        ebp
004811f4 8be3                     mov        esp, ebx
004811f6 5b                       pop        ebx
004811f7 c20400                   ret        4
004811fa cc                       int3       
004811fb cc                       int3       
004811fc cc                       int3       
004811fd cc                       int3       
004811fe cc                       int3       
004811ff cc                       int3       
