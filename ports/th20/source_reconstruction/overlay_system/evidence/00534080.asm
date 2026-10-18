00534080 55                       push       ebp
00534081 8bec                     mov        ebp, esp
00534083 51                       push       ecx
00534084 894dfc                   mov        dword ptr [ebp - 4], ecx
00534087 6a00                     push       0
00534089 e8f2fff2ff               call       0x464080
0053408e 83c404                   add        esp, 4
00534091 8bc8                     mov        ecx, eax
00534093 e878d7eeff               call       0x421810
00534098 50                       push       eax
00534099 8b4508                   mov        eax, dword ptr [ebp + 8]
0053409c 50                       push       eax
0053409d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005340a0 e87b030000               call       0x534420
005340a5 6a00                     push       0
005340a7 e8d4fff2ff               call       0x464080
005340ac 83c404                   add        esp, 4
005340af 8bc8                     mov        ecx, eax
005340b1 e83a8aeeff               call       0x41caf0
005340b6 50                       push       eax
005340b7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005340ba 51                       push       ecx
005340bb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005340be e80d070000               call       0x5347d0
005340c3 6a00                     push       0
005340c5 e8b6fff2ff               call       0x464080
005340ca 83c404                   add        esp, 4
005340cd 8bc8                     mov        ecx, eax
005340cf e8fc89eeff               call       0x41cad0
005340d4 50                       push       eax
005340d5 8b5508                   mov        edx, dword ptr [ebp + 8]
005340d8 52                       push       edx
005340d9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005340dc e8ef030000               call       0x5344d0
005340e1 6a00                     push       0
005340e3 e898fff2ff               call       0x464080
005340e8 83c404                   add        esp, 4
005340eb 8bc8                     mov        ecx, eax
005340ed e81ee6edff               call       0x412710
005340f2 50                       push       eax
005340f3 8b4508                   mov        eax, dword ptr [ebp + 8]
005340f6 50                       push       eax
005340f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005340fa e801050000               call       0x534600
005340ff 90                       nop        
00534100 8be5                     mov        esp, ebp
00534102 5d                       pop        ebp
00534103 c20400                   ret        4
00534106 cc                       int3       
00534107 cc                       int3       
00534108 cc                       int3       
00534109 cc                       int3       
0053410a cc                       int3       
0053410b cc                       int3       
0053410c cc                       int3       
0053410d cc                       int3       
0053410e cc                       int3       
0053410f cc                       int3       
