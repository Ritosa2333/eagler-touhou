005319f0 55                       push       ebp
005319f1 8bec                     mov        ebp, esp
005319f3 83ec0c                   sub        esp, 0xc
005319f6 894df4                   mov        dword ptr [ebp - 0xc], ecx
005319f9 6a00                     push       0
005319fb e88026f3ff               call       0x464080
00531a00 83c404                   add        esp, 4
00531a03 8bc8                     mov        ecx, eax
00531a05 e8f629f3ff               call       0x464400
00531a0a 0fb6c0                   movzx      eax, al
00531a0d 85c0                     test       eax, eax
00531a0f 741a                     je         0x531a2b
00531a11 6a00                     push       0
00531a13 e86826f3ff               call       0x464080
00531a18 83c404                   add        esp, 4
00531a1b 8945fc                   mov        dword ptr [ebp - 4], eax
00531a1e 6a0a                     push       0xa
00531a20 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00531a23 e8c8d0f8ff               call       0x4beaf0
00531a28 90                       nop        
00531a29 eb18                     jmp        0x531a43
00531a2b 6a00                     push       0
00531a2d e84e26f3ff               call       0x464080
00531a32 83c404                   add        esp, 4
00531a35 8945f8                   mov        dword ptr [ebp - 8], eax
00531a38 6a0f                     push       0xf
00531a3a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00531a3d e8aed0f8ff               call       0x4beaf0
00531a42 90                       nop        
00531a43 8be5                     mov        esp, ebp
00531a45 5d                       pop        ebp
00531a46 c3                       ret        
00531a47 cc                       int3       
00531a48 cc                       int3       
00531a49 cc                       int3       
00531a4a cc                       int3       
00531a4b cc                       int3       
00531a4c cc                       int3       
00531a4d cc                       int3       
00531a4e cc                       int3       
00531a4f cc                       int3       
