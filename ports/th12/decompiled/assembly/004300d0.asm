
// block 004300d0
004300d0  sub        esp, 0x104
004300d6  mov        eax, dword ptr [0x4ad138]
004300db  xor        eax, esp
004300dd  mov        dword ptr [esp + 0x100], eax
004300e4  mov        eax, dword ptr [esp + 0x10c]
004300eb  lea        edx, [esp]
004300ee  sub        edx, eax

// block 004300f0
004300f0  mov        cl, byte ptr [eax]
004300f2  mov        byte ptr [edx + eax], cl
004300f5  inc        eax
004300f6  test       cl, cl
004300f8  jne        0x4300f0

// block 004300fa
004300fa  push       edi
004300fb  lea        eax, [esp + 4]
004300ff  push       0x2e
00430101  push       eax
00430102  call       0x46d260

// block 00430107
00430107  mov        ecx, dword ptr [esp + 0x114]
0043010e  add        esp, 8
00430111  push       ecx
00430112  mov        byte ptr [eax + 1], 0x77
00430116  mov        byte ptr [eax + 2], 0x61
0043011a  mov        byte ptr [eax + 3], 0x76
0043011e  push       1
00430120  lea        edi, [esp + 0xc]
00430124  mov        eax, 0x4cf4e8
00430129  call       0x454960

// block 0043012e
0043012e  mov        ecx, dword ptr [esp + 0x104]
00430135  pop        edi
00430136  xor        ecx, esp
00430138  mov        eax, 1
0043013d  call       0x46c932

// block 00430142
00430142  add        esp, 0x104
00430148  ret        8
