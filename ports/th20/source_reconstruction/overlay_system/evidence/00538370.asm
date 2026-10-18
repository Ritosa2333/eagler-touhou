00538370 55                       push       ebp
00538371 8bec                     mov        ebp, esp
00538373 83ec0c                   sub        esp, 0xc
00538376 894dfc                   mov        dword ptr [ebp - 4], ecx
00538379 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053837c c6403400                 mov        byte ptr [eax + 0x34], 0
00538380 6a00                     push       0
00538382 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538385 83c114                   add        ecx, 0x14
00538388 e893b1eeff               call       0x423520
0053838d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538390 c7411001000000           mov        dword ptr [ecx + 0x10], 1
00538397 6a00                     push       0
00538399 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053839c 83c124                   add        ecx, 0x24
0053839f e87cb1eeff               call       0x423520
005383a4 6a00                     push       0
005383a6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005383a9 83c138                   add        ecx, 0x38
005383ac e86fb1eeff               call       0x423520
005383b1 90                       nop        
005383b2 6a00                     push       0
005383b4 8d4df8                   lea        ecx, [ebp - 8]
005383b7 e8e4beedff               call       0x4142a0
005383bc 50                       push       eax
005383bd b9cc695c00               mov        ecx, 0x5c69cc
005383c2 e8d9d3ffff               call       0x5357a0
005383c7 90                       nop        
005383c8 6a00                     push       0
005383ca 8d4df4                   lea        ecx, [ebp - 0xc]
005383cd e8cebeedff               call       0x4142a0
005383d2 50                       push       eax
005383d3 b9e0695c00               mov        ecx, 0x5c69e0
005383d8 e8c3d3ffff               call       0x5357a0
005383dd 90                       nop        
005383de 6a00                     push       0
005383e0 b9dc695c00               mov        ecx, 0x5c69dc
005383e5 e8b693edff               call       0x4117a0
005383ea 90                       nop        
005383eb 8be5                     mov        esp, ebp
005383ed 5d                       pop        ebp
005383ee c3                       ret        
005383ef cc                       int3       
