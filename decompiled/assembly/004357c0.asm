
// block 004357c0
004357c0  sub        esp, 0x10
004357c3  test       dword ptr [0x4cee78], 0x8000
004357cd  push       ebx
004357ce  push       ebp
004357cf  mov        ebp, dword ptr [esp + 0x1c]
004357d3  push       esi
004357d4  mov        ebx, 1
004357d9  je         0x4357ec

// block 004357db
004357db  push       0x4cf1d0
004357e0  call       dword ptr [0x498088]

// block 004357e6
004357e6  add        byte ptr [0x4cf221], bl

// block 004357ec
004357ec  add        dword ptr [edi + 0x130], ebx
004357f2  call       0x4621c0

// block 004357f7
004357f7  fldz       
004357f9  mov        esi, eax
004357fb  fst        dword ptr [esp + 0x10]
004357ff  mov        eax, dword ptr [esp + 0x10]
00435803  fst        dword ptr [esp + 0x14]
00435807  mov        ecx, dword ptr [esp + 0x14]
0043580b  fstp       dword ptr [esp + 0x18]
0043580f  mov        edx, dword ptr [esp + 0x18]
00435813  or         dword ptr [esi + 0x480], ebx
00435819  mov        dword ptr [esi + 0x430], eax
0043581f  mov        eax, dword ptr [esp + 0x24]
00435823  mov        dword ptr [esi + 0x434], ecx
00435829  push       eax
0043582a  push       esi
0043582b  mov        ecx, edi
0043582d  mov        dword ptr [esi + 0x438], edx
00435833  call       0x454d10

// block 00435838
00435838  mov        ebx, esi
0043583a  lea        eax, [esp + 0x20]
0043583e  call       0x461350

// block 00435843
00435843  test       dword ptr [0x4cee78], 0x8000
0043584d  mov        ecx, dword ptr [eax]
0043584f  mov        dword ptr [ebp], ecx
00435852  je         0x435865

// block 00435854
00435854  push       0x4cf1d0
00435859  call       dword ptr [0x49808c]

// block 0043585f
0043585f  dec        byte ptr [0x4cf221]

// block 00435865
00435865  pop        esi
00435866  mov        eax, ebp
00435868  pop        ebp
00435869  pop        ebx
0043586a  add        esp, 0x10
0043586d  ret        8
