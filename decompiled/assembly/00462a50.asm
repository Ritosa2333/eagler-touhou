
// block 00462a50
00462a50  test       cx, cx
00462a53  jge        0x462a5a

// block 00462a55
00462a55  xor        eax, eax
00462a57  ret        8

// block 00462a5a
00462a5a  mov        eax, 1
00462a5f  shl        eax, cl
00462a61  and        eax, dword ptr [esp + 8]
00462a65  neg        eax
00462a67  sbb        eax, eax
00462a69  and        eax, dword ptr [esp + 4]
00462a6d  or         dword ptr [edx], eax
00462a6f  ret        8
