
// block 00453030
00453030  mov        eax, dword ptr [0x4d4764]
00453035  test       eax, eax
00453037  je         0x4530da

// block 0045303d
0045303d  cmp        dword ptr [0x4d4770], 0
00453044  jne        0x453050

// block 00453046
00453046  mov        dword ptr [0x4d4770], 1

// block 00453050
00453050  push       esi
00453051  mov        esi, dword ptr [0x4980f4]
00453057  push       edi
00453058  push       0x64
0045305a  push       eax
0045305b  call       esi

// block 0045305d
0045305d  mov        edi, dword ptr [0x498084]
00453063  cmp        eax, 0x102
00453068  jne        0x453085

// block 0045306a
0045306a  lea        ebx, [ebx]

// block 00453070
00453070  push       1
00453072  call       edi

// block 00453074
00453074  mov        eax, dword ptr [0x4d4764]
00453079  push       0x64
0045307b  push       eax
0045307c  call       esi

// block 0045307e
0045307e  cmp        eax, 0x102
00453083  je         0x453070

// block 00453085
00453085  mov        ecx, dword ptr [0x4d4768]
0045308b  push       0x64
0045308d  push       ecx
0045308e  call       esi

// block 00453090
00453090  cmp        eax, 0x102
00453095  jne        0x4530ad

// block 00453097
00453097  push       1
00453099  call       edi

// block 0045309b
0045309b  mov        edx, dword ptr [0x4d4768]
004530a1  push       0x64
004530a3  push       edx
004530a4  call       esi

// block 004530a6
004530a6  cmp        eax, 0x102
004530ab  je         0x453097

// block 004530ad
004530ad  mov        eax, dword ptr [0x4d4764]
004530b2  mov        esi, dword ptr [0x4980a4]
004530b8  push       eax
004530b9  call       esi

// block 004530bb
004530bb  mov        ecx, dword ptr [0x4d4768]
004530c1  push       ecx
004530c2  call       esi

// block 004530c4
004530c4  pop        edi
004530c5  mov        dword ptr [0x4d4764], 0
004530cf  mov        dword ptr [0x4d4768], 0
004530d9  pop        esi

// block 004530da
004530da  xor        eax, eax
004530dc  ret        
