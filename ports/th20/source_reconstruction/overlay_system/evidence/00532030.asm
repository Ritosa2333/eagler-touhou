00532030 55                       push       ebp
00532031 8bec                     mov        ebp, esp
00532033 83ec60                   sub        esp, 0x60
00532036 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053203b 33c5                     xor        eax, ebp
0053203d 8945fc                   mov        dword ptr [ebp - 4], eax
00532040 894dc0                   mov        dword ptr [ebp - 0x40], ecx
00532043 e818220000               call       0x534260
00532048 50                       push       eax
00532049 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053204c 83c114                   add        ecx, 0x14
0053204f e8cc14efff               call       0x423520
00532054 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00532057 c6403401                 mov        byte ptr [eax + 0x34], 1
0053205b 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053205e 83c138                   add        ecx, 0x38
00532061 e86adcf1ff               call       0x44fcd0
00532066 8d4dc4                   lea        ecx, [ebp - 0x3c]
00532069 e8c299f4ff               call       0x47ba30
0053206e 6a00                     push       0
00532070 e8bbe7f2ff               call       0x460830
00532075 83c404                   add        esp, 4
00532078 8bc8                     mov        ecx, eax
0053207a e8d1e7f2ff               call       0x460850
0053207f 8b08                     mov        ecx, dword ptr [eax]
00532081 894dc4                   mov        dword ptr [ebp - 0x3c], ecx
00532084 8b5004                   mov        edx, dword ptr [eax + 4]
00532087 8955c8                   mov        dword ptr [ebp - 0x38], edx
0053208a 8b4008                   mov        eax, dword ptr [eax + 8]
0053208d 8945cc                   mov        dword ptr [ebp - 0x34], eax
00532090 6a00                     push       0
00532092 e88954f0ff               call       0x437520
00532097 83c404                   add        esp, 4
0053209a 8945bc                   mov        dword ptr [ebp - 0x44], eax
0053209d 6a00                     push       0
0053209f 8d4dc4                   lea        ecx, [ebp - 0x3c]
005320a2 51                       push       ecx
005320a3 6a09                     push       9
005320a5 8d55a0                   lea        edx, [ebp - 0x60]
005320a8 52                       push       edx
005320a9 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
005320ac e8bfbaf6ff               call       0x49db70
005320b1 8b00                     mov        eax, dword ptr [eax]
005320b3 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005320b6 894138                   mov        dword ptr [ecx + 0x38], eax
005320b9 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
005320bc f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
005320c4 f30f11425c               movss      dword ptr [edx + 0x5c], xmm0
005320c9 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
005320cc 83c038                   add        eax, 0x38
005320cf 8945b8                   mov        dword ptr [ebp - 0x48], eax
005320d2 51                       push       ecx
005320d3 f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
005320db f30f110424               movss      dword ptr [esp], xmm0
005320e0 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
005320e3 51                       push       ecx
005320e4 f30f10415c               movss      xmm0, dword ptr [ecx + 0x5c]
005320e9 f30f110424               movss      dword ptr [esp], xmm0
005320ee 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
005320f1 e8bae5f1ff               call       0x4506b0
005320f6 6a00                     push       0
005320f8 e833e7f2ff               call       0x460830
005320fd 83c404                   add        esp, 4
00532100 8bc8                     mov        ecx, eax
00532102 e849e7f2ff               call       0x460850
00532107 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
0053210a 81c28c000000             add        edx, 0x8c
00532110 8b08                     mov        ecx, dword ptr [eax]
00532112 890a                     mov        dword ptr [edx], ecx
00532114 8b4804                   mov        ecx, dword ptr [eax + 4]
00532117 894a04                   mov        dword ptr [edx + 4], ecx
0053211a 8b4008                   mov        eax, dword ptr [eax + 8]
0053211d 894208                   mov        dword ptr [edx + 8], eax
00532120 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
00532123 83c160                   add        ecx, 0x60
00532126 894dac                   mov        dword ptr [ebp - 0x54], ecx
00532129 f30f10057cfe5600         movss      xmm0, dword ptr [0x56fe7c]
00532131 f30f1145b4               movss      dword ptr [ebp - 0x4c], xmm0
00532136 f30f100590cd5600         movss      xmm0, dword ptr [0x56cd90]
0053213e f30f1145b0               movss      dword ptr [ebp - 0x50], xmm0
00532143 8d55b4                   lea        edx, [ebp - 0x4c]
00532146 52                       push       edx
00532147 8d45b0                   lea        eax, [ebp - 0x50]
0053214a 50                       push       eax
0053214b 6a04                     push       4
0053214d 6a14                     push       0x14
0053214f 8b4dac                   mov        ecx, dword ptr [ebp - 0x54]
00532152 e8f973f0ff               call       0x439550
00532157 90                       nop        
00532158 8b4dc0                   mov        ecx, dword ptr [ebp - 0x40]
0053215b 83c138                   add        ecx, 0x38
0053215e 894da8                   mov        dword ptr [ebp - 0x58], ecx
00532161 8b55c0                   mov        edx, dword ptr [ebp - 0x40]
00532164 52                       push       edx
00532165 8b4da8                   mov        ecx, dword ptr [ebp - 0x58]
00532168 e8e3e6f1ff               call       0x450850
0053216d 8b45c0                   mov        eax, dword ptr [ebp - 0x40]
00532170 83c038                   add        eax, 0x38
00532173 8945a4                   mov        dword ptr [ebp - 0x5c], eax
00532176 68101c5300               push       0x531c10
0053217b 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
0053217e e87dcff1ff               call       0x44f100
00532183 90                       nop        
00532184 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532187 33cd                     xor        ecx, ebp
00532189 e860080100               call       0x5429ee
0053218e 8be5                     mov        esp, ebp
00532190 5d                       pop        ebp
00532191 c3                       ret        
00532192 cc                       int3       
00532193 cc                       int3       
00532194 cc                       int3       
00532195 cc                       int3       
00532196 cc                       int3       
00532197 cc                       int3       
00532198 cc                       int3       
00532199 cc                       int3       
0053219a cc                       int3       
0053219b cc                       int3       
0053219c cc                       int3       
0053219d cc                       int3       
0053219e cc                       int3       
0053219f cc                       int3       
