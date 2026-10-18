004d2360 55                       push       ebp
004d2361 8bec                     mov        ebp, esp
004d2363 83ec0c                   sub        esp, 0xc
004d2366 894dfc                   mov        dword ptr [ebp - 4], ecx
004d2369 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d236c 8b8830130000             mov        ecx, dword ptr [eax + 0x1330]
004d2372 894df8                   mov        dword ptr [ebp - 8], ecx
004d2375 837df800                 cmp        dword ptr [ebp - 8], 0
004d2379 7426                     je         0x4d23a1
004d237b 8b55f8                   mov        edx, dword ptr [ebp - 8]
004d237e 8b02                     mov        eax, dword ptr [edx]
004d2380 8945f4                   mov        dword ptr [ebp - 0xc], eax
004d2383 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004d2386 51                       push       ecx
004d2387 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d238d e8de17f4ff               call       0x413b70
004d2392 c745f800000000           mov        dword ptr [ebp - 8], 0
004d2399 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
004d239c 8955f8                   mov        dword ptr [ebp - 8], edx
004d239f ebd4                     jmp        0x4d2375
004d23a1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d23a4 c7803013000000000000     mov        dword ptr [eax + 0x1330], 0
004d23ae 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d23b1 83b92c13000000           cmp        dword ptr [ecx + 0x132c], 0
004d23b8 7422                     je         0x4d23dc
004d23ba 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d23bd 8b822c130000             mov        eax, dword ptr [edx + 0x132c]
004d23c3 50                       push       eax
004d23c4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d23ca e8a1d2f4ff               call       0x41f670
004d23cf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004d23d2 c7812c13000000000000     mov        dword ptr [ecx + 0x132c], 0
004d23dc 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d23df 83ba2813000000           cmp        dword ptr [edx + 0x1328], 0
004d23e6 7422                     je         0x4d240a
004d23e8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004d23eb 8b8828130000             mov        ecx, dword ptr [eax + 0x1328]
004d23f1 51                       push       ecx
004d23f2 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004d23f8 e873d2f4ff               call       0x41f670
004d23fd 8b55fc                   mov        edx, dword ptr [ebp - 4]
004d2400 c7822813000000000000     mov        dword ptr [edx + 0x1328], 0
004d240a 33c0                     xor        eax, eax
004d240c 8be5                     mov        esp, ebp
004d240e 5d                       pop        ebp
004d240f c3                       ret        
