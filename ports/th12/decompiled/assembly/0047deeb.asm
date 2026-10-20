
// block 0047deeb
0047deeb  mov        edi, edi
0047deed  push       ebp
0047deee  mov        ebp, esp
0047def0  xor        ecx, ecx
0047def2  inc        ecx
0047def3  test       byte ptr [0x4b4364], cl
0047def9  jne        0x47df58

// block 0047defb
0047defb  or         dword ptr [0x4b4364], ecx
0047df01  mov        eax, 0x49ddd4
0047df06  xor        edx, edx
0047df08  mov        dword ptr [0x4b4334], eax
0047df0d  mov        dword ptr [0x4b4338], edx
0047df13  mov        dword ptr [0x4b433c], edx
0047df19  mov        dword ptr [0x4b4340], eax
0047df1e  mov        dword ptr [0x4b4344], ecx
0047df24  mov        dword ptr [0x4b4348], 4
0047df2e  mov        dword ptr [0x4b434c], eax
0047df33  mov        dword ptr [0x4b4350], 2
0047df3d  mov        dword ptr [0x4b4354], edx
0047df43  mov        dword ptr [0x4b4358], eax
0047df48  mov        dword ptr [0x4b435c], 3
0047df52  mov        dword ptr [0x4b4360], edx

// block 0047df58
0047df58  mov        eax, dword ptr [ebp + 8]
0047df5b  cmp        eax, 3
0047df5e  ja         0x47df6a

// block 0047df60
0047df60  imul       eax, eax, 0xc
0047df63  add        eax, 0x4b4334
0047df68  pop        ebp
0047df69  ret        

// block 0047df6a
0047df6a  mov        eax, 0x4b4358
0047df6f  pop        ebp
0047df70  ret        
