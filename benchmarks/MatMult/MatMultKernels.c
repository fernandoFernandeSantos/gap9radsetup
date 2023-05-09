#include "MatMultKernels.h"
L1_CL_MEM AT_L1_POINTER L1_Memory;
L2_MEM AT_L2_POINTER L2_Memory;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
void ParMatMult(
		short int * __restrict__ M1,
		short int * __restrict__ M2,
		short int * __restrict__ Out,
		unsigned int Norm)

{
	/* Shared L1: 44080 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	KerMatMultParallel16_ArgT S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Iteration space related variables */
	int T1Ind, T1Ind_Total=0, T1Ind_Last, T1Ind_NextLast;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _N_M1;
	unsigned int _SN_M1;
	unsigned int _N_M2;
	unsigned int _SN_M2;
	unsigned int _LN_M2;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[Tile1 Dim: 6][Tile0 Dim: 4]
	Ker Arg: M1, Tiled Space: Tile1
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 6 logical tiles, 6 physical tiles
			Total Size: 28000 [Tile1, 6:[70x38, 4:70x38, 70x10], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile1, 6:[70x38, 4:70x38, 70x10], 2]
		Tile0: [0, 5320, 5320], Tile1: [5320, 5320, 5320], Tile2; [10640, 5320, 5320]
	Ker Arg: M2, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 4 logical tiles, 4 physical tiles
			Total Size: 21000 [Tile0, 4:[70x38, 2:70x38, 70x36], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 4:[70x38, 2:70x38, 70x36], 2]
		Tile0: [0, 5320, 76], Tile1: [76, 5320, 76], Tile2; [152, 5320, 76]
	Ker Arg: Out, Tiled Space: Tile1
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 6 logical tiles, 6 physical tiles
			Total Size: 60000 [Tile1, 6:[150x38, 4:150x38, 150x10], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile1, 6:[150x38, 4:150x38, 150x10], 2]
		Tile0: [0, 11400, 11400], Tile1: [11400, 11400, 11400], Tile2; [22800, 11400, 11400]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->W_In1 = (unsigned int) (70);
	KerArg0->W_Out = (unsigned int) (150);
	KerArg0->Norm = (unsigned int) (Norm);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) M1+0), ((AT_L2_INT_ADDR_TYPE) L1_Memory+0+0), 5320, 0, DmaR_Evt1);
	_N_M1=0;
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) M2+0), ((AT_L2_INT_ADDR_TYPE) L1_Memory+33440+0), 5320, 300, 76, 0, DmaR_Evt2);
	_N_M2=0;
	_C_Out=0; _SC_Out=11400;
	_SP_Out=0;
	/*============================= End Read Tiles Prolog ===============================*/
	for (T1Ind=0; T1Ind<6; T1Ind++, T1Ind_Total++) { /* Iteration on Tile1 */
		int T1Ind_Last = (T1Ind==5), T1Ind_NextLast = ((T1Ind+1)==5);
		/*================================= Prepare Tiles ===================================*/
		_SN_M1 = 0;
		if (!(T1Ind_Last)) {
			_N_M1 = _N_M1 + (5320); _SN_M1 = ((T1Ind_NextLast)?1400:5320); 
		}
		/*============================= End Prepare Tiles ===================================*/
		/*================================= Read Tiles ======================================*/
		AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read M1 */
		if (_SN_M1) {
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) M1+_N_M1), ((AT_L2_INT_ADDR_TYPE) L1_Memory+0+5320*((T1Ind_Total+1)%2)),
					_SN_M1, 0, DmaR_Evt1);
		}
		/*============================= End Read Tiles ======================================*/
		for (T0Ind=0; T0Ind<4; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==3), T0Ind_NextLast = ((T0Ind+1)==3);
			/*================================= Prepare Tiles ===================================*/
			_SN_M2 = 0;
			if (!(T0Ind_Last)) {
				_N_M2 = _N_M2 + (76); _LN_M2 = ((T0Ind_NextLast)?72:76); _SN_M2 = (70*_LN_M2); 
			} else if (!(T1Ind_Last)) {
				_N_M2 = _N_M2 + (-228); _LN_M2 = (76); _SN_M2 = (70*_LN_M2); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read M2 */
			if (_SN_M2) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) M2+_N_M2), ((AT_L2_INT_ADDR_TYPE) L1_Memory+33440+5320*((T0Ind_Total+1)%2)),
						_SN_M2, 300, _LN_M2, 0, DmaR_Evt2);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_LOOP =========================*/
			KerArg0->In1 = (short int * __restrict__) (L1_Memory+0+5320*((T1Ind_Total)%2));
			KerArg0->H_In1 = (unsigned int) (T1Ind_Last?10:38);
			KerArg0->In2 = (short int * __restrict__) (L1_Memory+33440+5320*((T0Ind_Total)%2));
			KerArg0->W_In2 = (unsigned int) ((T0Ind_Last)?36:38);
			KerArg0->Out = (short int * __restrict__) (L1_Memory+10640+11400*((T1Ind_Total)%2));
			KerArg0->OutFirstCol = (unsigned int) ((T0Ind)*38);
			AT_FORK(gap_ncore(), (void *) KerMatMultParallel16, (void *) KerArg0);
			__CALL(KerMatMultParallel16, KerArg0);
			/*================================= Update Arg Pipeline =============================*/
			/*============================= End Update Arg Pipeline =============================*/
		} /* End iteration on Tile0 */
		/*================================= Write Tiles =====================================*/
		if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
		AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) L1_Memory+10640+11400*((T1Ind_Total)%2)),
				_SC_Out, 1, DmaW_Evt1);
		/*============================= End Write Tiles =====================================*/
		/*================================= Update Arg Pipeline =============================*/
		_SP_Out = _SC_Out;
		/*============================= End Update Arg Pipeline =============================*/
		/*================================= Prepare Tiles ===================================*/
		_SC_Out = 0;
		if (!(T1Ind_Last)) {
			_C_Out = _C_Out + (11400); _SC_Out = ((T1Ind_NextLast)?3000:11400); 
		}
		/*============================= End Prepare Tiles ===================================*/
	} /* End iteration on Tile1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
#pragma GCC diagnostic pop
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
void ParVectMatMult(
		short int * __restrict__ M1,
		short int * __restrict__ M2,
		short int * __restrict__ Out,
		unsigned int Norm)

{
	/* Shared L1: 44640 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	AT_L2_EVENT _DmaR_Evt1, *DmaR_Evt1 = &_DmaR_Evt1;
	AT_L2_EVENT _DmaR_Evt2, *DmaR_Evt2 = &_DmaR_Evt2;
	AT_L2_EVENT _DmaW_Evt1, *DmaW_Evt1 = &_DmaW_Evt1;
	KerMatMultParallel16_ArgT S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Iteration space related variables */
	int T1Ind, T1Ind_Total=0, T1Ind_Last, T1Ind_NextLast;
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _N_M2;
	unsigned int _SN_M2;
	unsigned int _LN_M2;
	unsigned int _N_M1;
	unsigned int _SN_M1;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[Tile1 Dim: 7][Tile0 Dim: 3]
	Ker Arg: M2, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 3 logical tiles, 3 physical tiles
			Total Size: 21000 [Tile0, 3:[70x61, 1:70x61, 70x28], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 3:[70x61, 1:70x61, 70x28], 2]
		Tile0: [0, 8540, 122], Tile1: [122, 8540, 122], Tile2; [244, 3920, 56]
	Ker Arg: M1, Tiled Space: Tile1
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 7 logical tiles, 7 physical tiles
			Total Size: 28000 [Tile1, 7:[70x31, 5:70x31, 70x14], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile1, 7:[70x31, 5:70x31, 70x14], 2]
		Tile0: [0, 4340, 4340], Tile1: [4340, 4340, 4340], Tile2; [8680, 4340, 4340]
	Ker Arg: KerBuff, Tiled Space: Buffer
		Min Pipe Depth: 0, Max Pipe Depth: 0
		KerArgItSpace: 3 logical tiles, 1 physical tiles
			Total Size: 280 [Tile0, 3:[140x1, 1:140x1, 140x1], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 3:[140x1, 1:140x1, 140x1], 2]
		Tile0: [0, 280, 280], Tile1: [0, 280, 280], Tile2; [0, 280, 280]
	Ker Arg: Out, Tiled Space: Tile1
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 7 logical tiles, 7 physical tiles
			Total Size: 60000 [Tile1, 7:[150x31, 5:150x31, 150x14], 2]
		KerArgItSpace (User Kernel Iter Order):
			[Tile1, 7:[150x31, 5:150x31, 150x14], 2]
		Tile0: [0, 9300, 9300], Tile1: [9300, 9300, 9300], Tile2; [18600, 9300, 9300]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->W_In1 = (unsigned int) (70);
	KerArg0->W_Out = (unsigned int) (150);
	KerArg0->BufferColIn2 = (short int * __restrict__) (L1_Memory+27280);
	KerArg0->Norm = (unsigned int) (Norm);
	/*================================= Read Tiles Prolog ===============================*/
	AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) M2+0), ((AT_L2_INT_ADDR_TYPE) L1_Memory+27560+0), 8540, 300, 122, 0, DmaR_Evt1);
	_N_M2=0;
	AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) M1+0), ((AT_L2_INT_ADDR_TYPE) L1_Memory+0+0), 4340, 0, DmaR_Evt2);
	_N_M1=0;
	_C_Out=0; _SC_Out=9300;
	_SP_Out=0;
	/*============================= End Read Tiles Prolog ===============================*/
	for (T1Ind=0; T1Ind<7; T1Ind++, T1Ind_Total++) { /* Iteration on Tile1 */
		int T1Ind_Last = (T1Ind==6), T1Ind_NextLast = ((T1Ind+1)==6);
		/*================================= Prepare Tiles ===================================*/
		_SN_M1 = 0;
		if (!(T1Ind_Last)) {
			_N_M1 = _N_M1 + (4340); _SN_M1 = ((T1Ind_NextLast)?1960:4340); 
		}
		/*============================= End Prepare Tiles ===================================*/
		/*================================= Read Tiles ======================================*/
		AT_L2_WAIT(0, DmaR_Evt2); /* Wait previous DMA read M1 */
		if (_SN_M1) {
			AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) M1+_N_M1), ((AT_L2_INT_ADDR_TYPE) L1_Memory+0+4340*((T1Ind_Total+1)%2)),
					_SN_M1, 0, DmaR_Evt2);
		}
		/*============================= End Read Tiles ======================================*/
		for (T0Ind=0; T0Ind<3; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
			int T0Ind_Last = (T0Ind==2), T0Ind_NextLast = ((T0Ind+1)==2);
			/*================================= Prepare Tiles ===================================*/
			_SN_M2 = 0;
			if (!(T0Ind_Last)) {
				_N_M2 = _N_M2 + (122); _LN_M2 = ((T0Ind_NextLast)?56:122); _SN_M2 = (70*_LN_M2); 
			} else if (!(T1Ind_Last)) {
				_N_M2 = _N_M2 + (-244); _LN_M2 = (122); _SN_M2 = (70*_LN_M2); 
			}
			/*============================= End Prepare Tiles ===================================*/
			/*================================= Read Tiles ======================================*/
			AT_L2_WAIT(0, DmaR_Evt1); /* Wait previous DMA read M2 */
			if (_SN_M2) {
				AT_L2_COPY2D(0, ((AT_L2_EXT_ADDR_TYPE) M2+_N_M2), ((AT_L2_INT_ADDR_TYPE) L1_Memory+27560+8540*((T0Ind_Total+1)%2)),
						_SN_M2, 300, _LN_M2, 0, DmaR_Evt1);
			}
			/*============================= End Read Tiles ======================================*/
			/*====================== Call Kernel LOC_LOOP =========================*/
			KerArg0->In1 = (short int * __restrict__) (L1_Memory+0+4340*((T1Ind_Total)%2));
			KerArg0->H_In1 = (unsigned int) (T1Ind_Last?14:31);
			KerArg0->In2 = (short int * __restrict__) (L1_Memory+27560+8540*((T0Ind_Total)%2));
			KerArg0->W_In2 = (unsigned int) ((T0Ind_Last)?28:61);
			KerArg0->Out = (short int * __restrict__) (L1_Memory+8680+9300*((T1Ind_Total)%2));
			KerArg0->OutFirstCol = (unsigned int) ((T0Ind)*61);
			AT_FORK(gap_ncore(), (void *) KerMatMultParallelVectorial16_v2, (void *) KerArg0);
			__CALL(KerMatMultParallelVectorial16_v2, KerArg0);
			/*================================= Update Arg Pipeline =============================*/
			/*============================= End Update Arg Pipeline =============================*/
		} /* End iteration on Tile0 */
		/*================================= Write Tiles =====================================*/
		if (_SP_Out) AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
		AT_L2_COPY(0, ((AT_L2_EXT_ADDR_TYPE) Out+_C_Out), ((AT_L2_INT_ADDR_TYPE) L1_Memory+8680+9300*((T1Ind_Total)%2)),
				_SC_Out, 1, DmaW_Evt1);
		/*============================= End Write Tiles =====================================*/
		/*================================= Update Arg Pipeline =============================*/
		_SP_Out = _SC_Out;
		/*============================= End Update Arg Pipeline =============================*/
		/*================================= Prepare Tiles ===================================*/
		_SC_Out = 0;
		if (!(T1Ind_Last)) {
			_C_Out = _C_Out + (9300); _SC_Out = ((T1Ind_NextLast)?4200:9300); 
		}
		/*============================= End Prepare Tiles ===================================*/
	} /* End iteration on Tile1 */
	/*================================ Write Tiles Epilog ===============================*/
	AT_L2_WAIT(0, DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
#pragma GCC diagnostic pop
