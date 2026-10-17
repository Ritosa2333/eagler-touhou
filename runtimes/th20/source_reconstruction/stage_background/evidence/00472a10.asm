00472a10 55                       push       ebp
00472a11 8bec                     mov        ebp, esp
00472a13 51                       push       ecx
00472a14 894dfc                   mov        dword ptr [ebp - 4], ecx
00472a17 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472a1a 8a889a040000             mov        cl, byte ptr [eax + 0x49a]
00472a20 80e1fe                   and        cl, 0xfe
00472a23 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472a26 888a9a040000             mov        byte ptr [edx + 0x49a], cl
00472a2c 8be5                     mov        esp, ebp
00472a2e 5d                       pop        ebp
00472a2f c3                       ret        
