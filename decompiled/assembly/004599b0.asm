
// block 004599b0
004599b0  fldz       
004599b2  mov        dword ptr [eax + 0x158], ecx
004599b8  movzx      ecx, byte ptr [esp + 4]
004599bd  mov        dword ptr [eax + 0x15c], ecx
004599c3  movzx      ecx, byte ptr [esp + 8]
004599c8  mov        dword ptr [eax + 0x134], ecx
004599ce  movzx      ecx, byte ptr [esp + 0xc]
004599d3  xor        edx, edx
004599d5  mov        dword ptr [eax + 0x13c], edx
004599db  mov        dword ptr [eax + 0x140], edx
004599e1  mov        dword ptr [eax + 0x138], ecx
004599e7  mov        ecx, dword ptr [eax + 0x154]
004599ed  test       cl, 1
004599f0  jne        0x459a1b

// block 004599f2
004599f2  or         ecx, 1
004599f5  fst        dword ptr [eax + 0x14c]
004599fb  mov        dword ptr [eax + 0x148], edx
00459a01  mov        dword ptr [eax + 0x144], 0xfff0bdc1
00459a0b  mov        dword ptr [eax + 0x150], 0x4b2ed0
00459a15  mov        dword ptr [eax + 0x154], ecx

// block 00459a1b
00459a1b  fstp       dword ptr [eax + 0x14c]
00459a21  mov        dword ptr [eax + 0x148], edx
00459a27  mov        dword ptr [eax + 0x144], 0xffffffff
00459a31  ret        0xc
