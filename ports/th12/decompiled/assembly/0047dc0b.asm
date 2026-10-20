
// block 0047dc0b
0047dc0b  mov        edi, edi
0047dc0d  push       ebp
0047dc0e  mov        ebp, esp
0047dc10  mov        eax, dword ptr [0x4b4328]
0047dc15  not        eax
0047dc17  test       al, 1
0047dc19  mov        eax, dword ptr [ebp + 8]
0047dc1c  mov        eax, dword ptr [eax*4 + 0x49dc30]
0047dc23  jne        0x47dc27

// block 0047dc25
0047dc25  inc        eax
0047dc26  inc        eax

// block 0047dc27
0047dc27  pop        ebp
0047dc28  ret        
