
// block 00421430
00421430  push       ebx
00421431  push       esi
00421432  mov        esi, dword ptr [0x4b43e4]
00421438  mov        eax, dword ptr [esi + 0x6cac]
0042143e  push       eax
0042143f  mov        ebx, 1
00421444  call       0x461970

// block 00421449
00421449  mov        ecx, dword ptr [esi + 0x6cb0]
0042144f  push       ecx
00421450  call       0x461970

// block 00421455
00421455  fldz       
00421457  and        dword ptr [esi + 0x6d18], 0xfffffdff
00421461  mov        eax, dword ptr [esi + 0x6d2c]
00421467  xor        ecx, ecx
00421469  test       bl, al
0042146b  jne        0x421495

// block 0042146d
0042146d  or         eax, ebx
0042146f  fst        dword ptr [esi + 0x6d24]
00421475  mov        dword ptr [esi + 0x6d20], ecx
0042147b  mov        dword ptr [esi + 0x6d1c], 0xfff0bdc1
00421485  mov        dword ptr [esi + 0x6d28], 0x4b2ed0
0042148f  mov        dword ptr [esi + 0x6d2c], eax

// block 00421495
00421495  fstp       dword ptr [esi + 0x6d24]
0042149b  mov        dword ptr [esi + 0x6d20], ecx
004214a1  mov        dword ptr [esi + 0x6d1c], 0xffffffff
004214ab  pop        esi
004214ac  pop        ebx
004214ad  ret        
