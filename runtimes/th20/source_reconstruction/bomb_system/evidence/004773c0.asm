004773c0 55                       push       ebp
004773c1 8bec                     mov        ebp, esp
004773c3 83ec08                   sub        esp, 8
004773c6 894df8                   mov        dword ptr [ebp - 8], ecx
004773c9 8b45f8                   mov        eax, dword ptr [ebp - 8]
004773cc 8b8830330000             mov        ecx, dword ptr [eax + 0x3330]
004773d2 894dfc                   mov        dword ptr [ebp - 4], ecx
004773d5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004773d8 833a00                   cmp        dword ptr [edx], 0
004773db 7c62                     jl         0x47743f
004773dd 8b45fc                   mov        eax, dword ptr [ebp - 4]
004773e0 0fbf4804                 movsx      ecx, word ptr [eax + 4]
004773e4 83f910                   cmp        ecx, 0x10
004773e7 7547                     jne        0x477430
004773e9 ba04000000               mov        edx, 4
004773ee 6bc200                   imul       eax, edx, 0
004773f1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004773f4 8b540108                 mov        edx, dword ptr [ecx + eax + 8]
004773f8 3b5508                   cmp        edx, dword ptr [ebp + 8]
004773fb 7533                     jne        0x477430
004773fd 8b45f8                   mov        eax, dword ptr [ebp - 8]
00477400 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477403 2b8830330000             sub        ecx, dword ptr [eax + 0x3330]
00477409 8b55f8                   mov        edx, dword ptr [ebp - 8]
0047740c 894a20                   mov        dword ptr [edx + 0x20], ecx
0047740f 8b45f8                   mov        eax, dword ptr [ebp - 8]
00477412 8b8830330000             mov        ecx, dword ptr [eax + 0x3330]
00477418 8b55f8                   mov        edx, dword ptr [ebp - 8]
0047741b 8b4220                   mov        eax, dword ptr [edx + 0x20]
0047741e 8b0c01                   mov        ecx, dword ptr [ecx + eax]
00477421 51                       push       ecx
00477422 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477425 83c110                   add        ecx, 0x10
00477428 e8f3c0faff               call       0x423520
0047742d 90                       nop        
0047742e eb0f                     jmp        0x47743f
00477430 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477433 0fbf4206                 movsx      eax, word ptr [edx + 6]
00477437 0345fc                   add        eax, dword ptr [ebp - 4]
0047743a 8945fc                   mov        dword ptr [ebp - 4], eax
0047743d eb96                     jmp        0x4773d5
0047743f 8be5                     mov        esp, ebp
00477441 5d                       pop        ebp
00477442 c20400                   ret        4
00477445 cc                       int3       
00477446 cc                       int3       
00477447 cc                       int3       
00477448 cc                       int3       
00477449 cc                       int3       
0047744a cc                       int3       
0047744b cc                       int3       
0047744c cc                       int3       
0047744d cc                       int3       
0047744e cc                       int3       
0047744f cc                       int3       
