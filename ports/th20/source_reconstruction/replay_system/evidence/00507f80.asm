00507f80 55                       push       ebp
00507f81 8bec                     mov        ebp, esp
00507f83 83ec30                   sub        esp, 0x30
00507f86 894df4                   mov        dword ptr [ebp - 0xc], ecx
00507f89 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
00507f90 750a                     jne        0x507f9c
00507f92 b801000000               mov        eax, 1
00507f97 e93e020000               jmp        0x5081da
00507f9c b8c0020000               mov        eax, 0x2c0
00507fa1 6bc800                   imul       ecx, eax, 0
00507fa4 bac0020000               mov        edx, 0x2c0
00507fa9 6bc200                   imul       eax, edx, 0
00507fac 8b894c8b5b00             mov        ecx, dword ptr [ecx + 0x5b8b4c]
00507fb2 8988508b5b00             mov        dword ptr [eax + 0x5b8b50], ecx
00507fb8 bac0020000               mov        edx, 0x2c0
00507fbd 6bc200                   imul       eax, edx, 0
00507fc0 8b88b0885b00             mov        ecx, dword ptr [eax + 0x5b88b0]
00507fc6 81e1ffff0000             and        ecx, 0xffff
00507fcc bac0020000               mov        edx, 0x2c0
00507fd1 6bc200                   imul       eax, edx, 0
00507fd4 89884c8b5b00             mov        dword ptr [eax + 0x5b8b4c], ecx
00507fda b9c0020000               mov        ecx, 0x2c0
00507fdf 6bc900                   imul       ecx, ecx, 0
00507fe2 81c1b0885b00             add        ecx, 0x5b88b0
00507fe8 e8e3240000               call       0x50a4d0
00507fed 90                       nop        
00507fee 8b158c4f5c00             mov        edx, dword ptr [0x5c4f8c]
00507ff4 c1ea08                   shr        edx, 8
00507ff7 83e201                   and        edx, 1
00507ffa 0f84a3000000             je         0x5080a3
00508000 33c0                     xor        eax, eax
00508002 7531                     jne        0x508035
00508004 b904000000               mov        ecx, 4
00508009 6bd100                   imul       edx, ecx, 0
0050800c 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00508013 7420                     je         0x508035
00508015 b804000000               mov        eax, 4
0050801a 6bc800                   imul       ecx, eax, 0
0050801d 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00508023 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00508026 6a01                     push       1
00508028 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0050802b e870f3f8ff               call       0x4973a0
00508030 8945f0                   mov        dword ptr [ebp - 0x10], eax
00508033 eb07                     jmp        0x50803c
00508035 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0050803c 837df000                 cmp        dword ptr [ebp - 0x10], 0
00508040 7461                     je         0x5080a3
00508042 33c0                     xor        eax, eax
00508044 7531                     jne        0x508077
00508046 b904000000               mov        ecx, 4
0050804b 6bd100                   imul       edx, ecx, 0
0050804e 83ba9c885b0000           cmp        dword ptr [edx + 0x5b889c], 0
00508055 7420                     je         0x508077
00508057 b804000000               mov        eax, 4
0050805c 6bc800                   imul       ecx, eax, 0
0050805f 8b919c885b00             mov        edx, dword ptr [ecx + 0x5b889c]
00508065 8955e0                   mov        dword ptr [ebp - 0x20], edx
00508068 6a00                     push       0
0050806a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0050806d e8eedafaff               call       0x4b5b60
00508072 8945ec                   mov        dword ptr [ebp - 0x14], eax
00508075 eb07                     jmp        0x50807e
00508077 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
0050807e 837dec0a                 cmp        dword ptr [ebp - 0x14], 0xa
00508082 721f                     jb         0x5080a3
00508084 b8c0020000               mov        eax, 0x2c0
00508089 6bc800                   imul       ecx, eax, 0
0050808c 8b914c8b5b00             mov        edx, dword ptr [ecx + 0x5b8b4c]
00508092 83ca08                   or         edx, 8
00508095 b8c0020000               mov        eax, 0x2c0
0050809a 6bc800                   imul       ecx, eax, 0
0050809d 89914c8b5b00             mov        dword ptr [ecx + 0x5b8b4c], edx
005080a3 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005080a6 83ba5002000000           cmp        dword ptr [edx + 0x250], 0
005080ad 7d0a                     jge        0x5080b9
005080af b801000000               mov        eax, 1
005080b4 e921010000               jmp        0x5081da
005080b9 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
005080bc 8b8050020000             mov        eax, dword ptr [eax + 0x250]
005080c2 99                       cdq        
005080c3 b91e000000               mov        ecx, 0x1e
005080c8 f7f9                     idiv       ecx
005080ca 85d2                     test       edx, edx
005080cc 7571                     jne        0x50813f
005080ce 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005080d1 8b8ae0000000             mov        ecx, dword ptr [edx + 0xe0]
005080d7 e82442f0ff               call       0x40c300
005080dc 8945d4                   mov        dword ptr [ebp - 0x2c], eax
005080df 8b0d004a5c00             mov        ecx, dword ptr [0x5c4a00]
005080e5 e8163df9ff               call       0x49be00
005080ea d95ddc                   fstp       dword ptr [ebp - 0x24]
005080ed f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
005080f2 f30f5805ece05600         addss      xmm0, dword ptr [0x56e0ec]
005080fa 0f2f05a0cd5600           comiss     xmm0, dword ptr [0x56cda0]
00508101 7209                     jb         0x50810c
00508103 c745e8ff000000           mov        dword ptr [ebp - 0x18], 0xff
0050810a eb25                     jmp        0x508131
0050810c 8b0d004a5c00             mov        ecx, dword ptr [0x5c4a00]
00508112 e8e93cf9ff               call       0x49be00
00508117 d95dd8                   fstp       dword ptr [ebp - 0x28]
0050811a f30f1045d8               movss      xmm0, dword ptr [ebp - 0x28]
0050811f f30f5805ece05600         addss      xmm0, dword ptr [0x56e0ec]
00508127 f30f2cc0                 cvttss2si  eax, xmm0
0050812b 0fb6c8                   movzx      ecx, al
0050812e 894de8                   mov        dword ptr [ebp - 0x18], ecx
00508131 0fb655e8                 movzx      edx, byte ptr [ebp - 0x18]
00508135 52                       push       edx
00508136 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00508139 e8a21d0000               call       0x509ee0
0050813e 90                       nop        
0050813f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00508142 8b88e0000000             mov        ecx, dword ptr [eax + 0xe0]
00508148 e8b341f0ff               call       0x40c300
0050814d 8945d0                   mov        dword ptr [ebp - 0x30], eax
00508150 b9c0020000               mov        ecx, 0x2c0
00508155 6bd100                   imul       edx, ecx, 0
00508158 668b825c8b5b00           mov        ax, word ptr [edx + 0x5b8b5c]
0050815f 668945fe                 mov        word ptr [ebp - 2], ax
00508163 b9c0020000               mov        ecx, 0x2c0
00508168 6bd100                   imul       edx, ecx, 0
0050816b 668b82588b5b00           mov        ax, word ptr [edx + 0x5b8b58]
00508172 668945fc                 mov        word ptr [ebp - 4], ax
00508176 b9c0020000               mov        ecx, 0x2c0
0050817b 6bd100                   imul       edx, ecx, 0
0050817e 668b824c8b5b00           mov        ax, word ptr [edx + 0x5b8b4c]
00508185 668945fa                 mov        word ptr [ebp - 6], ax
00508189 0fb74dfe                 movzx      ecx, word ptr [ebp - 2]
0050818d 51                       push       ecx
0050818e 0fb755fc                 movzx      edx, word ptr [ebp - 4]
00508192 52                       push       edx
00508193 0fb745fa                 movzx      eax, word ptr [ebp - 6]
00508197 50                       push       eax
00508198 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0050819b e8a01c0000               call       0x509e40
005081a0 85c0                     test       eax, eax
005081a2 741c                     je         0x5081c0
005081a4 b968a55b00               mov        ecx, 0x5ba568
005081a9 e8b2cbf6ff               call       0x474d60
005081ae 50                       push       eax
005081af 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005081b2 e829210000               call       0x50a2e0
005081b7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005081ba 8981e0000000             mov        dword ptr [ecx + 0xe0], eax
005081c0 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
005081c3 8b8250020000             mov        eax, dword ptr [edx + 0x250]
005081c9 83c001                   add        eax, 1
005081cc 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005081cf 898150020000             mov        dword ptr [ecx + 0x250], eax
005081d5 b801000000               mov        eax, 1
005081da 8be5                     mov        esp, ebp
005081dc 5d                       pop        ebp
005081dd c3                       ret        
005081de cc                       int3       
005081df cc                       int3       
