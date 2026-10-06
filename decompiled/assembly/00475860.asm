
// block 00475860
00475860  mov        ecx, dword ptr [esp + 4]
00475864  test       ecx, 3
0047586a  je         0x475890

// block 0047586c
0047586c  mov        al, byte ptr [ecx]
0047586e  add        ecx, 1
00475871  test       al, al
00475873  je         0x4758c3

// block 00475875
00475875  test       ecx, 3
0047587b  jne        0x47586c

// block 0047587d
0047587d  add        eax, 0
00475882  lea        esp, [esp]
00475889  lea        esp, [esp]

// block 00475890
00475890  mov        eax, dword ptr [ecx]
00475892  mov        edx, 0x7efefeff
00475897  add        edx, eax
00475899  xor        eax, 0xffffffff
0047589c  xor        eax, edx
0047589e  add        ecx, 4
004758a1  test       eax, 0x81010100
004758a6  je         0x475890

// block 004758a8
004758a8  mov        eax, dword ptr [ecx - 4]
004758ab  test       al, al
004758ad  je         0x4758e1

// block 004758af
004758af  test       ah, ah
004758b1  je         0x4758d7

// block 004758b3
004758b3  test       eax, 0xff0000
004758b8  je         0x4758cd

// block 004758ba
004758ba  test       eax, 0xff000000
004758bf  je         0x4758c3

// block 004758c1
004758c1  jmp        0x475890

// block 004758c3
004758c3  lea        eax, [ecx - 1]
004758c6  mov        ecx, dword ptr [esp + 4]
004758ca  sub        eax, ecx
004758cc  ret        

// block 004758cd
004758cd  lea        eax, [ecx - 2]
004758d0  mov        ecx, dword ptr [esp + 4]
004758d4  sub        eax, ecx
004758d6  ret        

// block 004758d7
004758d7  lea        eax, [ecx - 3]
004758da  mov        ecx, dword ptr [esp + 4]
004758de  sub        eax, ecx
004758e0  ret        

// block 004758e1
004758e1  lea        eax, [ecx - 4]
004758e4  mov        ecx, dword ptr [esp + 4]
004758e8  sub        eax, ecx
004758ea  ret        
