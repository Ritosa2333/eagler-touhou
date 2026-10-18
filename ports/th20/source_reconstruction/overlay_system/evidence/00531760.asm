00531760 55                       push       ebp
00531761 8bec                     mov        ebp, esp
00531763 83ec10                   sub        esp, 0x10
00531766 894dfc                   mov        dword ptr [ebp - 4], ecx
00531769 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053176c c6403400                 mov        byte ptr [eax + 0x34], 0
00531770 6a00                     push       0
00531772 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531775 83c114                   add        ecx, 0x14
00531778 e8a31defff               call       0x423520
0053177d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531780 c7411001000000           mov        dword ptr [ecx + 0x10], 1
00531787 6a00                     push       0
00531789 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053178c 83c124                   add        ecx, 0x24
0053178f e88c1defff               call       0x423520
00531794 90                       nop        
00531795 8d4df0                   lea        ecx, [ebp - 0x10]
00531798 e87316efff               call       0x422e10
0053179d 8b55fc                   mov        edx, dword ptr [ebp - 4]
005317a0 83c25c                   add        edx, 0x5c
005317a3 8b08                     mov        ecx, dword ptr [eax]
005317a5 890a                     mov        dword ptr [edx], ecx
005317a7 8b4804                   mov        ecx, dword ptr [eax + 4]
005317aa 894a04                   mov        dword ptr [edx + 4], ecx
005317ad 8b4008                   mov        eax, dword ptr [eax + 8]
005317b0 894208                   mov        dword ptr [edx + 8], eax
005317b3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005317b6 0f57c0                   xorps      xmm0, xmm0
005317b9 f30f114168               movss      dword ptr [ecx + 0x68], xmm0
005317be 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005317c1 83c16c                   add        ecx, 0x6c
005317c4 e8a781efff               call       0x429970
005317c9 6a00                     push       0
005317cb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005317ce 83c138                   add        ecx, 0x38
005317d1 e8caffedff               call       0x4117a0
005317d6 6a00                     push       0
005317d8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005317db 83c13c                   add        ecx, 0x3c
005317de e83d1defff               call       0x423520
005317e3 6a00                     push       0
005317e5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005317e8 83c14c                   add        ecx, 0x4c
005317eb e8301defff               call       0x423520
005317f0 90                       nop        
005317f1 8be5                     mov        esp, ebp
005317f3 5d                       pop        ebp
005317f4 c3                       ret        
005317f5 cc                       int3       
005317f6 cc                       int3       
005317f7 cc                       int3       
005317f8 cc                       int3       
005317f9 cc                       int3       
005317fa cc                       int3       
005317fb cc                       int3       
005317fc cc                       int3       
005317fd cc                       int3       
005317fe cc                       int3       
005317ff cc                       int3       
