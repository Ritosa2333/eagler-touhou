005383f0 55                       push       ebp
005383f1 8bec                     mov        ebp, esp
005383f3 83ec24                   sub        esp, 0x24
005383f6 894ddc                   mov        dword ptr [ebp - 0x24], ecx
005383f9 c745f4cc695c00           mov        dword ptr [ebp - 0xc], 0x5c69cc
00538400 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00538403 e89839edff               call       0x40bda0
00538408 8945fc                   mov        dword ptr [ebp - 4], eax
0053840b 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0053840e e85d95eeff               call       0x421970
00538413 8945e8                   mov        dword ptr [ebp - 0x18], eax
00538416 eb09                     jmp        0x538421
00538418 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053841b 83c004                   add        eax, 4
0053841e 8945fc                   mov        dword ptr [ebp - 4], eax
00538421 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00538424 3b4de8                   cmp        ecx, dword ptr [ebp - 0x18]
00538427 741b                     je         0x538444
00538429 8b55fc                   mov        edx, dword ptr [ebp - 4]
0053842c 8955f0                   mov        dword ptr [ebp - 0x10], edx
0053842f 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00538432 e89978f1ff               call       0x44fcd0
00538437 6a00                     push       0
00538439 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0053843c e85f93edff               call       0x4117a0
00538441 90                       nop        
00538442 ebd4                     jmp        0x538418
00538444 c745ece0695c00           mov        dword ptr [ebp - 0x14], 0x5c69e0
0053844b 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053844e e84d39edff               call       0x40bda0
00538453 8945f8                   mov        dword ptr [ebp - 8], eax
00538456 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00538459 e81295eeff               call       0x421970
0053845e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00538461 eb09                     jmp        0x53846c
00538463 8b45f8                   mov        eax, dword ptr [ebp - 8]
00538466 83c004                   add        eax, 4
00538469 8945f8                   mov        dword ptr [ebp - 8], eax
0053846c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0053846f 3b4de4                   cmp        ecx, dword ptr [ebp - 0x1c]
00538472 7411                     je         0x538485
00538474 8b55f8                   mov        edx, dword ptr [ebp - 8]
00538477 8955e0                   mov        dword ptr [ebp - 0x20], edx
0053847a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0053847d e87e9af8ff               call       0x4c1f00
00538482 90                       nop        
00538483 ebde                     jmp        0x538463
00538485 8be5                     mov        esp, ebp
00538487 5d                       pop        ebp
00538488 c3                       ret        
00538489 cc                       int3       
0053848a cc                       int3       
0053848b cc                       int3       
0053848c cc                       int3       
0053848d cc                       int3       
0053848e cc                       int3       
0053848f cc                       int3       
