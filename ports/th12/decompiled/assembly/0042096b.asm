
// block 0042096b
0042096b  mov        ecx, dword ptr [0x4b43dc]
00420971  mov        edx, dword ptr [ecx + 0x48]
00420974  push       0
00420976  push       0x17
00420978  lea        eax, [esp + 0x3c]
0042097c  push       eax
0042097d  push       edx
0042097e  mov        eax, 0x17
00420983  xor        ecx, ecx
00420985  call       0x4615a0

// block 0042098a
0042098a  mov        eax, dword ptr [esp + 0x34]
0042098e  mov        dword ptr [ebp + 0x5c], eax
00420991  jmp        0x420ced
