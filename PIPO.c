#include "PIPO.h"


void counter_inc(uint8_t* counter) {
	uint8_t c = counter[7];
	c++;
	counter[7] = c;
}
void keyadd(uint8_t* val, uint8_t* rk)
{
	val[0] ^= rk[0];
	val[1] ^= rk[1];
	val[2] ^= rk[2];
	val[3] ^= rk[3];
	val[4] ^= rk[4];
	val[5] ^= rk[5];
	val[6] ^= rk[6];
	val[7] ^= rk[7];
}

void sbox(uint8_t* X)
{
	uint8_t T[3] = { 0, };
	//(MSB: x[7], LSB: x[0]) 
	// Input: x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0] 
	//S5_1
	X[5] ^= (X[7] & X[6]);
	X[4] ^= (X[3] & X[5]);
	X[7] ^= X[4];
	X[6] ^= X[3];
	X[3] ^= (X[4] | X[5]);
	X[5] ^= X[7];
	X[4] ^= (X[5] & X[6]);
	//S3
	X[2] ^= X[1] & X[0];
	X[0] ^= X[2] | X[1];
	X[1] ^= X[2] | X[0];
	X[2] = ~X[2];
	// Extend XOR
	X[7] ^= X[1];	X[3] ^= X[2];	X[4] ^= X[0];
	//S5_2
	T[0] = X[7];	T[1] = X[3];	T[2] = X[4];
	X[6] ^= (T[0] & X[5]);
	T[0] ^= X[6];
	X[6] ^= (T[2] | T[1]);
	T[1] ^= X[5];
	X[5] ^= (X[6] | T[2]);
	T[2] ^= (T[1] & T[0]);
	// Truncate XOR and bit change
	X[2] ^= T[0];	T[0] = X[1] ^ T[2];	X[1] = X[0] ^ T[1];	X[0] = X[7];	X[7] = T[0];
	T[1] = X[3];	X[3] = X[6];	X[6] = T[1];
	T[2] = X[4];	X[4] = X[5];	X[5] = T[2];
	// Output: (MSb) x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0] (LSb)
}

void inv_sbox(uint8_t* X)
{	//(MSB: x[7], LSB: x[0]) 
	// Input: x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0] 

	uint8_t T[3] = { 0, };

	T[0] = X[7]; X[7] = X[0]; X[0] = X[1]; X[1] = T[0];
	T[0] = X[7];	T[1] = X[6];	T[2] = X[5];
	// S52 inv
	X[4] ^= (X[3] | T[2]);
	X[3] ^= (T[2] | T[1]);
	T[1] ^= X[4];
	T[0] ^= X[3];
	T[2] ^= (T[1] & T[0]);
	X[3] ^= (X[4] & X[7]);
	//  Extended XOR
	X[0] ^= T[1]; X[1] ^= T[2]; X[2] ^= T[0];
	T[0] = X[3]; X[3] = X[6]; X[6] = T[0];
	T[0] = X[5]; X[5] = X[4]; X[4] = T[0];
	//  Truncated XOR
	X[7] ^= X[1];	X[3] ^= X[2];	X[4] ^= X[0];
	// Inv_S5_1
	X[4] ^= (X[5] & X[6]);
	X[5] ^= X[7];
	X[3] ^= (X[4] | X[5]);
	X[6] ^= X[3];
	X[7] ^= X[4];
	X[4] ^= (X[3] & X[5]);
	X[5] ^= (X[7] & X[6]);
	// Inv_S3
	X[2] = ~X[2];
	X[1] ^= X[2] | X[0];
	X[0] ^= X[2] | X[1];
	X[2] ^= X[1] & X[0];
	// Output: x[7], x[6], x[5], x[4], x[3], x[2], x[1], x[0]
}

//left rotation: (0,7,4,3,6,5,1,2)
void pbox(uint8_t* X)
{
	X[1] = ((X[1] << 7)) | ((X[1] >> 1));
	X[2] = ((X[2] << 4)) | ((X[2] >> 4));
	X[3] = ((X[3] << 3)) | ((X[3] >> 5));
	X[4] = ((X[4] << 6)) | ((X[4] >> 2));
	X[5] = ((X[5] << 5)) | ((X[5] >> 3));
	X[6] = ((X[6] << 1)) | ((X[6] >> 7));
	X[7] = ((X[7] << 2)) | ((X[7] >> 6));

}

//left rotation(inverse): (0,1,4,5,2,3,7,6)
void inv_pbox(uint8_t* X)
{
	X[1] = ((X[1] << 1)) | ((X[1] >> 7));
	X[2] = ((X[2] << 4)) | ((X[2] >> 4));
	X[3] = ((X[3] << 5)) | ((X[3] >> 3));
	X[4] = ((X[4] << 2)) | ((X[4] >> 6));
	X[5] = ((X[5] << 3)) | ((X[5] >> 5));
	X[6] = ((X[6] << 7)) | ((X[6] >> 1));
	X[7] = ((X[7] << 6)) | ((X[7] >> 2));
}
void pipo128_keygen(uint8_t* rks, const uint8_t* mk)
{
	int MASTER_KEY_SIZE = 2;
	int ROUND = 13;
	uint8_t ROUND_KEY[112] = { 0, };
	uint32_t i, j;
	uint32_t RCON = 0;
	
	uint8_t MASTER_KEY[16] = { 0, };
	memcpy(MASTER_KEY, mk, 16);

	
	
	uint8_t t = 0;
	
		ROUND_KEY[0] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[1] = MASTER_KEY[1];
		ROUND_KEY[2] = MASTER_KEY[2];
		ROUND_KEY[3] = MASTER_KEY[3];
		ROUND_KEY[4] = MASTER_KEY[4];
		ROUND_KEY[5] = MASTER_KEY[5];
		ROUND_KEY[6] = MASTER_KEY[6];
		ROUND_KEY[7] = MASTER_KEY[7] ; 
		ROUND_KEY[8] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[9] = MASTER_KEY[9];
		ROUND_KEY[10] = MASTER_KEY[10];
		ROUND_KEY[11] = MASTER_KEY[11];
		ROUND_KEY[12] = MASTER_KEY[12];
		ROUND_KEY[13] = MASTER_KEY[13];
		ROUND_KEY[14] = MASTER_KEY[14];
		ROUND_KEY[15] = MASTER_KEY[15];

		ROUND_KEY[16] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[17] = MASTER_KEY[1];
		ROUND_KEY[18] = MASTER_KEY[2];
		ROUND_KEY[19] = MASTER_KEY[3];
		ROUND_KEY[20] = MASTER_KEY[4];
		ROUND_KEY[21] = MASTER_KEY[5];
		ROUND_KEY[22] = MASTER_KEY[6];
		ROUND_KEY[23] = MASTER_KEY[7];
		ROUND_KEY[24] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[25] = MASTER_KEY[9];
		ROUND_KEY[26] = MASTER_KEY[10];
		ROUND_KEY[27] = MASTER_KEY[11];
		ROUND_KEY[28] = MASTER_KEY[12];
		ROUND_KEY[29] = MASTER_KEY[13];
		ROUND_KEY[30] = MASTER_KEY[14];
		ROUND_KEY[31] = MASTER_KEY[15];

		ROUND_KEY[32] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[33] = MASTER_KEY[1];
		ROUND_KEY[34] = MASTER_KEY[2];
		ROUND_KEY[35] = MASTER_KEY[3];
		ROUND_KEY[36] = MASTER_KEY[4];
		ROUND_KEY[37] = MASTER_KEY[5];
		ROUND_KEY[38] = MASTER_KEY[6];
		ROUND_KEY[39] = MASTER_KEY[7];
		ROUND_KEY[40] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[41] = MASTER_KEY[9];
		ROUND_KEY[42] = MASTER_KEY[10];
		ROUND_KEY[43] = MASTER_KEY[11];
		ROUND_KEY[44] = MASTER_KEY[12];
		ROUND_KEY[45] = MASTER_KEY[13];
		ROUND_KEY[46] = MASTER_KEY[14];
		ROUND_KEY[47] = MASTER_KEY[15];

		ROUND_KEY[48] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[49] = MASTER_KEY[1];
		ROUND_KEY[50] = MASTER_KEY[2];
		ROUND_KEY[51] = MASTER_KEY[3];
		ROUND_KEY[52] = MASTER_KEY[4];
		ROUND_KEY[53] = MASTER_KEY[5];
		ROUND_KEY[54] = MASTER_KEY[6];
		ROUND_KEY[55] = MASTER_KEY[7];
		ROUND_KEY[56] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[57] = MASTER_KEY[9];
		ROUND_KEY[58] = MASTER_KEY[10];
		ROUND_KEY[59] = MASTER_KEY[11];
		ROUND_KEY[60] = MASTER_KEY[12];
		ROUND_KEY[61] = MASTER_KEY[13];
		ROUND_KEY[62] = MASTER_KEY[14];
		ROUND_KEY[63] = MASTER_KEY[15];

		ROUND_KEY[64] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[65] = MASTER_KEY[1];
		ROUND_KEY[66] = MASTER_KEY[2];
		ROUND_KEY[67] = MASTER_KEY[3];
		ROUND_KEY[68] = MASTER_KEY[4];
		ROUND_KEY[69] = MASTER_KEY[5];
		ROUND_KEY[70] = MASTER_KEY[6];
		ROUND_KEY[71] = MASTER_KEY[7];
		ROUND_KEY[72] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[73] = MASTER_KEY[9];
		ROUND_KEY[74] = MASTER_KEY[10];
		ROUND_KEY[75] = MASTER_KEY[11];
		ROUND_KEY[76] = MASTER_KEY[12];
		ROUND_KEY[77] = MASTER_KEY[13];
		ROUND_KEY[78] = MASTER_KEY[14];
		ROUND_KEY[79] = MASTER_KEY[15];

		ROUND_KEY[80] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[81] = MASTER_KEY[1];
		ROUND_KEY[82] = MASTER_KEY[2];
		ROUND_KEY[83] = MASTER_KEY[3];
		ROUND_KEY[84] = MASTER_KEY[4];
		ROUND_KEY[85] = MASTER_KEY[5];
		ROUND_KEY[86] = MASTER_KEY[6];
		ROUND_KEY[87] = MASTER_KEY[7];
		ROUND_KEY[88] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[89] = MASTER_KEY[9];
		ROUND_KEY[90] = MASTER_KEY[10];
		ROUND_KEY[91] = MASTER_KEY[11];
		ROUND_KEY[92] = MASTER_KEY[12];
		ROUND_KEY[93] = MASTER_KEY[13];
		ROUND_KEY[94] = MASTER_KEY[14];
		ROUND_KEY[95] = MASTER_KEY[15];

		ROUND_KEY[96] = MASTER_KEY[0] ^ t; t++;
		ROUND_KEY[97] = MASTER_KEY[1];
		ROUND_KEY[98] = MASTER_KEY[2];
		ROUND_KEY[99] = MASTER_KEY[3];
		ROUND_KEY[100] = MASTER_KEY[4];
		ROUND_KEY[101] = MASTER_KEY[5];
		ROUND_KEY[102] = MASTER_KEY[6];
		ROUND_KEY[103] = MASTER_KEY[7];
		ROUND_KEY[104] = MASTER_KEY[8] ^ t; t++;
		ROUND_KEY[105] = MASTER_KEY[9];
		ROUND_KEY[106] = MASTER_KEY[10];
		ROUND_KEY[107] = MASTER_KEY[11];
		ROUND_KEY[108] = MASTER_KEY[12];
		ROUND_KEY[109] = MASTER_KEY[13];
		ROUND_KEY[110] = MASTER_KEY[14];
		ROUND_KEY[111] = MASTER_KEY[15];
	

	memcpy(rks, ROUND_KEY, 112);

}

void pipo128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{


	
	uint8_t* P = src;
	uint8_t* RK = rks;
	keyadd(P, RK);

	//1
	sbox(P);
	pbox(P);
	keyadd(P, RK + (8));
	
	sbox(P);
	pbox(P);
	keyadd(P, RK + (16));
	
	//3
	sbox(P);
	pbox(P);
	keyadd(P, RK + (24));
	 
	//4
	sbox(P);
	pbox(P);
	keyadd(P, RK + (32));
	 
	//5
	sbox(P);
	pbox(P);
	keyadd(P, RK + (40));
	
	//6
	sbox(P);
	pbox(P);
	keyadd(P, RK + (48));
	 
	//7
	sbox(P);
	pbox(P);
	keyadd(P, RK + (56));
	
	//8
	sbox(P);
	pbox(P);
	keyadd(P, RK + (64));
	
	//9
	sbox(P);
	pbox(P);
	keyadd(P, RK + (72));
	
	//10
	sbox(P);
	pbox(P);
	keyadd(P, RK + (80));
	
	sbox(P);
	pbox(P);
	keyadd(P, RK + (88));
	
	//12
	sbox(P);
	pbox(P);
	keyadd(P, RK + (96));
	
	//13
	sbox(P);
	pbox(P);
	keyadd(P, RK + (104));
	

	memcpy(dst, P, 8);

}

void pipo128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
	
	
	uint8_t* C = src;
	uint8_t* RK = rks;
	
	
	
	keyadd(C, RK + (104));//1
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (96));//2
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (88));//3
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (80));//4
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (72));//5
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (64));//6
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (56));//7
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (48));//8
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (40));//9
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (32));//10
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (24));//11
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (16));//12
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (8));//13
	inv_pbox(C);
	inv_sbox(C);;
	keyadd(C, RK);

	
	memcpy(dst, C, 8);
}

void pipo128_ctr_encrypt(uint8_t* dst, const uint8_t* src, const uint16_t src_len, const uint8_t* iv, const uint8_t* rks)
{
	unsigned int n = 0;
	//uint8_t counter_enc =0;
	uint8_t pt[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t counter_buf[8] = { 0,0,0,0,0,0,0,0 };
	memcpy(pt, src, 8);
	memcpy(counter_buf, iv, 8);
	uint16_t length = src_len;
	uint8_t encrypted[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t rks1 = rks;
	while (length--) {
		if (n == 0) {
			pipo128_encrypt(encrypted, counter_buf, rks1);
			counter_inc(counter_buf);
		}
		pt[0] = pt[0] ^ encrypted[0];
		pt[1] = pt[1] ^ encrypted[1];
		pt[2] = pt[2] ^ encrypted[2];
		pt[3] = pt[3] ^ encrypted[3];
		pt[4] = pt[4] ^ encrypted[4];
		pt[5] = pt[5] ^ encrypted[5];
		pt[6] = pt[6] ^ encrypted[6];
		pt[7] = pt[7] ^ encrypted[7];
		n = (n + 1) % 8;
	}
	memcpy(dst, pt, 8);

}

void pipo256_keygen(uint8_t* rks, const uint8_t* mk)
{
	
	uint8_t ROUND_KEY[144] = { 0, };
	uint8_t MASTER_KEY[32] = { 0, };
	memcpy(MASTER_KEY, mk, 32);
	uint8_t t = 0;
	
	
	
		ROUND_KEY[0] = MASTER_KEY[0] ^ t; t++;//0
		ROUND_KEY[1] = MASTER_KEY[1];
		ROUND_KEY[2] = MASTER_KEY[2];
		ROUND_KEY[3] = MASTER_KEY[3];
		ROUND_KEY[4] = MASTER_KEY[4];
		ROUND_KEY[5] = MASTER_KEY[5];
		ROUND_KEY[6] = MASTER_KEY[6];
		ROUND_KEY[7] = MASTER_KEY[7];
		ROUND_KEY[8] = MASTER_KEY[8] ^ t; t++;//1
		ROUND_KEY[9] = MASTER_KEY[9];
		ROUND_KEY[10] = MASTER_KEY[10];
		ROUND_KEY[11] = MASTER_KEY[11];
		ROUND_KEY[12] = MASTER_KEY[12];
		ROUND_KEY[13] = MASTER_KEY[13];
		ROUND_KEY[14] = MASTER_KEY[14];
		ROUND_KEY[15] = MASTER_KEY[15];
		ROUND_KEY[16] = MASTER_KEY[16] ^ t; t++;//2
		ROUND_KEY[17] = MASTER_KEY[17];
		ROUND_KEY[18] = MASTER_KEY[18];
		ROUND_KEY[19] = MASTER_KEY[19];
		ROUND_KEY[20] = MASTER_KEY[20];
		ROUND_KEY[21] = MASTER_KEY[21];
		ROUND_KEY[22] = MASTER_KEY[22];
		ROUND_KEY[23] = MASTER_KEY[23];
		ROUND_KEY[24] = MASTER_KEY[24] ^ t; t++;//3
		ROUND_KEY[25] = MASTER_KEY[25];
		ROUND_KEY[26] = MASTER_KEY[26];
		ROUND_KEY[27] = MASTER_KEY[27];
		ROUND_KEY[28] = MASTER_KEY[28];
		ROUND_KEY[29] = MASTER_KEY[29];
		ROUND_KEY[30] = MASTER_KEY[30];
		ROUND_KEY[31] = MASTER_KEY[31];

		ROUND_KEY[32] = MASTER_KEY[0] ^ t; t++;//4
		ROUND_KEY[33] = MASTER_KEY[1];
		ROUND_KEY[34] = MASTER_KEY[2];
		ROUND_KEY[35] = MASTER_KEY[3];
		ROUND_KEY[36] = MASTER_KEY[4];
		ROUND_KEY[37] = MASTER_KEY[5];
		ROUND_KEY[38] = MASTER_KEY[6];
		ROUND_KEY[39] = MASTER_KEY[7];
		ROUND_KEY[40] = MASTER_KEY[8] ^ t; t++;//5
		ROUND_KEY[41] = MASTER_KEY[9];
		ROUND_KEY[42] = MASTER_KEY[10];
		ROUND_KEY[43] = MASTER_KEY[11];
		ROUND_KEY[44] = MASTER_KEY[12];
		ROUND_KEY[45] = MASTER_KEY[13];
		ROUND_KEY[46] = MASTER_KEY[14];
		ROUND_KEY[47] = MASTER_KEY[15];
		ROUND_KEY[48] = MASTER_KEY[16] ^ t; t++;//6
		ROUND_KEY[49] = MASTER_KEY[17];
		ROUND_KEY[50] = MASTER_KEY[18];
		ROUND_KEY[51] = MASTER_KEY[19];
		ROUND_KEY[52] = MASTER_KEY[20];
		ROUND_KEY[53] = MASTER_KEY[21];
		ROUND_KEY[54] = MASTER_KEY[22];
		ROUND_KEY[55] = MASTER_KEY[23];
		ROUND_KEY[56] = MASTER_KEY[24] ^ t; t++;//7
		ROUND_KEY[57] = MASTER_KEY[25];
		ROUND_KEY[58] = MASTER_KEY[26];
		ROUND_KEY[59] = MASTER_KEY[27];
		ROUND_KEY[60] = MASTER_KEY[28];
		ROUND_KEY[61] = MASTER_KEY[29];
		ROUND_KEY[62] = MASTER_KEY[30];
		ROUND_KEY[63] = MASTER_KEY[31];

		ROUND_KEY[64] = MASTER_KEY[0] ^ t; t++;//8
		ROUND_KEY[65] = MASTER_KEY[1];
		ROUND_KEY[66] = MASTER_KEY[2];
		ROUND_KEY[67] = MASTER_KEY[3];
		ROUND_KEY[68] = MASTER_KEY[4];
		ROUND_KEY[69] = MASTER_KEY[5];
		ROUND_KEY[70] = MASTER_KEY[6];
		ROUND_KEY[71] = MASTER_KEY[7];
		ROUND_KEY[72] = MASTER_KEY[8] ^ t; t++;//9
		ROUND_KEY[73] = MASTER_KEY[9];
		ROUND_KEY[74] = MASTER_KEY[10];
		ROUND_KEY[75] = MASTER_KEY[11];
		ROUND_KEY[76] = MASTER_KEY[12];
		ROUND_KEY[77] = MASTER_KEY[13];
		ROUND_KEY[78] = MASTER_KEY[14];
		ROUND_KEY[79] = MASTER_KEY[15];
		ROUND_KEY[80] = MASTER_KEY[16] ^ t; t++;//10
		ROUND_KEY[81] = MASTER_KEY[17];
		ROUND_KEY[82] = MASTER_KEY[18];
		ROUND_KEY[83] = MASTER_KEY[19];
		ROUND_KEY[84] = MASTER_KEY[20];
		ROUND_KEY[85] = MASTER_KEY[21];
		ROUND_KEY[86] = MASTER_KEY[22];
		ROUND_KEY[87] = MASTER_KEY[23];
		ROUND_KEY[88] = MASTER_KEY[24] ^ t; t++;//11
		ROUND_KEY[89] = MASTER_KEY[25];
		ROUND_KEY[90] = MASTER_KEY[26];
		ROUND_KEY[91] = MASTER_KEY[27];
		ROUND_KEY[92] = MASTER_KEY[28];
		ROUND_KEY[93] = MASTER_KEY[29];
		ROUND_KEY[94] = MASTER_KEY[30];
		ROUND_KEY[95] = MASTER_KEY[31];

		ROUND_KEY[96] = MASTER_KEY[0] ^ t; t++;//12
		ROUND_KEY[97] = MASTER_KEY[1];
		ROUND_KEY[98] = MASTER_KEY[2];
		ROUND_KEY[99] = MASTER_KEY[3];
		ROUND_KEY[100] = MASTER_KEY[4];
		ROUND_KEY[101] = MASTER_KEY[5];
		ROUND_KEY[102] = MASTER_KEY[6];
		ROUND_KEY[103] = MASTER_KEY[7];
		ROUND_KEY[104] = MASTER_KEY[8] ^ t; t++;//13
		ROUND_KEY[105] = MASTER_KEY[9];
		ROUND_KEY[106] = MASTER_KEY[10];
		ROUND_KEY[107] = MASTER_KEY[11];
		ROUND_KEY[108] = MASTER_KEY[12];
		ROUND_KEY[109] = MASTER_KEY[13];
		ROUND_KEY[110] = MASTER_KEY[14];
		ROUND_KEY[111] = MASTER_KEY[15];
		ROUND_KEY[112] = MASTER_KEY[16] ^ t; t++;//14
		ROUND_KEY[113] = MASTER_KEY[17];
		ROUND_KEY[114] = MASTER_KEY[18];
		ROUND_KEY[115] = MASTER_KEY[19];
		ROUND_KEY[116] = MASTER_KEY[20];
		ROUND_KEY[117] = MASTER_KEY[21];
		ROUND_KEY[118] = MASTER_KEY[22];
		ROUND_KEY[119] = MASTER_KEY[23];//15
		ROUND_KEY[120] = MASTER_KEY[24] ^ t; t++;
		ROUND_KEY[121] = MASTER_KEY[25];
		ROUND_KEY[122] = MASTER_KEY[26];
		ROUND_KEY[123] = MASTER_KEY[27];
		ROUND_KEY[124] = MASTER_KEY[28];
		ROUND_KEY[125] = MASTER_KEY[29];
		ROUND_KEY[126] = MASTER_KEY[30];
		ROUND_KEY[127] = MASTER_KEY[31];
	//16¶ó¿îµå

		ROUND_KEY[128] = MASTER_KEY[0] ^ t; t++;//0
		ROUND_KEY[129] = MASTER_KEY[1];
		ROUND_KEY[130] = MASTER_KEY[2];
		ROUND_KEY[131] = MASTER_KEY[3];
		ROUND_KEY[132] = MASTER_KEY[4];
		ROUND_KEY[133] = MASTER_KEY[5];
		ROUND_KEY[134] = MASTER_KEY[6];
		ROUND_KEY[135] = MASTER_KEY[7];
		ROUND_KEY[136] = MASTER_KEY[8]^t;//0
		ROUND_KEY[137] = MASTER_KEY[9];
		ROUND_KEY[138] = MASTER_KEY[10];
		ROUND_KEY[139] = MASTER_KEY[11];
		ROUND_KEY[140] = MASTER_KEY[12];
		ROUND_KEY[141] = MASTER_KEY[13];
		ROUND_KEY[142] = MASTER_KEY[14];
		ROUND_KEY[143] = MASTER_KEY[15];
		
	

memcpy(rks, ROUND_KEY, 144);
}

void pipo256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
	uint8_t* P = src;
	uint8_t* RK = rks;
	keyadd(P, RK);

	//1
	sbox(P);
	pbox(P);
	keyadd(P, RK + (8));

	sbox(P);
	pbox(P);
	keyadd(P, RK + (16));

	//3
	sbox(P);
	pbox(P);
	keyadd(P, RK + (24));

	//4
	sbox(P);
	pbox(P);
	keyadd(P, RK + (32));

	//5
	sbox(P);
	pbox(P);
	keyadd(P, RK + (40));

	//6
	sbox(P);
	pbox(P);
	keyadd(P, RK + (48));

	//7
	sbox(P);
	pbox(P);
	keyadd(P, RK + (56));

	//8
	sbox(P);
	pbox(P);
	keyadd(P, RK + (64));

	//9
	sbox(P);
	pbox(P);
	keyadd(P, RK + (72));

	//10
	sbox(P);
	pbox(P);
	keyadd(P, RK + (80));

	sbox(P);
	pbox(P);
	keyadd(P, RK + (88));

	//12
	sbox(P);
	pbox(P);
	keyadd(P, RK + (96));

	//13
	sbox(P);
	pbox(P);
	keyadd(P, RK + (104));

	//14
	sbox(P);
	pbox(P);
	keyadd(P, RK + (112));
	//15
	sbox(P);
	pbox(P);
	keyadd(P, RK + (120));
	//16
	sbox(P);
	pbox(P);
	keyadd(P, RK + (128));
	//17
	sbox(P);
	pbox(P);
	keyadd(P, RK + (136));
	
	memcpy(dst, P, 8);
}

void pipo256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
	uint8_t* C = src;
	uint8_t* RK = rks;
	


	keyadd(C, RK + (136));//1
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (128));//2
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (120));//3
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (112));//4
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (104));//5
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (96));//6
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (88));//7
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (80));//8
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (72));//9
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (64));//10
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (56));//11
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (48));//12
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (40));//13
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (32));//14
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (24));//15
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (16));//16
	inv_pbox(C);
	inv_sbox(C);//i--;
	keyadd(C, RK + (8));//17
	inv_pbox(C);
	inv_sbox(C);;
	keyadd(C, RK);

	memcpy(dst, C, 8);

}

void pipo256_ctr_encrypt(uint8_t* dst, const uint8_t* src, const uint16_t src_len, const uint8_t* iv, const uint8_t* rks)
{
	unsigned int n = 0;
	//uint8_t counter_enc =0;
	uint8_t pt[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t counter_buf[8] = { 0,0,0,0,0,0,0,0 };
	memcpy(pt, src, 8);
	memcpy(counter_buf, iv, 8);
	uint16_t length = src_len;
	uint8_t encrypted[8] = { 0,0,0,0,0,0,0,0 };
	uint8_t rks1 = rks;
	while (length--) {
		if (n == 0) {
			pipo256_encrypt(encrypted, counter_buf, rks1);
			counter_inc(counter_buf);
		}
		pt[0] = pt[0] ^ encrypted[0];
		pt[1] = pt[1] ^ encrypted[1];
		pt[2] = pt[2] ^ encrypted[2];
		pt[3] = pt[3] ^ encrypted[3];
		pt[4] = pt[4] ^ encrypted[4];
		pt[5] = pt[5] ^ encrypted[5];
		pt[6] = pt[6] ^ encrypted[6];
		pt[7] = pt[7] ^ encrypted[7];
		n = (n + 1) % 8;
	}
	memcpy(dst, pt, src_len);

}

