00531650 55                       push       ebp
00531651 8bec                     mov        ebp, esp
00531653 51                       push       ecx
00531654 894dfc                   mov        dword ptr [ebp - 4], ecx
00531657 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053165a e871e4ffff               call       0x52fad0
0053165f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531662 c700905c5700             mov        dword ptr [eax], 0x575c90
00531668 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053166b 83c138                   add        ecx, 0x38
0053166e e84d46efff               call       0x425cc0
00531673 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531676 83c13c                   add        ecx, 0x3c
00531679 e81217efff               call       0x422d90
0053167e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531681 83c14c                   add        ecx, 0x4c
00531684 e80717efff               call       0x422d90
00531689 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053168c 83c15c                   add        ecx, 0x5c
0053168f e87c17efff               call       0x422e10
00531694 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531697 0f57c0                   xorps      xmm0, xmm0
0053169a f30f114168               movss      dword ptr [ecx + 0x68], xmm0
0053169f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005316a2 83c16c                   add        ecx, 0x6c
005316a5 e83663f1ff               call       0x4479e0
005316aa 90                       nop        
005316ab 8b45fc                   mov        eax, dword ptr [ebp - 4]
005316ae 8be5                     mov        esp, ebp
005316b0 5d                       pop        ebp
005316b1 c3                       ret        
005316b2 cc                       int3       
005316b3 cc                       int3       
005316b4 cc                       int3       
005316b5 cc                       int3       
005316b6 cc                       int3       
005316b7 cc                       int3       
005316b8 cc                       int3       
005316b9 cc                       int3       
005316ba cc                       int3       
005316bb cc                       int3       
005316bc cc                       int3       
005316bd cc                       int3       
005316be cc                       int3       
005316bf cc                       int3       
