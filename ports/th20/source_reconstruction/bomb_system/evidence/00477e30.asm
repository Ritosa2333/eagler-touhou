00477e30 55                       push       ebp
00477e31 8bec                     mov        ebp, esp
00477e33 83ec18                   sub        esp, 0x18
00477e36 894dfc                   mov        dword ptr [ebp - 4], ecx
00477e39 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477e3c 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477e3f 51                       push       ecx
00477e40 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00477e46 e87579faff               call       0x41f7c0
00477e4b 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477e4e c7421400000000           mov        dword ptr [edx + 0x14], 0
00477e55 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477e58 8b4838                   mov        ecx, dword ptr [eax + 0x38]
00477e5b e8904cfaff               call       0x41caf0
00477e60 8bc8                     mov        ecx, eax
00477e62 e8c9a8f9ff               call       0x412730
00477e67 8b0c85ecef5a00           mov        ecx, dword ptr [eax*4 + 0x5aefec]
00477e6e 894df4                   mov        dword ptr [ebp - 0xc], ecx
00477e71 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477e74 8b4234                   mov        eax, dword ptr [edx + 0x34]
00477e77 8945f8                   mov        dword ptr [ebp - 8], eax
00477e7a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477e7d 51                       push       ecx
00477e7e ff55f4                   call       dword ptr [ebp - 0xc]
00477e81 83c404                   add        esp, 4
00477e84 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477e87 894214                   mov        dword ptr [edx + 0x14], eax
00477e8a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477e8d 8b4814                   mov        ecx, dword ptr [eax + 0x14]
00477e90 894df0                   mov        dword ptr [ebp - 0x10], ecx
00477e93 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477e96 8b4214                   mov        eax, dword ptr [edx + 0x14]
00477e99 8b08                     mov        ecx, dword ptr [eax]
00477e9b 8b5104                   mov        edx, dword ptr [ecx + 4]
00477e9e 8955ec                   mov        dword ptr [ebp - 0x14], edx
00477ea1 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477ea4 8b4834                   mov        ecx, dword ptr [eax + 0x34]
00477ea7 51                       push       ecx
00477ea8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00477eab ff55ec                   call       dword ptr [ebp - 0x14]
00477eae 8b55fc                   mov        edx, dword ptr [ebp - 4]
00477eb1 c7421801000000           mov        dword ptr [edx + 0x18], 1
00477eb8 6a00                     push       0
00477eba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477ebd 83c11c                   add        ecx, 0x1c
00477ec0 e85bb6faff               call       0x423520
00477ec5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00477ec8 8b4838                   mov        ecx, dword ptr [eax + 0x38]
00477ecb e8204cfaff               call       0x41caf0
00477ed0 8bc8                     mov        ecx, eax
00477ed2 e839980600               call       0x4e1710
00477ed7 6a00                     push       0
00477ed9 e8a2c1feff               call       0x464080
00477ede 83c404                   add        esp, 4
00477ee1 8945e8                   mov        dword ptr [ebp - 0x18], eax
00477ee4 6838ffffff               push       0xffffff38
00477ee9 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00477eec e86f000000               call       0x477f60
00477ef1 33c0                     xor        eax, eax
00477ef3 8be5                     mov        esp, ebp
00477ef5 5d                       pop        ebp
00477ef6 c3                       ret        
00477ef7 cc                       int3       
00477ef8 cc                       int3       
00477ef9 cc                       int3       
00477efa cc                       int3       
00477efb cc                       int3       
00477efc cc                       int3       
00477efd cc                       int3       
00477efe cc                       int3       
00477eff cc                       int3       
