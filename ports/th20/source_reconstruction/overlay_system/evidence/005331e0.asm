005331e0 55                       push       ebp
005331e1 8bec                     mov        ebp, esp
005331e3 83ec20                   sub        esp, 0x20
005331e6 894dfc                   mov        dword ptr [ebp - 4], ecx
005331e9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005331ec 8b4828                   mov        ecx, dword ptr [eax + 0x28]
005331ef 894df8                   mov        dword ptr [ebp - 8], ecx
005331f2 8b55fc                   mov        edx, dword ptr [ebp - 4]
005331f5 8b4228                   mov        eax, dword ptr [edx + 0x28]
005331f8 8b08                     mov        ecx, dword ptr [eax]
005331fa 8b5108                   mov        edx, dword ptr [ecx + 8]
005331fd 8955f4                   mov        dword ptr [ebp - 0xc], edx
00533200 8b4510                   mov        eax, dword ptr [ebp + 0x10]
00533203 50                       push       eax
00533204 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
00533207 51                       push       ecx
00533208 8b5508                   mov        edx, dword ptr [ebp + 8]
0053320b 52                       push       edx
0053320c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053320f ff55f4                   call       dword ptr [ebp - 0xc]
00533212 90                       nop        
00533213 6a00                     push       0
00533215 e816d6f2ff               call       0x460830
0053321a 83c404                   add        esp, 4
0053321d 8bc8                     mov        ecx, eax
0053321f e8bcc3fcff               call       0x4ff5e0
00533224 0fb6c0                   movzx      eax, al
00533227 85c0                     test       eax, eax
00533229 742c                     je         0x533257
0053322b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053322e 8b512c                   mov        edx, dword ptr [ecx + 0x2c]
00533231 8955f0                   mov        dword ptr [ebp - 0x10], edx
00533234 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533237 8b482c                   mov        ecx, dword ptr [eax + 0x2c]
0053323a 8b11                     mov        edx, dword ptr [ecx]
0053323c 8b420c                   mov        eax, dword ptr [edx + 0xc]
0053323f 8945ec                   mov        dword ptr [ebp - 0x14], eax
00533242 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00533245 51                       push       ecx
00533246 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00533249 52                       push       edx
0053324a 8b4508                   mov        eax, dword ptr [ebp + 8]
0053324d 50                       push       eax
0053324e 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00533251 ff55ec                   call       dword ptr [ebp - 0x14]
00533254 90                       nop        
00533255 eb2a                     jmp        0x533281
00533257 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053325a 8b5130                   mov        edx, dword ptr [ecx + 0x30]
0053325d 8955e8                   mov        dword ptr [ebp - 0x18], edx
00533260 8b45fc                   mov        eax, dword ptr [ebp - 4]
00533263 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00533266 8b11                     mov        edx, dword ptr [ecx]
00533268 8b4210                   mov        eax, dword ptr [edx + 0x10]
0053326b 8945e4                   mov        dword ptr [ebp - 0x1c], eax
0053326e 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
00533271 51                       push       ecx
00533272 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00533275 52                       push       edx
00533276 8b4508                   mov        eax, dword ptr [ebp + 8]
00533279 50                       push       eax
0053327a 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0053327d ff55e4                   call       dword ptr [ebp - 0x1c]
00533280 90                       nop        
00533281 6a00                     push       0
00533283 e8a8d5f2ff               call       0x460830
00533288 83c404                   add        esp, 4
0053328b 8bc8                     mov        ecx, eax
0053328d e81ec4fcff               call       0x4ff6b0
00533292 8945e0                   mov        dword ptr [ebp - 0x20], eax
00533295 6a00                     push       0
00533297 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053329a e861140000               call       0x534700
0053329f 90                       nop        
005332a0 8be5                     mov        esp, ebp
005332a2 5d                       pop        ebp
005332a3 c20c00                   ret        0xc
005332a6 cc                       int3       
005332a7 cc                       int3       
005332a8 cc                       int3       
005332a9 cc                       int3       
005332aa cc                       int3       
005332ab cc                       int3       
005332ac cc                       int3       
005332ad cc                       int3       
005332ae cc                       int3       
005332af cc                       int3       
