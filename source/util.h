#pragma once

#ifndef __UTIL_H__
#define __UTIL_H__

#include <switch.h>
#include "nca.h"

#define APP_BASE_PATH                   "sdmc:/switch/"
#define NXDUMPTOOL_BASE_PATH            APP_BASE_PATH "nxdumptool/"
#define XCI_DUMP_PATH                   NXDUMPTOOL_BASE_PATH "XCI/"
#define NSP_DUMP_PATH                   NXDUMPTOOL_BASE_PATH "NSP/"
#define HFS0_DUMP_PATH                  NXDUMPTOOL_BASE_PATH "HFS0/"
#define EXEFS_DUMP_PATH                 NXDUMPTOOL_BASE_PATH "ExeFS/"
#define ROMFS_DUMP_PATH                 NXDUMPTOOL_BASE_PATH "RomFS/"
#define CERT_DUMP_PATH                  NXDUMPTOOL_BASE_PATH "Certificate/"
#define BATCH_OVERRIDES_PATH            NSP_DUMP_PATH "BatchOverrides/"

#define KiB                             (1024.0)
#define MiB                             (1024.0 * KiB)
#define GiB                             (1024.0 * MiB)

#define NAME_BUF_LEN                    4096

#define SOCK_BUFFERSIZE                 65536

#define DUMP_BUFFER_SIZE                (u64)0x100000		                    // 1 MiB (1048576 bytes)

#define NCA_CTR_BUFFER_SIZE             DUMP_BUFFER_SIZE                        // 1 MiB (1048576 bytes)

#define NSP_XML_BUFFER_SIZE             (u64)0xA00000                           // 10 MiB (10485760 bytes)

#define APPLICATION_PATCH_BITMASK       (u64)0x800
#define APPLICATION_ADDON_BITMASK       (u64)0xFFFFFFFFFFFF0000

#define FILENAME_LENGTH                 512
#define FILENAME_MAX_CNT                20000
#define FILENAME_BUFFER_SIZE            (FILENAME_LENGTH * FILENAME_MAX_CNT)    // 10000 KiB

#define NACP_APPNAME_LEN                0x200
#define NACP_AUTHOR_LEN                 0x100
#define VERSION_STR_LEN                 0x40

#define GAMECARD_WAIT_TIME              3                                       // 3 seconds

#define GAMECARD_HEADER_SIZE            0x200
#define GAMECARD_SIZE_ADDR              0x10D
#define GAMECARD_DATAEND_ADDR           0x118

#define GAMECARD_ECC_BLOCK_SIZE         (u64)0x200                              // 512 bytes
#define GAMECARD_ECC_DATA_SIZE          (u64)0x24                               // 36 bytes

#define HFS0_OFFSET_ADDR                0x130
#define HFS0_SIZE_ADDR                  0x138
#define HFS0_MAGIC                      (u32)0x48465330                         // "HFS0"
#define HFS0_FILE_COUNT_ADDR            0x04
#define HFS0_STR_TABLE_SIZE_ADDR        0x08
#define HFS0_ENTRY_TABLE_ADDR           0x10

#define MEDIA_UNIT_SIZE                 0x200

#define GAMECARD_TYPE1_PARTITION_CNT    3                                       // "update" (0), "normal" (1), "update" (2)
#define GAMECARD_TYPE2_PARTITION_CNT    4                                       // "update" (0), "logo" (1), "normal" (2), "update" (3)
#define GAMECARD_TYPE(x)                ((x) == GAMECARD_TYPE1_PARTITION_CNT ? "Type 0x01" : ((x) == GAMECARD_TYPE2_PARTITION_CNT ? "Type 0x02" : "Unknown"))
#define GAMECARD_TYPE1_PART_NAMES(x)    ((x) == 0 ? "Update" : ((x) == 1 ? "Normal" : ((x) == 2 ? "Secure" : "Unknown")))
#define GAMECARD_TYPE2_PART_NAMES(x)    ((x) == 0 ? "Update" : ((x) == 1 ? "Logo" : ((x) == 2 ? "Normal" : ((x) == 3 ? "Secure" : "Unknown"))))
#define GAMECARD_PARTITION_NAME(x, y)   ((x) == GAMECARD_TYPE1_PARTITION_CNT ? GAMECARD_TYPE1_PART_NAMES(y) : ((x) == GAMECARD_TYPE2_PARTITION_CNT ? GAMECARD_TYPE2_PART_NAMES(y) : "Unknown"))

#define HFS0_TO_ISTORAGE_IDX(x, y)      ((x) == GAMECARD_TYPE1_PARTITION_CNT ? ((y) < 2 ? 0 : 1) : ((y) < 3 ? 0 : 1))

#define GAMECARD_SIZE_1GiB              (u64)0x40000000
#define GAMECARD_SIZE_2GiB              (u64)0x80000000
#define GAMECARD_SIZE_4GiB              (u64)0x100000000
#define GAMECARD_SIZE_8GiB              (u64)0x200000000
#define GAMECARD_SIZE_16GiB             (u64)0x400000000
#define GAMECARD_SIZE_32GiB             (u64)0x800000000

/* Reference: https://switchbrew.org/wiki/Title_list */
#define GAMECARD_UPDATE_TITLEID         (u64)0x0100000000000816

#define NACP_ICON_SQUARE_DIMENSION      256
#define NACP_ICON_DOWNSCALED            96

#define bswap_32(a)                     ((((a) << 24) & 0xff000000) | (((a) << 8) & 0xff0000) | (((a) >> 8) & 0xff00) | (((a) >> 24) & 0xff))
#define round_up(x, y)                  ((x) + (((y) - ((x) % (y))) % (y)))			// Aligns 'x' bytes to a 'y' bytes boundary

#define ORPHAN_ENTRY_TYPE_PATCH         1
#define ORPHAN_ENTRY_TYPE_ADDON         2

#define MAX_ELEMENTS(x)                 ((sizeof((x))) / (sizeof((x)[0])))          // Returns the max number of elements that can be stored in an array

typedef struct {
    u64 file_offset;
    u64 file_size;
    u32 filename_offset;
    u32 hashed_region_size;
    u64 reserved;
    u8 hashed_region_sha256[0x20];
} PACKED hfs0_entry_table;

typedef struct {
    int line_offset;
    u64 totalSize;
    char totalSizeStr[32];
    u64 curOffset;
    char curOffsetStr[32];
    u64 seqDumpCurOffset;
    u8 progress;
    u64 start;
    u64 now;
    u64 remainingTime;
    char etaInfo[32];
    double lastSpeed;
    double averageSpeed;
    u32 cancelBtnState;
    u32 cancelBtnStatePrev;
    u64 cancelStartTmr;
    u64 cancelEndTmr;
} PACKED progress_ctx_t;

typedef enum {
    ROMFS_TYPE_APP = 0,
    ROMFS_TYPE_PATCH,
    ROMFS_TYPE_ADDON
} selectedRomFsType;

typedef struct {
    u32 index;
    u8 type; // 1 = Patch, 2 = AddOn
} PACKED orphan_patch_addon_entry;

typedef enum {
    BATCH_SOURCE_ALL = 0,
    BATCH_SOURCE_SDCARD,
    BATCH_SOURCE_EMMC,
    BATCH_SOURCE_CNT
} batchModeSourceStorage;

typedef struct {
    bool isFat32;
    bool keepCert;
    bool trimDump;
    bool calcCrc;
    bool setXciArchiveBit;
} PACKED xciOptions;

typedef struct {
    bool isFat32;
    bool calcCrc;
    bool removeConsoleData;
    bool tiklessDump;
} PACKED nspOptions;

typedef struct {
    bool dumpAppTitles;
    bool dumpPatchTitles;
    bool dumpAddOnTitles;
    bool isFat32;
    bool removeConsoleData;
    bool tiklessDump;
    bool skipDumpedTitles;
    batchModeSourceStorage batchModeSrc;
    bool rememberDumpedTitles;
} PACKED batchOptions;

typedef struct {
    xciOptions xciDumpCfg;
    nspOptions nspDumpCfg;
    batchOptions batchDumpCfg;
} PACKED dumpOptions;

void loadConfig();

void saveConfig();

bool isGameCardInserted();

void fsGameCardDetectionThreadFunc(void *arg);

bool isServiceRunning(SmServiceName serviceName);

bool checkSxOsServices();

void delay(u8 seconds);

void formatETAString(u64 curTime, char *output, u32 outSize);

void initExeFsContext();

void freeExeFsContext();

void initRomFsContext();

void freeRomFsContext();

void initBktrContext();

void freeBktrContext();

void freeGlobalData();

bool loadTitlesFromSdCardAndEmmc(NcmContentMetaType titleType);

void freeTitlesFromSdCardAndEmmc(NcmContentMetaType titleType);

void convertTitleVersionToDecimal(u32 version, char *versionBuf, size_t versionBufSize);

void removeIllegalCharacters(char *name);

void createOutputDirectories();

void strtrim(char *str);

void loadTitleInfo();

bool getHfs0EntryDetails(u8 *hfs0Header, u64 hfs0HeaderOffset, u64 hfs0HeaderSize, u32 num_entries, u32 entry_idx, bool isRoot, u32 partitionIndex, u64 *out_offset, u64 *out_size);

bool getPartitionHfs0Header(u32 partition);

bool getHfs0FileList(u32 partition);

bool getPartitionHfs0FileByName(FsStorage *gameCardStorage, const char *filename, u8 *outBuf, u64 outBufSize);

bool calculateExeFsExtractedDataSize(u64 *out);

bool calculateRomFsFullExtractedSize(bool usePatch, u64 *out);

bool calculateRomFsExtractedDirSize(u32 dir_offset, bool usePatch, u64 *out);

bool readNcaExeFsSection(u32 titleIndex, bool usePatch);

bool readNcaRomFsSection(u32 titleIndex, selectedRomFsType curRomFsType);

bool getExeFsFileList();

bool getRomFsFileList(u32 dir_offset, bool usePatch);

int getSdCardFreeSpace(u64 *out);

void convertSize(u64 size, char *out, int bufsize);

void addStringToFilenameBuffer(const char *string, char **nextFilename);

char *generateFullDumpName();

char *generateNSPDumpName(nspDumpType selectedNspDumpType, u32 titleIndex);

void retrieveDescriptionForPatchOrAddOn(u64 titleID, u32 version, bool addOn, bool addAppName, const char *prefix, char *outBuf, size_t outBufSize);

bool checkOrphanPatchOrAddOn(bool addOn);

void generateOrphanPatchOrAddOnList();

bool checkIfBaseApplicationHasPatchOrAddOn(u32 appIndex, bool addOn);

bool checkIfPatchOrAddOnBelongsToBaseApplication(u32 titleIndex, u32 appIndex, bool addOn);

u32 retrieveFirstPatchOrAddOnIndexFromBaseApplication(u32 appIndex, bool addOn);

u32 retrievePreviousPatchOrAddOnIndexFromBaseApplication(u32 startTitleIndex, u32 appIndex, bool addOn);

u32 retrieveNextPatchOrAddOnIndexFromBaseApplication(u32 startTitleIndex, u32 appIndex, bool addOn);

u32 retrieveLastPatchOrAddOnIndexFromBaseApplication(u32 appIndex, bool addOn);

void waitForButtonPress();

void printProgressBar(progress_ctx_t *progressCtx, bool calcData, u64 chunkSize);

void setProgressBarError(progress_ctx_t *progressCtx);

bool cancelProcessCheck(progress_ctx_t *progressCtx);

void convertDataToHexString(const u8 *data, const u32 dataSize, char *outBuf, const u32 outBufSize);

bool checkIfFileExists(const char *path);

bool yesNoPrompt(const char *message);

bool checkIfDumpedXciContainsCertificate(const char *xciPath);

bool checkIfDumpedNspContainsConsoleData(const char *nspPath);

void removeDirectoryWithVerbose(const char *path, const char *msg);

void gameCardDumpNSWDBCheck(u32 crc);

void updateNSWDBXml();

void updateApplication();

#endif
