00477910 55                       push       ebp
00477911 8bec                     mov        ebp, esp
00477913 51                       push       ecx
00477914 894dfc                   mov        dword ptr [ebp - 4], ecx
00477917 6a0a                     push       0xa
00477919 6a00                     push       0
0047791b 6a00                     push       0
0047791d 6a00                     push       0
0047791f 6a1e                     push       0x1e
00477921 6a02                     push       2
00477923 e898d7faff               call       0x4250c0
00477928 83c418                   add        esp, 0x18
0047792b 6a1e                     push       0x1e
0047792d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00477930 81c148330000             add        ecx, 0x3348
00477936 e8e5bbfaff               call       0x423520
0047793b 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047793e 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00477944 83c902                   or         ecx, 2
00477947 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047794a 898a44330000             mov        dword ptr [edx + 0x3344], ecx
00477950 8be5                     mov        esp, ebp
00477952 5d                       pop        ebp
00477953 c3                       ret        
00477954 cc                       int3       
00477955 cc                       int3       
00477956 cc                       int3       
00477957 cc                       int3       
00477958 cc                       int3       
00477959 cc                       int3       
0047795a cc                       int3       
0047795b cc                       int3       
0047795c cc                       int3       
0047795d cc                       int3       
0047795e cc                       int3       
0047795f cc                       int3       
