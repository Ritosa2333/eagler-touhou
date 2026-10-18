005076b0 55                       push       ebp
005076b1 8bec                     mov        ebp, esp
005076b3 83ec08                   sub        esp, 8
005076b6 894dfc                   mov        dword ptr [ebp - 4], ecx
005076b9 8b45fc                   mov        eax, dword ptr [ebp - 4]
005076bc c70000000000             mov        dword ptr [eax], 0
005076c2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005076c5 c7410400000000           mov        dword ptr [ecx + 4], 0
005076cc 8b55fc                   mov        edx, dword ptr [ebp - 4]
005076cf c7420800000000           mov        dword ptr [edx + 8], 0
005076d6 8b45fc                   mov        eax, dword ptr [ebp - 4]
005076d9 c7400c00000000           mov        dword ptr [eax + 0xc], 0
005076e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005076e3 c7411000000000           mov        dword ptr [ecx + 0x10], 0
005076ea 8b55fc                   mov        edx, dword ptr [ebp - 4]
005076ed c7421400000000           mov        dword ptr [edx + 0x14], 0
005076f4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005076f7 83c118                   add        ecx, 0x18
005076fa e81113f1ff               call       0x418a10
005076ff 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507702 83c018                   add        eax, 0x18
00507705 8945f8                   mov        dword ptr [ebp - 8], eax
00507708 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0050770b 51                       push       ecx
0050770c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0050770f e85c2ff4ff               call       0x44a670
00507714 90                       nop        
00507715 8b45fc                   mov        eax, dword ptr [ebp - 4]
00507718 8be5                     mov        esp, ebp
0050771a 5d                       pop        ebp
0050771b c3                       ret        
0050771c cc                       int3       
0050771d cc                       int3       
0050771e cc                       int3       
0050771f cc                       int3       
