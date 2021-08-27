#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "sch/enh_api.h"

static char refresh_line[] = "\r                  \r";
void hardcoded_parser(SPH_ENH_ctrl_struct* Sph_Enh_ctrl);

int main(int argc, char* argv[]) {

	short aswPCMBuffer[1920]; // pcm working buffer
	short aswEPLBuffer[4800];

	SPH_ENH_ctrl_struct Sph_Enh_ctrl; // speech parameter
	hardcoded_parser(&Sph_Enh_ctrl);

	//get working buff memory
	int working_buff_memory_size = ENH_API_Get_Memory(NULL);
	int* working_buf_mem = malloc(working_buff_memory_size);
	memset(working_buf_mem, 0, working_buff_memory_size);

	ENH_API_Alloc(&Sph_Enh_ctrl, (int*)working_buf_mem);
	ENH_API_Init(working_buf_mem);
	ENH_API_Update_Parameter(&Sph_Enh_ctrl);
	ENH_API_Reset(working_buf_mem);

	short fake_in[320] = { 0 };
	short fake_out[320];
	int ENH_frame_length = ENH_API_Get_frame_length(working_buf_mem);

#define MAX_FRAME 1100
// Process
	for (int frame_count = 0; frame_count < MAX_FRAME; frame_count++) {
		Sph_Enh_ctrl.EPL_buffer = aswEPLBuffer;

		//get UL input pcm: UL0,UL00,REF
		memcpy(Sph_Enh_ctrl.PCM_buffer, fake_in, ENH_frame_length * sizeof(short)); //UL0 input
		memcpy(Sph_Enh_ctrl.PCM_buffer + ENH_frame_length, fake_in, ENH_frame_length * sizeof(short)); //UL00 input
		memcpy(Sph_Enh_ctrl.PCM_buffer + 3 * ENH_frame_length, fake_in, ENH_frame_length * sizeof(Word16)); //Ref input

		ENH_API_Process_EC(&Sph_Enh_ctrl);
		ENH_API_Process_NDC(&Sph_Enh_ctrl);
		//output UL result
		memcpy(fake_out, Sph_Enh_ctrl.PCM_buffer, ENH_frame_length * sizeof(short)); //UL1 output

	  //get DL input pcm: DL0
		memcpy(Sph_Enh_ctrl.PCM_buffer + 2 * ENH_frame_length, fake_in, ENH_frame_length * sizeof(short)); //DL0 input
		ENH_API_DL_Process(&Sph_Enh_ctrl);
		//output DL result
		memcpy(fake_out, Sph_Enh_ctrl.PCM_buffer + 2 * ENH_frame_length, ENH_frame_length * sizeof(short)); //DL1 output
	}
	
	ENH_API_Free(working_buf_mem);
}

void hardcoded_parser(SPH_ENH_ctrl_struct* Sph_Enh_ctrl) {

	Sph_Enh_ctrl->MMI_ctrl = 0xFFFFFFFF;
	Sph_Enh_ctrl->SPH_ENH_INTERNAL_PAR_ADDR = 0;
	Sph_Enh_ctrl->SPH_DEL_M_DL = 0;
	Sph_Enh_ctrl->UL_delay = 0;
	Sph_Enh_ctrl->Ref_delay = 0;
	Sph_Enh_ctrl->t2_cur = 0;
	Sph_Enh_ctrl->se_cntr = 0;
	Sph_Enh_ctrl->t2_SACCh = 0;
	Sph_Enh_ctrl->shift_SACCh = 0;
	Sph_Enh_ctrl->DTX_flag = 0;
	Sph_Enh_ctrl->SubChannelID = 0;
	Sph_Enh_ctrl->TxChannelType = 0;
	Sph_Enh_ctrl->SQoS_state = 0;

	Word16 MMI_GAIN_2_DG_GAIN[] = { // Note that MMI_GAIN should be always multiple of 4.
		0, 4, 4, 4, 4,
		4, 4, 4, 4, 4,
		4, 4, 4, 4, 4,
		4, 4, 4, 4,	5,
		6, 7, 8, 9, 10,
		11, 12, 13, 14, 15,
		16,	11, 12, 13, 14,
		15, 16,	11, 12, 13,
		14, 15, 16,	11, 12,
		13, 14, 15, 16,	11,
		12, 13, 14, 15, 16,
		17, 18, 19, 20, 21,
		22, 23, 24, 25
	};
	Word16 enhance_pars[48] = {
		33024,4848,32768,12335,45159,12415,410,144,32912,7367,606,0,19136,5379,1085,0,55746,749,20956,43446,47396,44820,6342,1057,18434,3218,0,256,0,0,55,0,62919,41099,41099,41106,41099,32907,519,3599,3599,4132,12260,239,8754,15430,28280,39808
	};
	Word16 common_pars[12] = {
		0, 55997, 31000, 32767, 32769, 20, 0, 0, 0, 0, 0, 0
	};
	Word16 DMNR_calibration[76] = {
		3689, 0, 3129, 1761, 3108,
		2720, 666, 5558, -135, 3071,
		2379, 1514, 2541, -1021, 1803,
		-1546, 1330, -1415, 757, -1720,
		3097, -3324, 4074, -2434, 2367,
		4132, 14054, -3207, 11965, -17727,
		-1084, -13339, 8, 5, 1,
		966, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		966, 6200, 4783, 3181, 6188,
		8289, 5501, 10043, 7594, 2443,
		1468, 5429, 8045, 5219, 14678,
		9728, 12176, 9694, 1, 0,
		0, 26299, 68, 0, 0,
		0
	};

	Word16 FIR_1[90] = {
		32767, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	Word16 FIR_2[90] = {
		32767, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};
	Word16 UL_MicIIR1_Tunable[20] = { 0xE0B7, 0x3F42, 0x1ECC, 0xC268, 0x1ECC, 0xE1B2, 0x3E47, 0x2000, 0xC000, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x2000 };

	Word16 UL_MicIIR2_Tunable[20] = { 0xE0B7, 0x3F42, 0x1ECC, 0xC268, 0x1ECC, 0xE1B2, 0x3E47, 0x2000, 0xC000, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x2000, 0x0, 0x0, 0x0, 0x0, 0x2000 };

	Word16 DMNR_cal_data_SWB2[120] = {
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xC524, 0x7A90, 0x4000, 0x8001, 0xC090, 0x7F54, 0x4000, 0x800A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3BB, 0x2, 0x8C7A, 0x69, 0x6900, 0xA, 0x2160, 0x0, 0x0, 0xC00, 0x0, 0x0, 0x0, 0x292E, 0x200, 0x845F, 0x3, 0x0, 0x0, 0xE492, 0x4200, 0x844A, 0x2
};
	Sph_Enh_ctrl->App_table = PHONE_CALL;
	Sph_Enh_ctrl->Fea_Cfg_table = 511;
	Sph_Enh_ctrl->MMI_MIC_GAIN = 208;
	Sph_Enh_ctrl->DP2_AGC_GAIN = MMI_GAIN_2_DG_GAIN[Sph_Enh_ctrl->MMI_MIC_GAIN / 4];
	Sph_Enh_ctrl->sample_rate_codec = 16000;
	Sph_Enh_ctrl->frame_rate = 20;
	memcpy(Sph_Enh_ctrl->enhance_pars_WB, enhance_pars, sizeof(enhance_pars));
	memcpy(Sph_Enh_ctrl->common_pars, common_pars, sizeof(common_pars));
	memcpy(Sph_Enh_ctrl->DMNR_cal_data_WB_set1, DMNR_calibration, sizeof(DMNR_calibration));
	memcpy(Sph_Enh_ctrl->DMNR_cal_data_WB_set2, DMNR_calibration, sizeof(DMNR_calibration));
	memcpy(Sph_Enh_ctrl->DMNR_cal_data_SWB_set2, DMNR_cal_data_SWB2, sizeof(DMNR_cal_data_SWB2));
	memcpy(Sph_Enh_ctrl->Compen_filter_WB, FIR_1, sizeof(FIR_1));
	memcpy(Sph_Enh_ctrl->Compen_filter_WB + 90, FIR_2, sizeof(FIR_2));
	memcpy(Sph_Enh_ctrl->MIC1_IIR_COEF_UL_WB, UL_MicIIR1_Tunable, sizeof(UL_MicIIR1_Tunable));
	memcpy(Sph_Enh_ctrl->MIC2_IIR_COEF_UL_WB, UL_MicIIR2_Tunable, sizeof(UL_MicIIR2_Tunable));

}
