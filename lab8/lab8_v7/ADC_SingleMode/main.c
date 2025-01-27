#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC100Series.h"

// #define PLL_CLOCK       50000000SSS
#define PLL_CLOCK 50000000

/******************************************************************
 * dataset format setting
 ******************************************************************/

#define train_data_num 175 // Total number of training data
#define test_data_num 35   // Total number of testing data

/******************************************************************
 * Network Configuration - customized per network
 ******************************************************************/
#define input_length 3 // The number of input
#define HiddenNodes 12 // The number of neurons in hidden layer
#define target_num 7   // The number of output

const float LearningRate = 0.005; // Learning Rate
const float Momentum = 0.9;
const float InitialWeightMax = 0.5;
const float goal_acc = 0.97; // Target accuracy

// Create training dataset/output
float train_data_input[train_data_num][input_length] = {
    // Ambient
    {487, 460, 222},
    {480, 460, 222},
    {485, 461, 222},
    {486, 462, 223},
    {480, 459, 219},
    {482, 462, 223},
    {475, 454, 220},
    {482, 458, 222},
    {475, 458, 222},
    {485, 462, 221},
    {484, 461, 225},
    {474, 456, 222},
    {483, 460, 220},
    {482, 457, 223},
    {483, 465, 221},
    {483, 461, 224},
    {484, 461, 224},
    {482, 463, 222},
    {475, 458, 222},
    {482, 458, 219},
    {475, 456, 221},
    {485, 461, 223},
    {481, 458, 220},
    {473, 453, 219},
    {485, 463, 223},

    // Blue
    {292, 248, 164},
    {280, 237, 149},
    {266, 243, 173},
    {288, 239, 152},
    {265, 236, 169},

    {283, 239, 151},
    {268, 228, 155},
    {252, 246, 184},
    {262, 226, 154},
    {256, 245, 178},

    {286, 240, 152},
    {267, 232, 154},
    {254, 242, 182},
    {256, 240, 176},
    {315, 263, 154},

    {284, 239, 155},
    {319, 267, 154},
    {317, 261, 153},
    {262, 226, 154},
    {265, 237, 169},

    {268, 227, 156},
    {316, 263, 156},
    {256, 238, 171},
    {274, 233, 153},
    {300, 251, 156},

    // Magenta
    {583, 318, 195},
    {480, 264, 156},
    {574, 313, 191},
    {546, 293, 181},
    {554, 305, 182},

    {596, 324, 197},
    {512, 276, 168},
    {560, 307, 187},
    {498, 266, 162},
    {560, 308, 185},

    {509, 274, 167},
    {531, 286, 179},
    {574, 314, 190},
    {564, 309, 185},
    {556, 298, 182},

    {589, 319, 195},
    {565, 305, 185},
    {540, 290, 177},
    {498, 266, 162},
    {559, 307, 185},

    {540, 293, 176},
    {599, 323, 197},
    {557, 306, 187},
    {492, 268, 165},
    {589, 314, 192},

    // Red
    {551, 314, 156},
    {536, 304, 156},
    {536, 306, 153},
    {537, 297, 153},
    {553, 312, 158},

    {530, 296, 149},
    {539, 301, 151},
    {554, 312, 159},
    {546, 304, 152},
    {550, 308, 155},

    {532, 297, 151},
    {533, 297, 148},
    {552, 316, 157},
    {551, 311, 158},
    {547, 312, 157},

    {547, 300, 154},
    {534, 303, 153},
    {532, 303, 152},
    {546, 304, 152},
    {544, 308, 154},

    {541, 302, 151},
    {556, 314, 158},
    {552, 314, 156},
    {552, 308, 155},
    {555, 315, 156},

    // Orange
    {604, 393, 172},
    {570, 398, 165},
    {603, 395, 173},
    {562, 396, 164},
    {592, 389, 171},

    {561, 394, 164},
    {568, 395, 164},
    {598, 393, 171},
    {586, 404, 169},
    {602, 393, 172},

    {569, 389, 163},
    {554, 391, 166},
    {601, 391, 172},
    {602, 396, 170},
    {607, 397, 172},

    {560, 395, 164},
    {596, 389, 171},
    {588, 383, 168},
    {586, 404, 169},
    {596, 390, 172},

    {557, 390, 164},
    {601, 395, 171},
    {596, 392, 171},
    {560, 389, 160},
    {596, 392, 170},

    // Yellow
    {574, 535, 210},
    {574, 533, 211},
    {590, 553, 215},
    {591, 549, 214},
    {585, 548, 214},

    {523, 477, 193},
    {539, 503, 196},
    {581, 541, 212},
    {544, 503, 200},
    {589, 552, 218},

    {581, 541, 212},
    {540, 497, 196},
    {612, 574, 224},
    {589, 550, 215},
    {577, 537, 209},

    {554, 516, 203},
    {591, 550, 215},
    {587, 542, 214},
    {544, 503, 200},
    {580, 544, 212},

    {548, 508, 198},
    {565, 518, 206},
    {588, 552, 215},
    {566, 526, 206},
    {559, 519, 203},

    // Green
    {260, 261, 135},
    {231, 261, 137},
    {261, 261, 134},
    {263, 248, 123},
    {256, 277, 143},

    {248, 245, 128},
    {217, 249, 133},
    {258, 267, 140},
    {214, 250, 133},
    {263, 266, 137},

    {254, 246, 126},
    {220, 252, 132},
    {256, 264, 138},
    {257, 265, 138},
    {257, 256, 133},

    {264, 247, 125},
    {265, 256, 134},
    {267, 254, 131},
    {214, 250, 133},
    {254, 285, 148},

    {226, 254, 134},
    {264, 253, 131},
    {258, 272, 143},
    {228, 264, 138},
    {250, 258, 134},

}; // You can put your training dataset here
int train_data_output[train_data_num][target_num] = {
    // Ambient
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},

    // Blue
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},

    // Magenta
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},

    // Red
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},

    // Orange
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},

    // Yellow
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},

    // Green
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1}}; // Label of the training data

// Create testing dataset/output
float test_data_input[test_data_num][input_length] = {
    // Ambient
    {487, 462, 222},
    {481, 462, 220},
    {481, 462, 222},
    {482, 459, 223},
    {482, 459, 221},

    // Blue
    {286, 243, 153},
    {246, 248, 185},
    {275, 235, 152},
    {256, 241, 173},
    {248, 252, 189},

    // Magenta
    {532, 283, 174},
    {579, 320, 192},
    {558, 304, 182},
    {555, 302, 184},
    {551, 300, 184},

    // Red
    {543, 304, 153},
    {569, 321, 161},
    {523, 295, 146},
    {552, 313, 156},
    {562, 317, 159},

    // Orange
    {591, 390, 168},
    {600, 390, 171},
    {559, 399, 167},
    {596, 388, 171},
    {600, 389, 171},

    // Yellow
    {573, 530, 210},
    {564, 527, 205},
    {540, 495, 197},
    {600, 562, 219},
    {568, 526, 208},

    // Green
    {251, 254, 132},
    {260, 278, 144},
    {239, 253, 133},
    {258, 264, 139},
    {258, 276, 142},

}; // You can put your testing dataset here
int test_data_output[test_data_num][target_num] = {
    // Ambient
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0},

    // Blue
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0},

    // Magenta
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0},

    // Red
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0},

    // Orange
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0},

    // Yellow
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 0},

    // Green
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 1}}; // Label of the testing data

/******************************************************************
 * End Network Configuration
 ******************************************************************/

int ReportEvery10;
int RandomizedIndex[train_data_num];
long TrainingCycle;
float Rando;
float Error;
float Accum;

float data_mean[3] = {0};
float data_std[3] = {0};

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
    float sum[3] = {0};
    int i, j;

    // Compute Data Mean
    for (i = 0; i < train_data_num; i++)
    { // 遍歷所有訓練資料
        for (j = 0; j < input_length; j++)
        {                                     // 遍歷每筆資料中的每個特徵
            sum[j] += train_data_input[i][j]; // 將每個特徵的值累加到 sum[j]
        }
    }
    for (j = 0; j < input_length; j++)
    {                                           // 遍歷每個特徵
        data_mean[j] = sum[j] / train_data_num; // 計算每個特徵的平均值
        printf("MEAN: %.2f\n", data_mean[j]);   // 印出平均值
        sum[j] = 0.0;                           // 將 sum 重置為 0，為下一步計算標準差準備
    }

    // Compute Data STD
    for (i = 0; i < train_data_num; i++)
    {
        for (j = 0; j < input_length; j++)
        {
            sum[j] += pow(train_data_input[i][j] - data_mean[j], 2); // 累加每個特徵與均值的平方差
        }
    }
    for (j = 0; j < input_length; j++)
    {                                                // 遍歷每個特徵
        data_std[j] = sqrt(sum[j] / train_data_num); // 計算標準差
        printf("STD: %.2f\n", data_std[j]);
        sum[j] = 0.0;
    }
}

void normalize(float *data)
{
    int i;

    // 遍歷輸入資料的每個特徵，並對每個特徵值進行標準化處理
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

int test_preprocess()
{
    int i;

    for (i = 0; i < test_data_num; i++)
    {
        normalize(test_data_input[i]);
    }

    return 0;
}

int data_setup()
{
    int i;
    // int j;
    int p, ret;
    unsigned int seed = 1;

    /* Set the ADC operation mode as single-cycle, input mode as single-end and
                enable the analog input channel 0, 1 and 2 */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0x7);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

    /* Start A/D conversion */
    ADC_START_CONV(ADC);

    /* Wait conversion done */
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
        ;

    for (i = 0; i < 3; i++)
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
    ret |= test_preprocess();
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
        /*train data 是哪一類(target value) */
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

        /* 找出最大的output 代表我預測這是哪一類 */
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

void run_test_data()
{
    int i, j, p;
    int correct = 0;
    float accuracy = 0;
    printf("Test result:\n");
    for (p = 0; p < test_data_num; p++)
    {
        max = 0;
        for (i = 1; i < target_num; i++)
        {
            if (test_data_output[p][i] > test_data_output[p][max])
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
                Accum += test_data_input[p][j] * HiddenWeights[j][i];
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
    accuracy = (float)correct / test_data_num;
    printf("Accuracy = %.2f /100 \n", accuracy * 100);
}

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
    while (1)
    {

        /* Set the ADC operation mode as single-cycle, input mode as single-end and
         enable the analog input channel 0, 1, 2 and 3 */
        ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, 0x7);

        /* Power on ADC module */
        ADC_POWER_ON(ADC);

        /* Clear the A/D interrupt flag for safe */
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

        /* Start A/D conversion */
        ADC_START_CONV(ADC);

        /* Wait conversion done */
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT))
            ;

        for (u32ChannelCount = 0; u32ChannelCount < 3; u32ChannelCount++)
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
            strcpy(output_string, "Ambient");
            break;
        case 1:
            strcpy(output_string, "Blue");
            break;
        case 2:
            strcpy(output_string, "Magenta");
            break;
        case 3:
            strcpy(output_string, "Red");
            break;
        case 4:
            strcpy(output_string, "Orange");
            break;
        case 5:
            strcpy(output_string, "Yellow");
            break;
        case 6:
            strcpy(output_string, "Green");
            break;
        }

        printf("\rPrediction output: %-8s", output_string);
        CLK_SysTickDelay(500000);
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
        for (j = 0; j <= input_length; j++) // 遍歷每個輸入特徵，包括偏置節點 (bias)
        {
            // 將權重變化量初始化為 0（動量項的初始值）
            ChangeHiddenWeights[j][i] = 0.0;

            // 隨機生成範圍在 [-InitialWeightMax, InitialWeightMax] 的初始權重
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
                Accum = HiddenWeights[input_length][i]; // 初始化加權和(包括 bias)
                for (j = 0; j < input_length; j++)
                {
                    Accum += train_data_input[p][j] * HiddenWeights[j][i]; // 累加輸入層的加權輸入
                }
                Hidden[i] = 1.0 / (1.0 + exp(-Accum)); // Sigmoid 激活函數，計算隱藏層第 i 個神經元的輸出
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
                    Accum += OutputWeights[i][j] * OutputDelta[j]; // 將輸出層的誤差乘以對應的權重，累加起來
                }
                HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]); // 計算隱藏層誤差項
            }

            // Update Input-->Hidden Weights
            for (i = 0; i < HiddenNodes; i++)
            {
                // 更新隱藏層的偏置權重 (輸入層到隱藏層的偏置對應的權重)
                ChangeHiddenWeights[input_length][i] = LearningRate * HiddenDelta[i] + Momentum * ChangeHiddenWeights[input_length][i];
                HiddenWeights[input_length][i] += ChangeHiddenWeights[input_length][i];
                for (j = 0; j < input_length; j++)
                {
                    // 計算輸入層到隱藏層的權重變化量 (包含學習率與動量項)
                    ChangeHiddenWeights[j][i] = LearningRate * train_data_input[p][j] * HiddenDelta[i] + Momentum * ChangeHiddenWeights[j][i];

                    // 更新輸入層到隱藏層的權重
                    HiddenWeights[j][i] += ChangeHiddenWeights[j][i];
                }
            }

            // Update Hidden-->Output Weights
            for (i = 0; i < target_num; i++)
            {
                // 更新輸出層的偏置權重 (隱藏層到輸出層的偏置對應的權重)
                ChangeOutputWeights[HiddenNodes][i] = LearningRate * OutputDelta[i] + Momentum * ChangeOutputWeights[HiddenNodes][i];
                OutputWeights[HiddenNodes][i] += ChangeOutputWeights[HiddenNodes][i];
                for (j = 0; j < HiddenNodes; j++)
                {
                    // 計算隱藏層到輸出層的權重變化量 (包含學習率與動量項)
                    ChangeOutputWeights[j][i] = LearningRate * Hidden[j] * OutputDelta[i] + Momentum * ChangeOutputWeights[j][i];

                    // 更新隱藏層到輸出層的權重
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
    }

    printf("\nTrainingCycle: %ld\n", TrainingCycle);
    printf("Error = %.5f\n", Error);
    run_train_data();
    printf("Training Set Solved!\n");
    printf("--------\n");
    printf("Testing Start!\n ");
    run_test_data();
    printf("--------\n");
    ReportEvery10 = 1;
    load_weight();

    printf("\nModel Training Phase has ended.\n");

    /* Start prediction */
    AdcSingleCycleScanModeTest();

    while (1)
        ;
}
