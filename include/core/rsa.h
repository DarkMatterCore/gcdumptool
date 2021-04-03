/*
 * rsa.c
 *
 * Copyright (c) 2018-2019, SciresM.
 * Copyright (c) 2018-2019, The-4n.
 * Copyright (c) 2020-2021, DarkMatterCore <pabloacurielz@gmail.com>.
 *
 * This file is part of nxdumptool (https://github.com/DarkMatterCore/nxdumptool).
 *
 * nxdumptool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * nxdumptool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef __RSA_H__
#define __RSA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RSA2048_SIG_SIZE    0x100
#define RSA2048_PUBKEY_SIZE RSA2048_SIG_SIZE

/// Generates a RSA-2048-PSS with SHA-256 signature using a custom RSA-2048 private key.
/// Suitable to replace the ACID signature in a Program NCA header.
/// Destination buffer size should be at least RSA2048_SIG_SIZE.
bool rsa2048GenerateSha256BasedPssSignature(void *dst, const void *src, size_t size);

/// Returns a pointer to the RSA-2048 public key that can be used to verify signatures generated by rsa2048GenerateSha256BasedPssSignature().
/// Suitable to replace the ACID public key in a NPDM.
const u8 *rsa2048GetCustomPublicKey(void);

/// Performs RSA-2048-OAEP decryption and verification. Used to decrypt the titlekey block from tickets with personalized crypto.
bool rsa2048OaepDecryptAndVerify(void *dst, size_t dst_size, const void *signature, const void *modulus, const void *exponent, size_t exponent_size, const void *label_hash, size_t *out_size);

#ifdef __cplusplus
}
#endif

#endif /* __RSA_H__ */