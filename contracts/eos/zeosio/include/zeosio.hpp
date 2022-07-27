#pragma once

#include <vector>
#include <tuple>
#include <array>
#include <eosio/eosio.hpp>
#include "blake2s.h"

using namespace std;

namespace zeos
{
    namespace groth16
    {

        // helper from zeos-bellman/inc/groth16/bls12_381/util.hpp needed by Scalar
        /// Compute a + b + carry, returning the result and the new carry over.
        tuple<uint64_t, uint64_t> adc(uint64_t a, uint64_t b, uint64_t carry)
        {
            uint128_t ret = (uint128_t)a + (uint128_t)b + (uint128_t)carry;
            return tuple<uint64_t, uint64_t>{(uint64_t)ret, (uint64_t)(ret >> 64)};
        }
        /// Compute a - (b + borrow), returning the result and the new borrow.
        tuple<uint64_t, uint64_t> sbb(uint64_t a, uint64_t b, uint64_t borrow)
        {
            uint128_t ret = (uint128_t)a - ((uint128_t)b + (uint128_t)(borrow >> 63));
            return tuple<uint64_t, uint64_t>{(uint64_t)ret, (uint64_t)(ret >> 64)};
        }
        /// Compute a + (b * c) + carry, returning the result and the new carry over.
        tuple<uint64_t, uint64_t> mac(uint64_t a, uint64_t b, uint64_t c, uint64_t carry)
        {
            uint128_t ret = (uint128_t)a + ((uint128_t)b * (uint128_t)c) + (uint128_t)carry;
            return tuple<uint64_t, uint64_t>{(uint64_t)ret, (uint64_t)(ret >> 64)};
        }

        // copied from zeos-bellman/inc/groth16/bls12_381/scalar.hpp and cpp file to make a header
        // only implementation to be used by smart contracts to compute_multipacking of inputs on chain
        class Scalar
        {
            public:

                // constantslet cap = Scalar::CAPACITY;
                /// INV = -(q^{-1} mod 2^64) mod 2^64
                static const uint64_t INV;

                /// Constant representing the modulus
                /// q = 0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001
                static const Scalar MODULUS;
                // The number of bits needed to represent the modulus.
                static const uint32_t MODULUS_BITS;
                static const uint32_t NUM_BITS;
                static const uint32_t CAPACITY;

                static const Scalar R;
                static const Scalar ZERO;

                // members
                vector<uint64_t> data;

                Scalar() : data(vector<uint64_t>())
                {
                }

                Scalar(const vector<uint64_t>& data) : data(data)
                {
                }

                vector<uint8_t> to_bytes() const
                {
                    // Turn into canonical form by computing
                    // (a.R) / R = a
                    Scalar tmp = montgomery_reduce(this->data[0], this->data[1], this->data[2], this->data[3], 0, 0, 0, 0);

                    vector<uint8_t> res = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
                    uint8_t* p = (uint8_t*)tmp.data.data();
                    // mschoenebeck: the following code probably assumes little endian on the target machine. if target is big endian flip byte order here (UNTESTED)
                    //res[0..8].copy_from_slice(&tmp.0[0].to_le_bytes());
                    res[0] = p[0]; res[1] = p[1]; res[2] = p[2]; res[3] = p[3]; res[4] = p[4]; res[5] = p[5]; res[6] = p[6]; res[7] = p[7];
                    //res[8..16].copy_from_slice(&tmp.0[1].to_le_bytes());
                    res[8] = p[8]; res[9] = p[9]; res[10] = p[10]; res[11] = p[11]; res[12] = p[12]; res[13] = p[13]; res[14] = p[14]; res[15] = p[15];
                    //res[16..24].copy_from_slice(&tmp.0[2].to_le_bytes());
                    res[16] = p[16]; res[17] = p[17]; res[18] = p[18]; res[19] = p[19]; res[20] = p[20]; res[21] = p[21]; res[22] = p[22]; res[23] = p[23];
                    //res[24..32].copy_from_slice(&tmp.0[3].to_le_bytes());
                    res[24] = p[24]; res[25] = p[25]; res[26] = p[26]; res[27] = p[27]; res[28] = p[28]; res[29] = p[29]; res[30] = p[30]; res[31] = p[31];

                    return res;
                }

                Scalar montgomery_reduce(const uint64_t& r0,
                                         const uint64_t& r1,
                                         const uint64_t& r2,
                                         const uint64_t& r3,
                                         const uint64_t& r4,
                                         const uint64_t& r5,
                                         const uint64_t& r6,
                                         const uint64_t& r7) const
                {
                    // The Montgomery reduction here is based on Algorithm 14.32 in
                    // Handbook of Applied Cryptography
                    // <http://cacr.uwaterloo.ca/hac/about/chap14.pdf>.

                    uint64_t _, rr0 = r0, rr1 = r1, rr2 = r2, rr3 = r3, rr4 = r4, rr5 = r5, rr6 = r6, rr7 = r7, carry, carry2;
                    uint64_t k = rr0 * INV;
                    tie(_,   carry) = mac(rr0, k, MODULUS.data[0], 0);
                    tie(rr1, carry) = mac(rr1, k, MODULUS.data[1], carry);
                    tie(rr2, carry) = mac(rr2, k, MODULUS.data[2], carry);
                    tie(rr3, carry) = mac(rr3, k, MODULUS.data[3], carry);
                    tie(rr4, carry2) = adc(rr4, 0, carry);

                    k = rr1 * INV;
                    tie(_,   carry) = mac(rr1, k, MODULUS.data[0], 0);
                    tie(rr2, carry) = mac(rr2, k, MODULUS.data[1], carry);
                    tie(rr3, carry) = mac(rr3, k, MODULUS.data[2], carry);
                    tie(rr4, carry) = mac(rr4, k, MODULUS.data[3], carry);
                    tie(rr5, carry2) = adc(rr5, carry2, carry);

                    k = rr2 * INV;
                    tie(_,   carry) = mac(rr2, k, MODULUS.data[0], 0);
                    tie(rr3, carry) = mac(rr3, k, MODULUS.data[1], carry);
                    tie(rr4, carry) = mac(rr4, k, MODULUS.data[2], carry);
                    tie(rr5, carry) = mac(rr5, k, MODULUS.data[3], carry);
                    tie(rr6, carry2) = adc(rr6, carry2, carry);

                    k = rr3 * INV;
                    tie(_,   carry) = mac(rr3, k, MODULUS.data[0], 0);
                    tie(rr4, carry) = mac(rr4, k, MODULUS.data[1], carry);
                    tie(rr5, carry) = mac(rr5, k, MODULUS.data[2], carry);
                    tie(rr6, carry) = mac(rr6, k, MODULUS.data[3], carry);
                    tie(rr7, _) = adc(rr7, carry2, carry);

                    // Result may be within MODULUS of the correct value
                    return (Scalar(vector<uint64_t>{rr4, rr5, rr6, rr7})).sub(MODULUS);
                }

                static Scalar zero()
                {
                    return ZERO;
                }

                static Scalar one()
                {
                    return R;
                }

                Scalar sub(const Scalar& rhs) const
                {
                    uint64_t _, d0, d1, d2, d3, borrow, carry;
                    tie(d0, borrow) = sbb(this->data[0], rhs.data[0], 0);
                    tie(d1, borrow) = sbb(this->data[1], rhs.data[1], borrow);
                    tie(d2, borrow) = sbb(this->data[2], rhs.data[2], borrow);
                    tie(d3, borrow) = sbb(this->data[3], rhs.data[3], borrow);

                    // If underflow occurred on the final limb, borrow = 0xfff...fff, otherwise
                    // borrow = 0x000...000. Thus, we use it as a mask to conditionally add the modulus.
                    tie(d0, carry) = adc(d0, MODULUS.data[0] & borrow, 0);
                    tie(d1, carry) = adc(d1, MODULUS.data[1] & borrow, carry);
                    tie(d2, carry) = adc(d2, MODULUS.data[2] & borrow, carry);
                    tie(d3, _)     = adc(d3, MODULUS.data[3] & borrow, carry);

                    return Scalar(vector<uint64_t>{d0, d1, d2, d3});
                }

                Scalar operator - (const Scalar& rhs) const
                {
                    return this->sub(rhs);
                }

                Scalar add(const Scalar& rhs) const
                {
                    uint64_t _, d0, d1, d2, d3, carry;
                    tie(d0, carry) = adc(this->data[0], rhs.data[0], 0);
                    tie(d1, carry) = adc(this->data[1], rhs.data[1], carry);
                    tie(d2, carry) = adc(this->data[2], rhs.data[2], carry);
                    tie(d3, _)     = adc(this->data[3], rhs.data[3], carry);

                    // Attempt to subtract the modulus, to ensure the value
                    // is smaller than the modulus.
                    return Scalar(vector<uint64_t>{d0, d1, d2, d3}).sub(MODULUS);
                }

                Scalar operator + (const Scalar& rhs) const
                {
                    return this->add(rhs);
                }

                Scalar dbl() const
                {
                    return this->add(*this);
                }
        };
        // constants
        /// INV = -(q^{-1} mod 2^64) mod 2^64
        const uint64_t Scalar::INV = 0xffff'fffe'ffff'ffff;
        /// Constant representing the modulus
        /// q = 0x73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001
        const Scalar Scalar::MODULUS = Scalar(vector<uint64_t>{
            0xffff'ffff'0000'0001,
            0x53bd'a402'fffe'5bfe,
            0x3339'd808'09a1'd805,
            0x73ed'a753'299d'7d48
        });
        const uint32_t Scalar::MODULUS_BITS = 255;
        const uint32_t Scalar::NUM_BITS = Scalar::MODULUS_BITS;
        const uint32_t Scalar::CAPACITY = Scalar::NUM_BITS -1;
        /// R = 2^256 mod q
        const Scalar Scalar::R = Scalar(vector<uint64_t>{
            0x0000'0001'ffff'fffe,
            0x5884'b7fa'0003'4802,
            0x998c'4fef'ecbc'4ff5,
            0x1824'b159'acc5'056f,
        });
        const Scalar Scalar::ZERO = Scalar({0, 0, 0, 0});

        void append_bits(vector<bool>& bits, const uint64_t& val)
        {
            for(int i = 0; i < 64; i++)
            {
                bits.push_back((val>>i & 1) == 1);
            }
        }

        void append_bits(vector<bool>& bits, const checksum256& val)
        {
            array<uint8_t, 32> arr = val.extract_as_byte_array();
            for(int i = 0; i < 32; i++)
            {
                for(int j = 0; j < 8; j++)
                {
                    bits.push_back((arr[i]>>j & 1) == 1);
                }
            }
        }

        vector<Scalar> compute_multipacking(const vector<bool>& bits)
        {
            vector<Scalar> result;

            int chunks = bits.size() / Scalar::CAPACITY + (bits.size() % Scalar::CAPACITY == 0 ? 0 : 1);
            for(int i = 0; i < chunks; i++)
            {
                Scalar cur = Scalar::zero();
                Scalar coeff = Scalar::one();

                int chunk_end = (i+1)*Scalar::CAPACITY >= bits.size() ? bits.size() : (i+1)*Scalar::CAPACITY;
                for(int j = i*Scalar::CAPACITY; j < chunk_end; j++)
                {
                    if(bits[j])
                    {
                        cur = cur + coeff;
                    }

                    coeff = coeff.dbl();
                }

                result.push_back(cur);
            }

            return result;
        }

        string inputs_json(const vector<Scalar>& inputs)
        {
            string res = "[";

            for(int i = 0; i < inputs.size(); i++)
            {
                res.append("{\"data\":[");
                res.append(to_string(inputs[i].data[0]));
                res.push_back(',');
                res.append(to_string(inputs[i].data[1]));
                res.push_back(',');
                res.append(to_string(inputs[i].data[2]));
                res.push_back(',');
                res.append(to_string(inputs[i].data[3]));
                res.append("]}");
                if(i < inputs.size() - 1)
                {
                    res.push_back(',');
                }
            }

            res.push_back(']');

            return res;
        }

        // from: https://stackoverflow.com/a/47526992/2340535
        // adjusted to C types
        string BytesArrayToHexString(uint8_t* src)
        {
            const uint8_t n = 8;
            static const char table[] = "0123456789ABCDEF";
            char dst[2 * n + 1];
            const uint8_t* srcPtr = src;
            char* dstPtr = dst;
            for (auto count = n; count > 0; --count)
            {
                unsigned char c = *srcPtr++;
                *dstPtr++ = table[c >> 4];
                *dstPtr++ = table[c & 0x0f];
            }
            *dstPtr = 0;
            return &dst[0];
        }

        string byte2str(const uint8_t b)
        {
            static const char table[] = "0123456789ABCDEF";
            char dst[3];
            char* dstPtr = dst;
            *dstPtr++ = table[b >> 4];
            *dstPtr++ = table[b & 0x0f];
            *dstPtr = 0;
            return dst;
        }

        string inputs_hexstr(const vector<Scalar>& inputs)
        {
            string res = byte2str(inputs.size());

            for(int i = 0; i < inputs.size(); i++)
            {
                vector<unsigned char> s = inputs[i].to_bytes();
                for(auto c = s.begin(); c != s.end(); c++)
                {
                    res += byte2str(*c);
                }
            }

            return res;
        }
    } // namespace groth16
} // namespace zeos
