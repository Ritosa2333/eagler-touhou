005306e0 55                       push       ebp
005306e1 8bec                     mov        ebp, esp
005306e3 51                       push       ecx
005306e4 894dfc                   mov        dword ptr [ebp - 4], ecx
005306e7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005306ea 83c138                   add        ecx, 0x38
005306ed e84e89f4ff               call       0x479040
005306f2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005306f5 e826280000               call       0x532f20
005306fa 33c0                     xor        eax, eax
005306fc 8be5                     mov        esp, ebp
005306fe 5d                       pop        ebp
005306ff c3                       ret        
