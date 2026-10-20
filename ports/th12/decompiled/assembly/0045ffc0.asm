
// block 0045ffc0
0045ffc0  push       ebp
0045ffc1  mov        ebp, esp
0045ffc3  and        esp, 0xfffffff8
0045ffc6  sub        esp, 0x10
0045ffc9  mov        eax, dword ptr [edi + 0x124]
0045ffcf  mov        ecx, dword ptr [edi]
0045ffd1  push       ebx
0045ffd2  push       esi
0045ffd3  push       eax
0045ffd4  push       ecx
0045ffd5  push       0x4a345c
0045ffda  call       0x4622e0

// block 0045ffdf
0045ffdf  mov        esi, dword ptr [edi + 0x108]
0045ffe5  xor        ebx, ebx
0045ffe7  add        esp, 0xc
0045ffea  xor        ecx, ecx
0045ffec  mov        dword ptr [esp + 0x10], ecx
0045fff0  mov        dword ptr [esp + 8], ebx
0045fff4  mov        dword ptr [esp + 0xc], ebx
0045fff8  mov        dword ptr [esp + 0x14], ebx
0045fffc  lea        esp, [esp]

// block 00460000
00460000  mov        edx, dword ptr [edi + 0x124]
00460006  dec        edx
00460007  cmp        ecx, edx
00460009  jne        0x46002d

// block 0046000b
0046000b  mov        eax, dword ptr [esp + 8]
0046000f  mov        ecx, dword ptr [esp + 0xc]
00460013  push       esi
00460014  push       eax
00460015  push       ebx
00460016  push       ecx
00460017  push       edi
00460018  call       0x4600a0

// block 0046001d
0046001d  test       eax, eax
0046001f  jl         0x460070

// block 00460021
00460021  mov        ecx, dword ptr [esp + 0x10]
00460025  mov        dword ptr [esp + 0x14], 1

// block 0046002d
0046002d  movzx      edx, word ptr [esi + 4]
00460031  movzx      eax, word ptr [esi + 6]
00460035  add        dword ptr [esp + 8], eax
00460039  mov        eax, dword ptr [esi + 0x24]
0046003c  add        ebx, edx

// block 00460070
00460070  mov        dword ptr [edi + 0x124], 0
0046007a  xor        eax, eax
0046007c  pop        esi
0046007d  pop        ebx
0046007e  mov        esp, ebp
00460080  pop        ebp
00460081  ret        
