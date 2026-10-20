
// block 00474084
00474084  mov        edi, edi
00474086  push       ebp
00474087  mov        ebp, esp
00474089  push       edi
0047408a  mov        edi, dword ptr [ebp + 8]
0047408d  test       edi, edi
0047408f  je         0x474118

// block 00474095
00474095  push       ebx
00474096  push       esi
00474097  mov        esi, dword ptr [0x49815c]
0047409d  push       edi
0047409e  call       esi

// block 004740a0
004740a0  mov        eax, dword ptr [edi + 0xb0]
004740a6  test       eax, eax
004740a8  je         0x4740ad

// block 004740aa
004740aa  push       eax
004740ab  call       esi

// block 004740ad
004740ad  mov        eax, dword ptr [edi + 0xb8]
004740b3  test       eax, eax
004740b5  je         0x4740ba

// block 004740b7
004740b7  push       eax
004740b8  call       esi

// block 004740ba
004740ba  mov        eax, dword ptr [edi + 0xb4]
004740c0  test       eax, eax
004740c2  je         0x4740c7

// block 004740c4
004740c4  push       eax
004740c5  call       esi

// block 004740c7
004740c7  mov        eax, dword ptr [edi + 0xc0]
004740cd  test       eax, eax
004740cf  je         0x4740d4

// block 004740d1
004740d1  push       eax
004740d2  call       esi

// block 004740d4
004740d4  lea        ebx, [edi + 0x50]
004740d7  mov        dword ptr [ebp + 8], 6

// block 004740de
004740de  cmp        dword ptr [ebx - 8], 0x4ad9d8
004740e5  je         0x4740f0

// block 004740e7
004740e7  mov        eax, dword ptr [ebx]
004740e9  test       eax, eax
004740eb  je         0x4740f0

// block 004740ed
004740ed  push       eax
004740ee  call       esi

// block 004740f0
004740f0  cmp        dword ptr [ebx - 4], 0
004740f4  je         0x474100

// block 004740f6
004740f6  mov        eax, dword ptr [ebx + 4]
004740f9  test       eax, eax
004740fb  je         0x474100

// block 004740fd
004740fd  push       eax
004740fe  call       esi

// block 00474100
00474100  add        ebx, 0x10
00474103  dec        dword ptr [ebp + 8]
00474106  jne        0x4740de

// block 00474108
00474108  mov        eax, dword ptr [edi + 0xd4]
0047410e  add        eax, 0xb4
00474113  push       eax
00474114  call       esi

// block 00474116
00474116  pop        esi
00474117  pop        ebx

// block 00474118
00474118  mov        eax, edi
0047411a  pop        edi
0047411b  pop        ebp
0047411c  ret        
