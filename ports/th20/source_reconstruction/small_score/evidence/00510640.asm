00510640 55                       push       ebp
00510641 8bec                     mov        ebp, esp
00510643 83ec0c                   sub        esp, 0xc
00510646 894dfc                   mov        dword ptr [ebp - 4], ecx
00510649 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051064c 8b0d98065c00             mov        ecx, dword ptr [0x5c0698]
00510652 8b9144a20100             mov        edx, dword ptr [ecx + 0x1a244]
00510658 895010                   mov        dword ptr [eax + 0x10], edx
0051065b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051065e 50                       push       eax
0051065f 6810085100               push       0x510810
00510664 6a19                     push       0x19
00510666 e8a51cf0ff               call       0x412310
0051066b 83c40c                   add        esp, 0xc
0051066e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00510671 894108                   mov        dword ptr [ecx + 8], eax
00510674 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510677 52                       push       edx
00510678 6820085100               push       0x510820
0051067d 6a33                     push       0x33
0051067f e82c1df0ff               call       0x4123b0
00510684 83c40c                   add        esp, 0xc
00510687 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051068a 89410c                   mov        dword ptr [ecx + 0xc], eax
0051068d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00510690 8b4210                   mov        eax, dword ptr [edx + 0x10]
00510693 8945f8                   mov        dword ptr [ebp - 8], eax
00510696 6821010000               push       0x121
0051069b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051069e 83c11c                   add        ecx, 0x1c
005106a1 51                       push       ecx
005106a2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
005106a5 e83602f6ff               call       0x4708e0
005106aa 8b55fc                   mov        edx, dword ptr [ebp - 4]
005106ad 8a82bf040000             mov        al, byte ptr [edx + 0x4bf]
005106b3 24fc                     and        al, 0xfc
005106b5 0c02                     or         al, 2
005106b7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005106ba 8881bf040000             mov        byte ptr [ecx + 0x4bf], al
005106c0 8b55fc                   mov        edx, dword ptr [ebp - 4]
005106c3 0fb682c0040000           movzx      eax, byte ptr [edx + 0x4c0]
005106ca 2400                     and        al, 0
005106cc 0c03                     or         al, 3
005106ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005106d1 8881c0040000             mov        byte ptr [ecx + 0x4c0], al
005106d7 8b5508                   mov        edx, dword ptr [ebp + 8]
005106da 52                       push       edx
005106db 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005106de e8ad010000               call       0x510890
005106e3 8b4508                   mov        eax, dword ptr [ebp + 8]
005106e6 50                       push       eax
005106e7 b968a55b00               mov        ecx, 0x5ba568
005106ec e8cfb4efff               call       0x40bbc0
005106f1 8945f4                   mov        dword ptr [ebp - 0xc], eax
005106f4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005106f7 51                       push       ecx
005106f8 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005106fb e870d7f0ff               call       0x41de70
00510700 33c0                     xor        eax, eax
00510702 8be5                     mov        esp, ebp
00510704 5d                       pop        ebp
00510705 c20400                   ret        4
00510708 cc                       int3       
00510709 cc                       int3       
0051070a cc                       int3       
0051070b cc                       int3       
0051070c cc                       int3       
0051070d cc                       int3       
0051070e cc                       int3       
0051070f cc                       int3       
