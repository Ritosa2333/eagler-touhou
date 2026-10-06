
// block 004821bc
004821bc  push       0x64
004821be  push       0x4aaed8
004821c3  call       0x46fcec

// block 004821c8
004821c8  mov        edi, dword ptr [ebp + 0x14]
004821cb  xor        esi, esi
004821cd  cmp        edi, esi
004821cf  jne        0x4821d5

// block 004821d1
004821d1  xor        eax, eax
004821d3  jmp        0x482247

// block 004821d5
004821d5  push       5
004821d7  call       0x46ebd7

// block 004821dc
004821dc  pop        ecx
004821dd  test       eax, eax
004821df  je         0x4821d1

// block 004821e1
004821e1  push       5
004821e3  call       0x46ec9a

// block 004821e8
004821e8  pop        ecx
004821e9  mov        dword ptr [ebp - 4], esi
004821ec  mov        dword ptr [0x4b42f8], edi
004821f2  mov        eax, dword ptr [ebp + 0x18]
004821f5  mov        dword ptr [0x4b42fc], eax
004821fa  mov        dword ptr [0x4b4308], esi
00482200  mov        dword ptr [0x4b4300], esi
00482206  mov        dword ptr [0x4b4304], esi
0048220c  movzx      eax, word ptr [ebp + 0x1c]
00482210  push       eax
00482211  push       esi
00482212  push       dword ptr [ebp + 0x10]
00482215  push       dword ptr [ebp + 0xc]
00482218  push       dword ptr [ebp + 8]
0048221b  lea        ecx, [ebp - 0x74]
0048221e  call       0x47e053

// block 00482223
00482223  lea        ecx, [ebp - 0x74]
00482226  call       0x481f3b

// block 0048222b
0048222b  mov        dword ptr [ebp - 0x1c], eax
0048222e  mov        ecx, 0x4b42f8
00482233  call       0x47d5c0

// block 00482238
00482238  mov        dword ptr [ebp - 4], 0xfffffffe
0048223f  call       0x48224d

// block 00482244
00482244  mov        eax, dword ptr [ebp - 0x1c]

// block 00482247
00482247  call       0x46fd31

// block 0048224c
0048224c  ret        
