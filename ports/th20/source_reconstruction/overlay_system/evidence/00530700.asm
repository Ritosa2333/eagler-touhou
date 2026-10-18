00530700 55                       push       ebp
00530701 8bec                     mov        ebp, esp
00530703 51                       push       ecx
00530704 894dfc                   mov        dword ptr [ebp - 4], ecx
00530707 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053070a c6403400                 mov        byte ptr [eax + 0x34], 0
0053070e 6a00                     push       0
00530710 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530713 83c114                   add        ecx, 0x14
00530716 e8052eefff               call       0x423520
0053071b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053071e c7411001000000           mov        dword ptr [ecx + 0x10], 1
00530725 6a00                     push       0
00530727 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053072a 83c124                   add        ecx, 0x24
0053072d e8ee2defff               call       0x423520
00530732 6a00                     push       0
00530734 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530737 83c138                   add        ecx, 0x38
0053073a e86110eeff               call       0x4117a0
0053073f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00530742 0f57c0                   xorps      xmm0, xmm0
00530745 f30f11423c               movss      dword ptr [edx + 0x3c], xmm0
0053074a 8be5                     mov        esp, ebp
0053074c 5d                       pop        ebp
0053074d c3                       ret        
0053074e cc                       int3       
0053074f cc                       int3       
