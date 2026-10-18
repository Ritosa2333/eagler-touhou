00530b90 55                       push       ebp
00530b91 8bec                     mov        ebp, esp
00530b93 51                       push       ecx
00530b94 894dfc                   mov        dword ptr [ebp - 4], ecx
00530b97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530b9a e831efffff               call       0x52fad0
00530b9f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00530ba2 c700245a5700             mov        dword ptr [eax], 0x575a24
00530ba8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530bab 83c138                   add        ecx, 0x38
00530bae e80d51efff               call       0x425cc0
00530bb3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00530bb6 0f57c0                   xorps      xmm0, xmm0
00530bb9 f30f11413c               movss      dword ptr [ecx + 0x3c], xmm0
00530bbe 8b45fc                   mov        eax, dword ptr [ebp - 4]
00530bc1 8be5                     mov        esp, ebp
00530bc3 5d                       pop        ebp
00530bc4 c3                       ret        
00530bc5 cc                       int3       
00530bc6 cc                       int3       
00530bc7 cc                       int3       
00530bc8 cc                       int3       
00530bc9 cc                       int3       
00530bca cc                       int3       
00530bcb cc                       int3       
00530bcc cc                       int3       
00530bcd cc                       int3       
00530bce cc                       int3       
00530bcf cc                       int3       
