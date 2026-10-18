004c4f90 55                       push       ebp
004c4f91 8bec                     mov        ebp, esp
004c4f93 51                       push       ecx
004c4f94 894dfc                   mov        dword ptr [ebp - 4], ecx
004c4f97 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c4f9a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004c4f9d 8988440c0000             mov        dword ptr [eax + 0xc44], ecx
004c4fa3 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c4fa6 8b82440c0000             mov        eax, dword ptr [edx + 0xc44]
004c4fac 50                       push       eax
004c4fad b968a55b00               mov        ecx, 0x5ba568
004c4fb2 e8096cf4ff               call       0x40bbc0
004c4fb7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c4fba 8981480c0000             mov        dword ptr [ecx + 0xc48], eax
004c4fc0 8be5                     mov        esp, ebp
004c4fc2 5d                       pop        ebp
004c4fc3 c20400                   ret        4
004c4fc6 cc                       int3       
004c4fc7 cc                       int3       
004c4fc8 cc                       int3       
004c4fc9 cc                       int3       
004c4fca cc                       int3       
004c4fcb cc                       int3       
004c4fcc cc                       int3       
004c4fcd cc                       int3       
004c4fce cc                       int3       
004c4fcf cc                       int3       
