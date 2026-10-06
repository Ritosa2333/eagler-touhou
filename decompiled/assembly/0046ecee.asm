
// block 0046ecee
0046ecee  mov        edi, edi
0046ecf0  push       ebp
0046ecf1  mov        ebp, esp
0046ecf3  mov        eax, dword ptr [ebp + 8]
0046ecf6  lea        ecx, [eax - 1]
0046ecf9  push       esi
0046ecfa  cmp        ecx, -2
0046ecfd  jbe        0x46ed1e

// block 0046ecff
0046ecff  call       0x46e96f

// block 0046ed04
0046ed04  xor        esi, esi

// block 0046ed06
0046ed06  push       esi
0046ed07  push       esi
0046ed08  push       esi
0046ed09  push       esi
0046ed0a  push       esi
0046ed0b  mov        dword ptr [eax], 0x16
0046ed11  call       0x470f2d

// block 0046ed16
0046ed16  add        esp, 0x14
0046ed19  push       0x16
0046ed1b  pop        eax
0046ed1c  jmp        0x46ed36

// block 0046ed1e
0046ed1e  xor        esi, esi
0046ed20  cmp        dword ptr [0x4b3c04], esi
0046ed26  jne        0x46ed2f

// block 0046ed28
0046ed28  call       0x46e96f

// block 0046ed2d
0046ed2d  jmp        0x46ed06

// block 0046ed2f
0046ed2f  mov        dword ptr [0x4ad2b0], eax
0046ed34  xor        eax, eax

// block 0046ed36
0046ed36  pop        esi
0046ed37  pop        ebp
0046ed38  ret        
