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

#define train_data_num 140 // Total number of training data
#define test_data_num 70   // Total number of testing data

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
    {448, 434, 245},
    {429, 420, 236},
    {447, 433, 244},
    {444, 434, 243},
    {450, 434, 242},
    {436, 424, 241},
    {428, 421, 237},
    {447, 435, 241},
    {429, 418, 237},
    {447, 436, 242},
    {449, 438, 244},
    {431, 421, 238},
    {448, 435, 241},
    {447, 434, 241},
    {450, 435, 243},
    {442, 429, 240},
    {451, 435, 244},
    {451, 437, 245},
    {429, 418, 237},
    {451, 433, 242},

    // Blue
    {270, 238, 170},
    {240, 216, 170},
    {274, 237, 171},
    {259, 224, 171},
    {261, 231, 172},
    {247, 218, 170},
    {246, 218, 170},
    {268, 236, 170},
    {248, 220, 169},
    {272, 237, 172},
    {260, 228, 171},
    {243, 217, 169},
    {268, 234, 172},
    {264, 231, 171},
    {268, 236, 171},
    {253, 223, 171},
    {274, 237, 170},
    {273, 234, 170},
    {248, 220, 169},
    {257, 229, 172},

    // Magenta
    {423, 249, 174},
    {423, 271, 174},
    {413, 252, 170},
    {411, 270, 173},
    {432, 249, 180},
    {415, 275, 172},
    {413, 254, 166},
    {421, 248, 175},
    {408, 250, 166},
    {410, 254, 169},
    {414, 262, 171},
    {425, 251, 173},
    {426, 247, 175},
    {412, 247, 171},
    {429, 248, 176},
    {411, 275, 171},
    {427, 251, 174},
    {427, 252, 175},
    {408, 250, 166},
    {430, 251, 178},

    // Red
    {408, 246, 162},
    {417, 244, 161},
    {422, 247, 164},
    {376, 239, 156},
    {431, 253, 165},
    {384, 236, 158},
    {380, 228, 156},
    {403, 247, 161},
    {388, 227, 156},
    {428, 249, 164},
    {386, 239, 158},
    {384, 233, 155},
    {418, 246, 164},
    {416, 248, 161},
    {395, 254, 158},
    {376, 236, 158},
    {394, 243, 161},
    {388, 256, 160},
    {388, 227, 156},
    {423, 251, 164},

    // Orange
    {420, 292, 171},
    {440, 299, 173},
    {410, 287, 167},
    {402, 279, 167},
    {411, 284, 169},
    {406, 288, 169},
    {412, 284, 166},
    {431, 298, 173},
    {409, 280, 168},
    {411, 285, 170},
    {399, 277, 165},
    {424, 293, 168},
    {426, 296, 171},
    {425, 293, 172},
    {427, 298, 171},
    {403, 280, 169},
    {424, 295, 170},
    {430, 297, 173},
    {409, 280, 168},
    {409, 284, 168},

    // Yellow
    {458, 410, 198},
    {445, 398, 197},
    {455, 405, 198},
    {454, 410, 202},
    {467, 418, 201},
    {450, 408, 198},
    {439, 399, 193},
    {456, 412, 198},
    {427, 380, 190},
    {439, 390, 192},
    {422, 374, 187},
    {492, 459, 215},
    {441, 393, 195},
    {475, 435, 207},
    {434, 387, 189},
    {465, 425, 207},
    {449, 404, 195},
    {435, 386, 191},
    {427, 380, 190},
    {444, 396, 197},

    // Green
    {306, 276, 163},
    {254, 256, 160},
    {303, 274, 161},
    {308, 280, 164},
    {316, 287, 163},
    {282, 260, 160},
    {276, 264, 158},
    {287, 263, 157},
    {275, 261, 161},
    {291, 267, 158},
    {313, 285, 165},
    {272, 258, 160},
    {297, 266, 158},
    {296, 270, 162},
    {306, 282, 160},
    {298, 272, 162},
    {310, 284, 162},
    {311, 281, 165},
    {275, 261, 161},
    {316, 284, 162},

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
    {0, 0, 0, 0, 0, 0, 1}}; // Label of the training data

// Create testing dataset/output
float test_data_input[test_data_num][input_length] = {
    // Ambient
    {433, 420, 238},
    {448, 438, 243},
    {449, 435, 241},
    {432, 424, 238},
    {450, 441, 245},
    {449, 439, 245},
    {447, 435, 241},
    {433, 422, 238},
    {448, 436, 241},
    {448, 435, 242},

    // Blue
    {242, 215, 169},
    {272, 239, 169},
    {258, 229, 173},
    {239, 213, 170},
    {272, 237, 170},
    {269, 233, 168},
    {268, 236, 170},
    {243, 217, 170},
    {263, 232, 172},
    {269, 234, 174},

    // Magenta
    {428, 257, 172},
    {421, 256, 171},
    {422, 246, 174},
    {423, 264, 171},
    {419, 256, 170},
    {418, 259, 170},
    {411, 252, 171},
    {421, 273, 172},
    {431, 246, 178},
    {413, 251, 171},

    // Red
    {410, 240, 159},
    {404, 246, 164},
    {410, 247, 160},
    {434, 250, 165},
    {410, 250, 162},
    {404, 245, 162},
    {419, 246, 162},
    {397, 235, 161},
    {427, 251, 164},
    {402, 247, 160},

    // Orange
    {440, 296, 171},
    {413, 285, 169},
    {413, 287, 169},
    {449, 301, 174},
    {407, 280, 168},
    {399, 279, 168},
    {414, 287, 170},
    {423, 293, 171},
    {419, 291, 171},
    {427, 296, 171},

    // Yellow
    {502, 465, 223},
    {452, 407, 197},
    {444, 396, 194},
    {470, 433, 211},
    {441, 392, 192},
    {425, 373, 187},
    {447, 402, 193},
    {437, 390, 194},
    {453, 405, 196},
    {463, 419, 202},

    // Green
    {257, 256, 159},
    {317, 285, 165},
    {304, 277, 162},
    {251, 253, 159},
    {319, 289, 164},
    {317, 289, 163},
    {283, 260, 162},
    {267, 258, 160},
    {306, 277, 159},
    {281, 260, 158},

}; // You can put your testing dataset here
int test_data_output[test_data_num][target_num] = {
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
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

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
        for (j = 0; j <= input_length; j++)    // 遍歷每個輸入特徵，包括偏置節點 (bias)
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
                Accum = HiddenWeights[input_length][i];                      //初始化加權和(包括 bias) 
                for (j = 0; j < input_length; j++)          
                {
                    Accum += train_data_input[p][j] * HiddenWeights[j][i];   // 累加輸入層的加權輸入
                }
                Hidden[i] = 1.0 / (1.0 + exp(-Accum));                       // Sigmoid 激活函數，計算隱藏層第 i 個神經元的輸出
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
                    Accum += OutputWeights[i][j] * OutputDelta[j];        // 將輸出層的誤差乘以對應的權重，累加起來
                }
                HiddenDelta[i] = Accum * Hidden[i] * (1.0 - Hidden[i]);   // 計算隱藏層誤差項
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

    while (1);
}
