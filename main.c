// lab.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
void test_pipo128()
{
	uint32_t MASTER_KEY128[4] = { 0x2E152297 ,0x7E1D20AD ,0x779428D2 ,0x6DC416DD };
	uint32_t PLAIN_TEXT128[2] = { 0x1E270026 ,0x098552F6 };
	uint8_t enc[8] = { 0, };
	uint8_t dec[8] = { 0, };
	uint32_t rks128[28] = { 0, };
	uint8_t* MASTER_KEY = (uint8_t*)MASTER_KEY128;
	uint8_t* PLAIN_TEXT = (uint8_t*)PLAIN_TEXT128;

	pipo128_keygen(rks128, MASTER_KEY);
	pipo128_encrypt(enc, PLAIN_TEXT, rks128);
	printf("\n  Wenc: %02X%02X%02X%02X, %02X%02X%02X%02X", enc[7], enc[6], enc[5], enc[4], enc[3], enc[2], enc[1], enc[0]);
	pipo128_decrypt(dec, enc, rks128);
	printf("\n  Wdec: %02X%02X%02X%02X, %02X%02X%02X%02X", dec[7], dec[6], dec[5], dec[4], dec[3], dec[2], dec[1], dec[0]);

	// key schedule
	// 1-block encrypt/decrypt
	// CTR encrypt/decrypt
}

void test_pipo256()
{
	uint32_t MASTER_KEY256[8] = { 0x2E152297 ,0x7E1D20AD ,0x779428D2 ,0x6DC416DD,0x26D15633,0x54A71206,0x76A96DB5,0x009A3AA4 };
	uint32_t PLAIN_TEXT256[2] = { 0x1E270026 ,0x098552F6 };
	uint8_t enc[8] = { 0, };
	uint8_t dec[8] = { 0, };
	uint32_t rks256[36] = { 0, };
	uint8_t* MASTER_KEY = (uint8_t*)MASTER_KEY256;
	uint8_t* PLAIN_TEXT = (uint8_t*)PLAIN_TEXT256;

	pipo256_keygen(rks256, MASTER_KEY);
	pipo256_encrypt(enc, PLAIN_TEXT, rks256);
	printf("\n  Wenc: %02X%02X%02X%02X, %02X%02X%02X%02X", enc[7], enc[6], enc[5], enc[4], enc[3], enc[2], enc[1], enc[0]);
	pipo256_decrypt(dec, enc, rks256);
	printf("\n  Wdec: %02X%02X%02X%02X, %02X%02X%02X%02X", dec[7], dec[6], dec[5], dec[4], dec[3], dec[2], dec[1], dec[0]);
	// key schedule
	// 1-block encrypt/decrypt
	// CTR encrypt/decrypt
}

int main(void)
{
	// variables...

	test_pipo128();
	test_pipo256();

	/* Replace with your application code */

}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
