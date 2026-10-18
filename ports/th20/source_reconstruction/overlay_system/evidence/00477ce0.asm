00477ce0 55                       push       ebp
00477ce1 8bec                     mov        ebp, esp
00477ce3 51                       push       ecx
00477ce4 894dfc                   mov        dword ptr [ebp - 4], ecx
00477ce7 33c0                     xor        eax, eax
00477ce9 8be5                     mov        esp, ebp
00477ceb 5d                       pop        ebp
00477cec c20800                   ret        8
00477cef cc                       int3       
