// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>
#include <shared_test_classes/base/benchmark.hpp>
#include "single_layer_tests/generate_proposals.hpp"
#include "common_test_utils/ov_tensor_utils.hpp"

using namespace ov::test;
using namespace ov::test::subgraph;

namespace {

const std::vector<float> min_size = { 1 };
const std::vector<float> nms_threshold = { 0.699999988079071 };
const std::vector<int64_t> post_nms_count = { 6 };
const std::vector<int64_t> pre_nms_count = { 1000 };

const std::vector<std::pair<std::string, std::vector<ov::Tensor>>> inputTensors = {
    {
        "empty",
        {
            // 3
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3}, {1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f}),
            // 2 x 6 x 3 x 4 = 144
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 6, 3, 4}, std::vector<float>(144, 1.0f)),
            // 2 x 12 x 2 x 6 = 144 * 2
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 12, 2, 6}, std::vector<float>(288, 1.0f)),
            // {2 x 3 x 2 x 6} = 36 * 2
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3, 2, 6}, {
                5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 4.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f,
                5.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 4.0f, 1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 8.0f, 1.0f})
        }
    },
    {
        "filled",
        {
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3}, {150.0, 150.0, 0.0, 150.0, 150.0, 0.0}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 6, 3, 4}, {
                12.0, 68.0, 102.0, 123.0, 46.0, 80.0,  79.0,  128.0, 33.0, 71.0, 127.0, 86.0,  33.0, 56.0, 150.0, 73.0,
                5.0,  41.0, 93.0,  150.0, 74.0, 66.0,  106.0, 115.0, 17.0, 37.0, 87.0,  150.0, 31.0, 27.0, 150.0, 39.0,
                29.0, 23.0, 112.0, 123.0, 41.0, 37.0,  103.0, 150.0, 8.0,  46.0, 98.0,  111.0, 7.0,  69.0, 114.0, 150.0,
                70.0, 21.0, 150.0, 125.0, 54.0, 19.0,  132.0, 68.0,  62.0, 8.0,  150.0, 101.0, 57.0, 81.0, 150.0, 97.0,
                79.0, 29.0, 109.0, 130.0, 12.0, 63.0,  100.0, 150.0, 17.0, 33.0, 113.0, 150.0, 90.0, 78.0, 150.0, 111.0,
                47.0, 68.0, 150.0, 71.0,  66.0, 103.0, 111.0, 150.0, 4.0,  17.0, 112.0, 94.0,  12.0, 8.0,  119.0, 98.0,
                54.0, 56.0, 120.0, 150.0, 56.0, 29.0,  150.0, 31.0,  42.0, 3.0,  139.0, 92.0,  41.0, 65.0, 150.0, 130.0,
                49.0, 13.0, 143.0, 30.0,  40.0, 60.0,  150.0, 150.0, 23.0, 73.0, 24.0,  115.0, 56.0, 84.0, 107.0, 108.0,
                63.0, 8.0,  142.0, 125.0, 78.0, 37.0,  93.0,  144.0, 40.0, 34.0, 150.0, 46.0,  30.0, 21.0, 150.0, 120.0}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 12, 2, 6}, {
                9.062256,   10.883133, 9.8441105,   12.694285,   0.41781136, 8.749107,      14.990341,  6.587644,  1.4206103,
                13.299262,  12.432549, 2.736371,    0.22732796,  6.3361835,  12.268727,     2.1009045,  4.771589,  2.5131326,
                5.610736,   9.3604145, 4.27379,     8.317948,    0.60510135, 6.7446275,     1.0207708,  1.1352817, 1.5785321,
                1.718335,   1.8093798, 0.99247587,  1.3233583,   1.7432803,  1.8534478,     1.2593061,  1.7394226, 1.7686696,
                1.647999,   1.7611449, 1.3119122,   0.03007332,  1.1106564,  0.55669737,    0.2546148,  1.9181818, 0.7134989,
                2.0407224,  1.7211134, 1.8565536,   14.562747,   2.8786168,  0.5927796,     0.2064463,  7.6794515, 8.672126,
                10.139171,  8.002429,  7.002932,    12.6314945,  10.550842,  0.15784842,    0.3194304,  10.752157, 3.709805,
                11.628928,  0.7136225, 14.619964,   15.177284,   2.2824087,  15.381494,     0.16618137, 7.507227,  11.173228,
                0.4923559,  1.8227729, 1.4749299,   1.7833921,   1.2363617,  -0.23659119,   1.5737582,  1.779316,  1.9828427,
                1.0482665,  1.4900246, 1.3563544,   1.5341306,   0.7634312,  4.6216766e-05, 1.6161222,  1.7512476, 1.9363779,
                0.9195784,  1.4906164, -0.03244795, 0.681073,    0.6192401,  1.8033613,     14.146055,  3.4043705, 15.292292,
                3.5295358,  11.138999, 9.952057,    5.633434,    12.114562,  9.427372,      12.384038,  9.583308,  8.427233,
                15.293704,  3.288159,  11.64898,    9.350885,    2.0037227,  13.523184,     4.4176426,  6.1057625, 14.400079,
                8.248259,   11.815807, 15.713364,   1.0023532,   1.3203261,  1.7100681,     0.7407832,  1.09448,   1.7188418,
                1.4412547,  1.4862992, 0.74790007,  0.31571656,  0.6398838,  2.0236106,     1.1869069,  1.7265586, 1.2624544,
                0.09934269, 1.3508598, 0.85212964,  -0.38968498, 1.7059708,  1.6533034,     1.7400402,  1.8123854, -0.43063712,
                9.062256,   10.883133, 9.8441105,   12.694285,   0.41781136, 8.749107,      14.990341,  6.587644,  1.4206103,
                13.299262,  12.432549, 2.736371,    0.22732796,  6.3361835,  12.268727,     2.1009045,  4.771589,  2.5131326,
                5.610736,   9.3604145, 4.27379,     8.317948,    0.60510135, 6.7446275,     1.0207708,  1.1352817, 1.5785321,
                1.718335,   1.8093798, 0.99247587,  1.3233583,   1.7432803,  1.8534478,     1.2593061,  1.7394226, 1.7686696,
                1.647999,   1.7611449, 1.3119122,   0.03007332,  1.1106564,  0.55669737,    0.2546148,  1.9181818, 0.7134989,
                2.0407224,  1.7211134, 1.8565536,   14.562747,   2.8786168,  0.5927796,     0.2064463,  7.6794515, 8.672126,
                10.139171,  8.002429,  7.002932,    12.6314945,  10.550842,  0.15784842,    0.3194304,  10.752157, 3.709805,
                11.628928,  0.7136225, 14.619964,   15.177284,   2.2824087,  15.381494,     0.16618137, 7.507227,  11.173228,
                0.4923559,  1.8227729, 1.4749299,   1.7833921,   1.2363617,  -0.23659119,   1.5737582,  1.779316,  1.9828427,
                1.0482665,  1.4900246, 1.3563544,   1.5341306,   0.7634312,  4.6216766e-05, 1.6161222,  1.7512476, 1.9363779,
                0.9195784,  1.4906164, -0.03244795, 0.681073,    0.6192401,  1.8033613,     14.146055,  3.4043705, 15.292292,
                3.5295358,  11.138999, 9.952057,    5.633434,    12.114562,  9.427372,      12.384038,  9.583308,  8.427233,
                15.293704,  3.288159,  11.64898,    9.350885,    2.0037227,  13.523184,     4.4176426,  6.1057625, 14.400079,
                8.248259,   11.815807, 15.713364,   1.0023532,   1.3203261,  1.7100681,     0.7407832,  1.09448,   1.7188418,
                1.4412547,  1.4862992, 0.74790007,  0.31571656,  0.6398838,  2.0236106,     1.1869069,  1.7265586, 1.2624544,
                0.09934269, 1.3508598, 0.85212964,  -0.38968498, 1.7059708,  1.6533034,     1.7400402,  1.8123854, -0.43063712}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3, 2, 6}, {
                0.7719922,  0.35906568, 0.29054508, 0.18124384, 0.5604661,  0.84750974, 0.98948747, 0.009793862, 0.7184191,
                0.5560748,  0.6952493,  0.6732593,  0.3306898,  0.6790913,  0.41128764, 0.34593266, 0.94296855,  0.7348507,
                0.24478768, 0.94024557, 0.05405676, 0.06466125, 0.36244348, 0.07942984, 0.10619422, 0.09412837,  0.9053611,
                0.22870538, 0.9237487,  0.20986171, 0.5067282,  0.29709867, 0.53138554, 0.189101,   0.4786443,   0.88421875,
                0.7719922,  0.35906568, 0.29054508, 0.18124384, 0.5604661,  0.84750974, 0.98948747, 0.009793862, 0.7184191,
                0.5560748,  0.6952493,  0.6732593,  0.3306898,  0.6790913,  0.41128764, 0.34593266, 0.94296855,  0.7348507,
                0.24478768, 0.94024557, 0.05405676, 0.06466125, 0.36244348, 0.07942984, 0.10619422, 0.09412837,  0.9053611,
                0.22870538, 0.9237487,  0.20986171, 0.5067282,  0.29709867, 0.53138554, 0.189101,   0.4786443,   0.88421875}),
        }
    },
    {
        "filled2",
        {
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3}, {200.0, 200.0, 4.0, 200.0, 200.0, 4.0}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 6, 3, 4}, {0.0f,  1.0f,  2.0f,  3.0f,
                                                                                            4.0f,  5.0f,  6.0f,  7.0f,
                                                                                            8.0f,  9.0f, 10.0f, 11.0f,
                                                                                            12.0f, 13.0f, 14.0f, 15.0f,
                                                                                            16.0f, 17.0f, 18.0f, 19.0f,
                                                                                            20.0f, 21.0f, 22.0f, 23.0f,
                                                                                            24.0f, 25.0f, 26.0f, 27.0f,
                                                                                            28.0f, 29.0f, 30.0f, 31.0f,
                                                                                            32.0f, 33.0f, 34.0f, 35.0f,
                                                                                            36.0f, 37.0f, 38.0f, 39.0f,
                                                                                            40.0f, 41.0f, 42.0f, 43.0f,
                                                                                            44.0f, 45.0f, 46.0f, 47.0f,
                                                                                            48.0f, 49.0f, 50.0f, 51.0f,
                                                                                            52.0f, 53.0f, 54.0f, 55.0f,
                                                                                            56.0f, 57.0f, 58.0f, 59.0f,
                                                                                            60.0f, 61.0f, 62.0f, 63.0f,
                                                                                            64.0f, 65.0f, 66.0f, 67.0f,
                                                                                            68.0f, 69.0f, 70.0f, 71.0f,
                                                                                            72.0f, 73.0f, 74.0f, 75.0f,
                                                                                            76.0f, 77.0f, 78.0f, 79.0f,
                                                                                            80.0f, 81.0f, 82.0f, 83.0f,
                                                                                            84.0f, 85.0f, 86.0f, 87.0f,
                                                                                            88.0f, 89.0f, 90.0f, 91.0f,
                                                                                            92.0f, 93.0f, 94.0f, 95.0f,
                                                                                            96.0f,  97.0f,  98.0f,  99.0f,
                                                                                            100.0f, 101.0f, 102.0f, 103.0f,
                                                                                            104.0f, 105.0f, 106.0f, 107.0f,
                                                                                            108.0f, 109.0f, 110.0f, 111.0f,
                                                                                            112.0f, 113.0f, 114.0f, 115.0f,
                                                                                            116.0f, 117.0f, 118.0f, 119.0f,
                                                                                            120.0f, 121.0f, 122.0f, 123.0f,
                                                                                            124.0f, 125.0f, 126.0f, 127.0f,
                                                                                            128.0f, 129.0f, 130.0f, 131.0f,
                                                                                            132.0f, 133.0f, 134.0f, 135.0f,
                                                                                            136.0f, 137.0f, 138.0f, 139.0f,
                                                                                            140.0f, 141.0f, 142.0f, 143.0f}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 12, 2, 6}, {
                    0.5337073,  0.86607957, 0.55151343, 0.21626699, 0.4462629,  0.03985678,
                    0.5157072,  0.9932138,  0.7565954,  0.43803605, 0.802818,   0.14834064,
                    0.53932905, 0.14314,    0.3817048,  0.95075196, 0.05516243, 0.2567484,
                    0.25508744, 0.77438325, 0.43561,    0.2094628,  0.8299043,  0.44982538,
                    0.95615596, 0.5651084,  0.11801951, 0.05352486, 0.9774733,  0.14439464,
                    0.62644225, 0.14370479, 0.54161614, 0.557915,   0.53102225, 0.0840179,
                    0.7249888,  0.9843559,  0.5490522,  0.53788143, 0.822474,   0.3278008,
                    0.39688024, 0.3286012,  0.5117038,  0.04743988, 0.9408995,  0.29885054,
                    0.81039643, 0.85277915, 0.06807619, 0.86430097, 0.36225632, 0.16606331,
                    0.5401001,  0.7541649,  0.11998601, 0.5131829,  0.40606487, 0.327888,
                    0.27721855, 0.6378373,  0.22795396, 0.4961256,  0.3215895,  0.15607187,
                    0.14782153, 0.8908137,  0.8835288,  0.834191,   0.29907143, 0.7983525,
                    0.755875,   0.30837986, 0.0839176,  0.26624718, 0.04371626, 0.09472824,
                    0.20689541, 0.37622106, 0.1083321,  0.1342548,  0.05815459, 0.7676379,
                    0.8105144,  0.92348766, 0.26761323, 0.7183306,  0.8947588,  0.19020908,
                    0.42731014, 0.7473663,  0.85775334, 0.9340091,  0.3278848,  0.755993,
                    0.05307213, 0.39705503, 0.21003333, 0.5625373,  0.66188884, 0.80521655,
                    0.6125863,  0.44678232, 0.97802377, 0.0204936,  0.02686367, 0.7390654,
                    0.74631,    0.58399844, 0.5988792,  0.37413648, 0.5946692,  0.6955776,
                    0.36377597, 0.7891322,  0.40900692, 0.99139464, 0.50169915, 0.41435778,
                    0.17142445, 0.26761186, 0.31591868, 0.14249913, 0.12919712, 0.5418711,
                    0.6523203,  0.50259084, 0.7379765,  0.01171071, 0.94423133, 0.00841132,
                    0.97486794, 0.2921785,  0.7633071,  0.88477814, 0.03563205, 0.50833166,
                    0.01354555, 0.535081,   0.41366324, 0.0694767,  0.9944055,  0.9981207,
                    0.5337073,  0.86607957, 0.55151343, 0.21626699, 0.4462629,  0.03985678,
                    0.5157072,  0.9932138,  0.7565954,  0.43803605, 0.802818,   0.14834064,
                    0.53932905, 0.14314,    0.3817048,  0.95075196, 0.05516243, 0.2567484,
                    0.25508744, 0.77438325, 0.43561,    0.2094628,  0.8299043,  0.44982538,
                    0.95615596, 0.5651084,  0.11801951, 0.05352486, 0.9774733,  0.14439464,
                    0.62644225, 0.14370479, 0.54161614, 0.557915,   0.53102225, 0.0840179,
                    0.7249888,  0.9843559,  0.5490522,  0.53788143, 0.822474,   0.3278008,
                    0.39688024, 0.3286012,  0.5117038,  0.04743988, 0.9408995,  0.29885054,
                    0.81039643, 0.85277915, 0.06807619, 0.86430097, 0.36225632, 0.16606331,
                    0.5401001,  0.7541649,  0.11998601, 0.5131829,  0.40606487, 0.327888,
                    0.27721855, 0.6378373,  0.22795396, 0.4961256,  0.3215895,  0.15607187,
                    0.14782153, 0.8908137,  0.8835288,  0.834191,   0.29907143, 0.7983525,
                    0.755875,   0.30837986, 0.0839176,  0.26624718, 0.04371626, 0.09472824,
                    0.20689541, 0.37622106, 0.1083321,  0.1342548,  0.05815459, 0.7676379,
                    0.8105144,  0.92348766, 0.26761323, 0.7183306,  0.8947588,  0.19020908,
                    0.42731014, 0.7473663,  0.85775334, 0.9340091,  0.3278848,  0.755993,
                    0.05307213, 0.39705503, 0.21003333, 0.5625373,  0.66188884, 0.80521655,
                    0.6125863,  0.44678232, 0.97802377, 0.0204936,  0.02686367, 0.7390654,
                    0.74631,    0.58399844, 0.5988792,  0.37413648, 0.5946692,  0.6955776,
                    0.36377597, 0.7891322,  0.40900692, 0.99139464, 0.50169915, 0.41435778,
                    0.17142445, 0.26761186, 0.31591868, 0.14249913, 0.12919712, 0.5418711,
                    0.6523203,  0.50259084, 0.7379765,  0.01171071, 0.94423133, 0.00841132,
                    0.97486794, 0.2921785,  0.7633071,  0.88477814, 0.03563205, 0.50833166,
                    0.01354555, 0.535081,   0.41366324, 0.0694767,  0.9944055,  0.9981207}),
            ov::test::utils::create_tensor<float>(ov::element::f32, ov::Shape{2, 3, 2, 6}, {
                    0.56637216, 0.90457034, 0.69827306, 0.4353543,  0.47985056, 0.42658508,
                    0.14516132, 0.08081771, 0.1799732,  0.9229515,  0.42420176, 0.50857586,
                    0.82664067, 0.4972319,  0.3752427,  0.56731623, 0.18241242, 0.33252355,
                    0.30608943, 0.6572437,  0.69185436, 0.88646156, 0.36985755, 0.5590753,
                    0.5256446,  0.03342898, 0.1344396,  0.68642473, 0.37953874, 0.32575172,
                    0.21108444, 0.5661886,  0.45378175, 0.62126315, 0.26799858, 0.37272978,
                    0.56637216, 0.90457034, 0.69827306, 0.4353543,  0.47985056, 0.42658508,
                    0.14516132, 0.08081771, 0.1799732,  0.9229515,  0.42420176, 0.50857586,
                    0.82664067, 0.4972319,  0.3752427,  0.56731623, 0.18241242, 0.33252355,
                    0.30608943, 0.6572437,  0.69185436, 0.88646156, 0.36985755, 0.5590753,
                    0.5256446,  0.03342898, 0.1344396,  0.68642473, 0.37953874, 0.32575172,
                    0.21108444, 0.5661886,  0.45378175, 0.62126315, 0.26799858, 0.37272978}),
        }
    }
};

const std::vector<std::vector<InputShape>> dynamicInputShape = {
    // im_info / anchors / deltas / scores
    static_shapes_to_test_representation({{2, 3}, {2, 6, 3, 4}, {2, 12, 2, 6}, {2, 3, 2, 6}}),
    {
        {{-1, -1}, {{2, 3}}},
        {{-1, -1, -1, -1}, {{2, 6, 3, 4}}},
        {{-1, -1, -1, -1}, {{2, 12, 2, 6}}},
        {{-1, -1, -1, -1}, {{2, 3, 2, 6}}}
    },
    {
        {{{1, 3}, {3, 6}}, {{2, 3}}},
        {{{2, 4}, {6, 12}, {3, 6}, {4, 8}}, {{2, 6, 3, 4}}},
        {{{1, 3}, {12, 24}, {2, 4}, {6, 12}}, {{2, 12, 2, 6}}},
        {{{1, 3}, {3, 6}, {2, 4}, {6, 12}}, {{2, 3, 2, 6}}}
    }
};

INSTANTIATE_TEST_SUITE_P(
    smoke_BasicTest,
    GenerateProposalsLayerTest,
    ::testing::Combine(
        ::testing::ValuesIn(dynamicInputShape),
        ::testing::ValuesIn(min_size),
        ::testing::ValuesIn(nms_threshold),
        ::testing::ValuesIn(post_nms_count),
        ::testing::ValuesIn(pre_nms_count),
        ::testing::ValuesIn({true, false}),
        ::testing::ValuesIn(inputTensors),
        ::testing::Values(ov::element::Type_t::f32),
        ::testing::Values(ov::element::Type_t::i32),
        ::testing::Values(CommonTestUtils::DEVICE_CPU)),
    GenerateProposalsLayerTest::getTestCaseName);

struct GenerateProposalsBenchmarkTest : ov::test::BenchmarkLayerTest<GenerateProposalsLayerTest> {};

TEST_P(GenerateProposalsBenchmarkTest, DISABLED_GenerateProposals_Benchmark) {
    run_benchmark("GenerateProposals", std::chrono::milliseconds(2000), 10000);
}

INSTANTIATE_TEST_SUITE_P(
        BasicTest,
        GenerateProposalsBenchmarkTest,
        ::testing::Combine(
                ::testing::ValuesIn(dynamicInputShape),
                ::testing::ValuesIn(min_size),
                ::testing::ValuesIn(nms_threshold),
                ::testing::ValuesIn(post_nms_count),
                ::testing::ValuesIn(pre_nms_count),
                ::testing::ValuesIn({true, false}),
                ::testing::ValuesIn(inputTensors),
                ::testing::Values(ov::element::Type_t::f32),
                ::testing::Values(ov::element::Type_t::i32),
                ::testing::Values(CommonTestUtils::DEVICE_CPU)),
        GenerateProposalsLayerTest::getTestCaseName);

} // namespace
