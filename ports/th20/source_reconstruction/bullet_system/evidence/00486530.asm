00486530 55                       push       ebp
00486531 8bec                     mov        ebp, esp
00486533 51                       push       ecx
00486534 894dfc                   mov        dword ptr [ebp - 4], ecx
00486537 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048653a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048653d 89889c6d2800             mov        dword ptr [eax + 0x286d9c], ecx
00486543 8b55fc                   mov        edx, dword ptr [ebp - 4]
00486546 8b829c6d2800             mov        eax, dword ptr [edx + 0x286d9c]
0048654c 50                       push       eax
0048654d b968a55b00               mov        ecx, 0x5ba568
00486552 e86956f8ff               call       0x40bbc0
00486557 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048655a 8981a06d2800             mov        dword ptr [ecx + 0x286da0], eax
00486560 8be5                     mov        esp, ebp
00486562 5d                       pop        ebp
00486563 c20400                   ret        4
00486566 cc                       int3       
00486567 cc                       int3       
00486568 cc                       int3       
00486569 cc                       int3       
0048656a cc                       int3       
0048656b cc                       int3       
0048656c cc                       int3       
0048656d cc                       int3       
0048656e cc                       int3       
0048656f cc                       int3       
