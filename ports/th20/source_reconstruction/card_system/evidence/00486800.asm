00486800 55                       push       ebp
00486801 8bec                     mov        ebp, esp
00486803 6aff                     push       -1
00486805 688d7c5600               push       0x567c8d
0048680a 64a100000000             mov        eax, dword ptr fs:[0]
00486810 50                       push       eax
00486811 51                       push       ecx
00486812 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00486817 33c5                     xor        eax, ebp
00486819 50                       push       eax
0048681a 8d45f4                   lea        eax, [ebp - 0xc]
0048681d 64a300000000             mov        dword ptr fs:[0], eax
00486823 894df0                   mov        dword ptr [ebp - 0x10], ecx
00486826 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486829 e852d7f8ff               call       0x413f80
0048682e c745fc00000000           mov        dword ptr [ebp - 4], 0
00486835 8b4508                   mov        eax, dword ptr [ebp + 8]
00486838 50                       push       eax
00486839 e8424af8ff               call       0x40b280
0048683e 83c404                   add        esp, 4
00486841 50                       push       eax
00486842 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00486845 e8e6010000               call       0x486a30
0048684a 90                       nop        
0048684b c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
00486852 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00486855 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00486858 64890d00000000           mov        dword ptr fs:[0], ecx
0048685f 59                       pop        ecx
00486860 8be5                     mov        esp, ebp
00486862 5d                       pop        ebp
00486863 c20400                   ret        4
00486866 cc                       int3       
00486867 cc                       int3       
00486868 cc                       int3       
00486869 cc                       int3       
0048686a cc                       int3       
0048686b cc                       int3       
0048686c cc                       int3       
0048686d cc                       int3       
0048686e cc                       int3       
0048686f cc                       int3       
