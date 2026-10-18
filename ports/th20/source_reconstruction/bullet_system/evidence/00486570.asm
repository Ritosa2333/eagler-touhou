00486570 55                       push       ebp
00486571 8bec                     mov        ebp, esp
00486573 51                       push       ecx
00486574 894dfc                   mov        dword ptr [ebp - 4], ecx
00486577 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048657a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048657d 89881c050000             mov        dword ptr [eax + 0x51c], ecx
00486583 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486586 8b821c050000             mov        eax, dword ptr [edx + 0x51c]
0048658c 50                       push       eax
0048658d b968a55b00               mov        ecx, 0x5ba568
00486592 e82956f8ff               call       0x40bbc0
00486597 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048659a 898120050000             mov        dword ptr [ecx + 0x520], eax
004865a0 8be5                     mov        esp, ebp
004865a2 5d                       pop        ebp
004865a3 c20400                   ret        4
004865a6 cc                       int3       
004865a7 cc                       int3       
004865a8 cc                       int3       
004865a9 cc                       int3       
004865aa cc                       int3       
004865ab cc                       int3       
004865ac cc                       int3       
004865ad cc                       int3       
004865ae cc                       int3       
004865af cc                       int3       
