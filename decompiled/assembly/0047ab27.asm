
// block 0047ab27
0047ab27  mov        edi, edi
0047ab29  push       ebp
0047ab2a  mov        ebp, esp
0047ab2c  cmp        dword ptr [0x4b40dc], 0
0047ab33  jne        0x47ab45

// block 0047ab35
0047ab35  mov        eax, dword ptr [ebp + 8]
0047ab38  lea        ecx, [eax - 0x41]
0047ab3b  cmp        ecx, 0x19
0047ab3e  ja         0x47ab51

// block 0047ab40
0047ab40  add        eax, 0x20
0047ab43  pop        ebp
0047ab44  ret        

// block 0047ab45
0047ab45  push       0
0047ab47  push       dword ptr [ebp + 8]
0047ab4a  call       0x47aa12

// block 0047ab4f
0047ab4f  pop        ecx
0047ab50  pop        ecx

// block 0047ab51
0047ab51  pop        ebp
0047ab52  ret        
