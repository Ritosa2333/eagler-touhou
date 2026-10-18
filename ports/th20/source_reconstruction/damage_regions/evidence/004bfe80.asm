004bfe80 55                       push       ebp
004bfe81 8bec                     mov        ebp, esp
004bfe83 6aff                     push       -1
004bfe85 6800765600               push       0x567600
004bfe8a 64a100000000             mov        eax, dword ptr fs:[0]
004bfe90 50                       push       eax
004bfe91 51                       push       ecx
004bfe92 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004bfe97 33c5                     xor        eax, ebp
004bfe99 50                       push       eax
004bfe9a 8d45f4                   lea        eax, [ebp - 0xc]
004bfe9d 64a300000000             mov        dword ptr fs:[0], eax
004bfea3 894df0                   mov        dword ptr [ebp - 0x10], ecx
004bfea6 68b0ff4b00               push       0x4bffb0
004bfeab 6800010000               push       0x100
004bfeb0 68c4000000               push       0xc4
004bfeb5 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bfeb8 50                       push       eax
004bfeb9 e862bdf4ff               call       0x40bc20
004bfebe 90                       nop        
004bfebf 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004bfec2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004bfec5 64890d00000000           mov        dword ptr fs:[0], ecx
004bfecc 59                       pop        ecx
004bfecd 8be5                     mov        esp, ebp
004bfecf 5d                       pop        ebp
004bfed0 c3                       ret        
004bfed1 cc                       int3       
004bfed2 cc                       int3       
004bfed3 cc                       int3       
004bfed4 cc                       int3       
004bfed5 cc                       int3       
004bfed6 cc                       int3       
004bfed7 cc                       int3       
004bfed8 cc                       int3       
004bfed9 cc                       int3       
004bfeda cc                       int3       
004bfedb cc                       int3       
004bfedc cc                       int3       
004bfedd cc                       int3       
004bfede cc                       int3       
004bfedf cc                       int3       
