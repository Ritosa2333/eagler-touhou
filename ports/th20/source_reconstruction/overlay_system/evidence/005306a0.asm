005306a0 55                       push       ebp
005306a1 8bec                     mov        ebp, esp
005306a3 51                       push       ecx
005306a4 894dfc                   mov        dword ptr [ebp - 4], ecx
005306a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005306aa e821f4ffff               call       0x52fad0
005306af 8b45fc                   mov        eax, dword ptr [ebp - 4]
005306b2 c70058595700             mov        dword ptr [eax], 0x575958
005306b8 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005306bb 83c138                   add        ecx, 0x38
005306be e8fd55efff               call       0x425cc0
005306c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005306c6 0f57c0                   xorps      xmm0, xmm0
005306c9 f30f11413c               movss      dword ptr [ecx + 0x3c], xmm0
005306ce 8b45fc                   mov        eax, dword ptr [ebp - 4]
005306d1 8be5                     mov        esp, ebp
005306d3 5d                       pop        ebp
005306d4 c3                       ret        
005306d5 cc                       int3       
005306d6 cc                       int3       
005306d7 cc                       int3       
005306d8 cc                       int3       
005306d9 cc                       int3       
005306da cc                       int3       
005306db cc                       int3       
005306dc cc                       int3       
005306dd cc                       int3       
005306de cc                       int3       
005306df cc                       int3       
