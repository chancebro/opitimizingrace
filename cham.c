/**
 * The MIT License
 *
 * Copyright (c) 2018-2020 Ilwoong Jeong (https://github.com/ilwoong)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "cham.h"

#include <string.h>

static const size_t BLOCKSIZE_64 = 8;
static const size_t BLOCKSIZE_128 = 16;

static const size_t CHAM_64_128_ROUNDS = 88;
static const size_t CHAM_128_128_ROUNDS = 112;
static const size_t CHAM_128_256_ROUNDS = 120;

	void counter_inc(uint8_t* counter) {
		counter[7]++;
	}
static inline uint16_t rol16(uint16_t value, size_t rot)
{
    return (value << rot) | (value >> (16 - rot));
}

static inline uint16_t ror16(uint16_t value, size_t rot)
{
    return (value >> rot) | (value << (16 - rot));
}

static inline uint32_t rol32(uint32_t value, size_t rot)
{
    return (value << rot) | (value >> (32 - rot));
}

static inline uint32_t ror32(uint32_t value, size_t rot)
{
    return (value >> rot) | (value << (32 - rot));
}

/**
 * CHAM 64-bit block, 128-bit key
 */ 
void cham64_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint16_t* key = (uint16_t*) mk;
    uint16_t* rk = (uint16_t*) rks;

   void keyRound(){
   for (size_t i = 0; i < 8; ++i) {
        rk[i] = key[i] ^ rol16(key[i], 1);
        rk[(i + 8) ^ (0x1)] = rk[i] ^ rol16(key[i], 11);
        rk[i] ^= rol16(key[i], 8);}
   }
	void keyRoundLoopRemove(){
        rk[0] = key[0] ^ rol16(key[0], 1);
        rk[(8)^(0x1)] = rk[0] ^ rol16(key[0], 11);
        rk[0] ^= rol16(key[0], 8);
		
		rk[1] = key[1] ^ rol16(key[1], 1);
        rk[(9)^(0x1)] = rk[1] ^ rol16(key[1], 11);
        rk[1] ^= rol16(key[1], 8);
		
		rk[2] = key[2] ^ rol16(key[2], 1);
        rk[(10)^(0x1)] = rk[2] ^ rol16(key[0], 11);
        rk[2] ^= rol16(key[2], 8);
		
		rk[3] = key[3] ^ rol16(key[3], 1);
        rk[(11)^(0x1)] = rk[3] ^ rol16(key[3], 11);
        rk[3] ^= rol16(key[3], 8);
		
		rk[4] = key[4] ^ rol16(key[4], 1);
        rk[(12)^(0x1)] = rk[4] ^ rol16(key[4], 11);
        rk[4] ^= rol16(key[4], 8);
		
		rk[5] = key[5] ^ rol16(key[5], 1);
        rk[(13)^(0x1)] = rk[5] ^ rol16(key[5], 11);
        rk[5] ^= rol16(key[5], 8);
		
		rk[6] = key[0] ^ rol16(key[6], 1);
        rk[(14)^(0x1)] = rk[6] ^ rol16(key[6], 11);
        rk[6] ^= rol16(key[6], 8);
		
		rk[7] = key[7] ^ rol16(key[7], 1);
        rk[(15)^(0x1)] = rk[7] ^ rol16(key[7], 11);
        rk[7] ^= rol16(key[7], 8);
	}
	keyRound();	
	//keyRoundLoopRemove();
}

void cham64_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint16_t blk[4] = {0,0,0,0};
    memcpy(blk, src, BLOCKSIZE_64);
	uint16_t temp = 0;
    const uint16_t* rk = (const uint16_t*) rks;
    uint16_t rc = 0;
	//이건그냥 ror함수 뺀거임
	void roundFucFake(){
	for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
        blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

        blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

        rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
    } memcpy(dst, blk, BLOCKSIZE_64);
	}
	//그냥 original code
	void roundFuc(){
	for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
        blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

        blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

        rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
    }

    memcpy(dst, blk, BLOCKSIZE_64);}
	//템프 사용 하지 않고 나중에 blk[0]에 ror8해주는 과정 
	void roundFucNotUseTemp(){
	for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
        blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

        blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
        blk[0]=rol16(blk[0],8);

        rk = (rk == (const uint16_t*)rks) ? rk + 8 : rk - 8;
    }
	memcpy(dst, blk, BLOCKSIZE_64);}
	//temp 사용해서  ror 연산 1개 줄임
	void roundFucUseTemp(){
	//템프에 (blk[0] ^ (rc++)) + (rol16(blk[1], 1)값저장하고 4라운드 ror8생략하는 코드
    for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

        temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
        blk[0] = rol16(temp, 8);
        blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);

        rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
    
	}
	
    memcpy(dst, blk, BLOCKSIZE_64);}
	
	//for루프 제거
	void roundFucloopRemove(){
		

	blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
	blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
	blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
	blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

	blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
	blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
	blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
	blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
	
	blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]), 8);
	blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
	blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
	blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[11]), 1);

	blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]), 8);
	blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
	blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
	blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[15]), 1);
//17~32
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
   
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[11]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[15]), 1);
   //32~48
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
   
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[11]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[15]), 1);
   //49~64
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
   
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[11]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[15]), 1);
   //64~80
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
   
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[11]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[15]), 1);
   //80~88
   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

   blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
   blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
   blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
   blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);
   
   
   memcpy(dst, blk, BLOCKSIZE_64);}
	//temp사용for루프제거
	void roundFucUseTemploopRemove(){
		//1
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[8]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[11]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[12]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[15]), 1);
		//2
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[8]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[11]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[12]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[15]), 1);
		//3
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[8]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[11]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[12]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[15]), 1);
		//4
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[8]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[11]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[12]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[15]), 1);
		//5
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[8]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[11]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[12]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[15]), 1);
		//5.5
		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[0]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[3]), 1);

		temp = (blk[0] ^ (rc++) + (rol16(blk[1], 1)) ^ rk[4]);
		blk[0] = rol16(temp, 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (temp ^ rk[7]), 1);
		memcpy(dst, blk, BLOCKSIZE_64);
	}
	void roundFucnew(){
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[3]), 1);

		blk[0] = rol16((blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]), 8);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (rol16(blk[0], 8) ^ rk[7]), 1);

		rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
		memcpy(dst, blk, BLOCKSIZE_64);}
		
	//ror하나 줄이고 loop사용 X
	void roundFucNotUseTemploopRemove(){
		//1
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[11]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[15]), 1);
		blk[0]=rol16(blk[0],8);
		//2
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[11]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[15]), 1);
		blk[0]=rol16(blk[0],8);
		//3
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[11]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[15]), 1);
		blk[0]=rol16(blk[0],8);
		//4
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[11]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[15]), 1);
		blk[0]=rol16(blk[0],8);
		//5
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[8]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[9]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[10]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[11]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[12]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[13]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[14]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[15]), 1);
		blk[0]=rol16(blk[0],8);
		//5.5
		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[0]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[1]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[2]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[3]), 1);
		blk[0]=rol16(blk[0],8);

		blk[0] = (blk[0] ^ (rc++)) + (rol16(blk[1], 1) ^ rk[4]);
		blk[1] = rol16((blk[1] ^ (rc++)) + (rol16(blk[2], 8) ^ rk[5]), 1);
		blk[2] = rol16((blk[2] ^ (rc++)) + (rol16(blk[3], 1) ^ rk[6]), 8);
		blk[3] = rol16((blk[3] ^ (rc++)) + (blk[0] ^ rk[7]), 1);
		blk[0]=rol16(blk[0],8);
		memcpy(dst, blk, BLOCKSIZE_64);
	}
		
   //roundFuc();//dec:loopremoce7050
   //roundFucFake();//cycle counter9878
   //roundFucnew();//dec:loopremoce6740
   roundFucloopRemove();//dec:loopremoce6740
   //roundFucUseTemp();//o3-cycle counter10110
   //roundFucNotUseTemp();//7338
   //roundFucUseTemploopRemove();//9605
   //roundFucNotUseTemploopRemove();dec:loopremoce6740
   
}
void cham64_ctr_encrypt(uint8_t* dst, const uint8_t* src, const uint16_t src_len, const uint8_t* counter, const uint8_t* rks)
{
	unsigned int n = 0;
	//uint8_t counter_enc =0;
	uint8_t pt[8]={0,0,0,0,0,0,0,0};
	uint8_t counter_buf[8]={0,0,0,0,0,0,0,0};	
	memcpy(pt,src,BLOCKSIZE_64);
	memcpy(counter_buf,counter,BLOCKSIZE_64);
	uint16_t length=src_len;
	uint8_t encrypted[8] = {0,0,0,0,0,0,0,0};

	
	while (length--) {
		if (n == 0) {
			cham64_encrypt(encrypted, counter_buf, rks);
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
		n = (n + 1) % BLOCKSIZE_64;
	}
	memcpy(dst, pt, src_len);
}

}
void cham64_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint16_t blk[4] = {0};
    memcpy(blk, src, BLOCKSIZE_64);
    
    const uint16_t* rk = (const uint16_t*) rks;
    uint16_t rc = CHAM_64_128_ROUNDS;
	void roundFucdec(){
		for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
			blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
			blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
			blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
			blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

			blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
			blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
			blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
			blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);

			rk = (rk == (const uint16_t*)rks) ? rk + 8 : rk - 8;
		}

		memcpy(dst, blk, BLOCKSIZE_64);}
	void roundFucdecrorde(){
    for (size_t round = 0; round < CHAM_64_128_ROUNDS; round += 8) {
		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);


        rk = (rk == (const uint16_t*) rks) ? rk + 8 : rk - 8;
    }

    memcpy(dst, blk, BLOCKSIZE_64);}
	
	//복호화 과정 루프 제거
	void roundFucdecLoopRemove(){
	//88~81
		

		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
	//80~64	
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[15])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[14])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[13])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[11])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[10])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[9])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);
		
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
		//63~48
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[15])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[14])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[13])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[11])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[10])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[9])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);
		
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
		//47~32
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[15])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[14])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[13])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[11])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[10])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[9])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);
		
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
		//31~16
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[15])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[14])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[13])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[11])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[10])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[9])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);
		
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
		//0~15
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[15])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[14])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[13])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[11])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[10])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[9])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);
		
		blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror16(blk[3], 1) - (rol16(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror16(blk[0], 8) - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);
		 memcpy(dst, blk, BLOCKSIZE_64);}
		 
		void roundFucdecrordeunloop(){
		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[15])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[14], 1) ^ rk[6])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[13], 8) ^ rk[5])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[11])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[10], 1) ^ rk[2])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[9], 8) ^ rk[1])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

		blk[0]=rol16(blk[0], 8);
		blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
		blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
		blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
		blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);


       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[15])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[14], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[13], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[11])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[10], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[9], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);


       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[15])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[14], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[13], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[11])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[10], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[9], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[15])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[14], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[13], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[11])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[10], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[9], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[15])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[14], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[13], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[12])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[11])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[10], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[9], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[8])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[7])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[6])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[5])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[4])) ^ (--rc);

       blk[0]=rol16(blk[0], 8);
       blk[3] = (ror16(blk[3], 1) - (blk[0] ^ rk[3])) ^ (--rc);
       blk[2] = (ror16(blk[2], 8) - (rol16(blk[3], 1) ^ rk[2])) ^ (--rc);
       blk[1] = (ror16(blk[1], 1) - (rol16(blk[2], 8) ^ rk[1])) ^ (--rc);
       blk[0] = (blk[0] - (rol16(blk[1], 1) ^ rk[0])) ^ (--rc);

		}
		//roundFucdecrorde();//7353
		//roundFucdec();//7353
		roundFucdecLoopRemove();//7050
		//roundFucdecrordeunloop();//8464
}


// * CHAM 128-bit block, 128-bit key

void cham128_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    for (size_t i = 0; i < 4; ++i) {
        rk[i] = key[i] ^ rol32(key[i], 1);
        rk[(i+4)^(0x1)] = rk[i] ^ rol32(key[i], 11);
        rk[i] ^= rol32(key[i], 8);
    }
}

void cham128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint32_t blk[4] = {0};
    memcpy(blk, src, BLOCKSIZE_128);
    
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t rc = 0;

    for (size_t round = 0; round < CHAM_128_128_ROUNDS; round += 8) {
		blk[0] = rol32((blk[0] ^ (rc++)) + (rol32(blk[1], 1) ^ rk[0]), 8);
        blk[1] = rol32((blk[1] ^ (rc++)) + (rol32(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol32((blk[2] ^ (rc++)) + (rol32(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol32((blk[3] ^ (rc++)) + (rol32(blk[0], 8) ^ rk[3]), 1);

        blk[0] = rol32((blk[0] ^ (rc++)) + (rol32(blk[1], 1) ^ rk[4]), 8);
        blk[1] = rol32((blk[1] ^ (rc++)) + (rol32(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol32((blk[2] ^ (rc++)) + (rol32(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol32((blk[3] ^ (rc++)) + (rol32(blk[0], 8) ^ rk[7]), 1);
    }

    memcpy(dst, blk, BLOCKSIZE_128);
}

void cham128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint32_t blk[4] = {0};
    memcpy(blk, src, BLOCKSIZE_128);
    
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t rc = CHAM_128_128_ROUNDS;

    for (size_t round = 0; round < CHAM_128_128_ROUNDS; round += 8) {        
        blk[3] = (ror32(blk[3], 1) - (rol32(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror32(blk[2], 8) - (rol32(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror32(blk[1], 1) - (rol32(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror32(blk[0], 8) - (rol32(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror32(blk[3], 1) - (rol32(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror32(blk[2], 8) - (rol32(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror32(blk[1], 1) - (rol32(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror32(blk[0], 8) - (rol32(blk[1], 1) ^ rk[0])) ^ (--rc);
    }

    memcpy(dst, blk, BLOCKSIZE_128);
}

/**
 * CHAM 128-bit block, 256-bit key
 */ 
void cham256_keygen(uint8_t* rks, const uint8_t* mk)
{
    const uint32_t* key = (uint32_t*) mk;
    uint32_t* rk = (uint32_t*) rks;

    for (size_t i = 0; i < 8; ++i) {
        rk[i] = key[i] ^ rol32(key[i], 1);
        rk[(i+8)^(0x1)] = rk[i] ^ rol32(key[i], 11);
        rk[i] ^= rol32(key[i], 8);
    }
}

void cham256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint32_t blk[4] = {0};
    memcpy(blk, src, BLOCKSIZE_128);
    
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t rc = 0;

    for (size_t round = 0; round < CHAM_128_256_ROUNDS; round += 8) {
        blk[0] = rol32((blk[0] ^ (rc++)) + (rol32(blk[1], 1) ^ rk[0]), 8);
        blk[1] = rol32((blk[1] ^ (rc++)) + (rol32(blk[2], 8) ^ rk[1]), 1);
        blk[2] = rol32((blk[2] ^ (rc++)) + (rol32(blk[3], 1) ^ rk[2]), 8);
        blk[3] = rol32((blk[3] ^ (rc++)) + (rol32(blk[0], 8) ^ rk[3]), 1);
        
        blk[0] = rol32((blk[0] ^ (rc++)) + (rol32(blk[1], 1) ^ rk[4]), 8);
        blk[1] = rol32((blk[1] ^ (rc++)) + (rol32(blk[2], 8) ^ rk[5]), 1);
        blk[2] = rol32((blk[2] ^ (rc++)) + (rol32(blk[3], 1) ^ rk[6]), 8);
        blk[3] = rol32((blk[3] ^ (rc++)) + (rol32(blk[0], 8) ^ rk[7]), 1);

        rk = (rk == (const uint32_t*) rks) ? rk + 8 : rk - 8;
    }

    memcpy(dst, blk, BLOCKSIZE_128);
}

void cham256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks)
{
    uint32_t blk[4] = {0};
    memcpy(blk, src, BLOCKSIZE_128);
    
    const uint32_t* rk = (const uint32_t*) rks;
    uint32_t rc = CHAM_128_256_ROUNDS;

    for (size_t round = 0; round < CHAM_128_256_ROUNDS; round += 8) {
        blk[3] = (ror32(blk[3], 1) - (rol32(blk[0], 8) ^ rk[7])) ^ (--rc);
        blk[2] = (ror32(blk[2], 8) - (rol32(blk[3], 1) ^ rk[6])) ^ (--rc);
        blk[1] = (ror32(blk[1], 1) - (rol32(blk[2], 8) ^ rk[5])) ^ (--rc);
        blk[0] = (ror32(blk[0], 8) - (rol32(blk[1], 1) ^ rk[4])) ^ (--rc);

        blk[3] = (ror32(blk[3], 1) - (rol32(blk[0], 8) ^ rk[3])) ^ (--rc);
        blk[2] = (ror32(blk[2], 8) - (rol32(blk[3], 1) ^ rk[2])) ^ (--rc);
        blk[1] = (ror32(blk[1], 1) - (rol32(blk[2], 8) ^ rk[1])) ^ (--rc);
        blk[0] = (ror32(blk[0], 8) - (rol32(blk[1], 1) ^ rk[0])) ^ (--rc);

        rk = (rk == (const uint32_t*) rks) ? rk + 8 : rk - 8;
    }

    memcpy(dst, blk, BLOCKSIZE_128);
	}

