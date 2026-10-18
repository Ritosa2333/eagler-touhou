005322c0 55                       push       ebp
005322c1 8bec                     mov        ebp, esp
005322c3 83ec10                   sub        esp, 0x10
005322c6 894dfc                   mov        dword ptr [ebp - 4], ecx
005322c9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005322cc 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
005322d2 e8b9dcedff               call       0x40ff90
005322d7 0530060000               add        eax, 0x630
005322dc 8945f8                   mov        dword ptr [ebp - 8], eax
005322df b908000000               mov        ecx, 8
005322e4 c1e100                   shl        ecx, 0
005322e7 8b55fc                   mov        edx, dword ptr [ebp - 4]
005322ea 8d840a80000000           lea        eax, [edx + ecx + 0x80]
005322f1 50                       push       eax
005322f2 8d4df0                   lea        ecx, [ebp - 0x10]
005322f5 51                       push       ecx
005322f6 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005322f9 e8f235fcff               call       0x4f58f0
005322fe 8b10                     mov        edx, dword ptr [eax]
00532300 8b4004                   mov        eax, dword ptr [eax + 4]
00532303 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00532306 895170                   mov        dword ptr [ecx + 0x70], edx
00532309 894174                   mov        dword ptr [ecx + 0x74], eax
0053230c 8be5                     mov        esp, ebp
0053230e 5d                       pop        ebp
0053230f c3                       ret        
