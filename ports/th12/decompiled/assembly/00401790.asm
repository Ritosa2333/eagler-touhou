
// block 00401790
00401790  push       ecx
00401791  push       ebx
00401792  push       ebp
00401793  mov        ebp, dword ptr [esp + 0x10]
00401797  cmp        dword ptr [ebp + 0x18f7c], 0
0040179e  push       esi
0040179f  mov        ecx, 1
004017a4  push       edi
004017a5  mov        dword ptr [esp + 0x18], ecx
004017a9  lea        ebx, [ebp + 0x97c]
004017af  mov        dword ptr [esp + 0x10], 0
004017b7  jle        0x40188d

// block 004017bd
004017bd  lea        ecx, [ecx]

// block 004017c0
004017c0  mov        eax, dword ptr [esp + 0x1c]
004017c4  cmp        dword ptr [ebx + 0x128], eax
004017ca  jne        0x40186e

// block 004017d0
004017d0  mov        eax, dword ptr [ebx + 0x11c]
004017d6  cmp        ecx, eax
004017d8  je         0x401863

// block 004017de
004017de  mov        esi, dword ptr [0x4ce8cc]
004017e4  mov        edi, eax
004017e6  mov        dword ptr [esp + 0x18], edi
004017ea  call       0x45a3c0

// block 004017ef
004017ef  test       edi, edi
004017f1  je         0x40182c

// block 004017f3
004017f3  mov        edi, 0x4ceaec
004017f8  mov        dword ptr [0x4cee34], edi
004017fe  call       0x430a70

// block 00401803
00401803  mov        edx, dword ptr [0x4cee34]
00401809  mov        eax, dword ptr [0x4ce8f0]
0040180e  mov        ecx, dword ptr [eax]
00401810  add        edx, 0xcc
00401816  push       edx
00401817  push       eax
00401818  mov        eax, dword ptr [ecx + 0xbc]
0040181e  call       eax

// block 00401820
00401820  mov        dword ptr [0x4cee38], 0
0040182a  jmp        0x401863

// block 0040182c
0040182c  mov        edi, 0x4cec04
00401831  mov        dword ptr [0x4cee34], edi
00401837  call       0x430a70

// block 0040183c
0040183c  mov        edx, dword ptr [0x4cee34]
00401842  mov        eax, dword ptr [0x4ce8f0]
00401847  mov        ecx, dword ptr [eax]
00401849  add        edx, 0xcc
0040184f  push       edx
00401850  push       eax
00401851  mov        eax, dword ptr [ecx + 0xbc]
00401857  call       eax

// block 00401859
00401859  mov        dword ptr [0x4cee38], 1

// block 00401863
00401863  push       ebx
00401864  push       ebp
00401865  call       0x4018f0

// block 0040186a
0040186a  mov        ecx, dword ptr [esp + 0x18]

// block 0040186e
0040186e  mov        eax, dword ptr [esp + 0x10]
00401872  inc        eax
00401873  add        ebx, 0x138
00401879  cmp        eax, dword ptr [ebp + 0x18f7c]
0040187f  mov        dword ptr [esp + 0x10], eax
00401883  jl         0x4017c0

// block 00401889
00401889  test       ecx, ecx
0040188b  je         0x4018d7

// block 0040188d
0040188d  mov        esi, dword ptr [0x4ce8cc]
00401893  call       0x45a3c0

// block 00401898
00401898  mov        edi, 0x4cec04
0040189d  mov        dword ptr [0x4cee34], edi
004018a3  call       0x430a70

// block 004018a8
004018a8  mov        edx, dword ptr [0x4cee34]
004018ae  mov        eax, dword ptr [0x4ce8f0]
004018b3  mov        ecx, dword ptr [eax]
004018b5  add        edx, 0xcc
004018bb  push       edx
004018bc  push       eax
004018bd  mov        eax, dword ptr [ecx + 0xbc]
004018c3  call       eax

// block 004018c5
004018c5  pop        edi
004018c6  pop        esi
004018c7  mov        eax, 1
004018cc  pop        ebp
004018cd  mov        dword ptr [0x4cee38], eax
004018d2  pop        ebx
004018d3  pop        ecx
004018d4  ret        8

// block 004018d7
004018d7  pop        edi
004018d8  pop        esi
004018d9  pop        ebp
004018da  mov        eax, 1
004018df  pop        ebx
004018e0  pop        ecx
004018e1  ret        8
