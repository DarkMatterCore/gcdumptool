#pragma once

#ifndef __NCA_H__
#define __NCA_H__

#include <switch.h>

#define NCA3_MAGIC                      (u32)0x4E434133     // "NCA3"
#define NCA2_MAGIC                      (u32)0x4E434132     // "NCA2"

#define NCA_HEADER_LENGTH               0x400
#define NCA_SECTION_HEADER_LENGTH       0x200
#define NCA_SECTION_HEADER_CNT          4
#define NCA_FULL_HEADER_LENGTH          (NCA_HEADER_LENGTH + (NCA_SECTION_HEADER_LENGTH * NCA_SECTION_HEADER_CNT))

#define NCA_AES_XTS_SECTOR_SIZE         0x200

#define NCA_KEY_AREA_KEY_CNT            4
#define NCA_KEY_AREA_KEY_SIZE           0x10
#define NCA_KEY_AREA_SIZE               (NCA_KEY_AREA_KEY_CNT * NCA_KEY_AREA_KEY_SIZE)

#define NCA_FS_HEADER_PARTITION_PFS0    0x01
#define NCA_FS_HEADER_FSTYPE_PFS0       0x02

#define NCA_FS_HEADER_PARTITION_ROMFS   0x00
#define NCA_FS_HEADER_FSTYPE_ROMFS      0x03

#define NCA_FS_HEADER_CRYPT_NONE        0x01
#define NCA_FS_HEADER_CRYPT_XTS         0x02
#define NCA_FS_HEADER_CRYPT_CTR         0x03
#define NCA_FS_HEADER_CRYPT_BKTR        0x04

#define PFS0_MAGIC                      (u32)0x50465330     // "PFS0"

#define IVFC_MAGIC                      (u32)0x49564643     // "IVFC"
#define IVFC_MAX_LEVEL                  6

#define BKTR_MAGIC                      (u32)0x424B5452     // "BKTR"

#define ROMFS_HEADER_SIZE               0x50
#define ROMFS_ENTRY_EMPTY               (u32)0xFFFFFFFF

#define ROMFS_NONAME_DIRENTRY_SIZE      0x18
#define ROMFS_NONAME_FILEENTRY_SIZE     0x20

#define ROMFS_ENTRY_DIR                 1
#define ROMFS_ENTRY_FILE                2

#define META_MAGIC                      (u32)0x4D455441     // "META"

#define NPDM_SIGNATURE_SIZE             0x100
#define NPDM_SIGNATURE_AREA_SIZE        0x200

#define NSP_NCA_FILENAME_LENGTH         0x25                // NCA ID + ".nca" + NULL terminator
#define NSP_CNMT_FILENAME_LENGTH        0x2A                // NCA ID + ".cnmt.nca" / ".cnmt.xml" + NULL terminator
#define NSP_PROGRAM_XML_FILENAME_LENGTH 0x31                // NCA ID + ".programinfo.xml" + NULL terminator
#define NSP_NACP_XML_FILENAME_LENGTH    0x2A                // NCA ID + ".nacp.xml" + NULL terminator
#define NSP_LEGAL_XML_FILENAME_LENGTH   0x2F                // NCA ID + ".legalinfo.xml" + NULL terminator
#define NSP_TIK_FILENAME_LENGTH         0x25                // Rights ID + ".tik" + NULL terminator
#define NSP_CERT_FILENAME_LENGTH        0x26                // Rights ID + ".cert" + NULL terminator

#define ETICKET_ENTRY_SIZE              0x400
#define ETICKET_TITLEKEY_OFFSET         0x180
#define ETICKET_RIGHTSID_OFFSET         0x2A0
#define ETICKET_UNKNOWN_FIELD_SIZE      0x140
#define ETICKET_DATA_OFFSET             0x140

#define ETICKET_CA_CERT_SIZE            0x400
#define ETICKET_XS_CERT_SIZE            0x300

#define ETICKET_TIK_FILE_SIZE           (ETICKET_ENTRY_SIZE - 0x140)
#define ETICKET_CERT_FILE_SIZE          (ETICKET_CA_CERT_SIZE + ETICKET_XS_CERT_SIZE)

#define ETICKET_TITLEKEY_COMMON         0
#define ETICKET_TITLEKEY_PERSONALIZED   1

typedef enum {
    DUMP_APP_NSP = 0,
    DUMP_PATCH_NSP,
    DUMP_ADDON_NSP
} nspDumpType;

typedef struct {
    u32 magic;
    u32 file_cnt;
    u32 str_table_size;
    u32 reserved;
} PACKED pfs0_header;

typedef struct {
    u64 file_offset;
    u64 file_size;
    u32 filename_offset;
    u32 reserved;
} PACKED pfs0_file_entry;

typedef struct {
    u32 media_start_offset;
    u32 media_end_offset;
    u8 _0x8[0x8]; /* Padding. */
} PACKED nca_section_entry_t;

typedef struct {
    u8 master_hash[0x20]; /* SHA-256 hash of the hash table. */
    u32 block_size; /* In bytes. */
    u32 always_2;
    u64 hash_table_offset; /* Normally zero. */
    u64 hash_table_size; 
    u64 pfs0_offset;
    u64 pfs0_size;
    u8 _0x48[0xF0];
} PACKED pfs0_superblock_t;

typedef struct {
    u64 logical_offset;
    u64 hash_data_size;
    u32 block_size;
    u32 reserved;
} PACKED ivfc_level_hdr_t;

typedef struct {
    u32 magic;
    u32 id;
    u32 master_hash_size;
    u32 num_levels;
    ivfc_level_hdr_t level_headers[IVFC_MAX_LEVEL];
    u8 _0xA0[0x20];
    u8 master_hash[0x20];
} PACKED ivfc_hdr_t;

typedef struct {
    ivfc_hdr_t ivfc_header;
    u8 _0xE0[0x58];
} PACKED romfs_superblock_t;

typedef struct {
    u64 offset;
    u64 size;
    u32 magic; /* "BKTR" */
    u32 _0x14; /* Version? */
    u32 num_entries;
    u32 _0x1C; /* Reserved? */
} PACKED bktr_header_t;

typedef struct {
    ivfc_hdr_t ivfc_header;
    u8 _0xE0[0x18];
    bktr_header_t relocation_header;
    bktr_header_t subsection_header;
} PACKED bktr_superblock_t;

/* NCA FS header. */
typedef struct {
    u8 _0x0;
    u8 _0x1;
    u8 partition_type;
    u8 fs_type;
    u8 crypt_type;
    u8 _0x5[0x3];
    union { /* FS-specific superblock. Size = 0x138. */
        pfs0_superblock_t pfs0_superblock;
        romfs_superblock_t romfs_superblock;
        bktr_superblock_t bktr_superblock;
    };
    union {
        u8 section_ctr[0x8];
        struct {
            u32 section_ctr_low;
            u32 section_ctr_high;
        };
    };
    u8 _0x148[0xB8]; /* Padding. */
} PACKED nca_fs_header_t;

/* Nintendo content archive header. */
typedef struct {
    u8 fixed_key_sig[0x100]; /* RSA-PSS signature over header with fixed key. */
    u8 npdm_key_sig[0x100]; /* RSA-PSS signature over header with key in NPDM. */
    u32 magic;
    u8 distribution; /* System vs gamecard. */
    u8 content_type;
    u8 crypto_type; /* Which keyblob (field 1) */
    u8 kaek_ind; /* Which kaek index? */
    u64 nca_size; /* Entire archive size. */
    u64 title_id;
    u8 _0x218[0x4]; /* Padding. */
    union {
        u32 sdk_version; /* What SDK was this built with? */
        struct {
            u8 sdk_revision;
            u8 sdk_micro;
            u8 sdk_minor;
            u8 sdk_major;
        };
    };
    u8 crypto_type2; /* Which keyblob (field 2) */
    u8 fixed_key_generation;
    u8 _0x222[0xE]; /* Padding. */
    u8 rights_id[0x10]; /* Rights ID (for titlekey crypto). */
    nca_section_entry_t section_entries[4]; /* Section entry metadata. */
    u8 section_hashes[4][0x20]; /* SHA-256 hashes for each section header. */
    u8 nca_keys[4][0x10]; /* Key area (encrypted) */
    u8 _0x340[0xC0]; /* Padding. */
    nca_fs_header_t fs_headers[4]; /* FS section headers. */
} PACKED nca_header_t;

typedef struct {
    u32 magic;
    u32 _0x4;
    u32 _0x8;
    u8 mmu_flags;
    u8 _0xD;
    u8 main_thread_prio;
    u8 default_cpuid;
    u64 _0x10;
    u32 process_category;
    u32 main_stack_size;
    char title_name[0x50];
    u32 aci0_offset;
    u32 aci0_size;
    u32 acid_offset;
    u32 acid_size;
} PACKED npdm_t;

typedef struct {
    u64 title_id;
    u32 version;
    u8 type;
    u8 unk1;
    u16 extended_header_size;
    u16 content_cnt;
    u16 content_meta_cnt;
    u8 attr;
    u8 unk2[0x03];
    u32 required_dl_sysver;
    u8 unk3[0x04];
} PACKED cnmt_header;

typedef struct {
    u64 patch_tid;  /* Patch TID / Application TID */
    u32 min_sysver; /* Minimum system/application version */
    u32 min_appver; /* Minimum application version (only for base applications) */
} PACKED cnmt_extended_header;

typedef struct {
    u8 hash[0x20];
    u8 nca_id[0x10];
    u8 size[6];
    u8 type;
    u8 id_offset;
} PACKED cnmt_content_record;

typedef struct {
    u8 type;
    u64 title_id;
    u32 version;
    u32 required_dl_sysver;
    u32 nca_cnt;
    u8 digest[SHA256_HASH_SIZE];
    char digest_str[(SHA256_HASH_SIZE * 2) + 1];
    u8 min_keyblob;
    u32 min_sysver;
    u64 patch_tid;
    u32 min_appver;
} cnmt_xml_program_info;

typedef struct {
    u8 type;
    u8 nca_id[SHA256_HASH_SIZE / 2];
    char nca_id_str[SHA256_HASH_SIZE + 1];
    u64 size;
    u8 hash[SHA256_HASH_SIZE];
    char hash_str[(SHA256_HASH_SIZE * 2) + 1];
    u8 keyblob;
    u8 id_offset;
    u64 cnt_record_offset; // Relative to the start of the content records section in the CNMT
    u8 decrypted_nca_keys[NCA_KEY_AREA_SIZE];
    u8 encrypted_header_mod[NCA_FULL_HEADER_LENGTH];
} cnmt_xml_content_info;

typedef struct {
    u32 nca_index;
    u8 *hash_table;
    u64 hash_table_offset; // Relative to NCA start
    u64 hash_table_size;
    u8 block_mod_cnt;
    u8 *block_data[2];
    u64 block_offset[2]; // Relative to NCA start
    u64 block_size[2];
} nca_program_mod_data;

typedef struct {
    u32 nca_index;
    u64 xml_size;
    char *xml_data;
    u8 nacp_icon_cnt; // Only used with Control NCAs
    nacp_icons_ctx *nacp_icons; // Only used with Control NCAs
} xml_record_info;

typedef struct {
    u64 section_offset; // Relative to NCA start
    u64 section_size;
    u64 hash_table_offset; // Relative to NCA start
    u64 hash_block_size;
    u32 hash_block_cnt;
    u64 pfs0_offset; // Relative to NCA start
    u64 pfs0_size;
    u64 title_cnmt_offset; // Relative to NCA start
    u64 title_cnmt_size;
} nca_cnmt_mod_data;

typedef struct {
    u32 sig_type;
    u8 signature[0x100];
    u8 padding[0x3C];
    char sig_issuer[0x40];
    u8 titlekey_block[0x100];
    u8 unk1;
    u8 titlekey_type;
    u8 unk2[0x03];
    u8 master_key_rev;
    u8 unk3[0x0A];
    u64 ticket_id;
    u64 device_id;
    u8 rights_id[0x10];
    u32 account_id;
    u8 unk4[0x0C];
} PACKED rsa2048_sha256_ticket;

typedef struct {
    bool has_rights_id;
    u8 rights_id[0x10];
    char rights_id_str[33];
    char tik_filename[37];
    char cert_filename[38];
    u8 enc_titlekey[0x10];
    u8 dec_titlekey[0x10];
    u8 cert_data[ETICKET_CERT_FILE_SIZE];
    rsa2048_sha256_ticket tik_data;
    bool retrieved_tik;
    bool missing_tik;
} title_rights_ctx;

// Used in HFS0 / ExeFS / RomFS browsers
typedef struct {
    u64 size;
    char sizeStr[32];
} browser_entry_size_info;

typedef struct {
    u8 type; // 1 = Dir, 2 = File
    u64 offset; // Relative to directory/file table, depending on type
    browser_entry_size_info sizeInfo; // Only used if type == 2
} romfs_browser_entry;

bool encryptNcaHeader(nca_header_t *input, u8 *outBuf, u64 outBufSize);

bool decryptNcaHeader(const u8 *ncaBuf, u64 ncaBufSize, nca_header_t *out, title_rights_ctx *rights_info, u8 *decrypted_nca_keys, bool retrieveTitleKeyData);

bool retrieveTitleKeyFromGameCardTicket(title_rights_ctx *rights_info, u8 *decrypted_nca_keys);

bool processProgramNca(NcmContentStorage *ncmStorage, const NcmContentId *ncaId, nca_header_t *dec_nca_header, cnmt_xml_content_info *xml_content_info, nca_program_mod_data **output, u32 *cur_mod_cnt, u32 idx);

bool retrieveCnmtNcaData(NcmStorageId curStorageId, u8 *ncaBuf, cnmt_xml_program_info *xml_program_info, cnmt_xml_content_info *xml_content_info, u32 cnmtNcaIndex, nca_cnmt_mod_data *output, title_rights_ctx *rights_info);

bool patchCnmtNca(u8 *ncaBuf, u64 ncaBufSize, cnmt_xml_program_info *xml_program_info, cnmt_xml_content_info *xml_content_info, nca_cnmt_mod_data *cnmt_mod);

#endif
