// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace CryptoNote {
namespace parameters {

const uint32_t CRYPTONOTE_MAX_BLOCK_NUMBER                   = 500000000;
const size_t   CRYPTONOTE_MAX_BLOCK_BLOB_SIZE                = 500000000;
const size_t   CRYPTONOTE_MAX_TX_SIZE                        = 1000000000;
const uint64_t CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX       = 2;
const uint32_t CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW          = 10;
const uint64_t CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT            = 60 * 60 * 2;

const size_t   BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW             = 60;

// MONEY_SUPPLY - total number coins to be generated
const uint64_t MONEY_SUPPLY                                  = UINT64_C(20000000000000000);
const size_t MIN_MIXIN                          = 0;
const uint8_t MANDATORY_MIXIN_BLOCK_VERSION     = 0;
const uint32_t MIXIN_START_HEIGHT                            = 1;
const uint32_t MANDATORY_TRANSACTION                         = 0;
const unsigned EMISSION_SPEED_FACTOR                         = 25;
const uint64_t GENESIS_BLOCK_REWARD                          = UINT64_C(10000000000000000);
static_assert(EMISSION_SPEED_FACTOR <= 8 * sizeof(uint64_t), "Bad EMISSION_SPEED_FACTOR");

const size_t   CRYPTONOTE_REWARD_BLOCKS_WINDOW               = 100;
const size_t   CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE     = 200000; //size of block (bytes) after which reward for block calculated using block size
const size_t   CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V2  = 200000;
const size_t   CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_V1  = 20000;
const size_t   CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;
const size_t   CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE        = 600;
const size_t   CRYPTONOTE_DISPLAY_DECIMAL_POINT              = 8;
const uint64_t MINIMUM_FEE                                   = UINT64_C(10000);
const uint64_t DEFAULT_DUST_THRESHOLD                        = UINT64_C(10000);
const uint64_t MAX_TRANSACTION_SIZE_LIMIT                          = 10000000000000000;
const uint64_t DEFAULT_FEE                          = MINIMUM_FEE;

const uint64_t DIFFICULTY_TARGET                             = 10; // seconds
const uint64_t EXPECTED_NUMBER_OF_BLOCKS_PER_DAY             = 24 * 60 * 60 / DIFFICULTY_TARGET;
const size_t   DIFFICULTY_WINDOW                             = 90;
const size_t   DIFFICULTY_WINDOW_V1                          = DIFFICULTY_WINDOW;
const size_t   DIFFICULTY_WINDOW_V2                          = DIFFICULTY_WINDOW;
const size_t   DIFFICULTY_CUT                                = 30;  // timestamps to cut after sorting
const size_t   DIFFICULTY_CUT_V1                             = DIFFICULTY_CUT;
const size_t   DIFFICULTY_CUT_V2                             = DIFFICULTY_CUT;
const size_t   DIFFICULTY_LAG                                = 15;  // !!!
const size_t   DIFFICULTY_LAG_V1                             = DIFFICULTY_LAG;
const size_t   DIFFICULTY_LAG_V2                             = DIFFICULTY_LAG;
static_assert(2 * DIFFICULTY_CUT <= DIFFICULTY_WINDOW - 2, "Bad DIFFICULTY_WINDOW or DIFFICULTY_CUT");

const size_t   MAX_BLOCK_SIZE_INITIAL                        = 32655;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_NUMERATOR         = 100 * 1024;
const uint64_t MAX_BLOCK_SIZE_GROWTH_SPEED_DENOMINATOR       = 365 * 24 * 60 * 60 / DIFFICULTY_TARGET;

const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS     = 1;
const uint64_t CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS    = DIFFICULTY_TARGET * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS;

const uint64_t CRYPTONOTE_MEMPOOL_TX_LIVETIME                =  3600;
const uint64_t CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME =  3600;
const uint64_t CRYPTONOTE_NUMBER_OF_PERIODS_TO_FORGET_TX_DELETED_FROM_POOL  =  1;

const size_t   FUSION_TX_MAX_SIZE                            = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE_CURRENT * 30 / 100;
const size_t   FUSION_TX_MIN_INPUT_COUNT                     = 12;
const size_t   FUSION_TX_MIN_IN_OUT_COUNT_RATIO              = 4;

const uint32_t KEY_IMAGE_CHECKING_BLOCK_INDEX                = 0;
const uint32_t UPGRADE_HEIGHT_V2                                = 1;
const uint32_t UPGRADE_HEIGHT_V3                                = 2;
const uint32_t UPGRADE_HEIGHT_V4                                = 611164;
const uint32_t UPGRADE_HEIGHT_V5                              =1417827;
const unsigned UPGRADE_VOTING_THRESHOLD                      = 90;               // percent
const uint32_t UPGRADE_VOTING_WINDOW                         = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
const uint32_t UPGRADE_WINDOW                                = EXPECTED_NUMBER_OF_BLOCKS_PER_DAY;  // blocks
static_assert(0 < UPGRADE_VOTING_THRESHOLD && UPGRADE_VOTING_THRESHOLD <= 100, "Bad UPGRADE_VOTING_THRESHOLD");
static_assert(UPGRADE_VOTING_WINDOW > 1, "Bad UPGRADE_VOTING_WINDOW");

const char     CRYPTONOTE_BLOCKS_FILENAME[]                  = "blocks.bin";
const char     CRYPTONOTE_BLOCKINDEXES_FILENAME[]            = "blockindexes.bin";
const char     CRYPTONOTE_POOLDATA_FILENAME[]                = "poolstate.bin";
const char     P2P_NET_DATA_FILENAME[]                       = "p2pstate.bin";
const char     MINER_CONFIG_FILE_NAME[]                      = "miner_conf.json";
} // parameters

const char     CRYPTONOTE_NAME[]                             = "avesta";

const uint8_t  TRANSACTION_VERSION_1                         =  1;
const uint8_t  TRANSACTION_VERSION_2                         =  2;
const uint8_t  CURRENT_TRANSACTION_VERSION                   =  TRANSACTION_VERSION_1;
const uint8_t  BLOCK_MAJOR_VERSION_1                         =  1;
const uint8_t  BLOCK_MAJOR_VERSION_2                         =  2;
const uint8_t  BLOCK_MAJOR_VERSION_3                         =  3;
const uint8_t  BLOCK_MAJOR_VERSION_4                         =  4;
const uint8_t  BLOCK_MAJOR_VERSION_5						 =  5;
const uint8_t  BLOCK_MAJOR_VERSION_6						 =  6;
const uint8_t  BLOCK_MINOR_VERSION_0                         =  0;
const uint8_t  BLOCK_MINOR_VERSION_1                         =  1;

const size_t   BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT        =  1000;  //by default, blocks ids count in synchronizing
const size_t   BLOCKS_SYNCHRONIZING_DEFAULT_COUNT            =  100;   //by default, blocks count in blocks downloading
const size_t   COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT         =  1000;

const int      P2P_DEFAULT_PORT                              =  9921;
const int      RPC_DEFAULT_PORT                              =  9971;

const size_t   P2P_LOCAL_WHITE_PEERLIST_LIMIT                =  1000;
const size_t   P2P_LOCAL_GRAY_PEERLIST_LIMIT                 =  5000;

const size_t   P2P_CONNECTION_MAX_WRITE_BUFFER_SIZE          = 64 * 1024 * 1024; // 64 MB
const uint32_t P2P_DEFAULT_CONNECTIONS_COUNT                 = 18;
const size_t   P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT     = 70;
const uint32_t P2P_DEFAULT_HANDSHAKE_INTERVAL                = 60;            // seconds
const uint32_t P2P_DEFAULT_PACKET_MAX_SIZE                   = 500000000;      // 500000000 bytes maximum packet size
const uint32_t P2P_DEFAULT_PEERS_IN_HANDSHAKE                = 250;
const uint32_t P2P_DEFAULT_CONNECTION_TIMEOUT                = 5000;          // 5 seconds
const uint32_t P2P_DEFAULT_PING_CONNECTION_TIMEOUT           = 2000;          // 2 seconds
const uint64_t P2P_DEFAULT_INVOKE_TIMEOUT                    = 60 * 2 * 1000; // 2 minutes
const size_t   P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT          = 5000;          // 5 seconds

const uint32_t  P2P_FAILED_ADDR_FORGET_SECONDS                  = (60*60);     //1 hour
const uint32_t  P2P_IP_BLOCKTIME                                 = (60*60*24);  //24 hour
const uint32_t  P2P_IP_FAILS_BEFORE_BLOCK                       = 10;
const uint32_t  P2P_IDLE_CONNECTION_KILL_INTERVAL               = (5*60); //5 minutes

const char     P2P_STAT_TRUSTED_PUB_KEY[]                    = "";

const char* const SEED_NODES[] = { 
	"45.32.125.246:9921",
 	"45.32.125.246:9922",
 	"45.32.125.246:9923",
	"207.246.116.218:9921",
	"207.246.116.218:9922",
	"207.246.116.218:9923",
	"149.28.154.116:9921",
	"149.28.154.116:9922",
	"149.28.154.116:9923",
	"171.100.118.106:9921"};


const std::string DEFAULT_MINER_ADDRESS           = "1RAoK1EqtsFZ6pmiaRhafmGRLWfDomf2i2WwqBmfJpaN5gitfpx3vfYPXZ819rr5DfhJk5LqTvhkKGtHLwkUfot71vGw5ep";

struct CheckpointData {
  uint32_t index;
  const char* blockId;
};

const std::initializer_list<CheckpointData> CHECKPOINTS = { 
  {1000, "51cd17164582fdb5d88d74f8147fccf6a283974570c3de1b8437ed579eb8044f"}
, {5000, "e3322f24c7592ec473d4653be2c41f884207e31ade3b5c0fa0b2414bf86ac644"}
, {10000, "ead063e67bbdd56524d3958728f06e965ea2f3f76543c7a87d374ac43637edfb"}
, {15000, "394db4adadac8b8252fb4df2a115d7489c3bcba5983afa694515c2a936a9eb31"}
, {25000, "6cd4e715e9aa7740559e62c3f45e1f47a2e844c1d1657f50201533c172f43281"}
, {50000, "13307bc2205eace1a1c836ba396743ecd976deb55ab7f2ba9381726a8cf99c8a"}
, {100000, "278216dab74512afccb228426014dc8f3e20545650f339fa4b21208016293d0f"}
, {150000, "05dc9dcfcefa0260c97585f4d461e465af9efab5119523b7cea21d166d68468c"}
, {200000, "0d3f05b6f36f5015d334fbf092651aa3710f1ea6fee73619a16ec447f5cf262f"}
, {250000, "fbb63556a3aea865697d0fa5efec3cc9575235152b442d3b9ee3d96e110703bf"}
, {300000, "8e090e0e35b6e914c1a9b3115ab84387527d848def0e9868efcd7b0a224bdda4"}
, {400000, "5297a58515bbc4a80741554791f794c480d2025f7f0ad00c2988895bc374efa7"}
, {430000, "4c4dd01cf6bb2ef8d6471acc27f0060197ee02d9d86fc5f517c6bee466c5cdcb"}
, {434000, "ed462fd7c4bd3d154a6f3a70fef201d98e4ce6fbc80e09e7e102c4937edde5af"}
, {611000, "8a089fae8b807cc811feaaa38e1d2bbd63450b388a1e6cf8a1f17dd402422035"}
, {650000, "2d52971c9a0fac18978deb4a9d6f5f03b8087f54a50e2382d4c9a40340937691"}
, {675000, "0a9ca5b21d2cef7da4b5df3c9aee23ec8daaa2416f979cc623333e716546964e"}
, {700000, "20999500720b3db91f3a9ecfe20e3fc037c188285ba4499ac423f18c90720f50"}
, {725000, "a3012b80840510010e560dd49bbb5d4598725b7b5c848bbd9c92e654103b1489"}
, {750000, "a6a74157725967fc783947c59a7930e6a1993d85060c42403f3020f0c43a64eb"}
, {775000, "c5ba7b6db7049d834afb6c15c0aa08b924c84f7a2a39145092f5b036783e490f"}
, {800000, "05f92456218f1e61bf5e7c07fee74c1aa4912dee493fff75ed18b192c0dea568"}
, {825000, "8d0094df74e9b0ba24e9f9b0095cb0de54a9ca416d4b07b2d271c8ee1dd22d3a"}
, {848770, "01b27494401aec4cb2576b3b5fb3ce79c56c61d10b68208fe0091f7781e9116e"}
, {1422900,"cb914dc8db23ac397de03c6def2f01a1ce0701bf56ec239d15bfa0b3d9fe1a88"}
, {1500000, "a6cb82bcbc59f5885e5c028ec07f94f590a3762bc8053b94f47f29737e1b1471"}
, {1683200, "a446538487fa6b4fa08239cd599aeb4c4e4af1d6afad1f6557d95f855b287ab4"}
, {1690000,"010e4ae6a3c141c397771187f0f56e62db86aa6ddcb7f61fecf8fbe3aa8a45d5"}
, {1702000,"6ecbd7e3505b6999a87dafb68fecdc53cdfa09ddade557104fbe182759079d2d"}
};

} // CryptoNote

#define ALLOW_DEBUG_COMMANDS



