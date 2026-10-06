
// block 0046dad3
0046dad3  mov        edi, edi
0046dad5  push       ebp
0046dad6  mov        ebp, esp
0046dad8  call       0x475279

// block 0046dadd
0046dadd  call       0x475273

// block 0046dae2
0046dae2  push       eax
0046dae3  call       0x475259

// block 0046dae8
0046dae8  test       eax, eax
0046daea  jne        0x46db0b

// block 0046daec
0046daec  push       dword ptr [ebp + 8]
0046daef  call       0x475273

// block 0046daf4
0046daf4  push       eax
0046daf5  call       0x4752ad

// block 0046dafa
0046dafa  test       eax, eax
0046dafc  jne        0x46db26

// block 0046dafe
0046dafe  call       dword ptr [0x4980e4]

// block 0046db04
0046db04  push       eax
0046db05  call       dword ptr [0x4981e0]

// block 0046db0b
0046db0b  mov        ecx, dword ptr [ebp + 8]
0046db0e  mov        edx, dword ptr [ecx + 0x54]
0046db11  mov        dword ptr [eax + 0x54], edx
0046db14  mov        edx, dword ptr [ecx + 0x58]
0046db17  mov        dword ptr [eax + 0x58], edx
0046db1a  mov        edx, dword ptr [ecx + 4]
0046db1d  push       ecx
0046db1e  mov        dword ptr [eax + 4], edx
0046db21  call       0x475481

// block 0046db26
0046db26  cmp        dword ptr [0x49d578], 0
0046db2d  je         0x46db44

// block 0046db2f
0046db2f  push       0x49d578
0046db34  call       0x477a40

// block 0046db39
0046db39  pop        ecx
0046db3a  test       eax, eax
0046db3c  je         0x46db44

// block 0046db3e
0046db3e  call       dword ptr [0x49d578]

// block 0046db44
0046db44  call       0x46da92

// block 0046db49
0046db49  int3       
