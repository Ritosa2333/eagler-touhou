
// block 004610d0
004610d0  push       ebp
004610d1  mov        ebp, esp
004610d3  and        esp, 0xfffffff8
004610d6  sub        esp, 8
004610d9  test       dword ptr [0x4cee78], 0x8000
004610e3  push       ebx
004610e4  push       esi
004610e5  je         0x4610f8

// block 004610e7
004610e7  push       0x4cf1d0
004610ec  call       dword ptr [0x498088]

// block 004610f2
004610f2  inc        byte ptr [0x4cf221]

// block 004610f8
004610f8  xor        ecx, ecx
004610fa  lea        eax, [edi + 0x88e3e0]
00461100  mov        dword ptr [edi + 0x88e3fc], ecx
00461106  mov        dword ptr [edi + 0x88e8b0], ecx
0046110c  mov        dword ptr [esp + 8], eax
00461110  mov        eax, dword ptr [edi + 0x8856c0]
00461116  lea        edx, [edi + 0x88e894]
0046111c  mov        dword ptr [esp + 0xc], edx
00461120  mov        dword ptr [edi + 0xb8], ecx
00461126  cmp        eax, ecx
00461128  je         0x46119b

// block 0046112a
0046112a  mov        esi, dword ptr [eax]
0046112c  test       dword ptr [esi + 0x47c], 0x10000000
00461136  mov        ebx, dword ptr [eax + 4]
00461139  je         0x461143

// block 0046113b
0046113b  push       edi
0046113c  call       0x461450

// block 00461141
00461141  jmp        0x46118f

// block 00461143
00461143  mov        eax, dword ptr [esi + 0x488]
00461149  test       eax, eax
0046114b  je         0x461151

// block 0046114d
0046114d  mov        ecx, esi
0046114f  call       eax

// block 00461151
00461151  push       esi
00461152  call       0x455630

// block 00461157
00461157  test       eax, eax
00461159  je         0x461163

// block 0046115b
0046115b  push       edi
0046115c  call       0x461450

// block 00461161
00461161  jmp        0x46118f

// block 00461163
00461163  mov        eax, dword ptr [esi + 0x20]
00461166  cmp        eax, 0x1e
00461169  je         0x461177

// block 0046116b
0046116b  cmp        eax, 0x1f
0046116e  je         0x461177

// block 00461170
00461170  mov        dword ptr [esi + 0x20], 0x1e

// block 00461177
00461177  mov        eax, dword ptr [esi + 0x20]
0046117a  mov        ecx, dword ptr [esp + eax*4 - 0x70]
0046117e  mov        dword ptr [ecx + 0x1c], esi
00461181  mov        edx, dword ptr [esi + 0x20]
00461184  mov        dword ptr [esp + edx*4 - 0x70], esi
00461188  mov        dword ptr [esi + 0x1c], 0

// block 0046118f
0046118f  inc        dword ptr [edi + 0xb8]
00461195  mov        eax, ebx
00461197  test       ebx, ebx
00461199  jne        0x46112a

// block 0046119b
0046119b  test       dword ptr [0x4cee78], 0x8000
004611a5  je         0x4611b8

// block 004611a7
004611a7  push       0x4cf1d0
004611ac  call       dword ptr [0x49808c]

// block 004611b2
004611b2  dec        byte ptr [0x4cf221]

// block 004611b8
004611b8  pop        esi
004611b9  mov        eax, 1
004611be  pop        ebx
004611bf  mov        esp, ebp
004611c1  pop        ebp
004611c2  ret        
