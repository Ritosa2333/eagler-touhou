
// block 00461840
00461840  test       dword ptr [0x4cee78], 0x8000
0046184a  push       ebx
0046184b  push       ebp
0046184c  mov        ebp, dword ptr [esp + 0xc]
00461850  push       esi
00461851  mov        ebx, eax
00461853  je         0x461866

// block 00461855
00461855  push       0x4cf1d0
0046185a  call       dword ptr [0x498088]

// block 00461860
00461860  inc        byte ptr [0x4cf221]

// block 00461866
00461866  inc        dword ptr [ebx + 0x130]
0046186c  call       0x4621c0

// block 00461871
00461871  mov        esi, eax
00461873  mov        eax, dword ptr [edi + 0x20]
00461876  or         dword ptr [esi + 0x480], 1
0046187d  mov        dword ptr [esi + 0x20], eax
00461880  mov        ecx, dword ptr [edi + 0x430]
00461886  mov        dword ptr [esi + 0x430], ecx
0046188c  mov        edx, dword ptr [edi + 0x434]
00461892  mov        ecx, dword ptr [esp + 0x14]
00461896  mov        dword ptr [esi + 0x434], edx
0046189c  mov        eax, dword ptr [edi + 0x438]
004618a2  push       ecx
004618a3  push       esi
004618a4  mov        ecx, ebx
004618a6  mov        dword ptr [esi + 0x438], eax
004618ac  call       0x454d10

// block 004618b1
004618b1  mov        edx, dword ptr [edi + 0x24]
004618b4  mov        dword ptr [esi + 0x24], edx
004618b7  mov        eax, dword ptr [edi + 0x28]
004618ba  mov        dword ptr [esi + 0x28], eax
004618bd  mov        ecx, dword ptr [edi + 0x2c]
004618c0  lea        eax, [edi + 0x424]
004618c6  mov        dword ptr [esi + 0x2c], ecx
004618c9  mov        edx, dword ptr [eax]
004618cb  mov        dword ptr [esi + 0x43c], edx
004618d1  mov        ecx, dword ptr [eax + 4]
004618d4  mov        dword ptr [esi + 0x440], ecx
004618da  mov        edx, dword ptr [eax + 8]
004618dd  mov        ebx, esi
004618df  lea        eax, [esp + 0x10]
004618e3  mov        dword ptr [esi + 0x444], edx
004618e9  call       0x461250

// block 004618ee
004618ee  test       dword ptr [0x4cee78], 0x8000
004618f8  mov        eax, dword ptr [eax]
004618fa  mov        dword ptr [ebp], eax
004618fd  je         0x461910

// block 004618ff
004618ff  push       0x4cf1d0
00461904  call       dword ptr [0x49808c]

// block 0046190a
0046190a  dec        byte ptr [0x4cf221]

// block 00461910
00461910  pop        esi
00461911  mov        eax, ebp
00461913  pop        ebp
00461914  pop        ebx
00461915  ret        8
