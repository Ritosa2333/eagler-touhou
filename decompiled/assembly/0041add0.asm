
// block 0041add0
0041add0  mov        eax, dword ptr [0x4b43dc]
0041add5  mov        eax, dword ptr [eax + 0x68]
0041add8  test       eax, eax
0041adda  je         0x41ae10

// block 0041addc
0041addc  lea        esp, [esp]

// block 0041ade0
0041ade0  mov        edx, dword ptr [eax + 4]
0041ade3  mov        eax, dword ptr [eax]
0041ade5  mov        ecx, dword ptr [eax + 0x26f8]
0041adeb  add        eax, 0x1040
0041adf0  test       ecx, 0x400
0041adf6  je         0x41ae0a

// block 0041adf8
0041adf8  and        ecx, 0xfffffbff
0041adfe  or         ecx, 0x100
0041ae04  mov        dword ptr [eax + 0x16b8], ecx

// block 0041ae0a
0041ae0a  mov        eax, edx
0041ae0c  test       edx, edx
0041ae0e  jne        0x41ade0

// block 0041ae10
0041ae10  xor        eax, eax
0041ae12  ret        
