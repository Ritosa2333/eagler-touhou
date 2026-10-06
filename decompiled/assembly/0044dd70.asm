
// block 0044dd70
0044dd70  sub        esp, 0x30
0044dd73  mov        eax, dword ptr [0x4ad138]
0044dd78  xor        eax, esp
0044dd7a  mov        dword ptr [esp + 0x2c], eax
0044dd7e  push       ebx
0044dd7f  push       edi
0044dd80  lea        eax, [esp + 8]
0044dd84  push       eax
0044dd85  mov        edi, ecx
0044dd87  call       0x44d420

// block 0044dd8c
0044dd8c  mov        eax, dword ptr [esp + 8]
0044dd90  mov        ecx, dword ptr [esp + 0x44]
0044dd94  mov        dword ptr [esp + 0xc], 0
0044dd9c  mov        dword ptr [esp + 0x10], ecx
0044dda0  test       eax, eax
0044dda2  je         0x44ddb3

// block 0044dda4
0044dda4  push       eax
0044dda5  call       dword ptr [0x498030]

// block 0044ddab
0044ddab  mov        dword ptr [esp + 8], 0

// block 0044ddb3
0044ddb3  cmp        dword ptr [esp + 0x2c], 0x10
0044ddb8  mov        eax, dword ptr [esp + 0x18]
0044ddbc  jae        0x44ddc2

// block 0044ddbe
0044ddbe  lea        eax, [esp + 0x18]

// block 0044ddc2
0044ddc2  mov        edx, dword ptr [esp + 0x30]
0044ddc6  push       eax
0044ddc7  mov        eax, dword ptr [esp + 0x10]
0044ddcb  push       0x30
0044ddcd  mov        ecx, dword ptr [esp + 0x18]
0044ddd1  push       0
0044ddd3  push       0
0044ddd5  push       0
0044ddd7  push       1
0044ddd9  push       0
0044dddb  push       0
0044dddd  push       0
0044dddf  push       edx
0044dde0  push       0
0044dde2  push       0
0044dde4  push       eax
0044dde5  push       ecx
0044dde6  call       dword ptr [0x498034]

// block 0044ddec
0044ddec  mov        edx, dword ptr [esp + 0x4c]
0044ddf0  mov        ecx, dword ptr [esp + 0x48]
0044ddf4  push       edx
0044ddf5  mov        edx, dword ptr [esp + 0x44]
0044ddf9  push       ecx
0044ddfa  push       eax
0044ddfb  mov        dword ptr [esp + 0x14], eax
0044ddff  mov        eax, dword ptr [esp + 0x48]
0044de03  push       edx
0044de04  push       eax
0044de05  mov        eax, esi
0044de07  call       0x44de50

// block 0044de0c
0044de0c  mov        bl, al
0044de0e  mov        eax, dword ptr [esp + 8]
0044de12  test       eax, eax
0044de14  je         0x44de25

// block 0044de16
0044de16  push       eax
0044de17  call       dword ptr [0x498030]

// block 0044de1d
0044de1d  mov        dword ptr [esp + 8], 0

// block 0044de25
0044de25  cmp        dword ptr [esp + 0x2c], 0x10
0044de2a  jb         0x44de39

// block 0044de2c
0044de2c  mov        ecx, dword ptr [esp + 0x18]
0044de30  push       ecx
0044de31  call       0x46ca4f

// block 0044de36
0044de36  add        esp, 4

// block 0044de39
0044de39  mov        ecx, dword ptr [esp + 0x34]
0044de3d  pop        edi
0044de3e  mov        al, bl
0044de40  pop        ebx
0044de41  xor        ecx, esp
0044de43  call       0x46c932

// block 0044de48
0044de48  add        esp, 0x30
0044de4b  ret        0x14
