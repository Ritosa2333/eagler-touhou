00511ed0 55                       push       ebp
00511ed1 8bec                     mov        ebp, esp
00511ed3 51                       push       ecx
00511ed4 894dfc                   mov        dword ptr [ebp - 4], ecx
00511ed7 8b4508                   mov        eax, dword ptr [ebp + 8]
00511eda 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
00511ee0 e8bbeef8ff               call       0x4a0da0
00511ee5 8bc8                     mov        ecx, eax
00511ee7 e8c48ef9ff               call       0x4aadb0
00511eec 50                       push       eax
00511eed 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00511ef0 e86bbef4ff               call       0x45dd60
00511ef5 33c0                     xor        eax, eax
00511ef7 8be5                     mov        esp, ebp
00511ef9 5d                       pop        ebp
00511efa c20400                   ret        4
00511efd cc                       int3       
00511efe cc                       int3       
00511eff cc                       int3       
