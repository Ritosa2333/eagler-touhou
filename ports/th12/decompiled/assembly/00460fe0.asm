
// block 00460fe0
00460fe0  push       ebp
00460fe1  mov        ebp, esp
00460fe3  and        esp, 0xfffffff8
00460fe6  sub        esp, 0x80
00460fec  test       dword ptr [0x4cee78], 0x8000
00460ff6  push       ebx
00460ff7  push       esi
00460ff8  je         0x46100b

// block 00460ffa
00460ffa  push       0x4cf1d0
00460fff  call       dword ptr [0x498088]

// block 00461005
00461005  inc        byte ptr [0x4cf221]

// block 0046100b
0046100b  xor        edx, edx
0046100d  xor        eax, eax
0046100f  lea        ecx, [edi + 0x8856c8]

// block 00461015
00461015  mov        dword ptr [esp + eax*4 + 8], ecx
00461019  mov        dword ptr [ecx + 0x1c], edx
0046101c  inc        eax
0046101d  add        ecx, 0x4b4
00461023  cmp        eax, 0x1e
00461026  jl         0x461015

// block 00461028
00461028  mov        eax, dword ptr [edi + 0x8856b8]
0046102e  cmp        eax, edx
00461030  je         0x46109b

// block 00461032
00461032  mov        esi, dword ptr [eax]
00461034  test       dword ptr [esi + 0x47c], 0x10000000
0046103e  mov        ebx, dword ptr [eax + 4]
00461041  je         0x46104b

// block 00461043
00461043  push       edi
00461044  call       0x461450

// block 00461049
00461049  jmp        0x46108f

// block 0046104b
0046104b  mov        eax, dword ptr [esi + 0x488]
00461051  test       eax, eax
00461053  je         0x461065

// block 00461055
00461055  mov        ecx, esi
00461057  call       eax

// block 00461059
00461059  test       eax, eax
0046105b  je         0x461065

// block 0046105d
0046105d  push       edi
0046105e  call       0x461450

// block 00461063
00461063  jmp        0x46108f

// block 00461065
00461065  push       esi
00461066  call       0x455630

// block 0046106b
0046106b  test       eax, eax
0046106d  je         0x461077

// block 0046106f
0046106f  push       edi
00461070  call       0x461450

// block 00461075
00461075  jmp        0x46108f

// block 00461077
00461077  mov        eax, dword ptr [esi + 0x20]
0046107a  mov        ecx, dword ptr [esp + eax*4 + 8]
0046107e  mov        dword ptr [ecx + 0x1c], esi
00461081  mov        edx, dword ptr [esi + 0x20]
00461084  mov        dword ptr [esp + edx*4 + 8], esi
00461088  mov        dword ptr [esi + 0x1c], 0

// block 0046108f
0046108f  inc        dword ptr [edi + 0xb8]
00461095  mov        eax, ebx
00461097  test       ebx, ebx
00461099  jne        0x461032

// block 0046109b
0046109b  test       dword ptr [0x4cee78], 0x8000
004610a5  je         0x4610b8

// block 004610a7
004610a7  push       0x4cf1d0
004610ac  call       dword ptr [0x49808c]

// block 004610b2
004610b2  dec        byte ptr [0x4cf221]

// block 004610b8
004610b8  pop        esi
004610b9  mov        eax, 1
004610be  pop        ebx
004610bf  mov        esp, ebp
004610c1  pop        ebp
004610c2  ret        
