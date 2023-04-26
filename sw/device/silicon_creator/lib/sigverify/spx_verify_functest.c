// Copyright lowRISC contributors.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#include "sw/device/lib/testing/test_framework/check.h"
#include "sw/device/lib/testing/test_framework/ottf_main.h"
#include "sw/device/silicon_creator/lib/sigverify/sphincsplus/verify.h"
#include "sw/device/silicon_creator/lib/sigverify/spx_verify.h"

/**
 * Generated using:
 *
 * ```
 * # Private key (not used, for reference only)
 * opentitantool -- spx key generate <OUTPUT_DIR> test_key
 * cat test_key_spx.pem | tail -n +2 | head -n -1 | base64 -d | \
 *     hexdump -v -e '/4 "0x%08x, \n"'
 * # Public key
 * cat test_key_spx.pub.pem | tail -n +2 | head -n -1 | base64 -d | \
 *     hexdump -v -e '/4 "0x%08x, \n"'
 * # Signature
 * echo -n "test message" > test_message
 * bazel run //sw/host/opentitantool:opentitantool -- \
 *     spx key sign -o <SIGNATURE_FILE> <MESSAGE_FILE> <SECRET_KEY>
 * cat signature | hexdump -v -e '/4 "0x%08x, \n"'
 * ```
 */
static const char kMessage[] = "test message";
static const size_t kMessageLen = sizeof(kMessage) - 1;
static const uint32_t kPrivKey[] = {
    0x406e9f01, 0x776aa175, 0x13e120d3, 0xa59578b3, 0xac8e4258, 0x26e2f1a5,
    0x9ec7a442, 0xcd9a4c0a, 0x9e055279, 0xb04a291b, 0x62cd168e, 0x97343a52,
    0x09c2810f, 0x9b75ef78, 0x103247f8, 0xcd9a9eb2, 0x406e9f01, 0x776aa175,
    0x13e120d3, 0xa59578b3, 0xac8e4258, 0x26e2f1a5, 0x9ec7a442, 0xcd9a4c0a,
};
static const sigverify_spx_key_t kPubKey = {.data = {
                                                0x406e9f01,
                                                0x776aa175,
                                                0x13e120d3,
                                                0xa59578b3,
                                                0xac8e4258,
                                                0x26e2f1a5,
                                                0x9ec7a442,
                                                0xcd9a4c0a,
                                            }};
static sigverify_spx_signature_t kSignature = {
    .data = {
        0x646b0224, 0xca8a2af4, 0x41b345e5, 0xf7e651f3, 0xaa3fa3d7, 0xeb744930,
        0x556fa0cf, 0x51b662c4, 0x4b5c6901, 0x7135f979, 0xd5424601, 0xecfe1597,
        0x4cac9dd2, 0x31255637, 0xf3732782, 0x35a73bbb, 0x6db5cc5a, 0x69d01fa4,
        0xa0dcac57, 0x8e68187c, 0x8b712c36, 0x937e8219, 0xba721ca6, 0x7f86bc41,
        0x194ef55b, 0x685483cb, 0xcaca2185, 0x7abf748c, 0x91d6cc14, 0x31edabc0,
        0x388d5a37, 0x2cbb6f82, 0x1339b7d7, 0x4d10f4ec, 0x14857faa, 0x51d294c6,
        0x391b9814, 0x6b6f1640, 0x72ef573e, 0xe18cdc65, 0xc510d5ef, 0x8818b9f3,
        0x864af170, 0xd530663e, 0xa8bd41d5, 0x0942115b, 0xcde940c8, 0x35142ca0,
        0xaafd17af, 0x0618b994, 0x4198a928, 0x96194d47, 0x8462c0a9, 0xc9c7b588,
        0xc1b45b65, 0xe589ec66, 0x845d4b73, 0xfbf9a0aa, 0x9c176891, 0x4996cc6f,
        0x2f8da5be, 0x0bd67153, 0xdaf5141f, 0x4c442804, 0x99df3d8e, 0xd3aa0441,
        0xb48729fa, 0xe9c91766, 0xcf03d57a, 0x3c388f47, 0x4d6682c3, 0x66eead60,
        0x8532e077, 0xaea6fd11, 0x939aeeec, 0x04e40434, 0x0deaa121, 0x4450dd91,
        0xb0a4c8d6, 0x447db297, 0xe9bf995d, 0x389bd6fe, 0x0e8ed9a4, 0xda32269a,
        0xc826a9f4, 0x796c30ca, 0x5f43c22d, 0x87aeba27, 0x97733a37, 0x0b07bd54,
        0x6c324164, 0x03fd7924, 0x559b138c, 0xc744fc35, 0xa5a4fcd4, 0x7a2ec89e,
        0x568c7b29, 0xfaa6159f, 0x8e318157, 0xcd947d3e, 0xce0a0856, 0xaa4ea36d,
        0xdb0ae57c, 0xcced45fd, 0x18c89bb6, 0xf49fc67e, 0xfca42e43, 0x7bd5b99b,
        0x1704879b, 0x2fdeb391, 0x797f6ee6, 0x32addba1, 0xc65d8a02, 0x5d709121,
        0xacadbd93, 0x838449d2, 0xa791a719, 0xf1998098, 0x7a889072, 0xa9953721,
        0x60537286, 0x563ecb17, 0xe8300d3d, 0x5e808193, 0x204115e0, 0xc92d0408,
        0x21d1079f, 0x83a319dd, 0x0b810807, 0x008c2cfe, 0x69ce9f94, 0x5e148ec5,
        0xf9dc9d96, 0xc2cd9a7c, 0xb0bc34c2, 0xee35f644, 0x90648e54, 0x68e3dacc,
        0xfe9692d7, 0x29429f96, 0xae8c263a, 0xa2382f35, 0xc4499712, 0x98d77d92,
        0x762e40e9, 0x6dcec530, 0x8c7a9984, 0x95dc479b, 0xb677208b, 0xe17f23e1,
        0xf9f7c7e6, 0x1ea96462, 0xabbcb243, 0xf55cb409, 0x768f617c, 0x0cb339db,
        0x15dd8c27, 0x46993388, 0x20c9c08d, 0xee41c5d6, 0x65e6f576, 0x12da6369,
        0x3953e456, 0x233db6bc, 0xfb7cdecf, 0x16cc819f, 0x8a685a98, 0x1ff1d9d3,
        0x9bff0527, 0xc8bee9e4, 0x1bb65526, 0x622dafea, 0x95be0b6f, 0x57e6d9da,
        0xe08965e4, 0x2de4fbea, 0x652ab60a, 0xdb409a40, 0x10152ded, 0x5a38e3bb,
        0x52f2b2e9, 0x9d1b6ce7, 0xd6cc05f6, 0xb5a10146, 0xacff1037, 0x1aa7ad48,
        0x63cc2184, 0x92f855a1, 0x29c4c71d, 0xcb133878, 0x6b835a83, 0x2f46d8e6,
        0x7f38f3c1, 0x6a0fcb25, 0xcc2f8c68, 0x7a03da18, 0x54ba6b69, 0x5f239ac8,
        0x2af78372, 0x39ecca4c, 0x2f1a20f9, 0x1c794304, 0x3c97ed0c, 0xdbb2b360,
        0x80190ad0, 0xc0697ea2, 0x677164df, 0x0ea3e754, 0x8cb38376, 0xc8989840,
        0xef9c0128, 0x39154527, 0x074521da, 0xb62519d2, 0xff8d5bc0, 0x31d66951,
        0xaa2a5306, 0x3284cbe7, 0x0876ceb3, 0xd7aa99ba, 0x74dd87bf, 0x63273652,
        0xd4c100b1, 0x53d267b3, 0x87c60544, 0xe787eab8, 0x2684e13a, 0xc4314415,
        0xe08d0809, 0xff2bafd6, 0x591eb8ce, 0xc62db04d, 0x17668ca8, 0x7b59ef0e,
        0x9fb25c3e, 0x03363554, 0xea2ccd0d, 0x21553a16, 0xa7520d9f, 0x56e93072,
        0xdc5eb3ed, 0x9e8f9d40, 0x5cc902ef, 0x60b74785, 0xec5e7596, 0x9da71bc8,
        0x2cd591cb, 0xff608adb, 0x25b3a3d6, 0x90d32ec7, 0xac9cce72, 0x3dfe7e4a,
        0x31adb7e0, 0x897a91df, 0x6205fcf3, 0x7af73c18, 0x6ebcdc88, 0xaffce96a,
        0x4bfa5285, 0xcb1a54cd, 0xd1b8ae53, 0x1a6a95a5, 0x0cedae7f, 0x72d9715f,
        0x765c948c, 0xae97de73, 0x4bf49958, 0xe2dd5a4d, 0x9b3f7604, 0x083b52a5,
        0x4feb3c68, 0x188c42f9, 0x7f6c8851, 0x16fc08fb, 0x55c7b81a, 0x4f329513,
        0xf264ddac, 0x8d3a44b7, 0x43582a6d, 0x05b47cab, 0x2ec91eee, 0xaba5e586,
        0xdb094923, 0x2c4ea269, 0x5422ac65, 0xb0fbe60d, 0xb8d57d82, 0x5e922153,
        0xbfde6c4c, 0x07eadc15, 0x6f2e9253, 0x6a0ac751, 0xcddd3219, 0x23b4afe2,
        0xc07fdfcc, 0x50f048b3, 0x348037e8, 0x3d16c0ee, 0xd2370a08, 0xe3b11a2a,
        0x020cb3b2, 0x1f405845, 0xcad643e5, 0x73e4d821, 0x8a658b13, 0xea449c35,
        0x32ee1fbe, 0x67e97065, 0x1739cbfb, 0x9205e81d, 0x509284eb, 0xc693963c,
        0x9a4a6345, 0xefc3487f, 0xe6f23388, 0x9970a493, 0xd85e3142, 0x51289892,
        0x27bc5021, 0x88f1d0e2, 0xa7db963e, 0x63855d75, 0x90571174, 0xaaf36e85,
        0x6a4361dc, 0x9c22356c, 0x1fa5d648, 0xbf20f9cb, 0x3002f1bf, 0xe5f156ad,
        0xd64245ce, 0x52a7d473, 0x7858806c, 0xca9f09a0, 0x5d7e6507, 0xc6abe74a,
        0xc10d7ff0, 0xc42e4a77, 0x30b544ae, 0x8b181d7b, 0x4baff2f6, 0x6c896c84,
        0x9b65d645, 0x45599993, 0x22c74f97, 0xc91e3066, 0xdfa7eb91, 0xccf4e3b5,
        0xe18b1198, 0xe5c684e2, 0x5751a895, 0x7a1e850a, 0xbb0af0b7, 0xd24b94c9,
        0xfc20a8f5, 0xa51013bf, 0xe01c6459, 0x3f446e0d, 0x12cd2f8e, 0xdbd36282,
        0xa8c72e03, 0xfa9f41b7, 0x820ec19e, 0x89658d99, 0x162a63a4, 0xffb57cd1,
        0x825e0963, 0xf82d1036, 0x284bd0a8, 0x50f2e5be, 0xde7d0078, 0x3a1e4767,
        0x36747258, 0x21c7d40a, 0xcfea8f3d, 0xc4ca4b29, 0x8e656596, 0xfbae1b03,
        0x63e82115, 0xccafd14b, 0xae60d02b, 0x76caf594, 0x2cf10a98, 0xe1c247a8,
        0xbb5f3f1e, 0xfb73594e, 0x1a65a433, 0x51c98b2b, 0xd35e909e, 0x7c542381,
        0xaeb08fe6, 0xb4f1b454, 0xa63b6c34, 0xd137f541, 0x793ee044, 0x37d0abcb,
        0xe45bed9a, 0x006143cb, 0xcf38f0de, 0x90d000c0, 0xfef1b4e3, 0xb50cab35,
        0x1bcf8bf6, 0x4c8bc836, 0x37f5ac40, 0xa1dfed4b, 0x00766af4, 0x9762c0a8,
        0x0d4afb54, 0x1ace9371, 0x0be19e7f, 0x7cf625d0, 0x0cb64ffd, 0x452bb54b,
        0xb32518fd, 0x4e41616a, 0xbe9511a2, 0x355a2234, 0x51f6ec69, 0xdfcd8309,
        0x41f7ad2d, 0x55326894, 0xa5056a95, 0xa17a5ca1, 0x83e90594, 0x966cfe36,
        0x585187b7, 0xa65655d4, 0x15f28e8c, 0x7ab07738, 0x04a980a7, 0xf5d48ce6,
        0xa9f70826, 0x11ef7ccd, 0xb3fb79fe, 0xcb04e009, 0x338be72f, 0x67dd5103,
        0x2f9f7fa3, 0x7a798ed7, 0xce660197, 0x94233fbd, 0xca677fb0, 0x0a727657,
        0x34712e33, 0x4ad0da1f, 0x6867d5b9, 0xfa994ea2, 0xe3d31808, 0x53d1211d,
        0x0961011a, 0xc675783a, 0xd6e5ef00, 0x8e2c97b7, 0x9bb39ade, 0xb39eb474,
        0x063000d2, 0x77d3581b, 0x5127c5fb, 0x5ac38a22, 0xcec99c0d, 0x35222cd8,
        0xaef194c5, 0x0d8b5467, 0xaa623488, 0x1b1b7ca8, 0x1bd9985c, 0xe4367233,
        0x9bd90e8a, 0xbf72718d, 0xe603badf, 0x366aea88, 0x0ba21128, 0x77505046,
        0x86fa1185, 0xdf6b87d8, 0x0758773b, 0xbe913798, 0xb3df2820, 0x1c253003,
        0x823e5f46, 0x0cb62141, 0x708f427c, 0x61e9612b, 0xfc28d270, 0x8b90a8c1,
        0x59cff5a1, 0xbd2ca763, 0x53c43286, 0x5234ae3d, 0x4ecfd9ca, 0xad15866b,
        0x24ae29c4, 0x5eb44197, 0x0809097a, 0x8f1d22ab, 0x7865e3b2, 0x1fc33728,
        0xa1340245, 0xba1438cb, 0x77b41d93, 0xf5fb7193, 0xb2ec6f67, 0x268112a2,
        0x02ab9dd1, 0x6042400b, 0x18db80fc, 0x31a84e1b, 0xb6d2a17c, 0xb359131c,
        0x29602902, 0xca9afbdd, 0x8d24f017, 0x758c018b, 0x304c522f, 0x591e5d39,
        0x60aaaabc, 0x71c9946a, 0x6e3f00de, 0x254a0959, 0x9a27600e, 0x702cd820,
        0x85083598, 0x00e4f316, 0xf6c29cfe, 0xe3e3bc6b, 0xaf771774, 0xdd862a89,
        0x4968a968, 0x0e66658f, 0xe6304b59, 0x88340ba8, 0x37a48eca, 0xc3975ce1,
        0x864621fb, 0xcb62f682, 0x90c705cd, 0xbbcb6a66, 0xbe7d6729, 0xca1849ac,
        0xcc031928, 0xc18b9f9d, 0xf93e770c, 0xb8b3b7cd, 0xbc1d66f9, 0x025e01ec,
        0x8f2da816, 0xe36851d6, 0x9f51e890, 0x4daa3f90, 0x34fd6231, 0xf643a3c5,
        0xb6207aa1, 0x36aece10, 0xb2b823b2, 0x4f8a803d, 0x591f0725, 0xfb39c3d5,
        0xd4cb5a87, 0x9512ce40, 0x69416ade, 0x7d2be4d6, 0xa7128218, 0x71f7eafc,
        0x078d9b6d, 0xc7501823, 0xa8a573fe, 0xb99075cb, 0x0e9b839b, 0x05f1da7b,
        0x296c8ac8, 0x9dc5bb2a, 0x268e0ab9, 0x82fbbc43, 0x79bea814, 0x56634a6b,
        0xb1dfbfb6, 0x0b94d4eb, 0xa41b3bd3, 0xaae21cc5, 0xf328b220, 0x41416a94,
        0x5e2bb846, 0xc3d91e0b, 0x971aae66, 0x549c8f8a, 0x079f4fac, 0x39d9b51b,
        0x7f65b592, 0x2390d857, 0x33190165, 0xde2750e7, 0x4cc38309, 0xb1be671a,
        0x0054f741, 0xa1a111fc, 0xa6aaba8d, 0xc16a632a, 0x70e9c707, 0x1667d048,
        0x26d23e67, 0x0791a907, 0x1d930954, 0x4fdabe8d, 0xe87766cc, 0xac4df25d,
        0x365b7927, 0x0aeaf7a5, 0x3b08f605, 0x8cc75cbb, 0x7d5bf9f0, 0x0417171d,
        0x03d490c4, 0x474fa92b, 0x1686a955, 0x1de686ca, 0x806c9b24, 0x2b6f8dee,
        0x71f7ddf6, 0x41478095, 0x416b1f1b, 0xb8647ecf, 0x6f4919ac, 0xd5848af0,
        0x716f471b, 0x5d3d5520, 0x11ccc50b, 0x8c417364, 0x3a9face2, 0xf0fc0acb,
        0xebf4f1f4, 0xff4d478a, 0x69b504ae, 0xd39dc493, 0x39f2ed49, 0x66e52b22,
        0x47015983, 0x46b647b6, 0x0867d3be, 0x329ebaba, 0x0b67e9d6, 0xe9b65341,
        0x41b355bd, 0x9dc3b097, 0xc657afce, 0x962eadef, 0x1d5bf852, 0x93326554,
        0xff1931c8, 0x174fceec, 0xd854e736, 0x4c1d061c, 0x41cf49ad, 0x9dd87c55,
        0x6d00293c, 0x1a846161, 0x3aaa9b08, 0x9bed8d53, 0x93e643cf, 0xbf7609d6,
        0x4faf006a, 0xb93ed3a6, 0x568d9309, 0x1fe973db, 0x822b76d9, 0x85e1143d,
        0x30a582ad, 0xeff964c4, 0x51b5d429, 0xfb453f64, 0xf91213a7, 0x447243e9,
        0x254ced37, 0x50a2ab5a, 0x11ae769b, 0x704ef675, 0x224641b0, 0x2bb393eb,
        0x8bf3ab07, 0x535495d8, 0xb9b238b2, 0xb3f2bcf5, 0x2df34397, 0xe8045cd0,
        0x1fbb9371, 0x299c2e1b, 0x28003f6e, 0x2e7cdcaa, 0x43808c86, 0xf06dba59,
        0x9440f5a3, 0xb0b5a696, 0xe86bc9c2, 0xf6eb7cf2, 0x8b0708b8, 0x31b89749,
        0x11e0edf4, 0xc8e1e5ab, 0xb9a412d3, 0x416f6130, 0xd4dcfdd7, 0x8e94dab3,
        0xc1426f73, 0xbd269883, 0xbf7d2c12, 0xc2639ad7, 0x308a6bc0, 0x48457bcd,
        0xee3a2aa3, 0x3c419c4b, 0x7d1e0cfd, 0xb72a25b5, 0xc9d002d8, 0xa51acabc,
        0xe12d35c0, 0xed2159e6, 0x692c1e4f, 0x5678e34f, 0x31a2c91d, 0xd2c6dc01,
        0xd919b5ed, 0x049cb357, 0x63d6a5b9, 0x8863ee54, 0x79b9ccee, 0x98447d29,
        0x4c6393d6, 0xb58204d9, 0x871591b9, 0x458cb4ba, 0xefa8193e, 0xc382acad,
        0xe4a1e1e6, 0x0637d282, 0x89820405, 0xcdc66352, 0x29d631fb, 0x0c12ff1d,
        0xa6f29841, 0xfbaddeea, 0x72adab56, 0xbca4a45f, 0x14825fd1, 0xb6221a72,
        0x423cc728, 0xb1c054f4, 0x3bd87deb, 0x35879cc9, 0x5125ae5c, 0x131e008c,
        0xe5350016, 0xb319b181, 0x34bb3d3c, 0x6c9289a1, 0x6e178a97, 0xd65ff49f,
        0x3af18e19, 0x3f0b3ca9, 0x4cc57a9b, 0xdb7d828d, 0xde91e712, 0xf412d1ee,
        0x87505041, 0xd5505edb, 0x1f6f57eb, 0x9c7b0612, 0x7fb5f0c9, 0x025476f6,
        0x3f2fc1b8, 0xa218890c, 0x8a8d157c, 0x5bea5fe4, 0x9531fa36, 0xc77f0c8d,
        0x546ae32a, 0xd1f654ac, 0x7717c370, 0x79bf50a4, 0xdd3abe39, 0xdd47ea5f,
        0x35165830, 0x763e5417, 0x727ad86b, 0xb81a1539, 0x84c36a8f, 0x72e8abe9,
        0xc99478d5, 0xbd5396af, 0xf6ef5142, 0x739ac600, 0x674fafe1, 0x13b4fa28,
        0x177faa70, 0x16fd7b21, 0x6bf118d9, 0xf78ca83a, 0x5ec5d844, 0x6056f922,
        0x1936bc22, 0x33679d18, 0xa91f8549, 0x11d1d018, 0xc4fa8ea7, 0xf53702ba,
        0xc37eafd0, 0x14ab1c3c, 0xbbdb3b06, 0xbbec3aa2, 0xcb5d496a, 0x5bc97fc6,
        0xff143287, 0x86b1fd2d, 0xfc8547e0, 0xd30106f8, 0x6cbd6b86, 0xb763c40e,
        0x264933fe, 0x5854f8a1, 0x081d1f96, 0x67231b78, 0x16314829, 0xb247edbf,
        0x0315fc42, 0x8c922879, 0x1a1ac6c0, 0x3f769088, 0x91011dca, 0x0330f807,
        0x720bd57c, 0x5c2000cc, 0xbe3d6d29, 0x2a4c925b, 0x6ee0f853, 0x2e2314bd,
        0x08b61e4c, 0xec6dab24, 0x5da732f9, 0x8e79a458, 0x08fbe10d, 0x1e281c81,
        0xed035571, 0x8267f280, 0x2985d11d, 0xc6c3272d, 0x09315950, 0xc3f0817c,
        0x33e93d87, 0xd3f25a46, 0x08c9b690, 0x9b8b9215, 0x89e6c708, 0xb4c00556,
        0xfa6aa8a8, 0xcf8b1bf9, 0x9e9a7c7f, 0x19ac4769, 0x4fa86e7c, 0x93ffcdc8,
        0x07130a7c, 0x1f028bf2, 0x7c279b2a, 0x95e195ba, 0x9a801bb0, 0x27791192,
        0xb37e558a, 0x78dd0fa7, 0xfa91f144, 0x92792bf6, 0xf64fb173, 0xdb242574,
        0x96eb6ef3, 0xe8b93b80, 0xa98fc251, 0x6fdb8d72, 0xcdb3b890, 0x724ec17e,
        0x1b4f9a8c, 0x55cc2081, 0xc3c9fa09, 0x56393efb, 0xdc039d86, 0x23f4dbd1,
        0x9cfe52fc, 0xf2fae20f, 0x327d0ab4, 0xb163481c, 0xd789b374, 0x4fd32289,
        0xb630810c, 0x6e898c46, 0xfd3499db, 0x91418619, 0xc857cb5a, 0xe776d0a3,
        0x881347c2, 0x328326b3, 0x3ef1990e, 0x460511f3, 0x2de4a265, 0x9d93d269,
        0x9c1f2882, 0x5efa09eb, 0xa20efe87, 0x1d299b6a, 0xdea1c637, 0x3ddb2c10,
        0xbc0413e4, 0x9a492836, 0xd9be8e6a, 0x855f5ac4, 0x10bc3b70, 0x71de6da0,
        0xe0eda525, 0xda10a722, 0xb30f6614, 0x94d52499, 0xfa9e8671, 0x4c80ef29,
        0xd90259d2, 0x3b82177c, 0xf5c7044e, 0x43ac11d4, 0xf7627cff, 0x8e118940,
        0x13b97875, 0x16c18c1a, 0x6ae8e347, 0x902fd64e, 0x41636210, 0x1674091b,
        0x3f722f52, 0x1e189270, 0xa35d9c10, 0xfce33cd7, 0xe3523986, 0xd0051ab6,
        0x663b4f9c, 0x32dd79a2, 0x3b22ad1b, 0x92116208, 0x00f121c0, 0x74f4484d,
        0xc10731ee, 0x6c2c45ab, 0x65b1554f, 0xdc7d129c, 0x99003a36, 0x7b95ad48,
        0xd1974730, 0xf4e5d0ce, 0x84ca02d1, 0xb244d43c, 0x7146ec99, 0x9da36bca,
        0x58964f24, 0x7431987b, 0x9daff34b, 0x35dd6dc3, 0x43e9b441, 0x61a3864b,
        0x16b06cf8, 0xf940d72a, 0x9652230f, 0xf7c1f4b3, 0x39f53803, 0xacd2e8fa,
        0x3091f873, 0xc776cee1, 0x20810f30, 0x92aeb185, 0x18bec02b, 0xab74b0e2,
        0xeda3f387, 0xae09ffaf, 0xd3397c9c, 0x7f396d70, 0x28f107b8, 0xa1a8b1cb,
        0xd0b64b4b, 0xaf461cd9, 0xd5fa2922, 0x84bc7a64, 0xb38471cb, 0xe9f56dac,
        0x13f4c985, 0x93bf4ee5, 0x2e794d77, 0xe08550c7, 0xd6df947d, 0x6689016a,
        0x7e5ac34a, 0x2ff74e08, 0x095afb41, 0xff5426be, 0xec934b69, 0x09d09318,
        0x535c288d, 0x90432a51, 0x07b52ed5, 0xc6eadce6, 0xa6200c9b, 0x42c7ee00,
        0x56c2b1f3, 0x3235b6e1, 0xbdec2f04, 0x451edf19, 0x509d7950, 0xdd72cd40,
        0x619275b2, 0xd24ce6a3, 0xc098a218, 0x61b873fd, 0x1d41865f, 0x72468612,
        0x8eaf0757, 0xcadc294e, 0xacb8c9a8, 0x087d0426, 0xe2cdacf2, 0xf5e496ff,
        0x403bd54e, 0xdab1e267, 0x8be61cc1, 0x920f9ce9, 0x8096733d, 0x74d651dd,
        0x35f5bdbc, 0xbe838450, 0xf0e0ae2e, 0xf8dde864, 0xcf094397, 0x7604dbe1,
        0xec2b5afc, 0xc59107aa, 0x3427b5b5, 0xe67b081c, 0x30b7a7a7, 0xa99b95ac,
        0x923fa5ae, 0xfc12173f, 0x74f5f09d, 0xd3a82d6c, 0x1763c03d, 0xfa58d157,
        0x258a0a3f, 0xad1507fc, 0xf3137419, 0x49806748, 0xde611730, 0xa899d735,
        0x06e6cce0, 0xb1d76dfa, 0x325aaa2d, 0xe6ce57db, 0x61cfdd20, 0x2b8b4926,
        0xbdfed592, 0xf1f77fc7, 0xff4e88f7, 0x65d747a2, 0x851bfefa, 0xed762a6e,
        0x0a15b043, 0x84db7960, 0xdeddde78, 0x4a148c01, 0x8f092ca5, 0x6e4eccdc,
        0x04773dbb, 0x718c968d, 0x308bfdc0, 0x618b336d, 0x0ca36707, 0x74ee9fe1,
        0xede44aab, 0x514cfb0c, 0xfad0cf98, 0x852ccf09, 0xd5ed966c, 0x715e7f69,
        0x4451cf2e, 0xf1ddcde1, 0xcb29c396, 0x923d634c, 0xf8412f4f, 0xb63a29a6,
        0x448da593, 0xd905a7ab, 0x29e0274a, 0x062ebec8, 0x9fa6c4be, 0xfc8d1303,
        0xe38f50db, 0x048659fe, 0x070a1f26, 0x15ae0f40, 0x34151627, 0xa5aad1d7,
        0xf6a743fb, 0xe9549be9, 0x82ad44e9, 0xb9043484, 0xcbbdcbf7, 0x1bba3b14,
        0xc5236049, 0xb7f45803, 0x7e04fd64, 0xb663205b, 0x07ed92f1, 0x23ba820f,
        0xf4a4574a, 0x1d0a7fd6, 0xaef77176, 0x3d6adbfd, 0xb412c4a2, 0xf0afd9c3,
        0x1d66406f, 0xc5902e77, 0xdc53686d, 0xa12724d8, 0xcbed3cb5, 0x20fb86fc,
        0x5c9a5070, 0x3aaf7e85, 0x5b571292, 0x888c8651, 0x1a5354b1, 0xba3af587,
        0x732790a1, 0x3c0bb72a, 0xee761643, 0x090e6846, 0xd1a4f00f, 0xcefcc994,
        0x92d3e271, 0xea8961e7, 0x3e93f0ef, 0xf77b7dfe, 0x665296cf, 0x9f22765c,
        0xce31a360, 0xcfef44e7, 0xad5f46ad, 0x90a826aa, 0x8ca8b2ea, 0x3fd072e2,
        0xc72e3b07, 0xd434069a, 0x94e2b93d, 0x75007f82, 0x0a7dbb71, 0x2243e2f0,
        0xce57f8c2, 0x89f80b44, 0xa635938a, 0x40f69b2f, 0x123527c2, 0x387901b3,
        0xfb025429, 0x701c3621, 0x1cc7355f, 0x909af821, 0x9169557a, 0x63f24a9a,
        0x7cb48718, 0x74f21fd6, 0x5bab8101, 0x7a654c57, 0x37f112df, 0x7ecf558c,
        0x71557a98, 0x17681f66, 0xfb9e9cd6, 0x34636ae3, 0xab13b549, 0x3f0ecf8f,
        0xca04ce91, 0xf154a8f2, 0xbb6f62c3, 0xa446eb21, 0x0e2c2b1f, 0x6cdec7e7,
        0xb1cff81b, 0xba9d4db1, 0xd8eec4cd, 0xf3b09115, 0xc2b137ff, 0x11b5285c,
        0x67062672, 0x480fa6b6, 0x3fd9a49f, 0xd24f819f, 0x20549ce6, 0x420da3fc,
        0x958b0629, 0x355ca50b, 0x1c9b58a4, 0x219c19bb, 0x31955352, 0x26d3dc78,
        0x5927a227, 0x47820eb4, 0x88179f96, 0xa6833252, 0xe976ea62, 0x2e79aa05,
        0x7934f3d1, 0x4aa473a1, 0x1d8d162a, 0x5ba820de, 0x7853ecf1, 0x6bd89493,
        0xc059476c, 0xbded8ae9, 0xa5391df6, 0xa69c6386, 0xc0c619b2, 0xfc26fd5b,
        0x1104c2b4, 0x05b4c86b, 0x476b6a58, 0xf2c47f53, 0x7f36b535, 0x2095f504,
        0xce9375db, 0xa93fd90a, 0x2247be32, 0xf8665685, 0x85af2469, 0x702ad2c0,
        0x6227a1b2, 0xc0ea4434, 0x5d073e84, 0x721ac1fe, 0xb33920d5, 0x0caceb1a,
        0xcc65f7ae, 0x6585cb69, 0x485c669b, 0x331e4844, 0x7f1e7b85, 0xcca864e1,
        0x916726fc, 0xb6a8eb66, 0x12071e7f, 0xcdd1112c, 0x6d888326, 0x244dea36,
        0xb75912a7, 0x4f4619c8, 0x3316e391, 0x315d8fba, 0x8051c198, 0x4b153cc0,
        0xe2c398aa, 0x967877a3, 0x2d6f06f6, 0x3e509d83, 0x4a768bde, 0x5e192f86,
        0xd9f5e6e5, 0x1f1c5ae0, 0xf04d594d, 0x2a7276ac, 0xed3d7f33, 0xa545b508,
        0x2867ea8e, 0x57d7917b, 0xa4b869cc, 0x5eab0ebf, 0x3d6c7662, 0x7f07568c,
        0xa00a77f1, 0x710771b9, 0x8f409b3e, 0x597e1deb, 0x53a9d405, 0x3ea6b9df,
        0x8f16865a, 0xc10f7bc2, 0x3fac19e5, 0x9e2d2b84, 0x26efa3af, 0xcfddb021,
        0x68f814c6, 0xa1aa8a82, 0xd34ef275, 0x10bfdd99, 0x07178d06, 0x31fd6c7e,
        0x7e171cc2, 0x7823e638, 0x21d82257, 0x23af0ac2, 0xe2e390f8, 0xc2e059a0,
        0x826eb7ce, 0x6f11b82d, 0xadf9d69a, 0xa08eae30, 0x0421e067, 0xd496a606,
        0x54305394, 0x8ded3ab5, 0xd1fe625d, 0xe5c52945, 0xa56121ce, 0x87abe060,
        0x6a1bdfed, 0xda2ab5b5, 0x7af7c393, 0x9b762b2f, 0x84a1a0a7, 0x7aeefd97,
        0xe85d94e4, 0xf9b72a6c, 0xfbfc5360, 0xa6f8a690, 0x3e237738, 0xb1c41c0d,
        0x19cadcaa, 0x7efd77ea, 0x67090d7a, 0x2798f5b0, 0x2677eb4f, 0x2eab1608,
        0xeb924389, 0x2011db79, 0x34d48275, 0x5df24f6b, 0xee83f2ce, 0xd6b54138,
        0xe03cc53d, 0xfaac94d1, 0x65ec8fd7, 0x56f60ee1, 0x72da7094, 0x86f4adc9,
        0x2ce1ac5e, 0xca41e7a8, 0x8d2cd380, 0x1a24e08b, 0xbbb5c682, 0x8fec0e3b,
        0xe2ddaf46, 0x171b77ab, 0x432c14ed, 0x26a96fb1, 0x1b88f936, 0xcc3d8ad7,
        0xbbfcb9f3, 0xecd65b17, 0xb82713e7, 0x831b843e, 0x255dcbcd, 0xa2008307,
        0x8f85d1f7, 0x205f1fe4, 0x6f778873, 0xb5167020, 0xf105d2be, 0x43cdbe97,
        0xa67e1b17, 0xe720bba7, 0x132fd660, 0x79f45397, 0x0363aa9d, 0xd5aad575,
        0xc7e6e2cc, 0x90122be7, 0xec9777a9, 0x37ce1f98, 0xa0212621, 0x29cb5502,
        0x82c176bf, 0x4c434610, 0xa3e0b0bb, 0x88a8b46a, 0x3d38e218, 0x7a3e9db8,
        0xcbdd42d8, 0x9febca33, 0x4264f68d, 0x80cb162e, 0xe58c78ae, 0x0e47a970,
        0x9eff0344, 0xdc9d43d6, 0xa56a5f9f, 0x2b663911, 0x8bcaf2a4, 0xc8f76f43,
        0x417de0e9, 0x406ca3cc, 0x7dc29dfa, 0x8afc33ca, 0x62bc0df1, 0x6a43ad83,
        0x043a655e, 0xcad53ff5, 0xa8aa28bc, 0x8d46c6d6, 0x6137fb1e, 0xfa260c44,
        0xba07c4ab, 0x7471fabe, 0x9de6bddb, 0x62067584, 0x6f53dd3b, 0x7476cb21,
        0x35c34eef, 0xa53d5e6e, 0x00021788, 0x11694b3c, 0xa2c25ac7, 0xbda9e9d0,
        0xdfc50866, 0x75a27f2e, 0x2fbc602d, 0xbe230ba4, 0x05a902f0, 0x463dfd52,
        0xeb4f63b3, 0xedba5558, 0xf150be0c, 0x62449e98, 0x45543746, 0x5b484678,
        0xb81113fd, 0xed4c7e1b, 0x3017ba5a, 0x6afbceb9, 0x74fbe015, 0x13bd973f,
        0xb4dc7a48, 0xfa24d6f2, 0xc4325f4b, 0xe16d24d7, 0x12d4dd07, 0x36bb07df,
        0x05c67851, 0x0d587b9d, 0xbdf70ec3, 0xc4822b96, 0x7fb362ca, 0xae1874d6,
        0xf8d18512, 0x01bc9da1, 0x73ab71f2, 0x3dab3286, 0x02b01000, 0x7b4c32c4,
        0xda108034, 0x2de9f53a, 0x31155a51, 0xa36a75da, 0xceaf8f92, 0xc2f64afe,
        0xd8dd4372, 0x495f66e4, 0x1f843fff, 0x18d0b22e, 0x411d8e8e, 0x8f8354fb,
        0x76414087, 0xdadc3c3c, 0x7f35a9dd, 0x96865c82, 0x656c332e, 0x5791fcf9,
        0x235a2c3d, 0x8184b11d, 0x8488bb1a, 0x4ae1bed9, 0x5fec393f, 0x3f7568cb,
        0xd29a79c0, 0xc42fe130, 0x6089019a, 0xaefbeea4, 0x7d71aef5, 0x6e2a335e,
        0x64254330, 0x3a900444, 0x7df756f2, 0x33b673f0, 0x558bc1ae, 0x8deab748,
        0xc0974ed2, 0xa69ab974, 0xf872bd8c, 0x01664559, 0x8eff6676, 0x281f8925,
        0x86c26982, 0x7be423ff, 0x21fa895a, 0x54f57dfa, 0x51937d13, 0xbcd53c9a,
        0x361709c2, 0x040ddb01, 0x8708ed2a, 0x929d06ab, 0x44dc94c7, 0xf0f70cc9,
        0xd6a0a874, 0x671bbf95, 0x9ac7830b, 0x2a97e0eb, 0xf1937e0a, 0x6d17f2c1,
        0xf42930dc, 0xf188a271, 0xa6619ddc, 0xb8ff6c0d, 0x86bd8e88, 0xff4d70ac,
        0xb78978f9, 0xc03bd567, 0x7db661e5, 0x4c31e2b4, 0xea66fa7c, 0xbafaacf5,
        0x162c00e4, 0x1cf7481b, 0xf8220c69, 0xad17e659, 0x4ed65abf, 0x3f9aa8bb,
        0xdc0ff2ec, 0x371d8b6b, 0x8d7c2674, 0x91fc0080, 0xde33782c, 0x971a0477,
        0x6c9ba28e, 0x61523be7, 0x2d2e3909, 0x71863690, 0xaa858637, 0x9f9b17d0,
        0x48868df5, 0x577eb5c5, 0xe94f93fb, 0xe822ae8d, 0x56d6d760, 0x099919bc,
        0xac3ee429, 0xb89821c2, 0x2da0f70b, 0x9fb8be86, 0x9783209a, 0x870e5b73,
        0x5469c045, 0xacc71055, 0x888f5a95, 0x71d7303f, 0x144b2cd3, 0x7f0016fd,
        0xc9c18017, 0xb192ec62, 0x30ceb8b6, 0x28f242d6, 0xc42308fa, 0xc96b7cf2,
        0x0b55c4d7, 0x6acbe9d8, 0x4ec1e80f, 0xe42dcf0e, 0x9e491c08, 0x91ebec23,
        0xf2416e2d, 0x411a7731, 0xda95e76f, 0x0b232f5e, 0x7785c929, 0x2769344f,
        0xb6cf246b, 0x9be84834, 0xd80340b0, 0xffa88fe0, 0x6a964707, 0x69f279fa,
        0x341454f5, 0x95d3265d, 0x90d0a761, 0xe748e9f3, 0xe358a332, 0xeb670153,
        0xcdf82f75, 0xc8695a00, 0x3ad770d5, 0x7b704114, 0xa7491a3f, 0xd88744cf,
        0x64e06db3, 0x447b9942, 0x37427889, 0x920a4c31, 0xcfbd6a71, 0xfc692e9c,
        0x4711eeec, 0x2ff98627, 0xa42d0b12, 0x1967cbde, 0xa11082b0, 0xd06feb8f,
        0x4aa2c31e, 0x83db443b, 0x24629ebc, 0x179d48fb, 0xd08b2237, 0xf684da26,
        0x1f8da9b5, 0x15847cc3, 0xbbd4578c, 0xba884f85, 0x2058572c, 0xda30f627,
        0x7a9fe391, 0x83ee762e, 0xdfb40093, 0xee321a25, 0x6c7a1105, 0x357c1105,
        0x99757810, 0x283d30b6, 0x29c96b15, 0x1f71456f, 0x9addae55, 0xcec63639,
        0xf408ad23, 0xeab52d99, 0x0fa1cb05, 0x412fa52c, 0x055f6653, 0x34532b40,
        0x3a6118e0, 0xfaff95d7, 0x5c57687a, 0x0fc89224, 0x73b00be4, 0x3a599061,
        0x31cce580, 0xbdb7cdc4, 0x02f8bae4, 0x7dd35319, 0xe18bc281, 0xcbab5bfa,
        0x0d25b6ba, 0x590667ba, 0xf3b16104, 0xe3de5a01, 0xedc00ec7, 0x8c29ee29,
        0x49f6d92a, 0x9f2a2192, 0x98fa5b36, 0x7c1b2b50, 0xe63e5ca7, 0x9e51519c,
        0x4e10d1d5, 0xc03b3ee8, 0x52604837, 0x5fdd9dcc, 0xe70ff1ab, 0x4d055e59,
        0x29c193ea, 0x17936090, 0x368f0359, 0xfe19803d, 0x3a3f6de9, 0x45fbc755,
        0xd5b84bd1, 0x59233af7, 0xca16e149, 0x0fa1cde8, 0x90cfe5f3, 0xe5b3107f,
        0x40445df5, 0x7b7acd5c, 0x04c22f5b, 0x9466a6fd, 0xacaaaf42, 0xcf38a457,
        0xbdda4982, 0x9d412f46, 0x180299fd, 0x1f5e8078, 0x952513dc, 0xc2ad9649,
        0x3ccac4f4, 0xcafb9b54, 0xd29fdd0b, 0x67ac9cb2, 0x30eac13e, 0x146acfd5,
        0xcd9a011c, 0x4b67fb03, 0xb2e4be7b, 0x34512011, 0xd5ac721e, 0x49b48763,
        0x3b108ce9, 0x805fb60d, 0x68cbb337, 0xbd061b3a, 0xbd3c6f11, 0xb025375c,
        0xfa230d0e, 0x4f4112d1, 0x5196547d, 0x0854172d, 0x1f3f4f07, 0x0c861a06,
        0x0b93b79d, 0x3ed70910, 0x59bebba5, 0x4f2ed163, 0x8453fb7e, 0x5a2ee73b,
        0xbf2212fe, 0x921abd3d, 0xf454e196, 0x0ef6e501, 0x0e39ca03, 0x7e426319,
        0x349251b9, 0x0a878996, 0xe937a065, 0x9e30202d, 0x80b528fe, 0x85937424,
        0x85998f4c, 0x74a83aca, 0x962ca4fd, 0x8871d671, 0x050ae8f5, 0x54c8e24c,
        0xefa44429, 0xaffba09c, 0x6dc3a6c1, 0x27918e9f, 0x4d806f33, 0x764b38c1,
        0x5ed70ed7, 0xb0f49e1b, 0x596960fb, 0xe212d5b6, 0x22675c73, 0x7abbb0cb,
        0xe618baaf, 0x362d6231, 0x21bc1c3d, 0x78fa5ef3, 0x1e06b7c6, 0x7c983641,
        0xb82db327, 0xc20a89e5, 0x7c2d8f65, 0xd30e1ce3, 0xdcf4fb38, 0x381f0b48,
        0x9d9442cf, 0xf23656fa, 0xa62134b8, 0xb15e6329, 0x1296f954, 0x8f62f639,
        0x1044c356, 0x83880a59, 0xabd38ee7, 0x4296965a, 0xae54474e, 0xe25eaba7,
        0x2763b5be, 0x9a6100a9, 0xbae57bc5, 0xa6de971e, 0xd7604088, 0x491292b5,
        0xcbf0531f, 0xf040840c, 0xf3555395, 0xce2202f0, 0xf6a53a36, 0x1111b9c3,
        0xd1ef03dd, 0x098188ba, 0xf11962af, 0xa015fb6c, 0x18b67008, 0xa0d8c393,
        0x14a86b22, 0x2cadb09d, 0xb5be9165, 0x523af604, 0xe7cda179, 0x9fea3c7e,
        0xa22b21a6, 0x427a5d2d, 0x9d4943c8, 0x07315439, 0x027a4e7f, 0x152d33cc,
        0x57408a08, 0xb7f19ce2, 0xc69bc124, 0xbee226df, 0xaf990f50, 0x6f4841bd,
        0xf04b50ef, 0x072e5d85, 0x7ad2ac3a, 0x82718697, 0xf2cc6b6c, 0xb3d358cb,
        0xb0550382, 0xb3e52426, 0x1947566a, 0xb13aaae8, 0xc9b1de0d, 0x46f7fe87,
        0xbaf525df, 0xba48a885, 0x5ba3ca06, 0xb5aace28, 0x37d6a50e, 0x9afb1802,
        0x04f6119a, 0x53b83aed, 0x780bc444, 0xb96f2582, 0x17ecead2, 0x9727e310,
        0x803c8afa, 0x084a48fe, 0x5c033b98, 0x8d91956a, 0x1efdcb75, 0x716c7d18,
        0x649036ae, 0xcb71bcbb, 0xbc563e58, 0xb1f8029a, 0x3ee197eb, 0xb139276c,
        0x7e368ea8, 0x1a1f1449, 0x75e74136, 0xf5aed3db, 0xfc0101d4, 0x8902f0d1,
        0x39e23468, 0x60d40750, 0x3bf9fd43, 0x47c3bbcb, 0xc415ad24, 0xf9d249f3,
        0x883abf06, 0xde940346, 0xd5a75fc1, 0x89fa8dd7, 0xf9f8b222, 0x071bc6c3,
        0x332d5d76, 0xb0fe6a84, 0x8658de3a, 0x0ebc58e1, 0x80ff97f5, 0x6220280d,
        0x3d17185d, 0x8aa9b59a, 0xade580c4, 0x767ca517, 0x17fe5094, 0xb4c68c17,
        0x43be73ee, 0x90067821, 0xf1e47ec9, 0x147c4b89, 0x2eda66ec, 0xe12408b8,
        0x769f1465, 0x1fac1d60, 0xe0be0fae, 0x6cfd0f21, 0xf72ccd6e, 0xd093705c,
        0xf5c053bb, 0xfa0fb38b, 0x93c3143a, 0x6d42eb5c, 0xebc98525, 0xf99b54df,
        0x0827c6c1, 0xc1c22002, 0x09bd13c1, 0x837cb865, 0xdeb11457, 0x6f8b78f8,
        0xcc89d9df, 0x5d8e3d8e, 0x9f8a49ba, 0x385df157, 0x5b78d372, 0xc1fe5244,
        0x26d2cd02, 0x5d5c7c61, 0xd56c698d, 0x9d4beecf, 0x1da8a984, 0x52152bc9,
        0xed83d0f2, 0x8f635d07, 0x02970c5c, 0x9499ab61, 0x169ccbf5, 0x2ee10ba8,
        0x226f7f42, 0xf7e3e5e1, 0xa430f672, 0x40850f33, 0x89df04a5, 0x33a4a435,
        0x35ffa868, 0xb61f7627, 0xe4ad7393, 0xc65cf40f, 0xbbf638cc, 0x4e678479,
        0xa3be94b8, 0x78d34cf5, 0xc18cb5b5, 0xef79cb82, 0x2cc21d38, 0xd96cc6a5,
        0x76f77852, 0xe78b2ba9, 0x456f5f1e, 0x1f4e9ea0, 0x9155112d, 0x944c5de4,
        0x8e7c5088, 0xc0d69c04, 0xd98d5a6a, 0x39491a5b, 0x883ad445, 0x688b87cc,
        0x69da4122, 0x284ae1ff, 0xbc0f1af4, 0x5de75d35, 0x12652b4f, 0x1e5bbb22,
        0x0d46950c, 0x664c3b93, 0x8701e7dd, 0x3656fed1, 0x63f0a050, 0x5931b0c2,
        0x61333612, 0xdbd4fd78,
    }};

static uint32_t kSpxEnabled = 0;

static void enable_spx_verify(void) { kSpxEnabled = 0; }

static void disable_spx_verify(void) { kSpxEnabled = kSigverifySpxDisabledOtp; }

uint32_t sigverify_spx_verify_enabled(lifecycle_state_t lc_state) {
  if (kSpxEnabled != kSigverifySpxDisabledOtp) {
    LOG_INFO("spx_verify is enabled");
  } else {
    LOG_INFO("spx_verify is disabled");
  }
  return kSpxEnabled;
}

static rom_error_t spx_success_to_ok_test_impl(uint32_t v, bool eq) {
  uint32_t res = sigverify_spx_success_to_ok(v);
  LOG_INFO("0x%08x -> 0x%08x", v, res);
  if ((res == kErrorOk) ^ eq) {
    LOG_ERROR("Wrong result 0x%08x for 0x%08x", res, v);
    return kErrorUnknown;
  }
  return kErrorOk;
}

static rom_error_t spx_success_to_ok_test(void) {
  rom_error_t error = kErrorOk;
  bool failed = false;
  failed |=
      spx_success_to_ok_test_impl(kSigverifySpxDisabledOtp, true) != kErrorOk;
  failed |= spx_success_to_ok_test_impl(kSigverifySpxSuccess, true) != kErrorOk;
  failed |= spx_success_to_ok_test_impl(UINT32_MAX, false) != kErrorOk;
  failed |= spx_success_to_ok_test_impl(0, false) != kErrorOk;
  failed |= spx_success_to_ok_test_impl(kErrorOk, false) != kErrorOk;
  if (failed) {
    return kErrorUnknown;
  }
  return kErrorOk;
}

static rom_error_t spx_verify_impl_test(void) {
  sigverify_spx_root_t expected_root;
  sigverify_spx_root_t actual_root;
  spx_public_key_root((const uint8_t *)&kPubKey, expected_root.data);
  rom_error_t error = spx_verify((const uint8_t *)&kSignature, NULL, 0, NULL, 0,
                                 (const uint8_t *)kMessage, kMessageLen,
                                 (const uint8_t *)&kPubKey, actual_root.data);
  if (memcmp(&expected_root, &actual_root, sizeof(sigverify_spx_root_t)) != 0) {
    return kErrorUnknown;
  }
  return error;
}

static rom_error_t spx_verify_disabled_bad_signature_test(void) {
  disable_spx_verify();

  uint32_t flash_exec = 0;
  uint32_t good_word = kSignature.data[0];
  kSignature.data[0] = 0;
  rom_error_t error =
      sigverify_spx_verify(&kSignature, &kPubKey, kLcStateProd, NULL, 0, NULL,
                           0, &kMessage, kMessageLen, &flash_exec);
  kSignature.data[0] = good_word;

  if (flash_exec != kSigverifySpxSuccess) {
    LOG_ERROR("flash_exec must be 0x%08x, not 0x%08x", kSigverifySpxSuccess,
              flash_exec);
    return kErrorUnknown;
  }
  return error;
}

static rom_error_t spx_verify_disabled_good_signature_test(void) {
  disable_spx_verify();

  uint32_t flash_exec = 0;
  rom_error_t error =
      sigverify_spx_verify(&kSignature, &kPubKey, kLcStateProd, NULL, 0, NULL,
                           0, &kMessage, kMessageLen, &flash_exec);

  if (flash_exec != kSigverifySpxSuccess) {
    LOG_ERROR("flash_exec must be 0x%08x, not 0x%08x", kSigverifySpxSuccess,
              flash_exec);
    return kErrorUnknown;
  }
  return error;
}

static rom_error_t spx_verify_enabled_bad_signature_test(void) {
  enable_spx_verify();

  uint32_t flash_exec = 0;
  uint32_t good_word = kSignature.data[0];
  kSignature.data[0] = 0;
  rom_error_t error =
      sigverify_spx_verify(&kSignature, &kPubKey, kLcStateProd, NULL, 0, NULL,
                           0, &kMessage, kMessageLen, &flash_exec);
  kSignature.data[0] = good_word;

  if (error == kErrorOk) {
    LOG_ERROR("error should not be kErrorOk");
    return kErrorUnknown;
  }
  if (flash_exec != UINT32_MAX) {
    LOG_ERROR("flash_exec must be UINT32_MAX");
    return kErrorUnknown;
  }

  return kErrorOk;
}

static rom_error_t spx_verify_enabled_good_signature_test(void) {
  enable_spx_verify();

  uint32_t flash_exec = 0;
  rom_error_t error =
      sigverify_spx_verify(&kSignature, &kPubKey, kLcStateProd, NULL, 0, NULL,
                           0, &kMessage, kMessageLen, &flash_exec);

  if (flash_exec != kSigverifySpxSuccess) {
    LOG_ERROR("flash_exec must be 0x%08x, not 0x%08x", kSigverifySpxSuccess,
              flash_exec);
    return kErrorUnknown;
  }
  return error;
}

OTTF_DEFINE_TEST_CONFIG();

bool test_main(void) {
  status_t error = OK_STATUS();

  EXECUTE_TEST(error, spx_success_to_ok_test);
  EXECUTE_TEST(error, spx_verify_impl_test);
  EXECUTE_TEST(error, spx_verify_disabled_bad_signature_test);
  EXECUTE_TEST(error, spx_verify_disabled_good_signature_test);
  EXECUTE_TEST(error, spx_verify_enabled_bad_signature_test);
  EXECUTE_TEST(error, spx_verify_enabled_good_signature_test);

  return status_ok(error);
}
