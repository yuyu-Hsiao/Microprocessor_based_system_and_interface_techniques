// 固定
//  三角 70 and 圓形 70 正方形 70 amb 50
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC100Series.h"

#define PLL_CLOCK 50000000

/******************************************************************
 * dataset format setting
 ******************************************************************/

#define train_data_num 260 // Total number of training data
// #define test_data_num 60   // Total number of testing data

/******************************************************************
 * Network Configuration - customized per network
 ******************************************************************/
#define input_length 8 // The number of input
#define HiddenNodes 10 // The number of neurons in hidden layer
#define target_num 4   // The number of output

const float LearningRate = 0.01; // Learning Rate
const float Momentum = 0.9;
const float InitialWeightMax = 0.5;
const float goal_acc = 0.95; // Target accuracy

uint32_t c, t, s;

// Create training dataset/output
float train_data_input[train_data_num][input_length] = {

    // triangle
    // {251, 598, 2271, 1855, 2071, 1995, 2037, 905},
    // {249, 600, 2271, 1858, 2071, 1997, 2036, 904},
    // {250, 597, 2273, 1859, 2073, 2000, 2039, 906},
    // {254, 599, 2272, 1856, 2070, 1993, 2035, 899},
    // {252, 597, 2273, 1856, 2071, 1994, 2037, 903},
    // {250, 595, 2271, 1855, 2072, 1995, 2037, 904},
    // {252, 596, 2272, 1855, 2070, 1993, 2035, 902},
    // {250, 598, 2274, 1860, 2074, 1999, 2037, 904},
    // {253, 596, 2272, 1857, 2068, 1994, 2032, 903},
    // {251, 598, 2272, 1858, 2069, 1997, 2035, 905},

    {442, 2450, 2278, 965, 2076, 1996, 2035, 917},
    {1064, 2463, 2276, 864, 2072, 1994, 2034, 919},
    {1701, 2467, 2276, 788, 2075, 1993, 2033, 915},
    {1919, 2472, 2276, 723, 2072, 1995, 2035, 921},
    {2011, 2473, 2275, 673, 2074, 1990, 2033, 913},
    {2057, 2472, 2277, 629, 2075, 1973, 2033, 917},
    {2077, 2476, 2277, 593, 2073, 1858, 2033, 915},
    {2088, 2475, 2274, 561, 2074, 1727, 2034, 915},
    {2097, 2479, 2278, 535, 2074, 1570, 2032, 918},
    {2099, 2477, 2276, 513, 2072, 1173, 2032, 913},
    {2107, 2481, 2278, 496, 2075, 1042, 2034, 916},
    {2104, 2480, 2278, 489, 2074, 929, 2030, 916},
    {2110, 2476, 2277, 492, 2075, 831, 2032, 916},
    {2111, 2477, 2276, 625, 2076, 746, 2033, 917},
    {2114, 2480, 2276, 1074, 2072, 668, 2030, 916},
    {2111, 2478, 2276, 1381, 2076, 608, 2030, 918},
    {2111, 2477, 2272, 1549, 2074, 552, 2030, 915},
    {2115, 2482, 2278, 1663, 2074, 496, 2024, 915},
    {2114, 2481, 2277, 1734, 2075, 453, 1998, 914},
    {2112, 2481, 2275, 1771, 2071, 418, 1912, 915},
    {2120, 2483, 2277, 1796, 2074, 388, 1810, 917},
    {2113, 2481, 2274, 1812, 2070, 367, 1692, 916},
    {2116, 2480, 2276, 1822, 2075, 348, 1568, 917},
    {2116, 2482, 2276, 1832, 2073, 332, 1170, 916},
    {2113, 2480, 2275, 1837, 2073, 337, 1044, 914},
    {2117, 2478, 2276, 1842, 2076, 504, 1177, 917},
    {2117, 2479, 2275, 1843, 2072, 904, 1653, 916},
    {2116, 2482, 2277, 1849, 2074, 1192, 1834, 915},
    {2115, 2477, 2276, 1851, 2075, 1638, 1923, 917},
    {2115, 2475, 2274, 1851, 2074, 1785, 1976, 918},

    {2106, 408, 358, 1866, 2067, 1997, 2037, 914},
    {2106, 394, 419, 1868, 1973, 1999, 2038, 917},
    {2105, 379, 594, 1865, 1798, 1997, 2035, 915},
    {2106, 367, 909, 1863, 1649, 1998, 2039, 919},
    {2107, 360, 1279, 1864, 1250, 1995, 2039, 919},
    {2105, 348, 1606, 1861, 1078, 1997, 2037, 915},
    {2106, 347, 1866, 1863, 898, 1998, 2038, 917},
    {2105, 347, 2035, 1859, 768, 1993, 2037, 917},
    {2103, 350, 2162, 1863, 679, 1998, 2034, 918},
    {2105, 364, 2215, 1865, 613, 1997, 2037, 916},
    {2104, 394, 2240, 1864, 563, 1993, 2035, 915},
    {2107, 444, 2253, 1864, 522, 1998, 2038, 918},
    {2103, 628, 2260, 1866, 489, 1994, 2035, 912},
    {2108, 1072, 2268, 1865, 460, 1994, 2035, 918},
    {2104, 1545, 2271, 1866, 441, 1996, 2032, 919},
    {2103, 1901, 2269, 1865, 435, 1995, 2030, 914},
    {2105, 2179, 2274, 1867, 457, 1969, 1959, 881},
    {2106, 2349, 2274, 1863, 541, 1926, 1816, 785},
    {2104, 2410, 2277, 1866, 761, 1907, 1644, 692},
    {2106, 2438, 2274, 1864, 1096, 1912, 1242, 611},
    {2107, 2453, 2277, 1864, 1649, 1927, 1035, 484},
    {2103, 2464, 2278, 1867, 1854, 1936, 852, 374},
    {2103, 2468, 2274, 1864, 1951, 1941, 732, 315},
    {2103, 2473, 2279, 1867, 2002, 1955, 640, 282},
    {2103, 2475, 2278, 1865, 2026, 1965, 576, 262},
    {2104, 2477, 2280, 1865, 2040, 1978, 523, 252},
    {2106, 2477, 2279, 1862, 2051, 1984, 484, 244},
    {2104, 2477, 2275, 1864, 2054, 1989, 450, 237},
    {2106, 2479, 2280, 1865, 2063, 1994, 421, 233},
    {2102, 2478, 2279, 1865, 2064, 1992, 396, 230},

    // circle
    {68, 99, 2261, 785, 2070, 1991, 2036, 912},
    {68, 100, 2261, 786, 2070, 1987, 2032, 908},
    {66, 100, 2264, 787, 2072, 1989, 2032, 908},
    {67, 101, 2262, 785, 2069, 1987, 2031, 911},
    {67, 101, 2263, 787, 2069, 1987, 2030, 908},
    {67, 100, 2261, 787, 2072, 1991, 2036, 911},
    {68, 99, 2260, 784, 2066, 1986, 2031, 909},
    {68, 101, 2262, 786, 2070, 1989, 2031, 910},
    {70, 98, 2260, 786, 2070, 1989, 2034, 910},
    {69, 99, 2263, 786, 2071, 1988, 2031, 907},

    {236, 293, 442, 1401, 2050, 1981, 2023, 907},
    {228, 286, 479, 1294, 2030, 1983, 2026, 907},
    {220, 279, 549, 1179, 1985, 1984, 2026, 907},
    {212, 273, 677, 1054, 1912, 1983, 2023, 907},
    {209, 268, 887, 937, 1810, 1984, 2026, 910},
    {204, 264, 1145, 845, 1705, 1981, 2023, 904},
    {202, 259, 1384, 769, 1606, 1984, 2025, 909},
    {198, 256, 1590, 710, 1270, 1982, 2023, 904},
    {199, 250, 1766, 658, 1166, 1982, 2020, 908},
    {201, 247, 1938, 618, 1064, 1983, 2001, 906},
    {215, 243, 2076, 580, 958, 1980, 1937, 905},
    {265, 239, 2172, 550, 862, 1983, 1836, 908},
    {520, 235, 2210, 523, 776, 1982, 1698, 905},
    {1062, 235, 2234, 500, 705, 1977, 1557, 902},
    {1556, 232, 2244, 479, 650, 1961, 1195, 906},
    {1852, 241, 2250, 459, 602, 1903, 1048, 903},
    {1973, 267, 2255, 444, 572, 1844, 877, 904},
    {2025, 380, 2259, 428, 551, 1773, 746, 901},
    {2060, 846, 2259, 414, 543, 1713, 657, 898},
    {2076, 1604, 2260, 404, 548, 1650, 586, 885},
    {2087, 2099, 2261, 390, 560, 1562, 532, 863},
    {2094, 2292, 2261, 381, 588, 1205, 487, 827},
    {2096, 2368, 2260, 374, 638, 1056, 450, 773},
    {2100, 2410, 2266, 365, 692, 906, 419, 719},
    {2102, 2430, 2262, 356, 768, 778, 396, 670},
    {2104, 2441, 2265, 348, 892, 673, 372, 610},
    {2105, 2446, 2266, 344, 1077, 598, 353, 544},
    {2106, 2452, 2264, 336, 1276, 532, 339, 463},
    {2106, 2458, 2266, 334, 1714, 479, 321, 388},
    {2108, 2457, 2265, 333, 1870, 439, 312, 333},

    {1556, 363, 353, 1840, 2071, 1962, 2027, 912},
    {1487, 353, 335, 1845, 2073, 1965, 2027, 912},
    {1418, 342, 315, 1841, 2068, 1962, 2022, 910},
    {1380, 331, 297, 1843, 2057, 1964, 2027, 914},
    {1378, 320, 284, 1842, 1979, 1964, 2028, 912},
    {1414, 312, 271, 1840, 1831, 1961, 2027, 911},
    {1475, 305, 261, 1845, 1692, 1965, 2027, 913},
    {1556, 299, 251, 1842, 1556, 1961, 2025, 908},
    {1651, 293, 240, 1844, 1176, 1965, 2028, 911},
    {1741, 284, 228, 1842, 1027, 1964, 2027, 912},
    {1837, 278, 220, 1837, 878, 1965, 2028, 913},
    {1923, 273, 214, 1820, 752, 1964, 2026, 914},
    {1991, 269, 210, 1773, 665, 1961, 2021, 910},
    {2045, 262, 201, 1711, 600, 1966, 2027, 914},
    {2064, 258, 200, 1625, 552, 1964, 2023, 910},
    {2078, 253, 194, 1537, 509, 1964, 2026, 912},
    {2086, 250, 198, 1465, 476, 1965, 1978, 911},
    {2090, 247, 205, 1402, 446, 1962, 1850, 909},
    {2094, 243, 254, 1344, 425, 1966, 1670, 907},
    {2098, 239, 598, 1286, 403, 1962, 1229, 884},
    {2099, 238, 1429, 1259, 388, 1964, 994, 792},
    {2101, 272, 1951, 1276, 372, 1962, 827, 698},
    {2097, 803, 2123, 1341, 358, 1943, 711, 603},
    {2099, 1795, 2193, 1423, 344, 1894, 627, 491},
    {2100, 2199, 2223, 1516, 337, 1825, 567, 393},
    {2101, 2339, 2237, 1616, 326, 1768, 516, 323},
    {2101, 2395, 2249, 1689, 318, 1705, 475, 284},
    {2098, 2422, 2256, 1734, 311, 1626, 440, 263},
    {2099, 2438, 2260, 1762, 304, 1533, 411, 250},
    {2101, 2448, 2261, 1776, 297, 1176, 388, 239},

    // square
    {1991, 348, 295, 1784, 1764, 1963, 2017, 899},
    {1988, 339, 281, 1780, 1625, 1959, 2013, 897},
    {1985, 332, 269, 1783, 1540, 1961, 2010, 898},
    {1986, 322, 260, 1783, 1222, 1960, 2013, 897},
    {1978, 311, 250, 1782, 1092, 1961, 2011, 895},
    {1977, 306, 239, 1783, 941, 1962, 2011, 899},
    {1975, 295, 230, 1779, 795, 1960, 2010, 897},
    {1971, 289, 224, 1779, 695, 1962, 2013, 897},
    {1967, 286, 219, 1780, 619, 1961, 2011, 894},
    {1971, 279, 211, 1777, 564, 1958, 2011, 895},

    {54, 89, 2272, 755, 2064, 1956, 2024, 897},
    {54, 90, 2272, 754, 2062, 1956, 2020, 900},
    {52, 89, 2273, 755, 2064, 1959, 2023, 901},
    {54, 88, 2272, 754, 2064, 1956, 2022, 899},
    {54, 88, 2270, 754, 2064, 1959, 2026, 900},
    {52, 89, 2272, 754, 2063, 1957, 2021, 900},
    {54, 89, 2272, 753, 2062, 1956, 2021, 900},
    {55, 87, 2271, 751, 2065, 1957, 2026, 903},
    {54, 87, 2270, 750, 2062, 1956, 2024, 900},
    {54, 89, 2272, 755, 2065, 1958, 2024, 900},

    {286, 438, 2265, 1765, 2056, 1969, 2021, 898},
    {274, 418, 2266, 1593, 2060, 1972, 2024, 898},
    {260, 402, 2270, 1440, 2060, 1970, 2021, 897},
    {253, 386, 2267, 1349, 2057, 1965, 2021, 897},
    {239, 374, 2271, 1276, 2061, 1971, 2023, 897},
    {233, 358, 2266, 1146, 2059, 1969, 2021, 896},
    {222, 349, 2268, 988, 2058, 1970, 2019, 899},
    {215, 338, 2269, 870, 2060, 1968, 2019, 896},
    {211, 326, 2268, 781, 2056, 1969, 2020, 897},
    {205, 317, 2268, 714, 2060, 1972, 2022, 901},
    {198, 309, 2267, 662, 2059, 1969, 2019, 895},
    {193, 301, 2269, 615, 2057, 1971, 2021, 900},
    {188, 296, 2270, 581, 2060, 1966, 2019, 894},
    {186, 288, 2270, 546, 2058, 1914, 2019, 897},
    {179, 283, 2271, 520, 2058, 1829, 2011, 899},
    {177, 276, 2266, 497, 2057, 1739, 1948, 895},
    {193, 274, 2273, 475, 2060, 1659, 1830, 896},
    {528, 270, 2270, 457, 2060, 1601, 1684, 892},
    {1333, 278, 2269, 441, 2060, 1502, 1557, 894},
    {1751, 463, 2271, 426, 2059, 1099, 1211, 893},
    {1888, 1312, 2270, 412, 2058, 889, 1123, 887},
    {1961, 2055, 2270, 401, 2060, 736, 975, 890},
    {2008, 2275, 2265, 390, 2057, 634, 809, 888},
    {2037, 2360, 2267, 378, 2056, 559, 695, 889},
    {2056, 2407, 2267, 369, 2056, 502, 611, 889},
    {2063, 2430, 2267, 362, 2057, 456, 548, 885},
    {2071, 2446, 2270, 352, 2059, 418, 500, 886},
    {2073, 2452, 2267, 345, 2058, 390, 463, 886},
    {2077, 2457, 2266, 339, 2057, 365, 432, 889},
    {2077, 2459, 2269, 346, 2060, 342, 404, 887},

    {2069, 292, 209, 1819, 490, 1957, 1996, 891},
    {2069, 289, 208, 1818, 458, 1954, 1908, 880},
    {2072, 318, 202, 1813, 433, 1950, 1754, 788},
    {2072, 859, 243, 1815, 414, 1955, 1603, 695},
    {2069, 1861, 852, 1813, 396, 1949, 1231, 614},
    {2068, 2216, 1819, 1812, 379, 1949, 1105, 570},
    {2070, 2336, 2063, 1810, 367, 1947, 907, 487},
    {2070, 2388, 2154, 1806, 354, 1938, 765, 385},
    {2072, 2413, 2197, 1809, 343, 1931, 663, 313},
    {2070, 2428, 2216, 1807, 330, 1917, 590, 279},
    {2073, 2438, 2229, 1808, 321, 1912, 535, 262},
    {2073, 2442, 2235, 1807, 314, 1911, 490, 249},
    {2069, 2445, 2240, 1811, 306, 1905, 452, 238},
    {2073, 2448, 2242, 1811, 299, 1906, 424, 235},
    {2071, 2450, 2243, 1814, 300, 1904, 398, 231},
    {2071, 2454, 2248, 1818, 738, 1903, 373, 226},
    {2072, 2450, 2247, 1819, 1333, 1906, 356, 223},
    {2070, 2454, 2249, 1821, 1805, 1905, 338, 222},
    {2073, 2451, 2250, 1823, 1918, 1910, 326, 219},
    {2069, 2453, 2248, 1821, 1963, 1908, 315, 220},
    {2072, 2454, 2249, 1820, 1992, 1914, 304, 217},
    {2072, 2455, 2247, 1824, 2006, 1917, 293, 215},
    {2070, 2456, 2250, 1823, 2013, 1919, 282, 216},
    {2072, 2455, 2252, 1824, 2022, 1925, 274, 212},
    {2069, 2454, 2248, 1823, 2025, 1928, 274, 218},
    {2071, 2457, 2253, 1825, 2030, 1936, 500, 362},
    {2069, 2452, 2250, 1823, 2032, 1942, 1046, 603},
    {2069, 2455, 2251, 1823, 2035, 1945, 1679, 748},
    {2072, 2456, 2251, 1821, 2034, 1947, 1841, 825},
    {2069, 2453, 2246, 1821, 2035, 1952, 1912, 852},

    // ambiet
    {2125, 2470, 2266, 1863, 2060, 1978, 2023, 900},
    {2119, 2468, 2261, 1860, 2057, 1975, 2023, 896},
    {2122, 2472, 2264, 1861, 2056, 1976, 2022, 900},
    {2122, 2472, 2264, 1860, 2057, 1973, 2022, 896},
    {2121, 2471, 2264, 1861, 2058, 1975, 2022, 896},
    {2126, 2471, 2263, 1862, 2057, 1977, 2024, 900},
    {2122, 2470, 2262, 1859, 2055, 1973, 2022, 899},
    {2122, 2470, 2262, 1861, 2059, 1977, 2025, 898},
    {2122, 2470, 2262, 1860, 2058, 1976, 2024, 897},
    {2124, 2470, 2263, 1859, 2056, 1975, 2022, 899},
    {2126, 2469, 2266, 1859, 2058, 1973, 2024, 899},
    {2120, 2469, 2265, 1860, 2059, 1974, 2021, 896},
    {2123, 2471, 2265, 1863, 2061, 1977, 2023, 898},
    {2124, 2471, 2264, 1859, 2057, 1971, 2021, 896},
    {2124, 2471, 2263, 1861, 2056, 1975, 2023, 900},
    {2122, 2470, 2264, 1862, 2060, 1975, 2024, 897},
    {2122, 2471, 2262, 1859, 2055, 1973, 2020, 898},
    {2125, 2471, 2262, 1862, 2057, 1978, 2025, 901},
    {2124, 2469, 2262, 1860, 2057, 1975, 2024, 896},
    {2126, 2470, 2265, 1859, 2058, 1973, 2024, 898},
    {2124, 2468, 2264, 1858, 2058, 1975, 2025, 900},
    {2120, 2468, 2264, 1861, 2059, 1974, 2021, 896},
    {2123, 2473, 2266, 1863, 2060, 1975, 2024, 897},
    {2122, 2467, 2263, 1858, 2058, 1975, 2023, 897},
    {2122, 2472, 2265, 1862, 2060, 1975, 2023, 895},
    {2124, 2470, 2263, 1859, 2059, 1976, 2024, 900},
    {2123, 2469, 2261, 1860, 2057, 1975, 2024, 896},
    {2122, 2470, 2266, 1862, 2059, 1976, 2022, 899},
    {2120, 2469, 2262, 1861, 2057, 1974, 2023, 895},
    {2124, 2472, 2266, 1861, 2058, 1975, 2023, 896},
    {2124, 2468, 2264, 1859, 2059, 1976, 2025, 898},
    {2124, 2470, 2261, 1860, 2056, 1975, 2024, 898},
    {2124, 2470, 2266, 1863, 2060, 1978, 2023, 899},
    {2124, 2469, 2263, 1859, 2057, 1971, 2022, 896},
    {2124, 2472, 2266, 1861, 2059, 1975, 2023, 897},
    {2121, 2467, 2264, 1858, 2057, 1976, 2024, 899},
    {2120, 2470, 2265, 1861, 2058, 1974, 2021, 897},
    {2122, 2472, 2266, 1864, 2058, 1975, 2022, 898},
    {2120, 2470, 2265, 1861, 2057, 1973, 2021, 896},
    {2122, 2472, 2261, 1862, 2057, 1977, 2024, 900},
    {2122, 2469, 2264, 1861, 2059, 1975, 2021, 897},
    {2121, 2471, 2263, 1861, 2058, 1976, 2024, 898},
    {2125, 2469, 2264, 1860, 2059, 1976, 2024, 901},
    {2122, 2471, 2265, 1862, 2058, 1973, 2021, 894},
    {2122, 2471, 2261, 1862, 2058, 1977, 2024, 900},
    {2122, 2468, 2264, 1860, 2059, 1975, 2021, 897},
    {2123, 2470, 2263, 1859, 2059, 1974, 2023, 898},
    {2120, 2470, 2266, 1862, 2060, 1976, 2023, 898},
    {2121, 2472, 2264, 1860, 2056, 1973, 2017, 897},
    {2124, 2472, 2262, 1861, 2057, 1976, 2023, 901}};
int train_data_output[train_data_num][target_num] = {
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    // {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1}

};
/******************************************************************
 * End Network Configuration
 ******************************************************************/

int ReportEvery10;
int RandomizedIndex[train_data_num];
long TrainingCycle;
float Rando;
float Error;
float Accum;

float data_mean[8] = {0};
float data_std[8] = {0};

float Hidden[HiddenNodes];
float Output[target_num];
float HiddenWeights[input_length + 1][HiddenNodes];
float OutputWeights[HiddenNodes + 1][target_num];
float HiddenDelta[HiddenNodes];
float OutputDelta[target_num];
float ChangeHiddenWeights[input_length + 1][HiddenNodes];
float ChangeOutputWeights[HiddenNodes + 1][target_num];

int target_value;
int out_value;
int max;

/*---------------------------------------------------------------------------------------------------------*/
/* Define Function Prototypes                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void);
void UART0_Init(void);
void AdcSingleCycleScanModeTest(void);

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /* ADC clock source is 22.1184MHz, set divider to 7, ADC clock is 22.1184/7 MHz */
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(7));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Disable the GPA0 - GPA3 digital input path to avoid the leakage current. */
    GPIO_DISABLE_DIGITAL_PATH(PA, 0xF);

    /* Configure the GPA0 - GPA3 ADC analog input pins */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2 | SYS_GPA_MFP_PA3_ADC3;
    SYS->ALT_MFP1 = 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void scale_data()
{
    float sum[8] = {0};
    int i, j;

    // Compute Data Mean
    for (i = 0; i < train_data_num; i++)
    {
        for (j = 0; j < input_length; j++)
        {
            sum[j] += train_data_input[i][j];
        }
    }
    for (j = 0; j < input_length; j++)
    {
        data_mean[j] = sum[j] / train_data_num;
        printf("MEAN: %.2f\n", data_mean[j]);
        sum[j] = 0.0;
    }

    // Compute Data STD
    for (i = 0; i < train_data_num; i++)
    {
        for (j = 0; j < input_length; j++)
        {
            sum[j] += pow(train_data_input[i][j] - data_mean[j], 2);
        }
    }
    for (j = 0; j < input_length; j++)
    {
        data_std[j] = sqrt(sum[j] / train_data_num);
        printf("STD: %.2f\n", data_std[j]);
        sum[j] = 0.0;
    }
}

void normalize(float *data)
{
    int i;

    for (i = 0; i < input_length; i++)
    {
        data[i] = (data[i] - data_mean[i]) / data_std[i];
    }
}

int train_preprocess()
{
    int i;

    for (i = 0; i < train_data_num; i++)
    {
        normalize(train_data_input[i]);
    }

    return 0;
}

// int test_preprocess()
// {
//     int i;

//     for (i = 0; i < test_data_num; i++)
//     {
//         normalize(test_data_input[i]);
//     }

//     return 0;
// }

int data_setup()
{
    int i;
    // int j;
    int p, ret;
    unsigned int seed = 1;

    /* Set the ADC operation mode as single-cycle, input mode as single-end and
             enable the analog input channel 0, 1 and 2 */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0xFF);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Start A/D conversion */
    ADC_START_CONV(ADC);

    /* Wait conversion done */
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
        ;

    for (i = 0; i < 8; i++)
    {
        seed *= ADC_GET_CONVERSION_DATA(ADC, i);
    }
    seed *= 1000;
    printf("\nRandom seed: %d\n", seed);
    srand(seed);

    ReportEvery10 = 1;
    for (p = 0; p < train_data_num; p++)
    {
        RandomizedIndex[p] = p;
    }

    scale_data();
    ret = train_preprocess();
    // ret |= test_preprocess();
    if (ret) // Error Check
        return 1;

    return 0;
}

void run_train_data()
{
    int i, j, p;
    int correct = 0;
    float accuracy = 0;
    printf("Train result:\n");
    for (p = 0; p < train_data_num; p++)
    {
        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (train_data_output[p][i] > train_data_output[p][max])
            {
                max = i;
            }
        }
        target_value = max;

        /******************************************************************
         * Compute hidden layer activations
         ******************************************************************/

        for (i = 0; i < HiddenNodes; i++)
        {
            Accum = HiddenWeights[input_length][i];
            for (j = 0; j < input_length; j++)
            {
                Accum += train_data_input[p][j] * HiddenWeights[j][i];
            }
            Hidden[i] = 1.0 / (1.0 + exp(-Accum));
        }

        /******************************************************************
         * Compute output layer activations and calculate errors
         ******************************************************************/

        for (i = 0; i < target_num; i++)
        {
            Accum = OutputWeights[HiddenNodes][i];
            for (j = 0; j < HiddenNodes; j++)
            {
                Accum += Hidden[j] * OutputWeights[j][i];
            }
            Output[i] = 1.0 / (1.0 + exp(-Accum));
        }

        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (Output[i] > Output[max])
            {
                max = i;
            }
        }
        out_value = max;

        if (out_value != target_value)
            printf("Error --> Training Pattern: %d,Target : %d, Output : %d\n", p, target_value, out_value);
        else
            correct++;
    }
    // Calculate accuracy
    accuracy = (float)correct / train_data_num;
    printf("Accuracy = %.2f /100 \n", accuracy * 100);
}

// void run_test_data()
// {
//     int i, j, p;
//     int correct = 0;
//     float accuracy = 0;
//     printf("Test result:\n");
//     for (p = 0; p < test_data_num; p++)
//     {
//         max = 0;
//         for (i = 1; i < target_num; i++)
//         {
//             if (test_data_output[p][i] > test_data_output[p][max])
//             {
//                 max = i;
//             }
//         }
//         target_value = max;

//         /******************************************************************
//          * Compute hidden layer activations
//          ******************************************************************/

//         for (i = 0; i < HiddenNodes; i++)
//         {
//             Accum = HiddenWeights[input_length][i];
//             for (j = 0; j < input_length; j++)
//             {
//                 Accum += test_data_input[p][j] * HiddenWeights[j][i];
//             }
//             Hidden[i] = 1.0 / (1.0 + exp(-Accum));
//         }

//         /******************************************************************
//          * Compute output layer activations and calculate errors
//          ******************************************************************/

//         for (i = 0; i < target_num; i++)
//         {
//             Accum = OutputWeights[HiddenNodes][i];
//             for (j = 0; j < HiddenNodes; j++)
//             {
//                 Accum += Hidden[j] * OutputWeights[j][i];
//             }
//             Output[i] = 1.0 / (1.0 + exp(-Accum));
//         }
//         max = 0;
//         for (i = 1; i < target_num; i++)
//         {
//             if (Output[i] > Output[max])
//             {
//                 max = i;
//             }
//         }
//         out_value = max;

//         if (out_value != target_value)
//             printf("Error --> Training Pattern: %d,Target : %d, Output : %d\n", p, target_value, out_value);
//         else
//             correct++;
//     }
//     // Calculate accuracy
//     accuracy = (float)correct / test_data_num;
//     printf("Accuracy = %.2f /100 \n", accuracy * 100);
// }

float Get_Train_Accuracy()
{
    int i, j, p;
    int correct = 0;
    float accuracy = 0;
    for (p = 0; p < train_data_num; p++)
    {
        /******************************************************************
         * Compute hidden layer activations
         ******************************************************************/

        for (i = 0; i < HiddenNodes; i++)
        {
            Accum = HiddenWeights[input_length][i];
            for (j = 0; j < input_length; j++)
            {
                Accum += train_data_input[p][j] * HiddenWeights[j][i];
            }
            Hidden[i] = 1.0 / (1.0 + exp(-Accum));
        }

        /******************************************************************
         * Compute output layer activations
         ******************************************************************/

        for (i = 0; i < target_num; i++)
        {
            Accum = OutputWeights[HiddenNodes][i];
            for (j = 0; j < HiddenNodes; j++)
            {
                Accum += Hidden[j] * OutputWeights[j][i];
            }
            Output[i] = 1.0 / (1.0 + exp(-Accum));
        }
        // get target value
        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (train_data_output[p][i] > train_data_output[p][max])
            {
                max = i;
            }
        }
        target_value = max;
        // get output value
        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (Output[i] > Output[max])
            {
                max = i;
            }
        }
        out_value = max;
        // compare output and target
        if (out_value == target_value)
        {
            correct++;
        }
    }

    // Calculate accuracy
    accuracy = (float)correct / train_data_num;
    return accuracy;
}

void load_weight()
{
    int i, j;
    printf("\n=======Hidden Weight=======\n");
    printf("{");
    for (i = 0; i <= input_length; i++)
    {
        printf("{");
        for (j = 0; j < HiddenNodes; j++)
        {
            if (j != HiddenNodes - 1)
            {
                printf("%f,", HiddenWeights[i][j]);
            }
            else
            {
                printf("%f", HiddenWeights[i][j]);
            }
        }
        if (i != input_length)
        {
            printf("},\n");
        }
        else
        {
            printf("}");
        }
    }
    printf("}\n");

    printf("\n=======Output Weight=======\n");

    for (i = 0; i <= HiddenNodes; i++)
    {
        printf("{");
        for (j = 0; j < target_num; j++)
        {
            if (j != target_num - 1)
            {
                printf("%f,", OutputWeights[i][j]);
            }
            else
            {
                printf("%f", OutputWeights[i][j]);
            }
        }
        if (i != HiddenNodes)
        {
            printf("},\n");
        }
        else
        {
            printf("}");
        }
    }
    printf("}\n");
}

void AdcSingleCycleScanModeTest()
{
    int i, j;
    uint32_t u32ChannelCount;
    float single_data_input[3];
    char output_string[10] = {NULL};

    printf("\n");
    printf("[Phase 3] Start Prediction ...\n\n");
    PB2 = 1;

    // c = t = s = a = 0;
    while (1)
    {

        /* Set the ADC operation mode as single-cycle, input mode as single-end and
         enable the analog input channel 0, 1, 2 and 3 */
        ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0xFF);

        /* Power on ADC module */
        ADC_POWER_ON(ADC);

        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        /* Wait conversion done */
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
            ;

        for (u32ChannelCount = 0; u32ChannelCount < input_length; u32ChannelCount++)
        {
            single_data_input[u32ChannelCount] = ADC_GET_CONVERSION_DATA(ADC, u32ChannelCount);
        }
        normalize(single_data_input);

        // Compute hidden layer activations
        for (i = 0; i < HiddenNodes; i++)
        {
            Accum = HiddenWeights[input_length][i];
            for (j = 0; j < input_length; j++)
            {
                Accum += single_data_input[j] * HiddenWeights[j][i];
            }
            Hidden[i] = 1.0 / (1.0 + exp(-Accum));
        }

        // Compute output layer activations
        for (i = 0; i < target_num; i++)
        {
            Accum = OutputWeights[HiddenNodes][i];
            for (j = 0; j < HiddenNodes; j++)
            {
                Accum += Hidden[j] * OutputWeights[j][i];
            }
            Output[i] = 1.0 / (1.0 + exp(-Accum));
        }

        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (Output[i] > Output[max])
            {
                max = i;
            }
        }
        out_value = max;

        switch (out_value)
        {
        case 0:
            strcpy(output_string, "triangle*****");
            t++;
            break;
        case 1:
            strcpy(output_string, "circle****************************");
            c++;
            break;
        case 2:
            strcpy(output_string, "square******************************************************");
            s++;
            break;
        case 3:
            strcpy(output_string, "Ambient");
            break;
            // case 3:
            //     strcpy(output_string, "Red");
            //     break;
            // case 4:
            //     strcpy(output_string, "Orange");
            //     break;
            // case 5:
            //     strcpy(output_string, "Yellow");
            //     break;
            // case 6:
            //     strcpy(output_string, "Green");
            //     break;
        }

        if (c > 3)
        {
            printf("circle\n");
            CLK_SysTickDelay(1000000);
            c = t = s = 0;
        }

        else if (s > 0)
        {
            printf("square\n");
            CLK_SysTickDelay(1000000);
            c = t = s = 0;
        }
        else if (t > 8)
        {
            printf("triangle\n");
            CLK_SysTickDelay(1000000);
            c = t = s = 0;
        }

        // printf("\nPrediction output: %-8s", output_string);
        CLK_SysTickDelay(100);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    int i, j, p, q, r;
    float accuracy = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    GPIO_SetMode(PB, BIT2, GPIO_PMD_OUTPUT);
    PB2 = 0;

    printf("\n+-----------------------------------------------------------------------+\n");
    printf("|                        LAB8 - Machine Learning                        |\n");
    printf("+-----------------------------------------------------------------------+\n");
    printf("System clock rate: %d Hz\n", SystemCoreClock);

    printf("\n[Phase 1] Initialize DataSet ...");
    /* Data Init (Input / Output Preprocess) */
    if (data_setup())
    {
        printf("[Error] Datasets Setup Error\n");
        return 0;
    }
    else
        printf("Done!\n\n");

    printf("[Phase 2] Start Model Training ...\n");
    // Initialize HiddenWeights and ChangeHiddenWeights
    for (i = 0; i < HiddenNodes; i++)
    {
        for (j = 0; j <= input_length; j++)
        {
            ChangeHiddenWeights[j][i] = 0.0;
            Rando = (float)((rand() % 100)) / 100;
            HiddenWeights[j][i] = 2.0 * (Rando - 0.5) * InitialWeightMax;
        }
    }

    // Initialize OutputWeights and ChangeOutputWeights
    for (i = 0; i < target_num; i++)
    {
        for (j = 0; j <= HiddenNodes; j++)
        {
            ChangeOutputWeights[j][i] = 0.0;
            Rando = (float)((rand() % 100)) / 100;
            OutputWeights[j][i] = 2.0 * (Rando - 0.5) * InitialWeightMax;
        }
    }

    // Begin training
    for (TrainingCycle = 1; TrainingCycle < 2147483647; TrainingCycle++)
    {
        Error = 0.0;

        // Randomize order of training patterns
        for (p = 0; p < train_data_num; p++)
        {
            q = rand() % train_data_num;
            r = RandomizedIndex[p];
            RandomizedIndex[p] = RandomizedIndex[q];
            RandomizedIndex[q] = r;
        }

        // Cycle through each training pattern in the randomized order
        for (q = 0; q < train_data_num; q++)
        {
            p = RandomizedIndex[q];

            // Compute hidden layer activations
            for (i = 0; i < HiddenNodes; i++)
            {
                Accum = HiddenWeights[input_length][i];
                for (j = 0; j < input_length; j++)
                {
                    Accum += train_data_input[p][j] * HiddenWeights[j][i];
                }
                Hidden[i] = 1.0 / (1.0 + exp(-Accum));
            }

            // Compute output layer activations and calculate errors
            for (i = 0; i < target_num; i++)
            {
                Accum = OutputWeights[HiddenNodes][i];
                for (j = 0; j < HiddenNodes; j++)
                {
                    Accum += Hidden[j] * OutputWeights[j][i];
                }
                Output[i] = 1.0 / (1.0 + exp(-Accum));
                OutputDelta[i] = (train_data_output[p][i] - Output[i]) * Output[i] * (1.0 - Output[i]);
                Error += 0.5 * (train_data_output[p][i] - Output[i]) * (train_data_output[p][i] - Output[i]);
            }

            // Backpropagate errors to hidden layer
            for (i = 0; i < HiddenNodes; i++)
            {
                Accum = 0.0;
                for (j = 0; j < target_num; j++)
                {
                    Accum += OutputWeights[i][j] * OutputDelta[j];
                }
                HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]);
            }

            // Update Input-->Hidden Weights
            for (i = 0; i < HiddenNodes; i++)
            {
                ChangeHiddenWeights[input_length][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[input_length][i];
                HiddenWeights[input_length][i] += ChangeHiddenWeights[input_length][i];
                for (j = 0; j < input_length; j++)
                {
                    ChangeHiddenWeights[j][i] = LearningRate * train_data_input[p][j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];
                    HiddenWeights[j][i] += ChangeHiddenWeights[j][i];
                }
            }

            // Update Hidden-->Output Weights
            for (i = 0; i < target_num; i++)
            {
                ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i];
                OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i];
                for (j = 0; j < HiddenNodes; j++)
                {
                    ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i];
                    OutputWeights[j][i] += ChangeOutputWeights[j][i];
                }
            }
        }
        accuracy = Get_Train_Accuracy();

        // Every 10 cycles send data to terminal for display
        ReportEvery10 = ReportEvery10 - 1;
        if (ReportEvery10 == 0)
        {

            printf("\nTrainingCycle: %ld\n", TrainingCycle);
            printf("Error = %.5f\n", Error);
            printf("Accuracy = %.2f /100 \n", accuracy * 100);
            // run_train_data();

            if (TrainingCycle == 1)
            {
                ReportEvery10 = 9;
            }
            else
            {
                ReportEvery10 = 10;
            }
        }

        // If error rate is less than pre-determined threshold then end
        if (accuracy >= goal_acc)
            break;

        if (TrainingCycle >= 100)
            break;
    }

    printf("\nTrainingCycle: %ld\n", TrainingCycle);
    printf("Error = %.5f\n", Error);
    run_train_data();
    printf("Training Set Solved!\n");
    printf("--------\n");
    printf("Testing Start!\n ");
    // run_test_data();
    printf("--------\n");
    ReportEvery10 = 1;
    load_weight();

    printf("\nModel Training Phase has ended.\n");

    /* Start prediction */
    AdcSingleCycleScanModeTest();

    while (1)
        ;
}