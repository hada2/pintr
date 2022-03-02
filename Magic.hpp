#pragma once

#include "pintr.hpp"

class GlobalMagicTable {
private:
    // database
    map<UINT64, string> _mapMagicNumberToCrypt;
    vector<string> _listCryptApi;
    
    // magic number
    static UINT64 SHA256_K_magic[];
    static UINT64 SHA512_K_magic[];
    static UINT64 AES_Rijndael_Te0_magic[];
    //static UINT64 AES_Rijndael_Te1_magic[];
    //static UINT64 AES_Rijndael_Te2_magic[];
    //static UINT64 AES_Rijndael_Te3_magic[];
    //static UINT64 AES_Rijndael_Te4_magic[];
    static UINT64 AES_Rijndael_Td0_magic[];
    //static UINT64 AES_Rijndael_Td1_magic[];
    //static UINT64 AES_Rijndael_Td2_magic[];
    //static UINT64 AES_Rijndael_Td3_magic[];
    //static UINT64 AES_Rijndael_Td4_magic[];
    static UINT64 Blowfish_P_array_magic[];
    static UINT64 Blowfish_S_boxes_magic[];
    static UINT64 SHA1_H_magic[];
    static UINT64 SHA224_H_magic[];
    static UINT64 SHA256_H_magic[];
    static UINT64 RC5_RC6_magic[];
    static UINT64 MD5_T_magic[];
    static UINT64 MD5_initstate_magic[];
    static UINT64 TEA_delta_magic[];
    static UINT64 xxHash32_PRIME32_magic[];
    static UINT64 xxHash64_PRIME64_magic[];

    // method
    VOID _initMapMagic();
    VOID _initListCryptApi();
    VOID _setupMapMagic(string, UINT64 *);

public:
    GlobalMagicTable();

    // method
    string isInMagicNumberMap(UINT64);
    BOOL isInCryptApiList(string);
};

/* TODO:
    DES_ip, DES_fp, DES_ei,
    DES_sbox1, DES_sbox2, DES_sbox3, DES_sbox4, DES_sbox5, DES_sbox6, DES_sbox7, DES_sbox8,
    DES_p32i, DES_pc1, DES_pc2,
    Rijndael_sbox, Rijndael_inv_sbox,
    Salsa20_ChaCha_sigma, Salsa20_ChaCha_tau, Camellia_sigma,
    Camellia_SBOX1, Camellia_SBOX2, Camellia_SBOX3, Camellia_SBOX4,
    xxHash64_PRIME64_1, xxHash64_PRIME64_2, xxHash64_PRIME64_3, xxHash64_PRIME64_4, xxHash64_PRIME64_5
*/




