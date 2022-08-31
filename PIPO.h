/*
 * PIPO.h
 *
 * Created: 2022-06-10 ¿ÀÈÄ 3:07:21
 *  Author:
 */


#ifndef PIPO_H_
#define PIPO_H_

#include <stdint.h>
#include <stddef.h>

void pipo128_keygen(uint8_t* rks, const uint8_t* mk);
void pipo128_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void pipo128_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void pipo128_ctr_encrypt(uint8_t* dst, const uint8_t* src, const uint16_t src_len, const uint8_t* iv, const uint8_t* rks);

void pipo256_keygen(uint8_t* rks, const uint8_t* mk);
void pipo256_encrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void pipo256_decrypt(uint8_t* dst, const uint8_t* src, const uint8_t* rks);
void pipo256_ctr_encrypt(uint8_t* dst, const uint8_t* src, const uint16_t src_len, const uint8_t* iv, const uint8_t* rks);


#endif /* PIPO_H_ */
