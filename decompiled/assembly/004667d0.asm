
// block 004667d0
004667d0  sub        esp, 0x20
004667d3  push       ebp
004667d4  mov        ebp, dword ptr [esp + 0x28]
004667d8  mov        eax, dword ptr [ebp + 4]
004667db  push       edi
004667dc  xor        edi, edi
004667de  cmp        eax, edi
004667e0  je         0x46680f

// block 004667e2
004667e2  cmp        dword ptr [ebp + 0xc], edi
004667e5  je         0x46680f

// block 004667e7
004667e7  mov        eax, dword ptr [eax]
004667e9  mov        ecx, dword ptr [eax]
004667eb  lea        edx, [esp + 0x18]
004667ef  push       edx
004667f0  lea        edx, [esp + 0x24]
004667f4  push       edx
004667f5  push       eax
004667f6  mov        eax, dword ptr [ecx + 0x10]
004667f9  call       eax

// block 004667fb
004667fb  mov        ecx, dword ptr [esp + 0x18]
004667ff  mov        eax, dword ptr [ebp + 0x68]
00466802  mov        edx, ecx
00466804  sub        edx, dword ptr [ebp + 0x70]
00466807  cmp        eax, edx
00466809  jb         0x46681c

// block 0046680b
0046680b  cmp        eax, ecx
0046680d  jae        0x46681c

// block 0046680f
0046680f  pop        edi
00466810  mov        eax, 0x800401f0
00466815  pop        ebp
00466816  add        esp, 0x20
00466819  ret        4

// block 0046681c
0046681c  mov        eax, dword ptr [ebp + 4]
0046681f  push       ebx
00466820  push       esi
00466821  mov        esi, dword ptr [eax]
00466823  cmp        esi, edi
00466825  jne        0x466836

// block 00466827
00466827  pop        esi
00466828  pop        ebx
00466829  pop        edi
0046682a  mov        eax, 0x800401f0
0046682f  pop        ebp
00466830  add        esp, 0x20
00466833  ret        4

// block 00466836
00466836  lea        ebx, [esp + 0x1c]
0046683a  call       0x466220

// block 0046683f
0046683f  cmp        eax, edi
00466841  jl         0x4669e5

// block 00466847
00466847  push       edi
00466848  cmp        dword ptr [esp + 0x20], edi
0046684c  je         0x46686f

// block 0046684e
0046684e  mov        ecx, dword ptr [ebp + 4]
00466851  mov        edx, dword ptr [ecx]
00466853  push       edx
00466854  mov        ebx, ebp
00466856  call       0x465fe0

// block 0046685b
0046685b  xor        ecx, ecx
0046685d  cmp        eax, edi
0046685f  setge      cl
00466862  pop        esi
00466863  pop        ebx
00466864  pop        edi
00466865  pop        ebp
00466866  dec        ecx
00466867  and        eax, ecx
00466869  add        esp, 0x20
0046686c  ret        4

// block 0046686f
0046686f  mov        edx, dword ptr [ebp + 4]
00466872  mov        dword ptr [esp + 0x14], edi
00466876  mov        dword ptr [esp + 0x1c], edi
0046687a  mov        eax, dword ptr [edx]
0046687c  mov        ecx, dword ptr [eax]
0046687e  lea        edx, [esp + 0x30]
00466882  push       edx
00466883  lea        edx, [esp + 0x20]
00466887  push       edx
00466888  lea        edx, [esp + 0x40]
0046688c  push       edx
0046688d  lea        edx, [esp + 0x20]
00466891  push       edx
00466892  mov        edx, dword ptr [ebp + 0x70]
00466895  push       edx
00466896  mov        edx, dword ptr [ebp + 0x68]
00466899  push       edx
0046689a  push       eax
0046689b  mov        eax, dword ptr [ecx + 0x2c]
0046689e  call       eax

// block 004668a0
004668a0  cmp        eax, edi
004668a2  jl         0x4669e5

// block 004668a8
004668a8  cmp        dword ptr [esp + 0x18], edi
004668ac  je         0x4668bd

// block 004668ae
004668ae  pop        esi
004668af  pop        ebx
004668b0  pop        edi
004668b1  mov        eax, 0x8000ffff
004668b6  pop        ebp
004668b7  add        esp, 0x20
004668ba  ret        4

// block 004668bd
004668bd  mov        eax, dword ptr [esp + 0x34]
004668c1  cmp        dword ptr [ebp + 0x6c], edi
004668c4  jne        0x466982

// block 004668ca
004668ca  mov        edx, dword ptr [esp + 0x10]
004668ce  mov        esi, dword ptr [ebp + 0xc]
004668d1  lea        ecx, [esp + 0x14]
004668d5  push       ecx
004668d6  push       edx
004668d7  call       0x466d70

// block 004668dc
004668dc  cmp        eax, edi
004668de  jl         0x4669e5

// block 004668e4
004668e4  mov        eax, dword ptr [esp + 0x14]
004668e8  cmp        eax, dword ptr [esp + 0x34]
004668ec  jae        0x46693b

// block 004668ee
004668ee  mov        dword ptr [esp + 0x1c], eax

// block 004668f2
004668f2  mov        esi, dword ptr [ebp + 0xc]
004668f5  xor        edi, edi
004668f7  mov        bl, 1
004668f9  call       0x466c90

// block 004668fe
004668fe  test       eax, eax
00466900  jl         0x4669e5

// block 00466906
00466906  mov        edi, dword ptr [esp + 0x1c]
0046690a  mov        ecx, dword ptr [esp + 0x10]
0046690e  mov        eax, dword ptr [esp + 0x34]
00466912  mov        esi, dword ptr [ebp + 0xc]
00466915  lea        edx, [esp + 0x14]
00466919  push       edx
0046691a  lea        edx, [edi + ecx]
0046691d  sub        eax, edi
0046691f  push       edx
00466920  call       0x466d70

// block 00466925
00466925  test       eax, eax
00466927  jl         0x4669e5

// block 0046692d
0046692d  add        edi, dword ptr [esp + 0x14]
00466931  mov        dword ptr [esp + 0x1c], edi
00466935  cmp        edi, dword ptr [esp + 0x34]
00466939  jb         0x4668f2

// block 0046693b
0046693b  mov        edx, dword ptr [esp + 0x34]
0046693f  mov        eax, dword ptr [ebp + 4]
00466942  mov        eax, dword ptr [eax]
00466944  mov        ecx, dword ptr [eax]
00466946  push       0
00466948  push       0
0046694a  push       edx
0046694b  mov        edx, dword ptr [esp + 0x1c]
0046694f  push       edx
00466950  push       eax
00466951  mov        eax, dword ptr [ecx + 0x4c]
00466954  call       eax

// block 00466956
00466956  mov        ecx, dword ptr [ebp + 4]
00466959  mov        eax, dword ptr [ecx]
0046695b  mov        edx, dword ptr [eax]
0046695d  mov        edx, dword ptr [edx + 0x10]
00466960  push       0
00466962  lea        ecx, [esp + 0x28]
00466966  push       ecx
00466967  push       eax
00466968  call       edx

// block 0046696a
0046696a  test       eax, eax
0046696c  jl         0x4669e5

// block 0046696e
0046696e  mov        edx, dword ptr [ebp + 0x60]
00466971  mov        ecx, dword ptr [esp + 0x24]
00466975  cmp        ecx, edx
00466977  jae        0x4669ac

// block 00466979
00466979  mov        eax, dword ptr [ebp + 8]
0046697c  sub        eax, edx
0046697e  add        eax, ecx
00466980  jmp        0x4669b0

// block 00466982
00466982  mov        ecx, dword ptr [ebp + 0xc]
00466985  mov        edx, dword ptr [ecx + 0x90]
0046698b  mov        ecx, dword ptr [esp + 0x10]
0046698f  push       eax
00466990  xor        eax, eax
00466992  cmp        word ptr [edx + 0x2e], 8
00466997  setne      al
0046699a  dec        eax
0046699b  and        eax, 0x80
004669a0  push       eax
004669a1  push       ecx
004669a2  call       0x477420

// block 004669a7
004669a7  add        esp, 0xc
004669aa  jmp        0x46693b

// block 004669ac
004669ac  mov        eax, ecx
004669ae  sub        eax, edx

// block 004669b0
004669b0  add        dword ptr [ebp + 0x64], eax
004669b3  cmp        dword ptr [ebp + 0x6c], 0
004669b7  mov        eax, dword ptr [ebp + 0x64]
004669ba  mov        dword ptr [ebp + 0x60], ecx
004669bd  je         0x4669d4

// block 004669bf
004669bf  mov        ecx, dword ptr [ebp + 0xc]
004669c2  cmp        eax, dword ptr [ecx + 0x2c]
004669c5  jb         0x4669d4

// block 004669c7
004669c7  mov        edx, dword ptr [ebp + 4]
004669ca  mov        eax, dword ptr [edx]
004669cc  mov        ecx, dword ptr [eax]
004669ce  mov        edx, dword ptr [ecx + 0x48]
004669d1  push       eax
004669d2  call       edx

// block 004669d4
004669d4  mov        eax, dword ptr [ebp + 0x68]
004669d7  add        eax, dword ptr [esp + 0x34]
004669db  xor        edx, edx
004669dd  div        dword ptr [ebp + 8]
004669e0  xor        eax, eax
004669e2  mov        dword ptr [ebp + 0x68], edx

// block 004669e5
004669e5  pop        esi
004669e6  pop        ebx
004669e7  pop        edi
004669e8  pop        ebp
004669e9  add        esp, 0x20
004669ec  ret        4
