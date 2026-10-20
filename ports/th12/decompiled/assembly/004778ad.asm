
// block 004778ad
004778ad  mov        edi, edi
004778af  push       ebp
004778b0  mov        ebp, esp
004778b2  push       esi
004778b3  push       edi
004778b4  xor        esi, esi

// block 004778b6
004778b6  push       dword ptr [ebp + 0x10]
004778b9  push       dword ptr [ebp + 0xc]
004778bc  push       dword ptr [ebp + 8]
004778bf  call       0x48b821

// block 004778c4
004778c4  mov        edi, eax
004778c6  add        esp, 0xc
004778c9  test       edi, edi
004778cb  jne        0x4778f9

// block 004778cd
004778cd  cmp        dword ptr [ebp + 0x10], eax
004778d0  je         0x4778f9

// block 004778d2
004778d2  cmp        dword ptr [0x4b41d0], eax
004778d8  jbe        0x4778f9

// block 004778da
004778da  push       esi
004778db  call       dword ptr [0x498084]

// block 004778e1
004778e1  lea        eax, [esi + 0x3e8]
004778e7  cmp        eax, dword ptr [0x4b41d0]
004778ed  jbe        0x4778f2

// block 004778ef
004778ef  or         eax, 0xffffffff

// block 004778f2
004778f2  mov        esi, eax
004778f4  cmp        eax, -1
004778f7  jne        0x4778b6

// block 004778f9
004778f9  mov        eax, edi
004778fb  pop        edi
004778fc  pop        esi
004778fd  pop        ebp
004778fe  ret        
