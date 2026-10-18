00423ee0 55                       push       ebp
00423ee1 8bec                     mov        ebp, esp
00423ee3 83ec10                   sub        esp, 0x10
00423ee6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00423eeb 33c5                     xor        eax, ebp
00423eed 8945fc                   mov        dword ptr [ebp - 4], eax
00423ef0 894df4                   mov        dword ptr [ebp - 0xc], ecx
00423ef3 6a0a                     push       0xa
00423ef5 b940025c00               mov        ecx, 0x5c0240
00423efa e81184feff               call       0x40c310
00423eff 50                       push       eax
00423f00 8d4df8                   lea        ecx, [ebp - 8]
00423f03 e82878feff               call       0x40b730
00423f08 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00423f0b 83c104                   add        ecx, 4
00423f0e e8ddf6ffff               call       0x4235f0
00423f13 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00423f16 894114                   mov        dword ptr [ecx + 0x14], eax
00423f19 8b55f4                   mov        edx, dword ptr [ebp - 0xc]
00423f1c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00423f1f 8b4214                   mov        eax, dword ptr [edx + 0x14]
00423f22 33d2                     xor        edx, edx
00423f24 f77110                   div        dword ptr [ecx + 0x10]
00423f27 8955f0                   mov        dword ptr [ebp - 0x10], edx
00423f2a 8d4df8                   lea        ecx, [ebp - 8]
00423f2d e8ce79feff               call       0x40b900
00423f32 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00423f35 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00423f38 33cd                     xor        ecx, ebp
00423f3a e8afea1100               call       0x5429ee
00423f3f 8be5                     mov        esp, ebp
00423f41 5d                       pop        ebp
00423f42 c3                       ret        
00423f43 cc                       int3       
00423f44 cc                       int3       
00423f45 cc                       int3       
00423f46 cc                       int3       
00423f47 cc                       int3       
00423f48 cc                       int3       
00423f49 cc                       int3       
00423f4a cc                       int3       
00423f4b cc                       int3       
00423f4c cc                       int3       
00423f4d cc                       int3       
00423f4e cc                       int3       
00423f4f cc                       int3       
