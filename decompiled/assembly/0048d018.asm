
// block 0048d018
0048d018  push       0xc
0048d01a  push       0x4ab0e0
0048d01f  call       0x46fcec

// block 0048d024
0048d024  push       3
0048d026  call       0x46ec9a

// block 0048d02b
0048d02b  pop        ecx
0048d02c  and        dword ptr [ebp - 4], 0
0048d030  push       dword ptr [ebp + 8]
0048d033  call       0x48ceb4

// block 0048d038
0048d038  pop        ecx
0048d039  movzx      eax, ax
0048d03c  mov        dword ptr [ebp - 0x1c], eax
0048d03f  mov        dword ptr [ebp - 4], 0xfffffffe
0048d046  call       0x48d055

// block 0048d04b
0048d04b  mov        ax, word ptr [ebp - 0x1c]
0048d04f  call       0x46fd31

// block 0048d054
0048d054  ret        
