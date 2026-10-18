00537490 55                       push       ebp
00537491 8bec                     mov        ebp, esp
00537493 83ec0c                   sub        esp, 0xc
00537496 894dfc                   mov        dword ptr [ebp - 4], ecx
00537499 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053749c c6403400                 mov        byte ptr [eax + 0x34], 0
005374a0 6a00                     push       0
005374a2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005374a5 83c114                   add        ecx, 0x14
005374a8 e873c0eeff               call       0x423520
005374ad 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005374b0 c7411001000000           mov        dword ptr [ecx + 0x10], 1
005374b7 6a00                     push       0
005374b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005374bc 83c124                   add        ecx, 0x24
005374bf e85cc0eeff               call       0x423520
005374c4 90                       nop        
005374c5 6a00                     push       0
005374c7 8d4df8                   lea        ecx, [ebp - 8]
005374ca e8d1cdedff               call       0x4142a0
005374cf 50                       push       eax
005374d0 b9a8695c00               mov        ecx, 0x5c69a8
005374d5 e8c6e2ffff               call       0x5357a0
005374da 90                       nop        
005374db 6a00                     push       0
005374dd 8d4df4                   lea        ecx, [ebp - 0xc]
005374e0 e8bbcdedff               call       0x4142a0
005374e5 50                       push       eax
005374e6 b9bc695c00               mov        ecx, 0x5c69bc
005374eb e8b0e2ffff               call       0x5357a0
005374f0 90                       nop        
005374f1 6a00                     push       0
005374f3 b9b8695c00               mov        ecx, 0x5c69b8
005374f8 e8a3a2edff               call       0x4117a0
005374fd 6a00                     push       0
005374ff 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537502 83c138                   add        ecx, 0x38
00537505 e816c0eeff               call       0x423520
0053750a 6a00                     push       0
0053750c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053750f 83c148                   add        ecx, 0x48
00537512 e809c0eeff               call       0x423520
00537517 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053751a c7425800000000           mov        dword ptr [edx + 0x58], 0
00537521 8be5                     mov        esp, ebp
00537523 5d                       pop        ebp
00537524 c3                       ret        
00537525 cc                       int3       
00537526 cc                       int3       
00537527 cc                       int3       
00537528 cc                       int3       
00537529 cc                       int3       
0053752a cc                       int3       
0053752b cc                       int3       
0053752c cc                       int3       
0053752d cc                       int3       
0053752e cc                       int3       
0053752f cc                       int3       
