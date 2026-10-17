00486430 55                       push       ebp
00486431 8bec                     mov        ebp, esp
00486433 83ec0c                   sub        esp, 0xc
00486436 894dfc                   mov        dword ptr [ebp - 4], ecx
00486439 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048643c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048643f 8988b4000000             mov        dword ptr [eax + 0xb4], ecx
00486445 c745f864000000           mov        dword ptr [ebp - 8], 0x64
0048644c c745f400000000           mov        dword ptr [ebp - 0xc], 0
00486453 8d55f8                   lea        edx, [ebp - 8]
00486456 52                       push       edx
00486457 8d45f4                   lea        eax, [ebp - 0xc]
0048645a 50                       push       eax
0048645b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048645e 81c1b4000000             add        ecx, 0xb4
00486464 51                       push       ecx
00486465 e806adfeff               call       0x471170
0048646a 83c40c                   add        esp, 0xc
0048646d 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486470 8b00                     mov        eax, dword ptr [eax]
00486472 8982b4000000             mov        dword ptr [edx + 0xb4], eax
00486478 8be5                     mov        esp, ebp
0048647a 5d                       pop        ebp
0048647b c20400                   ret        4
0048647e cc                       int3       
0048647f cc                       int3       
