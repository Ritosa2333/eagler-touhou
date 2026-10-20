
// block 004933d4
004933d4  fxch       st(1)

// block 004933d6
004933d6  cmp        dword ptr [0x4b4114], 1
004933dd  je         0x4933e3

// block 004933df
004933df  fprem      
004933e1  jmp        0x4933e8

// block 004933e3
004933e3  call       0x494d1c

// block 004933e8
004933e8  wait       
004933e9  fnstsw     ax
004933eb  wait       
004933ec  sahf       
004933ed  jp         0x4933d6

// block 004933ef
004933ef  fstp       st(1)
004933f1  ret        
