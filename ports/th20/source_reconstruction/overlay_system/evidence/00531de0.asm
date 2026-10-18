00531de0 55                       push       ebp
00531de1 8bec                     mov        ebp, esp
00531de3 83ec10                   sub        esp, 0x10
00531de6 894dfc                   mov        dword ptr [ebp - 4], ecx
00531de9 8b45fc                   mov        eax, dword ptr [ebp - 4]
00531dec c6403400                 mov        byte ptr [eax + 0x34], 0
00531df0 6a00                     push       0
00531df2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531df5 83c114                   add        ecx, 0x14
00531df8 e82317efff               call       0x423520
00531dfd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e00 c7411001000000           mov        dword ptr [ecx + 0x10], 1
00531e07 6a00                     push       0
00531e09 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e0c 83c124                   add        ecx, 0x24
00531e0f e80c17efff               call       0x423520
00531e14 90                       nop        
00531e15 8d4df0                   lea        ecx, [ebp - 0x10]
00531e18 e8f30fefff               call       0x422e10
00531e1d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00531e20 81c28c000000             add        edx, 0x8c
00531e26 8b08                     mov        ecx, dword ptr [eax]
00531e28 890a                     mov        dword ptr [edx], ecx
00531e2a 8b4804                   mov        ecx, dword ptr [eax + 4]
00531e2d 894a04                   mov        dword ptr [edx + 4], ecx
00531e30 8b4008                   mov        eax, dword ptr [eax + 8]
00531e33 894208                   mov        dword ptr [edx + 8], eax
00531e36 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e39 0f57c0                   xorps      xmm0, xmm0
00531e3c f30f11415c               movss      dword ptr [ecx + 0x5c], xmm0
00531e41 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e44 83c160                   add        ecx, 0x60
00531e47 e8247befff               call       0x429970
00531e4c 6a00                     push       0
00531e4e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e51 83c138                   add        ecx, 0x38
00531e54 e847f9edff               call       0x4117a0
00531e59 6a00                     push       0
00531e5b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e5e 83c13c                   add        ecx, 0x3c
00531e61 e8ba16efff               call       0x423520
00531e66 6a00                     push       0
00531e68 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531e6b 83c14c                   add        ecx, 0x4c
00531e6e e8ad16efff               call       0x423520
00531e73 90                       nop        
00531e74 8be5                     mov        esp, ebp
00531e76 5d                       pop        ebp
00531e77 c3                       ret        
00531e78 cc                       int3       
00531e79 cc                       int3       
00531e7a cc                       int3       
00531e7b cc                       int3       
00531e7c cc                       int3       
00531e7d cc                       int3       
00531e7e cc                       int3       
00531e7f cc                       int3       
