0044cfb0 55                       push       ebp
0044cfb1 8bec                     mov        ebp, esp
0044cfb3 b9404d5c00               mov        ecx, 0x5c4d40
0044cfb8 e88355fcff               call       0x412540
0044cfbd 85c0                     test       eax, eax
0044cfbf 7437                     je         0x44cff8
0044cfc1 8b4508                   mov        eax, dword ptr [ebp + 8]
0044cfc4 833c8508e2560015         cmp        dword ptr [eax*4 + 0x56e208], 0x15
0044cfcc 740d                     je         0x44cfdb
0044cfce 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0044cfd1 833c8d08e2560000         cmp        dword ptr [ecx*4 + 0x56e208], 0
0044cfd9 7509                     jne        0x44cfe4
0044cfdb c7450805000000           mov        dword ptr [ebp + 8], 5
0044cfe2 eb14                     jmp        0x44cff8
0044cfe4 8b5508                   mov        edx, dword ptr [ebp + 8]
0044cfe7 833c9508e2560014         cmp        dword ptr [edx*4 + 0x56e208], 0x14
0044cfef 7507                     jne        0x44cff8
0044cff1 c7450803000000           mov        dword ptr [ebp + 8], 3
0044cff8 8b4508                   mov        eax, dword ptr [ebp + 8]
0044cffb 5d                       pop        ebp
0044cffc c3                       ret        
0044cffd cc                       int3       
0044cffe cc                       int3       
0044cfff cc                       int3       
