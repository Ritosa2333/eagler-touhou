
// block 00402660
00402660  push       esi
00402661  mov        esi, eax
00402663  call       0x402520

// block 00402668
00402668  mov        ecx, dword ptr [esp + 8]
0040266c  mov        eax, esi
0040266e  mov        edx, edi
00402670  mov        dword ptr [esi + 0x3f8], edi
00402676  call       0x454b80

// block 0040267b
0040267b  pop        esi
0040267c  ret        4
