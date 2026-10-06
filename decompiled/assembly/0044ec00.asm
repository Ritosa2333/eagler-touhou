
// block 0044ec00
0044ec00  cmp        byte ptr [esp + 4], 0
0044ec05  push       esi
0044ec06  push       edi
0044ec07  mov        edi, dword ptr [esp + 0x10]
0044ec0b  mov        esi, ecx
0044ec0d  je         0x44ec36

// block 0044ec0f
0044ec0f  cmp        dword ptr [esi + 0x18], 0x10
0044ec13  jb         0x44ec36

// block 0044ec15
0044ec15  lea        eax, [esi + 4]
0044ec18  push       ebx
0044ec19  mov        ebx, dword ptr [eax]
0044ec1b  test       edi, edi
0044ec1d  jbe        0x44ec2c

// block 0044ec1f
0044ec1f  push       edi
0044ec20  push       ebx
0044ec21  push       0x10
0044ec23  push       eax
0044ec24  call       0x46e210

// block 0044ec29
0044ec29  add        esp, 0x10

// block 0044ec2c
0044ec2c  push       ebx
0044ec2d  call       0x46ca4f

// block 0044ec32
0044ec32  add        esp, 4
0044ec35  pop        ebx

// block 0044ec36
0044ec36  mov        dword ptr [esi + 0x14], edi
0044ec39  mov        dword ptr [esi + 0x18], 0xf
0044ec40  mov        byte ptr [esi + edi + 4], 0
0044ec45  pop        edi
0044ec46  pop        esi
0044ec47  ret        8
