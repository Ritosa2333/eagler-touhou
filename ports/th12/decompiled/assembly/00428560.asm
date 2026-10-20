
// block 00428560
00428560  push       esi
00428561  push       edi
00428562  mov        esi, eax
00428564  call       0x427ec0

// block 00428569
00428569  push       0x208
0042856e  lea        edi, [esi + 0x454]
00428574  push       0
00428576  push       edi
00428577  mov        dword ptr [esi], 0x4a0704
0042857d  call       0x477420

// block 00428582
00428582  fld        dword ptr [0x4a3e54]
00428588  add        esp, 0xc
0042858b  fstp       dword ptr [edi + 0x2c]
0042858e  lea        ecx, [esi + 0x660]
00428594  call       0x4027e0

// block 00428599
00428599  lea        ecx, [esi + 0xb14]
0042859f  call       0x4027e0

// block 004285a4
004285a4  pop        edi
004285a5  mov        eax, esi
004285a7  pop        esi
004285a8  ret        
