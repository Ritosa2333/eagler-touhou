
// block 00406f60
00406f60  mov        eax, dword ptr [0x4b4514]
00406f65  mov        ecx, dword ptr [eax + 0xc410]
00406f6b  mov        edx, dword ptr [esp + 4]
00406f6f  test       cl, 1
00406f72  jne        0x406fa3

// block 00406f74
00406f74  fldz       
00406f76  or         ecx, 1
00406f79  fstp       dword ptr [eax + 0xc408]
00406f7f  mov        dword ptr [eax + 0xc404], 0
00406f89  mov        dword ptr [eax + 0xc400], 0xfff0bdc1
00406f93  mov        dword ptr [eax + 0xc40c], 0x4b2ed0
00406f9d  mov        dword ptr [eax + 0xc410], ecx

// block 00406fa3
00406fa3  fild       dword ptr [esp + 4]
00406fa7  mov        dword ptr [eax + 0xc404], edx
00406fad  dec        edx
00406fae  mov        dword ptr [eax + 0xc400], edx
00406fb4  fstp       dword ptr [eax + 0xc408]
00406fba  ret        4
