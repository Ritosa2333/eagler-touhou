
// block 00450580
00450580  push       ecx
00450581  call       0x450810

// block 00450586
00450586  mov        eax, dword ptr [0x4ce8f0]
0045058b  mov        ecx, dword ptr [eax]
0045058d  mov        edx, dword ptr [ecx + 0x44]
00450590  push       0
00450592  push       0
00450594  push       0
00450596  push       0
00450598  push       eax
00450599  call       edx

// block 0045059b
0045059b  test       eax, eax
0045059d  jge        0x4505d9

// block 0045059f
0045059f  call       0x431700

// block 004505a4
004505a4  call       0x44f370

// block 004505a9
004505a9  mov        eax, dword ptr [0x4ce8f0]
004505ae  mov        ecx, dword ptr [eax]
004505b0  mov        edx, dword ptr [ecx + 0x40]
004505b3  push       0x4ce9dc
004505b8  push       eax
004505b9  call       edx

// block 004505bb
004505bb  call       0x44f400

// block 004505c0
004505c0  mov        eax, 0x4ce8e8
004505c5  call       0x431630

// block 004505ca
004505ca  call       0x451200

// block 004505cf
004505cf  mov        dword ptr [0x4cee5c], 2

// block 004505d9
004505d9  cmp        dword ptr [0x4b43e0], 0
004505e0  je         0x4505e7

// block 004505e2
004505e2  call       0x41cb70

// block 004505e7
004505e7  cmp        dword ptr [0x4b43cc], 0
004505ee  je         0x4505f5

// block 004505f0
004505f0  call       0x40dd50

// block 004505f5
004505f5  pop        ecx
004505f6  ret        
