
// block 004629b0
004629b0  sub        esp, 0x34
004629b3  push       esi
004629b4  mov        esi, dword ptr [0x49829c]
004629ba  lea        eax, [esp + 4]
004629be  push       eax
004629bf  push       0
004629c1  mov        dword ptr [esp + 0xc], 0x34
004629c9  mov        dword ptr [esp + 0x10], 0xff
004629d1  call       esi

// block 004629d3
004629d3  test       eax, eax
004629d5  je         0x462a00

// block 004629d7
004629d7  lea        ecx, [esp + 4]
004629db  push       ecx
004629dc  push       1
004629de  call       esi

// block 004629e0
004629e0  test       eax, eax
004629e2  je         0x462a00

// block 004629e4
004629e4  push       0x4a3570
004629e9  mov        ecx, 0x4b0ec8
004629ee  call       0x464220

// block 004629f3
004629f3  add        esp, 4
004629f6  mov        eax, 1
004629fb  pop        esi
004629fc  add        esp, 0x34
004629ff  ret        

// block 00462a00
00462a00  push       0x194
00462a05  push       0x4ce570
00462a0a  push       0
00462a0c  call       dword ptr [0x49828c]

// block 00462a12
00462a12  xor        eax, eax
00462a14  pop        esi
00462a15  add        esp, 0x34
00462a18  ret        
