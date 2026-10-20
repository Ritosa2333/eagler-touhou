
// block 00454960
00454960  test       dword ptr [0x4cee78], 0x8000
0045496a  push       esi
0045496b  mov        esi, eax
0045496d  je         0x454980

// block 0045496f
0045496f  push       0x4cf200
00454974  call       dword ptr [0x498088]

// block 0045497a
0045497a  inc        byte ptr [0x4cf223]

// block 00454980
00454980  xor        eax, eax
00454982  lea        ecx, [esi + 0x1fec]

// block 00454988
00454988  cmp        dword ptr [ecx], 0
0045498b  je         0x45499b

// block 0045498d
0045498d  inc        eax
0045498e  add        ecx, 0x10c
00454994  cmp        eax, 0x1f
00454997  jl         0x454988

// block 00454999
00454999  jmp        0x4549d6

// block 0045499b
0045499b  mov        ecx, dword ptr [esp + 0xc]
0045499f  imul       eax, eax, 0x10c
004549a5  lea        edx, [eax + esi]
004549a8  mov        eax, dword ptr [esp + 8]
004549ac  lea        esi, [edx + 0x1ff8]
004549b2  mov        dword ptr [edx + 0x1fec], eax
004549b8  mov        dword ptr [edx + 0x1ff0], ecx
004549be  mov        eax, edi
004549c0  sub        esi, edi

// block 004549c2
004549c2  mov        cl, byte ptr [eax]
004549c4  mov        byte ptr [esi + eax], cl
004549c7  inc        eax
004549c8  test       cl, cl
004549ca  jne        0x4549c2

// block 004549cc
004549cc  mov        dword ptr [edx + 0x1ff4], 0

// block 004549d6
004549d6  test       dword ptr [0x4cee78], 0x8000
004549e0  pop        esi
004549e1  je         0x4549f4

// block 004549e3
004549e3  push       0x4cf200
004549e8  call       dword ptr [0x49808c]

// block 004549ee
004549ee  dec        byte ptr [0x4cf223]

// block 004549f4
004549f4  ret        8
