
// block 00450a40
00450a40  push       0x20
00450a42  call       0x46c6b6

// block 00450a47
00450a47  mov        dword ptr [0x4ce8ec], eax
00450a4c  test       eax, eax
00450a4e  jne        0x450a6a

// block 00450a50
00450a50  push       edi
00450a51  push       0x4a2664
00450a56  mov        edi, 0x4b0ec8
00450a5b  call       0x464300

// block 00450a60
00450a60  add        esp, 4
00450a63  mov        eax, 1
00450a68  pop        edi
00450a69  ret        

// block 00450a6a
00450a6a  xor        eax, eax
00450a6c  ret        
