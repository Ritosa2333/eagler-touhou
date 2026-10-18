00531200 55                       push       ebp
00531201 8bec                     mov        ebp, esp
00531203 51                       push       ecx
00531204 894dfc                   mov        dword ptr [ebp - 4], ecx
00531207 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053120a c6403400                 mov        byte ptr [eax + 0x34], 0
0053120e 6a00                     push       0
00531210 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531213 83c114                   add        ecx, 0x14
00531216 e80523efff               call       0x423520
0053121b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053121e c7411001000000           mov        dword ptr [ecx + 0x10], 1
00531225 6a00                     push       0
00531227 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053122a 83c124                   add        ecx, 0x24
0053122d e8ee22efff               call       0x423520
00531232 6a00                     push       0
00531234 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531237 83c138                   add        ecx, 0x38
0053123a e86105eeff               call       0x4117a0
0053123f 6a00                     push       0
00531241 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531244 83c13c                   add        ecx, 0x3c
00531247 e8d422efff               call       0x423520
0053124c 6a00                     push       0
0053124e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531251 83c14c                   add        ecx, 0x4c
00531254 e8c722efff               call       0x423520
00531259 90                       nop        
0053125a 8be5                     mov        esp, ebp
0053125c 5d                       pop        ebp
0053125d c3                       ret        
0053125e cc                       int3       
0053125f cc                       int3       
