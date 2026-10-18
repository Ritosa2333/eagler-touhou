00479f90 55                       push       ebp
00479f91 8bec                     mov        ebp, esp
00479f93 83ec08                   sub        esp, 8
00479f96 894df8                   mov        dword ptr [ebp - 8], ecx
00479f99 c745fc00000000           mov        dword ptr [ebp - 4], 0
00479fa0 eb09                     jmp        0x479fab
00479fa2 8b45fc                   mov        eax, dword ptr [ebp - 4]
00479fa5 83c001                   add        eax, 1
00479fa8 8945fc                   mov        dword ptr [ebp - 4], eax
00479fab 837dfc18                 cmp        dword ptr [ebp - 4], 0x18
00479faf 7d12                     jge        0x479fc3
00479fb1 694dfcd8000000           imul       ecx, dword ptr [ebp - 4], 0xd8
00479fb8 034df8                   add        ecx, dword ptr [ebp - 8]
00479fbb e850feffff               call       0x479e10
00479fc0 90                       nop        
00479fc1 ebdf                     jmp        0x479fa2
00479fc3 8be5                     mov        esp, ebp
00479fc5 5d                       pop        ebp
00479fc6 c3                       ret        
00479fc7 cc                       int3       
00479fc8 cc                       int3       
00479fc9 cc                       int3       
00479fca cc                       int3       
00479fcb cc                       int3       
00479fcc cc                       int3       
00479fcd cc                       int3       
00479fce cc                       int3       
00479fcf cc                       int3       
