
// block 004727c2
004727c2  push       0xc
004727c4  push       0x4aab60
004727c9  call       0x46fcec

// block 004727ce
004727ce  mov        edi, dword ptr [ebp + 8]
004727d1  xor        ebx, ebx
004727d3  cmp        dword ptr [edi + 4], ebx
004727d6  jne        0x4728a2

// block 004727dc
004727dc  push       0x2800
004727e1  push       0x46c941
004727e6  push       0x46d04a
004727eb  push       ebx
004727ec  lea        eax, [edi + 9]
004727ef  push       eax
004727f0  push       ebx
004727f1  call       0x4821bc

// block 004727f6
004727f6  add        esp, 0x18
004727f9  mov        dword ptr [ebp - 0x1c], eax
004727fc  cmp        eax, ebx
004727fe  jne        0x472807

// block 00472800
00472800  xor        eax, eax
00472802  jmp        0x4728a5

// block 00472807
00472807  push       eax
00472808  call       0x475860

// block 0047280d
0047280d  pop        ecx
0047280e  mov        esi, eax
00472810  jmp        0x47281f

// block 00472812
00472812  dec        esi
00472813  mov        eax, dword ptr [ebp - 0x1c]
00472816  add        eax, esi
00472818  cmp        byte ptr [eax], 0x20
0047281b  jne        0x472824

// block 0047281d
0047281d  mov        byte ptr [eax], bl

// block 0047281f
0047281f  cmp        esi, ebx
00472821  ja         0x472812

// block 00472823
00472823  dec        esi

// block 00472824
00472824  push       0xe
00472826  call       0x46ec9a

// block 0047282b
0047282b  pop        ecx
0047282c  mov        dword ptr [ebp - 4], ebx
0047282f  cmp        dword ptr [edi + 4], ebx
00472832  jne        0x47288d

// block 00472834
00472834  push       8
00472836  call       0x46d04a

// block 0047283b
0047283b  pop        ecx
0047283c  mov        ebx, eax
0047283e  test       ebx, ebx
00472840  je         0x47288d

// block 00472842
00472842  add        esi, 2
00472845  push       esi
00472846  call       0x46d04a

// block 0047284b
0047284b  pop        ecx
0047284c  mov        dword ptr [edi + 4], eax
0047284f  test       eax, eax
00472851  je         0x472886

// block 00472853
00472853  push       dword ptr [ebp - 0x1c]
00472856  push       esi
00472857  push       eax
00472858  call       0x47a2b9

// block 0047285d
0047285d  add        esp, 0xc
00472860  xor        ecx, ecx
00472862  cmp        eax, ecx
00472864  je         0x472873

// block 00472866
00472866  push       ecx
00472867  push       ecx
00472868  push       ecx
00472869  push       ecx
0047286a  push       ecx
0047286b  call       0x470dc6

// block 00472870
00472870  add        esp, 0x14

// block 00472873
00472873  mov        eax, dword ptr [edi + 4]
00472876  mov        dword ptr [ebx], eax
00472878  mov        eax, dword ptr [ebp + 0xc]
0047287b  mov        ecx, dword ptr [eax + 4]
0047287e  mov        dword ptr [ebx + 4], ecx
00472881  mov        dword ptr [eax + 4], ebx
00472884  jmp        0x47288d

// block 00472886
00472886  push       ebx
00472887  call       0x46c941

// block 0047288c
0047288c  pop        ecx

// block 0047288d
0047288d  push       dword ptr [ebp - 0x1c]
00472890  call       0x46c941

// block 00472895
00472895  pop        ecx
00472896  mov        dword ptr [ebp - 4], 0xfffffffe
0047289d  call       0x4728ae

// block 004728a2
004728a2  mov        eax, dword ptr [edi + 4]

// block 004728a5
004728a5  call       0x46fd31

// block 004728aa
004728aa  ret        
