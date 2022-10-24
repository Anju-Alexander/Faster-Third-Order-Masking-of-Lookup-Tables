A Faster-Third-Order MAsking of Lookup Tables

The provided c code is to run the third-order look-up table (LUT) scheme. Our code supports the masked execution of AES and PRESENT block-ciphers using third-order LUT.
It also provides the masked bitsliced implementation of 32-bit AES. 
The proposed lookup table countermeasure is shown to be 3-SNI secure.

The target device for the code is FRDM-K64f from NXP and the microcontroller on the target architecture is MK64FN1M0VLL12. 
The code makes use of the RNGA module built-in to the microcontroller for generating the input random seed/initial sharing of key/plaintext.
The Keil version 5.37.0.0 is used as SDE to run  the microcontroller device.

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
depending on the choice of TRNG parameter.S-CTR PRG) or to "one(1)" to use device built-in RNGA. 

------------------------------------------------------------------------------------------------------------------------------------------
The following input choices will decide the outcome.

Set the following inputs in Utils/common.h

Set TRNG to 0 or 1.

And the below input parameters inside main:

variable "cipher" decides the scheme to execute.
Inorder to run the customised third-order scheme for AES/PRESENT, set "cipher" as "AES_THIRD"/"PRESENT_THIRD" respectively.

Set "cipher" as "AES_HIGHER_ORDER_INCREASING_SHARES" and the variable "type" as "BASIC" or "LRV" depending on your choice, to run corons generic higher order increasing shares scheme (without or with LRV).

Repository also contains the code for 32-bit masked bitsliced AES-128 as well as Rivian-Prouff's third-order instantiation, which can be run by setting the CIPHER as "BITSLICE" OR "AES_RP" respectively.

-----------------------------------------------------------------------------------------------------------------------------------------
The following is the output from the code

Summary of input choices
Indicative flag of execution status: success/failure
On successful execution, The offline and online execution times indicating the pre-processing time and online time, respectively.

----------------------------------------------------------------------------------------------------------------------------------------
Please note for circuit-based schemes (bitslice), the code outputs online time only, as there is no offline time. 
If TRNG=0, our code outputs time in seconds whereas when TRNG=1, it outputs clock_cycle_count.
