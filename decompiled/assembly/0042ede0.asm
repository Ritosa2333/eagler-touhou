
// block 0042ede0
0042ede0  push       esi
0042ede1  push       edi
0042ede2  push       0x4f8
0042ede7  call       0x46c9ea

// block 0042edec
0042edec  mov        esi, eax
0042edee  xor        edi, edi
0042edf0  add        esp, 4
0042edf3  cmp        esi, edi
0042edf5  je         0x42ee2c

// block 0042edf7
0042edf7  lea        ecx, [esi + 0x30]
0042edfa  mov        dword ptr [esi + 0x10], 0x4a3738
0042ee01  mov        dword ptr [esi + 0x14], edi
0042ee04  mov        dword ptr [esi + 0x18], edi
0042ee07  mov        dword ptr [esi + 0x1c], edi
0042ee0a  mov        dword ptr [esi + 0x20], edi
0042ee0d  call       0x4027e0

// block 0042ee12
0042ee12  push       0x4f8
0042ee17  push       edi
0042ee18  push       esi
0042ee19  call       0x477420

// block 0042ee1e
0042ee1e  add        esp, 0xc
0042ee21  or         dword ptr [esi], 2
0042ee24  mov        dword ptr [0x4b44f8], esi
0042ee2a  jmp        0x42ee2e

// block 0042ee2c
0042ee2c  xor        esi, esi

// block 0042ee2e
0042ee2e  push       esi
0042ee2f  call       0x42eb10

// block 0042ee34
0042ee34  test       eax, eax
0042ee36  je         0x42ee50

// block 0042ee38
0042ee38  cmp        esi, edi
0042ee3a  je         0x42ee4b

// block 0042ee3c
0042ee3c  push       esi
0042ee3d  call       0x42ec00

// block 0042ee42
0042ee42  push       esi
0042ee43  call       0x46ca4f

// block 0042ee48
0042ee48  add        esp, 4

// block 0042ee4b
0042ee4b  pop        edi
0042ee4c  xor        eax, eax
0042ee4e  pop        esi
0042ee4f  ret        

// block 0042ee50
0042ee50  pop        edi
0042ee51  mov        eax, esi
0042ee53  pop        esi
0042ee54  ret        
