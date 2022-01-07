#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstring>
#include <iomanip>

// https://github.com/System-Glitch/SHA256.git
class Sha256 {

public:
	Sha256(): m_blocklen(0), m_bitlen(0) {
	  m_state[0] = 0x6a09e667;
	  m_state[1] = 0xbb67ae85;
	  m_state[2] = 0x3c6ef372;
	  m_state[3] = 0xa54ff53a;
	  m_state[4] = 0x510e527f;
	  m_state[5] = 0x9b05688c;
	  m_state[6] = 0x1f83d9ab;
	  m_state[7] = 0x5be0cd19;
  }
	void update(const uint8_t * data, size_t length) {
	  for (size_t i = 0 ; i < length ; i++) {
		  m_data[m_blocklen++] = data[i];
		  if (m_blocklen == 64) {
			  transform();

			  // End of the block
			  m_bitlen += 512;
			  m_blocklen = 0;
		  }
	  }
  }
	void update(const std::string &data) {
	  update(reinterpret_cast<const uint8_t*> (data.c_str()), data.size());
  }
	uint8_t * digest() {
	  uint8_t * hash = new uint8_t[32];

	  pad();
	  revert(hash);

	  return hash;
  }

	static std::string toString(const uint8_t * digest) {
	  // mschoenebeck: implementation without stringstream
	  auto format = "%02x";
    auto size = std::snprintf(nullptr, 0, format, digest[0]) * 32;
    std::string output(size + 1, '\0');
    for(uint8_t i = 0 ; i < 32 ; i++) {
		  std::sprintf(&output[i*2], format, digest[i]);
	  }
    return output;
  }

private:
	uint8_t  m_data[64];
	uint32_t m_blocklen;
	uint64_t m_bitlen;
	uint32_t m_state[8]; //A, B, C, D, E, F, G, H

	const std::array<uint32_t, 64> K = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
		0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
		0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
		0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
		0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
		0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
		0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
		0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
		0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
		0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
		0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};
	
	static uint32_t rotr(uint32_t x, uint32_t n) {
	  return (x >> n) | (x << (32 - n));
  }

  static uint32_t choose(uint32_t e, uint32_t f, uint32_t g) {
	  return (e & f) ^ (~e & g);
  }

  static uint32_t majority(uint32_t a, uint32_t b, uint32_t c) {
	  return (a & (b | c)) | (b & c);
  }

  static uint32_t sig0(uint32_t x) {
	  return Sha256::rotr(x, 7) ^ Sha256::rotr(x, 18) ^ (x >> 3);
  }

  static uint32_t sig1(uint32_t x) {
	  return Sha256::rotr(x, 17) ^ Sha256::rotr(x, 19) ^ (x >> 10);
  }

  void transform() {
	  uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
	  uint32_t state[8];

	  for (uint8_t i = 0, j = 0; i < 16; i++, j += 4) { // Split data in 32 bit blocks for the 16 first words
		  m[i] = (m_data[j] << 24) | (m_data[j + 1] << 16) | (m_data[j + 2] << 8) | (m_data[j + 3]);
	  }

	  for (uint8_t k = 16 ; k < 64; k++) { // Remaining 48 blocks
		  m[k] = Sha256::sig1(m[k - 2]) + m[k - 7] + Sha256::sig0(m[k - 15]) + m[k - 16];
	  }

	  for(uint8_t i = 0 ; i < 8 ; i++) {
		  state[i] = m_state[i];
	  }

	  for (uint8_t i = 0; i < 64; i++) {
		  maj   = Sha256::majority(state[0], state[1], state[2]);
		  xorA  = Sha256::rotr(state[0], 2) ^ Sha256::rotr(state[0], 13) ^ Sha256::rotr(state[0], 22);

		  ch = choose(state[4], state[5], state[6]);

		  xorE  = Sha256::rotr(state[4], 6) ^ Sha256::rotr(state[4], 11) ^ Sha256::rotr(state[4], 25);

		  sum  = m[i] + K[i] + state[7] + ch + xorE;
		  newA = xorA + maj + sum;
		  newE = state[3] + sum;

		  state[7] = state[6];
		  state[6] = state[5];
		  state[5] = state[4];
		  state[4] = newE;
		  state[3] = state[2];
		  state[2] = state[1];
		  state[1] = state[0];
		  state[0] = newA;
	  }

	  for(uint8_t i = 0 ; i < 8 ; i++) {
		  m_state[i] += state[i];
	  }
  }

  void pad() {

	  uint64_t i = m_blocklen;
	  uint8_t end = m_blocklen < 56 ? 56 : 64;

	  m_data[i++] = 0x80; // Append a bit 1
	  while (i < end) {
		  m_data[i++] = 0x00; // Pad with zeros
	  }

	  if(m_blocklen >= 56) {
		  transform();
		  memset(m_data, 0, 56);
	  }

	  // Append to the padding the total message's length in bits and transform.
	  m_bitlen += m_blocklen * 8;
	  m_data[63] = m_bitlen;
	  m_data[62] = m_bitlen >> 8;
	  m_data[61] = m_bitlen >> 16;
	  m_data[60] = m_bitlen >> 24;
	  m_data[59] = m_bitlen >> 32;
	  m_data[58] = m_bitlen >> 40;
	  m_data[57] = m_bitlen >> 48;
	  m_data[56] = m_bitlen >> 56;
	  transform();
  }

  void revert(uint8_t * hash) {
	  // SHA uses big endian byte ordering
	  // Revert all bytes
	  for (uint8_t i = 0 ; i < 4 ; i++) {
		  for(uint8_t j = 0 ; j < 8 ; j++) {
			  hash[i + (j * 4)] = (m_state[j] >> (24 - i * 8)) & 0x000000ff;
		  }
	  }
  }
};


// https://stackoverflow.com/questions/17261798/converting-a-hex-string-to-a-byte-array
int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  return 0;
}
// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, unsigned char* target)
{
  while(*src && src[1])
  {
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
  }
}

// https://bitcoin.stackexchange.com/questions/76480/encode-decode-base-58-c
const char * const ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
const double iFactor = 1.36565823730976103695740418120764243208481439700722980119458355862779176747360903943915516885072037696111192757109;
// reslen is the allocated length for result, feel free to overallocate
int EncodeBase58(const unsigned char *source, int len, unsigned char result[], int reslen) {
    int zeros = 0, length = 0, pbegin = 0, pend;
    if (!(pend = len)) return 0;
    while (pbegin != pend && !source[pbegin]) pbegin = ++zeros;
    int size = 1 + iFactor * (double)(pend - pbegin);
    unsigned char b58[size];
    for (int i = 0; i < size; i++) b58[i] = 0;
    while (pbegin != pend) {
        unsigned int carry = source[pbegin];
        int i = 0;
        for (int it1 = size - 1; (carry || i < length) && (it1 != -1); it1--,i++) {
            carry += 256 * b58[it1];
            b58[it1] = carry % 58;
            carry /= 58;
        }
        if (carry) return 0;
        length = i;
        pbegin++;
    }
    int it2 = size - length;
    while ((it2-size) && !b58[it2]) it2++;
    if ((zeros + size - it2 + 1) > reslen) return 0;
    int ri = 0;
    while(ri < zeros) result[ri++] = '1';
    for (; it2 < size; ++it2) result[ri++] = ALPHABET[b58[it2]];
    result[ri] = 0;
    return ri;
}

// mschoenebeck: calculate liquidstorage uri from data string
std::string get_liquidstorage_uri(const std::string data)
{
    // calculate SHA256 hash
    Sha256 sha;
    sha.update(data);
    uint8_t * digest = sha.digest();
    std::string str = Sha256::toString(digest);
    
    // add liquidstorage bytes
    str = std::string("01551220") + str;
    
    // encode as base58
    unsigned char target[1024];
    hex2bin(str.c_str(), target);
    unsigned char result[1024];
    EncodeBase58(target, 4+32, result, 1024);
    
    return std::string("ipfs://z") + std::string((char*)result);
}

