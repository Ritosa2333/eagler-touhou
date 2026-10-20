
// block 00492012
00492012  mov        edi, edi
00492014  push       ebp
00492015  mov        ebp, esp
00492017  mov        eax, dword ptr [ebp + 8]
0049201a  mov        eax, dword ptr [eax]
0049201c  mov        eax, dword ptr [eax]
0049201e  cmp        eax, 0xe0434f4d
00492023  je         0x49203d

// block 00492025
00492025  cmp        eax, 0xe06d7363
0049202a  jne        0x492057

// block 0049202c
0049202c  call       0x475467

// block 00492031
00492031  and        dword ptr [eax + 0x90], 0
00492038  jmp        0x472b48

// block 0049203d
0049203d  call       0x475467

// block 00492042
00492042  cmp        dword ptr [eax + 0x90], 0
00492049  jle        0x492057

// block 0049204b
0049204b  call       0x475467

// block 00492050
00492050  add        eax, 0x90
00492055  dec        dword ptr [eax]

// block 00492057
00492057  xor        eax, eax
00492059  pop        ebp
0049205a  ret        
