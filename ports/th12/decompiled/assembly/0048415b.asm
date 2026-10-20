
// block 0048415b
0048415b  mov        edi, edi
0048415d  push       ebp
0048415e  mov        ebp, esp
00484160  push       esi
00484161  mov        esi, dword ptr [ebp + 8]
00484164  test       esi, esi
00484166  je         0x4841e6

// block 00484168
00484168  mov        eax, dword ptr [esi + 0xc]
0048416b  cmp        eax, dword ptr [0x4adf74]
00484171  je         0x48417a

// block 00484173
00484173  push       eax
00484174  call       0x46c941

// block 00484179
00484179  pop        ecx

// block 0048417a
0048417a  mov        eax, dword ptr [esi + 0x10]
0048417d  cmp        eax, dword ptr [0x4adf78]
00484183  je         0x48418c

// block 00484185
00484185  push       eax
00484186  call       0x46c941

// block 0048418b
0048418b  pop        ecx

// block 0048418c
0048418c  mov        eax, dword ptr [esi + 0x14]
0048418f  cmp        eax, dword ptr [0x4adf7c]
00484195  je         0x48419e

// block 00484197
00484197  push       eax
00484198  call       0x46c941

// block 0048419d
0048419d  pop        ecx

// block 0048419e
0048419e  mov        eax, dword ptr [esi + 0x18]
004841a1  cmp        eax, dword ptr [0x4adf80]
004841a7  je         0x4841b0

// block 004841a9
004841a9  push       eax
004841aa  call       0x46c941

// block 004841af
004841af  pop        ecx

// block 004841b0
004841b0  mov        eax, dword ptr [esi + 0x1c]
004841b3  cmp        eax, dword ptr [0x4adf84]
004841b9  je         0x4841c2

// block 004841bb
004841bb  push       eax
004841bc  call       0x46c941

// block 004841c1
004841c1  pop        ecx

// block 004841c2
004841c2  mov        eax, dword ptr [esi + 0x20]
004841c5  cmp        eax, dword ptr [0x4adf88]
004841cb  je         0x4841d4

// block 004841cd
004841cd  push       eax
004841ce  call       0x46c941

// block 004841d3
004841d3  pop        ecx

// block 004841d4
004841d4  mov        esi, dword ptr [esi + 0x24]
004841d7  cmp        esi, dword ptr [0x4adf8c]
004841dd  je         0x4841e6

// block 004841df
004841df  push       esi
004841e0  call       0x46c941

// block 004841e5
004841e5  pop        ecx

// block 004841e6
004841e6  pop        esi
004841e7  pop        ebp
004841e8  ret        
