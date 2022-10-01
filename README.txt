Third-Order Lookup Table Masking

The provided c code is to run the third-order look-up table (LUT) scheme. Our code supports the masked execution of AES and PRESENT block-ciphers using third-order LUT.
It also provides the masked bitsliced implementation of 16-bit AES. 
The proposed lookup table countermeasure is shown to be 3-SNI secure.

The target device for the code is FRDM-K64f from NXP and the microcontroller on the target architecture is MK64FN1M0VLL12. 
The code makes use of the RNGA module built-in to the microcontroller for generating the input random seed/initial sharing of key/plaintext.

-------------------------------------------------------------------------------------------------------------------------------------
To build the code,

If you are using a Windows System 
run $make clean_windows

If you are using a Linux based System 
run $make clean_linux

to run the code
$make

The code can either run on the target micro-controller or on a desktop. Set the value of TRNG(in Utils/common.h) to "zero (0)" to run on a desktop 
(where the random seed is obtained using AES-CTR PRG) or to "one(1)" to use device built-in RNGA. This code will include the appropriate header files 
depending on the choice of TRNG parameter.S-CTR PRG) or to "one(1)" to use device built-in RNGA. This code will include the appropriate header files 
depending on the choice of TRNG parameter.

------------------------------------------------------------------------------------------------------------------------------------------
The following input choices will decide the outcome.

Set the following inputs in Utils/common.h

Set TRNG to 0 or 1.
Set #shares using the constant shares_N.
Set SNI_RM to 0 or 1 depending on whether to run the AES_THIRD scheme using 3-SNI Refresh Mask or the fullrefresh scheme. 

And the below input parameters inside main:

CIPHER decides the block cipher and the scheme to execute. It can be AES or AES_HIGHER_ORDER_INCREASING_SHARES or PRESENT or BITSLICE.
type indicates the type scheme can be BASIC or LRV

-----------------------------------------------------------------------------------------------------------------------------------------
The following is the output from the code

Summary of input choices (cipher, #shares, type)
Indicative flag of execution status: success/failure
On successful execution, The offline and online execution times indicating the pre-processing time and online time, respectively.

----------------------------------------------------------------------------------------------------------------------------------------
Please note for circuit-based schemes (bitslice), the code outputs online time only, as there is no offline time. 
If TRNG=0, our code outputs time in seconds whereas when TRNG=1, it outputs clock_cycle_count.
