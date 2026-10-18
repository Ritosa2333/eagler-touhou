004b8300 55                       push       ebp
004b8301 8bec                     mov        ebp, esp
004b8303 83ec0c                   sub        esp, 0xc
004b8306 894dfc                   mov        dword ptr [ebp - 4], ecx
004b8309 c745f810270000           mov        dword ptr [ebp - 8], 0x2710
004b8310 c745f488130000           mov        dword ptr [ebp - 0xc], 0x1388
004b8317 8d45f8                   lea        eax, [ebp - 8]
004b831a 50                       push       eax
004b831b 8d4df4                   lea        ecx, [ebp - 0xc]
004b831e 51                       push       ecx
004b831f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b8322 83c248                   add        edx, 0x48
004b8325 52                       push       edx
004b8326 e8458efbff               call       0x471170
004b832b 83c40c                   add        esp, 0xc
004b832e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b8331 8b10                     mov        edx, dword ptr [eax]
004b8333 895148                   mov        dword ptr [ecx + 0x48], edx
004b8336 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b8339 8b4048                   mov        eax, dword ptr [eax + 0x48]
004b833c 8be5                     mov        esp, ebp
004b833e 5d                       pop        ebp
004b833f c3                       ret        
