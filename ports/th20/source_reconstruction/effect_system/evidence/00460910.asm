00460910 55                       push       ebp
00460911 8bec                     mov        ebp, esp
00460913 51                       push       ecx
00460914 894dfc                   mov        dword ptr [ebp - 4], ecx
00460917 8b4508                   mov        eax, dword ptr [ebp + 8]
0046091a 50                       push       eax
0046091b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046091e e83d62feff               call       0x446b60
00460923 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460926 c70130ef5600             mov        dword ptr [ecx], 0x56ef30
0046092c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046092f 83c108                   add        ecx, 8
00460932 e8f9f7ffff               call       0x460130
00460937 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046093a 81c188020000             add        ecx, 0x288
00460940 e89bf7ffff               call       0x4600e0
00460945 8b55fc                   mov        edx, dword ptr [ebp - 4]
00460948 0f57c0                   xorps      xmm0, xmm0
0046094b f30f1182c8030000         movss      dword ptr [edx + 0x3c8], xmm0
00460953 8b45fc                   mov        eax, dword ptr [ebp - 4]
00460956 0f57c0                   xorps      xmm0, xmm0
00460959 f30f1180cc030000         movss      dword ptr [eax + 0x3cc], xmm0
00460961 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460964 81c1d0030000             add        ecx, 0x3d0
0046096a e82124fcff               call       0x422d90
0046096f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460972 c781e003000000000000     mov        dword ptr [ecx + 0x3e0], 0
0046097c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046097f 81c1e4030000             add        ecx, 0x3e4
00460985 e88624fcff               call       0x422e10
0046098a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0046098d c682f003000001           mov        byte ptr [edx + 0x3f0], 1
00460994 8b45fc                   mov        eax, dword ptr [ebp - 4]
00460997 8be5                     mov        esp, ebp
00460999 5d                       pop        ebp
0046099a c20400                   ret        4
0046099d cc                       int3       
0046099e cc                       int3       
0046099f cc                       int3       
