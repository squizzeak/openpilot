#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_5211353213920976706) {
   out_5211353213920976706[0] = delta_x[0] + nom_x[0];
   out_5211353213920976706[1] = delta_x[1] + nom_x[1];
   out_5211353213920976706[2] = delta_x[2] + nom_x[2];
   out_5211353213920976706[3] = delta_x[3] + nom_x[3];
   out_5211353213920976706[4] = delta_x[4] + nom_x[4];
   out_5211353213920976706[5] = delta_x[5] + nom_x[5];
   out_5211353213920976706[6] = delta_x[6] + nom_x[6];
   out_5211353213920976706[7] = delta_x[7] + nom_x[7];
   out_5211353213920976706[8] = delta_x[8] + nom_x[8];
   out_5211353213920976706[9] = delta_x[9] + nom_x[9];
   out_5211353213920976706[10] = delta_x[10] + nom_x[10];
   out_5211353213920976706[11] = delta_x[11] + nom_x[11];
   out_5211353213920976706[12] = delta_x[12] + nom_x[12];
   out_5211353213920976706[13] = delta_x[13] + nom_x[13];
   out_5211353213920976706[14] = delta_x[14] + nom_x[14];
   out_5211353213920976706[15] = delta_x[15] + nom_x[15];
   out_5211353213920976706[16] = delta_x[16] + nom_x[16];
   out_5211353213920976706[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_9125851500026127750) {
   out_9125851500026127750[0] = -nom_x[0] + true_x[0];
   out_9125851500026127750[1] = -nom_x[1] + true_x[1];
   out_9125851500026127750[2] = -nom_x[2] + true_x[2];
   out_9125851500026127750[3] = -nom_x[3] + true_x[3];
   out_9125851500026127750[4] = -nom_x[4] + true_x[4];
   out_9125851500026127750[5] = -nom_x[5] + true_x[5];
   out_9125851500026127750[6] = -nom_x[6] + true_x[6];
   out_9125851500026127750[7] = -nom_x[7] + true_x[7];
   out_9125851500026127750[8] = -nom_x[8] + true_x[8];
   out_9125851500026127750[9] = -nom_x[9] + true_x[9];
   out_9125851500026127750[10] = -nom_x[10] + true_x[10];
   out_9125851500026127750[11] = -nom_x[11] + true_x[11];
   out_9125851500026127750[12] = -nom_x[12] + true_x[12];
   out_9125851500026127750[13] = -nom_x[13] + true_x[13];
   out_9125851500026127750[14] = -nom_x[14] + true_x[14];
   out_9125851500026127750[15] = -nom_x[15] + true_x[15];
   out_9125851500026127750[16] = -nom_x[16] + true_x[16];
   out_9125851500026127750[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_5103205818133336040) {
   out_5103205818133336040[0] = 1.0;
   out_5103205818133336040[1] = 0.0;
   out_5103205818133336040[2] = 0.0;
   out_5103205818133336040[3] = 0.0;
   out_5103205818133336040[4] = 0.0;
   out_5103205818133336040[5] = 0.0;
   out_5103205818133336040[6] = 0.0;
   out_5103205818133336040[7] = 0.0;
   out_5103205818133336040[8] = 0.0;
   out_5103205818133336040[9] = 0.0;
   out_5103205818133336040[10] = 0.0;
   out_5103205818133336040[11] = 0.0;
   out_5103205818133336040[12] = 0.0;
   out_5103205818133336040[13] = 0.0;
   out_5103205818133336040[14] = 0.0;
   out_5103205818133336040[15] = 0.0;
   out_5103205818133336040[16] = 0.0;
   out_5103205818133336040[17] = 0.0;
   out_5103205818133336040[18] = 0.0;
   out_5103205818133336040[19] = 1.0;
   out_5103205818133336040[20] = 0.0;
   out_5103205818133336040[21] = 0.0;
   out_5103205818133336040[22] = 0.0;
   out_5103205818133336040[23] = 0.0;
   out_5103205818133336040[24] = 0.0;
   out_5103205818133336040[25] = 0.0;
   out_5103205818133336040[26] = 0.0;
   out_5103205818133336040[27] = 0.0;
   out_5103205818133336040[28] = 0.0;
   out_5103205818133336040[29] = 0.0;
   out_5103205818133336040[30] = 0.0;
   out_5103205818133336040[31] = 0.0;
   out_5103205818133336040[32] = 0.0;
   out_5103205818133336040[33] = 0.0;
   out_5103205818133336040[34] = 0.0;
   out_5103205818133336040[35] = 0.0;
   out_5103205818133336040[36] = 0.0;
   out_5103205818133336040[37] = 0.0;
   out_5103205818133336040[38] = 1.0;
   out_5103205818133336040[39] = 0.0;
   out_5103205818133336040[40] = 0.0;
   out_5103205818133336040[41] = 0.0;
   out_5103205818133336040[42] = 0.0;
   out_5103205818133336040[43] = 0.0;
   out_5103205818133336040[44] = 0.0;
   out_5103205818133336040[45] = 0.0;
   out_5103205818133336040[46] = 0.0;
   out_5103205818133336040[47] = 0.0;
   out_5103205818133336040[48] = 0.0;
   out_5103205818133336040[49] = 0.0;
   out_5103205818133336040[50] = 0.0;
   out_5103205818133336040[51] = 0.0;
   out_5103205818133336040[52] = 0.0;
   out_5103205818133336040[53] = 0.0;
   out_5103205818133336040[54] = 0.0;
   out_5103205818133336040[55] = 0.0;
   out_5103205818133336040[56] = 0.0;
   out_5103205818133336040[57] = 1.0;
   out_5103205818133336040[58] = 0.0;
   out_5103205818133336040[59] = 0.0;
   out_5103205818133336040[60] = 0.0;
   out_5103205818133336040[61] = 0.0;
   out_5103205818133336040[62] = 0.0;
   out_5103205818133336040[63] = 0.0;
   out_5103205818133336040[64] = 0.0;
   out_5103205818133336040[65] = 0.0;
   out_5103205818133336040[66] = 0.0;
   out_5103205818133336040[67] = 0.0;
   out_5103205818133336040[68] = 0.0;
   out_5103205818133336040[69] = 0.0;
   out_5103205818133336040[70] = 0.0;
   out_5103205818133336040[71] = 0.0;
   out_5103205818133336040[72] = 0.0;
   out_5103205818133336040[73] = 0.0;
   out_5103205818133336040[74] = 0.0;
   out_5103205818133336040[75] = 0.0;
   out_5103205818133336040[76] = 1.0;
   out_5103205818133336040[77] = 0.0;
   out_5103205818133336040[78] = 0.0;
   out_5103205818133336040[79] = 0.0;
   out_5103205818133336040[80] = 0.0;
   out_5103205818133336040[81] = 0.0;
   out_5103205818133336040[82] = 0.0;
   out_5103205818133336040[83] = 0.0;
   out_5103205818133336040[84] = 0.0;
   out_5103205818133336040[85] = 0.0;
   out_5103205818133336040[86] = 0.0;
   out_5103205818133336040[87] = 0.0;
   out_5103205818133336040[88] = 0.0;
   out_5103205818133336040[89] = 0.0;
   out_5103205818133336040[90] = 0.0;
   out_5103205818133336040[91] = 0.0;
   out_5103205818133336040[92] = 0.0;
   out_5103205818133336040[93] = 0.0;
   out_5103205818133336040[94] = 0.0;
   out_5103205818133336040[95] = 1.0;
   out_5103205818133336040[96] = 0.0;
   out_5103205818133336040[97] = 0.0;
   out_5103205818133336040[98] = 0.0;
   out_5103205818133336040[99] = 0.0;
   out_5103205818133336040[100] = 0.0;
   out_5103205818133336040[101] = 0.0;
   out_5103205818133336040[102] = 0.0;
   out_5103205818133336040[103] = 0.0;
   out_5103205818133336040[104] = 0.0;
   out_5103205818133336040[105] = 0.0;
   out_5103205818133336040[106] = 0.0;
   out_5103205818133336040[107] = 0.0;
   out_5103205818133336040[108] = 0.0;
   out_5103205818133336040[109] = 0.0;
   out_5103205818133336040[110] = 0.0;
   out_5103205818133336040[111] = 0.0;
   out_5103205818133336040[112] = 0.0;
   out_5103205818133336040[113] = 0.0;
   out_5103205818133336040[114] = 1.0;
   out_5103205818133336040[115] = 0.0;
   out_5103205818133336040[116] = 0.0;
   out_5103205818133336040[117] = 0.0;
   out_5103205818133336040[118] = 0.0;
   out_5103205818133336040[119] = 0.0;
   out_5103205818133336040[120] = 0.0;
   out_5103205818133336040[121] = 0.0;
   out_5103205818133336040[122] = 0.0;
   out_5103205818133336040[123] = 0.0;
   out_5103205818133336040[124] = 0.0;
   out_5103205818133336040[125] = 0.0;
   out_5103205818133336040[126] = 0.0;
   out_5103205818133336040[127] = 0.0;
   out_5103205818133336040[128] = 0.0;
   out_5103205818133336040[129] = 0.0;
   out_5103205818133336040[130] = 0.0;
   out_5103205818133336040[131] = 0.0;
   out_5103205818133336040[132] = 0.0;
   out_5103205818133336040[133] = 1.0;
   out_5103205818133336040[134] = 0.0;
   out_5103205818133336040[135] = 0.0;
   out_5103205818133336040[136] = 0.0;
   out_5103205818133336040[137] = 0.0;
   out_5103205818133336040[138] = 0.0;
   out_5103205818133336040[139] = 0.0;
   out_5103205818133336040[140] = 0.0;
   out_5103205818133336040[141] = 0.0;
   out_5103205818133336040[142] = 0.0;
   out_5103205818133336040[143] = 0.0;
   out_5103205818133336040[144] = 0.0;
   out_5103205818133336040[145] = 0.0;
   out_5103205818133336040[146] = 0.0;
   out_5103205818133336040[147] = 0.0;
   out_5103205818133336040[148] = 0.0;
   out_5103205818133336040[149] = 0.0;
   out_5103205818133336040[150] = 0.0;
   out_5103205818133336040[151] = 0.0;
   out_5103205818133336040[152] = 1.0;
   out_5103205818133336040[153] = 0.0;
   out_5103205818133336040[154] = 0.0;
   out_5103205818133336040[155] = 0.0;
   out_5103205818133336040[156] = 0.0;
   out_5103205818133336040[157] = 0.0;
   out_5103205818133336040[158] = 0.0;
   out_5103205818133336040[159] = 0.0;
   out_5103205818133336040[160] = 0.0;
   out_5103205818133336040[161] = 0.0;
   out_5103205818133336040[162] = 0.0;
   out_5103205818133336040[163] = 0.0;
   out_5103205818133336040[164] = 0.0;
   out_5103205818133336040[165] = 0.0;
   out_5103205818133336040[166] = 0.0;
   out_5103205818133336040[167] = 0.0;
   out_5103205818133336040[168] = 0.0;
   out_5103205818133336040[169] = 0.0;
   out_5103205818133336040[170] = 0.0;
   out_5103205818133336040[171] = 1.0;
   out_5103205818133336040[172] = 0.0;
   out_5103205818133336040[173] = 0.0;
   out_5103205818133336040[174] = 0.0;
   out_5103205818133336040[175] = 0.0;
   out_5103205818133336040[176] = 0.0;
   out_5103205818133336040[177] = 0.0;
   out_5103205818133336040[178] = 0.0;
   out_5103205818133336040[179] = 0.0;
   out_5103205818133336040[180] = 0.0;
   out_5103205818133336040[181] = 0.0;
   out_5103205818133336040[182] = 0.0;
   out_5103205818133336040[183] = 0.0;
   out_5103205818133336040[184] = 0.0;
   out_5103205818133336040[185] = 0.0;
   out_5103205818133336040[186] = 0.0;
   out_5103205818133336040[187] = 0.0;
   out_5103205818133336040[188] = 0.0;
   out_5103205818133336040[189] = 0.0;
   out_5103205818133336040[190] = 1.0;
   out_5103205818133336040[191] = 0.0;
   out_5103205818133336040[192] = 0.0;
   out_5103205818133336040[193] = 0.0;
   out_5103205818133336040[194] = 0.0;
   out_5103205818133336040[195] = 0.0;
   out_5103205818133336040[196] = 0.0;
   out_5103205818133336040[197] = 0.0;
   out_5103205818133336040[198] = 0.0;
   out_5103205818133336040[199] = 0.0;
   out_5103205818133336040[200] = 0.0;
   out_5103205818133336040[201] = 0.0;
   out_5103205818133336040[202] = 0.0;
   out_5103205818133336040[203] = 0.0;
   out_5103205818133336040[204] = 0.0;
   out_5103205818133336040[205] = 0.0;
   out_5103205818133336040[206] = 0.0;
   out_5103205818133336040[207] = 0.0;
   out_5103205818133336040[208] = 0.0;
   out_5103205818133336040[209] = 1.0;
   out_5103205818133336040[210] = 0.0;
   out_5103205818133336040[211] = 0.0;
   out_5103205818133336040[212] = 0.0;
   out_5103205818133336040[213] = 0.0;
   out_5103205818133336040[214] = 0.0;
   out_5103205818133336040[215] = 0.0;
   out_5103205818133336040[216] = 0.0;
   out_5103205818133336040[217] = 0.0;
   out_5103205818133336040[218] = 0.0;
   out_5103205818133336040[219] = 0.0;
   out_5103205818133336040[220] = 0.0;
   out_5103205818133336040[221] = 0.0;
   out_5103205818133336040[222] = 0.0;
   out_5103205818133336040[223] = 0.0;
   out_5103205818133336040[224] = 0.0;
   out_5103205818133336040[225] = 0.0;
   out_5103205818133336040[226] = 0.0;
   out_5103205818133336040[227] = 0.0;
   out_5103205818133336040[228] = 1.0;
   out_5103205818133336040[229] = 0.0;
   out_5103205818133336040[230] = 0.0;
   out_5103205818133336040[231] = 0.0;
   out_5103205818133336040[232] = 0.0;
   out_5103205818133336040[233] = 0.0;
   out_5103205818133336040[234] = 0.0;
   out_5103205818133336040[235] = 0.0;
   out_5103205818133336040[236] = 0.0;
   out_5103205818133336040[237] = 0.0;
   out_5103205818133336040[238] = 0.0;
   out_5103205818133336040[239] = 0.0;
   out_5103205818133336040[240] = 0.0;
   out_5103205818133336040[241] = 0.0;
   out_5103205818133336040[242] = 0.0;
   out_5103205818133336040[243] = 0.0;
   out_5103205818133336040[244] = 0.0;
   out_5103205818133336040[245] = 0.0;
   out_5103205818133336040[246] = 0.0;
   out_5103205818133336040[247] = 1.0;
   out_5103205818133336040[248] = 0.0;
   out_5103205818133336040[249] = 0.0;
   out_5103205818133336040[250] = 0.0;
   out_5103205818133336040[251] = 0.0;
   out_5103205818133336040[252] = 0.0;
   out_5103205818133336040[253] = 0.0;
   out_5103205818133336040[254] = 0.0;
   out_5103205818133336040[255] = 0.0;
   out_5103205818133336040[256] = 0.0;
   out_5103205818133336040[257] = 0.0;
   out_5103205818133336040[258] = 0.0;
   out_5103205818133336040[259] = 0.0;
   out_5103205818133336040[260] = 0.0;
   out_5103205818133336040[261] = 0.0;
   out_5103205818133336040[262] = 0.0;
   out_5103205818133336040[263] = 0.0;
   out_5103205818133336040[264] = 0.0;
   out_5103205818133336040[265] = 0.0;
   out_5103205818133336040[266] = 1.0;
   out_5103205818133336040[267] = 0.0;
   out_5103205818133336040[268] = 0.0;
   out_5103205818133336040[269] = 0.0;
   out_5103205818133336040[270] = 0.0;
   out_5103205818133336040[271] = 0.0;
   out_5103205818133336040[272] = 0.0;
   out_5103205818133336040[273] = 0.0;
   out_5103205818133336040[274] = 0.0;
   out_5103205818133336040[275] = 0.0;
   out_5103205818133336040[276] = 0.0;
   out_5103205818133336040[277] = 0.0;
   out_5103205818133336040[278] = 0.0;
   out_5103205818133336040[279] = 0.0;
   out_5103205818133336040[280] = 0.0;
   out_5103205818133336040[281] = 0.0;
   out_5103205818133336040[282] = 0.0;
   out_5103205818133336040[283] = 0.0;
   out_5103205818133336040[284] = 0.0;
   out_5103205818133336040[285] = 1.0;
   out_5103205818133336040[286] = 0.0;
   out_5103205818133336040[287] = 0.0;
   out_5103205818133336040[288] = 0.0;
   out_5103205818133336040[289] = 0.0;
   out_5103205818133336040[290] = 0.0;
   out_5103205818133336040[291] = 0.0;
   out_5103205818133336040[292] = 0.0;
   out_5103205818133336040[293] = 0.0;
   out_5103205818133336040[294] = 0.0;
   out_5103205818133336040[295] = 0.0;
   out_5103205818133336040[296] = 0.0;
   out_5103205818133336040[297] = 0.0;
   out_5103205818133336040[298] = 0.0;
   out_5103205818133336040[299] = 0.0;
   out_5103205818133336040[300] = 0.0;
   out_5103205818133336040[301] = 0.0;
   out_5103205818133336040[302] = 0.0;
   out_5103205818133336040[303] = 0.0;
   out_5103205818133336040[304] = 1.0;
   out_5103205818133336040[305] = 0.0;
   out_5103205818133336040[306] = 0.0;
   out_5103205818133336040[307] = 0.0;
   out_5103205818133336040[308] = 0.0;
   out_5103205818133336040[309] = 0.0;
   out_5103205818133336040[310] = 0.0;
   out_5103205818133336040[311] = 0.0;
   out_5103205818133336040[312] = 0.0;
   out_5103205818133336040[313] = 0.0;
   out_5103205818133336040[314] = 0.0;
   out_5103205818133336040[315] = 0.0;
   out_5103205818133336040[316] = 0.0;
   out_5103205818133336040[317] = 0.0;
   out_5103205818133336040[318] = 0.0;
   out_5103205818133336040[319] = 0.0;
   out_5103205818133336040[320] = 0.0;
   out_5103205818133336040[321] = 0.0;
   out_5103205818133336040[322] = 0.0;
   out_5103205818133336040[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_8265616698399609189) {
   out_8265616698399609189[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_8265616698399609189[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_8265616698399609189[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_8265616698399609189[3] = dt*state[12] + state[3];
   out_8265616698399609189[4] = dt*state[13] + state[4];
   out_8265616698399609189[5] = dt*state[14] + state[5];
   out_8265616698399609189[6] = state[6];
   out_8265616698399609189[7] = state[7];
   out_8265616698399609189[8] = state[8];
   out_8265616698399609189[9] = state[9];
   out_8265616698399609189[10] = state[10];
   out_8265616698399609189[11] = state[11];
   out_8265616698399609189[12] = state[12];
   out_8265616698399609189[13] = state[13];
   out_8265616698399609189[14] = state[14];
   out_8265616698399609189[15] = state[15];
   out_8265616698399609189[16] = state[16];
   out_8265616698399609189[17] = state[17];
}
void F_fun(double *state, double dt, double *out_6282295074003234978) {
   out_6282295074003234978[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6282295074003234978[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6282295074003234978[2] = 0;
   out_6282295074003234978[3] = 0;
   out_6282295074003234978[4] = 0;
   out_6282295074003234978[5] = 0;
   out_6282295074003234978[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6282295074003234978[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6282295074003234978[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6282295074003234978[9] = 0;
   out_6282295074003234978[10] = 0;
   out_6282295074003234978[11] = 0;
   out_6282295074003234978[12] = 0;
   out_6282295074003234978[13] = 0;
   out_6282295074003234978[14] = 0;
   out_6282295074003234978[15] = 0;
   out_6282295074003234978[16] = 0;
   out_6282295074003234978[17] = 0;
   out_6282295074003234978[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6282295074003234978[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6282295074003234978[20] = 0;
   out_6282295074003234978[21] = 0;
   out_6282295074003234978[22] = 0;
   out_6282295074003234978[23] = 0;
   out_6282295074003234978[24] = 0;
   out_6282295074003234978[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6282295074003234978[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6282295074003234978[27] = 0;
   out_6282295074003234978[28] = 0;
   out_6282295074003234978[29] = 0;
   out_6282295074003234978[30] = 0;
   out_6282295074003234978[31] = 0;
   out_6282295074003234978[32] = 0;
   out_6282295074003234978[33] = 0;
   out_6282295074003234978[34] = 0;
   out_6282295074003234978[35] = 0;
   out_6282295074003234978[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6282295074003234978[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6282295074003234978[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6282295074003234978[39] = 0;
   out_6282295074003234978[40] = 0;
   out_6282295074003234978[41] = 0;
   out_6282295074003234978[42] = 0;
   out_6282295074003234978[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6282295074003234978[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6282295074003234978[45] = 0;
   out_6282295074003234978[46] = 0;
   out_6282295074003234978[47] = 0;
   out_6282295074003234978[48] = 0;
   out_6282295074003234978[49] = 0;
   out_6282295074003234978[50] = 0;
   out_6282295074003234978[51] = 0;
   out_6282295074003234978[52] = 0;
   out_6282295074003234978[53] = 0;
   out_6282295074003234978[54] = 0;
   out_6282295074003234978[55] = 0;
   out_6282295074003234978[56] = 0;
   out_6282295074003234978[57] = 1;
   out_6282295074003234978[58] = 0;
   out_6282295074003234978[59] = 0;
   out_6282295074003234978[60] = 0;
   out_6282295074003234978[61] = 0;
   out_6282295074003234978[62] = 0;
   out_6282295074003234978[63] = 0;
   out_6282295074003234978[64] = 0;
   out_6282295074003234978[65] = 0;
   out_6282295074003234978[66] = dt;
   out_6282295074003234978[67] = 0;
   out_6282295074003234978[68] = 0;
   out_6282295074003234978[69] = 0;
   out_6282295074003234978[70] = 0;
   out_6282295074003234978[71] = 0;
   out_6282295074003234978[72] = 0;
   out_6282295074003234978[73] = 0;
   out_6282295074003234978[74] = 0;
   out_6282295074003234978[75] = 0;
   out_6282295074003234978[76] = 1;
   out_6282295074003234978[77] = 0;
   out_6282295074003234978[78] = 0;
   out_6282295074003234978[79] = 0;
   out_6282295074003234978[80] = 0;
   out_6282295074003234978[81] = 0;
   out_6282295074003234978[82] = 0;
   out_6282295074003234978[83] = 0;
   out_6282295074003234978[84] = 0;
   out_6282295074003234978[85] = dt;
   out_6282295074003234978[86] = 0;
   out_6282295074003234978[87] = 0;
   out_6282295074003234978[88] = 0;
   out_6282295074003234978[89] = 0;
   out_6282295074003234978[90] = 0;
   out_6282295074003234978[91] = 0;
   out_6282295074003234978[92] = 0;
   out_6282295074003234978[93] = 0;
   out_6282295074003234978[94] = 0;
   out_6282295074003234978[95] = 1;
   out_6282295074003234978[96] = 0;
   out_6282295074003234978[97] = 0;
   out_6282295074003234978[98] = 0;
   out_6282295074003234978[99] = 0;
   out_6282295074003234978[100] = 0;
   out_6282295074003234978[101] = 0;
   out_6282295074003234978[102] = 0;
   out_6282295074003234978[103] = 0;
   out_6282295074003234978[104] = dt;
   out_6282295074003234978[105] = 0;
   out_6282295074003234978[106] = 0;
   out_6282295074003234978[107] = 0;
   out_6282295074003234978[108] = 0;
   out_6282295074003234978[109] = 0;
   out_6282295074003234978[110] = 0;
   out_6282295074003234978[111] = 0;
   out_6282295074003234978[112] = 0;
   out_6282295074003234978[113] = 0;
   out_6282295074003234978[114] = 1;
   out_6282295074003234978[115] = 0;
   out_6282295074003234978[116] = 0;
   out_6282295074003234978[117] = 0;
   out_6282295074003234978[118] = 0;
   out_6282295074003234978[119] = 0;
   out_6282295074003234978[120] = 0;
   out_6282295074003234978[121] = 0;
   out_6282295074003234978[122] = 0;
   out_6282295074003234978[123] = 0;
   out_6282295074003234978[124] = 0;
   out_6282295074003234978[125] = 0;
   out_6282295074003234978[126] = 0;
   out_6282295074003234978[127] = 0;
   out_6282295074003234978[128] = 0;
   out_6282295074003234978[129] = 0;
   out_6282295074003234978[130] = 0;
   out_6282295074003234978[131] = 0;
   out_6282295074003234978[132] = 0;
   out_6282295074003234978[133] = 1;
   out_6282295074003234978[134] = 0;
   out_6282295074003234978[135] = 0;
   out_6282295074003234978[136] = 0;
   out_6282295074003234978[137] = 0;
   out_6282295074003234978[138] = 0;
   out_6282295074003234978[139] = 0;
   out_6282295074003234978[140] = 0;
   out_6282295074003234978[141] = 0;
   out_6282295074003234978[142] = 0;
   out_6282295074003234978[143] = 0;
   out_6282295074003234978[144] = 0;
   out_6282295074003234978[145] = 0;
   out_6282295074003234978[146] = 0;
   out_6282295074003234978[147] = 0;
   out_6282295074003234978[148] = 0;
   out_6282295074003234978[149] = 0;
   out_6282295074003234978[150] = 0;
   out_6282295074003234978[151] = 0;
   out_6282295074003234978[152] = 1;
   out_6282295074003234978[153] = 0;
   out_6282295074003234978[154] = 0;
   out_6282295074003234978[155] = 0;
   out_6282295074003234978[156] = 0;
   out_6282295074003234978[157] = 0;
   out_6282295074003234978[158] = 0;
   out_6282295074003234978[159] = 0;
   out_6282295074003234978[160] = 0;
   out_6282295074003234978[161] = 0;
   out_6282295074003234978[162] = 0;
   out_6282295074003234978[163] = 0;
   out_6282295074003234978[164] = 0;
   out_6282295074003234978[165] = 0;
   out_6282295074003234978[166] = 0;
   out_6282295074003234978[167] = 0;
   out_6282295074003234978[168] = 0;
   out_6282295074003234978[169] = 0;
   out_6282295074003234978[170] = 0;
   out_6282295074003234978[171] = 1;
   out_6282295074003234978[172] = 0;
   out_6282295074003234978[173] = 0;
   out_6282295074003234978[174] = 0;
   out_6282295074003234978[175] = 0;
   out_6282295074003234978[176] = 0;
   out_6282295074003234978[177] = 0;
   out_6282295074003234978[178] = 0;
   out_6282295074003234978[179] = 0;
   out_6282295074003234978[180] = 0;
   out_6282295074003234978[181] = 0;
   out_6282295074003234978[182] = 0;
   out_6282295074003234978[183] = 0;
   out_6282295074003234978[184] = 0;
   out_6282295074003234978[185] = 0;
   out_6282295074003234978[186] = 0;
   out_6282295074003234978[187] = 0;
   out_6282295074003234978[188] = 0;
   out_6282295074003234978[189] = 0;
   out_6282295074003234978[190] = 1;
   out_6282295074003234978[191] = 0;
   out_6282295074003234978[192] = 0;
   out_6282295074003234978[193] = 0;
   out_6282295074003234978[194] = 0;
   out_6282295074003234978[195] = 0;
   out_6282295074003234978[196] = 0;
   out_6282295074003234978[197] = 0;
   out_6282295074003234978[198] = 0;
   out_6282295074003234978[199] = 0;
   out_6282295074003234978[200] = 0;
   out_6282295074003234978[201] = 0;
   out_6282295074003234978[202] = 0;
   out_6282295074003234978[203] = 0;
   out_6282295074003234978[204] = 0;
   out_6282295074003234978[205] = 0;
   out_6282295074003234978[206] = 0;
   out_6282295074003234978[207] = 0;
   out_6282295074003234978[208] = 0;
   out_6282295074003234978[209] = 1;
   out_6282295074003234978[210] = 0;
   out_6282295074003234978[211] = 0;
   out_6282295074003234978[212] = 0;
   out_6282295074003234978[213] = 0;
   out_6282295074003234978[214] = 0;
   out_6282295074003234978[215] = 0;
   out_6282295074003234978[216] = 0;
   out_6282295074003234978[217] = 0;
   out_6282295074003234978[218] = 0;
   out_6282295074003234978[219] = 0;
   out_6282295074003234978[220] = 0;
   out_6282295074003234978[221] = 0;
   out_6282295074003234978[222] = 0;
   out_6282295074003234978[223] = 0;
   out_6282295074003234978[224] = 0;
   out_6282295074003234978[225] = 0;
   out_6282295074003234978[226] = 0;
   out_6282295074003234978[227] = 0;
   out_6282295074003234978[228] = 1;
   out_6282295074003234978[229] = 0;
   out_6282295074003234978[230] = 0;
   out_6282295074003234978[231] = 0;
   out_6282295074003234978[232] = 0;
   out_6282295074003234978[233] = 0;
   out_6282295074003234978[234] = 0;
   out_6282295074003234978[235] = 0;
   out_6282295074003234978[236] = 0;
   out_6282295074003234978[237] = 0;
   out_6282295074003234978[238] = 0;
   out_6282295074003234978[239] = 0;
   out_6282295074003234978[240] = 0;
   out_6282295074003234978[241] = 0;
   out_6282295074003234978[242] = 0;
   out_6282295074003234978[243] = 0;
   out_6282295074003234978[244] = 0;
   out_6282295074003234978[245] = 0;
   out_6282295074003234978[246] = 0;
   out_6282295074003234978[247] = 1;
   out_6282295074003234978[248] = 0;
   out_6282295074003234978[249] = 0;
   out_6282295074003234978[250] = 0;
   out_6282295074003234978[251] = 0;
   out_6282295074003234978[252] = 0;
   out_6282295074003234978[253] = 0;
   out_6282295074003234978[254] = 0;
   out_6282295074003234978[255] = 0;
   out_6282295074003234978[256] = 0;
   out_6282295074003234978[257] = 0;
   out_6282295074003234978[258] = 0;
   out_6282295074003234978[259] = 0;
   out_6282295074003234978[260] = 0;
   out_6282295074003234978[261] = 0;
   out_6282295074003234978[262] = 0;
   out_6282295074003234978[263] = 0;
   out_6282295074003234978[264] = 0;
   out_6282295074003234978[265] = 0;
   out_6282295074003234978[266] = 1;
   out_6282295074003234978[267] = 0;
   out_6282295074003234978[268] = 0;
   out_6282295074003234978[269] = 0;
   out_6282295074003234978[270] = 0;
   out_6282295074003234978[271] = 0;
   out_6282295074003234978[272] = 0;
   out_6282295074003234978[273] = 0;
   out_6282295074003234978[274] = 0;
   out_6282295074003234978[275] = 0;
   out_6282295074003234978[276] = 0;
   out_6282295074003234978[277] = 0;
   out_6282295074003234978[278] = 0;
   out_6282295074003234978[279] = 0;
   out_6282295074003234978[280] = 0;
   out_6282295074003234978[281] = 0;
   out_6282295074003234978[282] = 0;
   out_6282295074003234978[283] = 0;
   out_6282295074003234978[284] = 0;
   out_6282295074003234978[285] = 1;
   out_6282295074003234978[286] = 0;
   out_6282295074003234978[287] = 0;
   out_6282295074003234978[288] = 0;
   out_6282295074003234978[289] = 0;
   out_6282295074003234978[290] = 0;
   out_6282295074003234978[291] = 0;
   out_6282295074003234978[292] = 0;
   out_6282295074003234978[293] = 0;
   out_6282295074003234978[294] = 0;
   out_6282295074003234978[295] = 0;
   out_6282295074003234978[296] = 0;
   out_6282295074003234978[297] = 0;
   out_6282295074003234978[298] = 0;
   out_6282295074003234978[299] = 0;
   out_6282295074003234978[300] = 0;
   out_6282295074003234978[301] = 0;
   out_6282295074003234978[302] = 0;
   out_6282295074003234978[303] = 0;
   out_6282295074003234978[304] = 1;
   out_6282295074003234978[305] = 0;
   out_6282295074003234978[306] = 0;
   out_6282295074003234978[307] = 0;
   out_6282295074003234978[308] = 0;
   out_6282295074003234978[309] = 0;
   out_6282295074003234978[310] = 0;
   out_6282295074003234978[311] = 0;
   out_6282295074003234978[312] = 0;
   out_6282295074003234978[313] = 0;
   out_6282295074003234978[314] = 0;
   out_6282295074003234978[315] = 0;
   out_6282295074003234978[316] = 0;
   out_6282295074003234978[317] = 0;
   out_6282295074003234978[318] = 0;
   out_6282295074003234978[319] = 0;
   out_6282295074003234978[320] = 0;
   out_6282295074003234978[321] = 0;
   out_6282295074003234978[322] = 0;
   out_6282295074003234978[323] = 1;
}
void h_4(double *state, double *unused, double *out_6113181109752145272) {
   out_6113181109752145272[0] = state[6] + state[9];
   out_6113181109752145272[1] = state[7] + state[10];
   out_6113181109752145272[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_4349044816078623433) {
   out_4349044816078623433[0] = 0;
   out_4349044816078623433[1] = 0;
   out_4349044816078623433[2] = 0;
   out_4349044816078623433[3] = 0;
   out_4349044816078623433[4] = 0;
   out_4349044816078623433[5] = 0;
   out_4349044816078623433[6] = 1;
   out_4349044816078623433[7] = 0;
   out_4349044816078623433[8] = 0;
   out_4349044816078623433[9] = 1;
   out_4349044816078623433[10] = 0;
   out_4349044816078623433[11] = 0;
   out_4349044816078623433[12] = 0;
   out_4349044816078623433[13] = 0;
   out_4349044816078623433[14] = 0;
   out_4349044816078623433[15] = 0;
   out_4349044816078623433[16] = 0;
   out_4349044816078623433[17] = 0;
   out_4349044816078623433[18] = 0;
   out_4349044816078623433[19] = 0;
   out_4349044816078623433[20] = 0;
   out_4349044816078623433[21] = 0;
   out_4349044816078623433[22] = 0;
   out_4349044816078623433[23] = 0;
   out_4349044816078623433[24] = 0;
   out_4349044816078623433[25] = 1;
   out_4349044816078623433[26] = 0;
   out_4349044816078623433[27] = 0;
   out_4349044816078623433[28] = 1;
   out_4349044816078623433[29] = 0;
   out_4349044816078623433[30] = 0;
   out_4349044816078623433[31] = 0;
   out_4349044816078623433[32] = 0;
   out_4349044816078623433[33] = 0;
   out_4349044816078623433[34] = 0;
   out_4349044816078623433[35] = 0;
   out_4349044816078623433[36] = 0;
   out_4349044816078623433[37] = 0;
   out_4349044816078623433[38] = 0;
   out_4349044816078623433[39] = 0;
   out_4349044816078623433[40] = 0;
   out_4349044816078623433[41] = 0;
   out_4349044816078623433[42] = 0;
   out_4349044816078623433[43] = 0;
   out_4349044816078623433[44] = 1;
   out_4349044816078623433[45] = 0;
   out_4349044816078623433[46] = 0;
   out_4349044816078623433[47] = 1;
   out_4349044816078623433[48] = 0;
   out_4349044816078623433[49] = 0;
   out_4349044816078623433[50] = 0;
   out_4349044816078623433[51] = 0;
   out_4349044816078623433[52] = 0;
   out_4349044816078623433[53] = 0;
}
void h_10(double *state, double *unused, double *out_8504819869403414406) {
   out_8504819869403414406[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_8504819869403414406[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_8504819869403414406[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_275795050531195524) {
   out_275795050531195524[0] = 0;
   out_275795050531195524[1] = 9.8100000000000005*cos(state[1]);
   out_275795050531195524[2] = 0;
   out_275795050531195524[3] = 0;
   out_275795050531195524[4] = -state[8];
   out_275795050531195524[5] = state[7];
   out_275795050531195524[6] = 0;
   out_275795050531195524[7] = state[5];
   out_275795050531195524[8] = -state[4];
   out_275795050531195524[9] = 0;
   out_275795050531195524[10] = 0;
   out_275795050531195524[11] = 0;
   out_275795050531195524[12] = 1;
   out_275795050531195524[13] = 0;
   out_275795050531195524[14] = 0;
   out_275795050531195524[15] = 1;
   out_275795050531195524[16] = 0;
   out_275795050531195524[17] = 0;
   out_275795050531195524[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_275795050531195524[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_275795050531195524[20] = 0;
   out_275795050531195524[21] = state[8];
   out_275795050531195524[22] = 0;
   out_275795050531195524[23] = -state[6];
   out_275795050531195524[24] = -state[5];
   out_275795050531195524[25] = 0;
   out_275795050531195524[26] = state[3];
   out_275795050531195524[27] = 0;
   out_275795050531195524[28] = 0;
   out_275795050531195524[29] = 0;
   out_275795050531195524[30] = 0;
   out_275795050531195524[31] = 1;
   out_275795050531195524[32] = 0;
   out_275795050531195524[33] = 0;
   out_275795050531195524[34] = 1;
   out_275795050531195524[35] = 0;
   out_275795050531195524[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_275795050531195524[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_275795050531195524[38] = 0;
   out_275795050531195524[39] = -state[7];
   out_275795050531195524[40] = state[6];
   out_275795050531195524[41] = 0;
   out_275795050531195524[42] = state[4];
   out_275795050531195524[43] = -state[3];
   out_275795050531195524[44] = 0;
   out_275795050531195524[45] = 0;
   out_275795050531195524[46] = 0;
   out_275795050531195524[47] = 0;
   out_275795050531195524[48] = 0;
   out_275795050531195524[49] = 0;
   out_275795050531195524[50] = 1;
   out_275795050531195524[51] = 0;
   out_275795050531195524[52] = 0;
   out_275795050531195524[53] = 1;
}
void h_13(double *state, double *unused, double *out_2405456616869928481) {
   out_2405456616869928481[0] = state[3];
   out_2405456616869928481[1] = state[4];
   out_2405456616869928481[2] = state[5];
}
void H_13(double *state, double *unused, double *out_3261586392238077496) {
   out_3261586392238077496[0] = 0;
   out_3261586392238077496[1] = 0;
   out_3261586392238077496[2] = 0;
   out_3261586392238077496[3] = 1;
   out_3261586392238077496[4] = 0;
   out_3261586392238077496[5] = 0;
   out_3261586392238077496[6] = 0;
   out_3261586392238077496[7] = 0;
   out_3261586392238077496[8] = 0;
   out_3261586392238077496[9] = 0;
   out_3261586392238077496[10] = 0;
   out_3261586392238077496[11] = 0;
   out_3261586392238077496[12] = 0;
   out_3261586392238077496[13] = 0;
   out_3261586392238077496[14] = 0;
   out_3261586392238077496[15] = 0;
   out_3261586392238077496[16] = 0;
   out_3261586392238077496[17] = 0;
   out_3261586392238077496[18] = 0;
   out_3261586392238077496[19] = 0;
   out_3261586392238077496[20] = 0;
   out_3261586392238077496[21] = 0;
   out_3261586392238077496[22] = 1;
   out_3261586392238077496[23] = 0;
   out_3261586392238077496[24] = 0;
   out_3261586392238077496[25] = 0;
   out_3261586392238077496[26] = 0;
   out_3261586392238077496[27] = 0;
   out_3261586392238077496[28] = 0;
   out_3261586392238077496[29] = 0;
   out_3261586392238077496[30] = 0;
   out_3261586392238077496[31] = 0;
   out_3261586392238077496[32] = 0;
   out_3261586392238077496[33] = 0;
   out_3261586392238077496[34] = 0;
   out_3261586392238077496[35] = 0;
   out_3261586392238077496[36] = 0;
   out_3261586392238077496[37] = 0;
   out_3261586392238077496[38] = 0;
   out_3261586392238077496[39] = 0;
   out_3261586392238077496[40] = 0;
   out_3261586392238077496[41] = 1;
   out_3261586392238077496[42] = 0;
   out_3261586392238077496[43] = 0;
   out_3261586392238077496[44] = 0;
   out_3261586392238077496[45] = 0;
   out_3261586392238077496[46] = 0;
   out_3261586392238077496[47] = 0;
   out_3261586392238077496[48] = 0;
   out_3261586392238077496[49] = 0;
   out_3261586392238077496[50] = 0;
   out_3261586392238077496[51] = 0;
   out_3261586392238077496[52] = 0;
   out_3261586392238077496[53] = 0;
}
void h_14(double *state, double *unused, double *out_1182614674018052430) {
   out_1182614674018052430[0] = state[6];
   out_1182614674018052430[1] = state[7];
   out_1182614674018052430[2] = state[8];
}
void H_14(double *state, double *unused, double *out_7431833248373995729) {
   out_7431833248373995729[0] = 0;
   out_7431833248373995729[1] = 0;
   out_7431833248373995729[2] = 0;
   out_7431833248373995729[3] = 0;
   out_7431833248373995729[4] = 0;
   out_7431833248373995729[5] = 0;
   out_7431833248373995729[6] = 1;
   out_7431833248373995729[7] = 0;
   out_7431833248373995729[8] = 0;
   out_7431833248373995729[9] = 0;
   out_7431833248373995729[10] = 0;
   out_7431833248373995729[11] = 0;
   out_7431833248373995729[12] = 0;
   out_7431833248373995729[13] = 0;
   out_7431833248373995729[14] = 0;
   out_7431833248373995729[15] = 0;
   out_7431833248373995729[16] = 0;
   out_7431833248373995729[17] = 0;
   out_7431833248373995729[18] = 0;
   out_7431833248373995729[19] = 0;
   out_7431833248373995729[20] = 0;
   out_7431833248373995729[21] = 0;
   out_7431833248373995729[22] = 0;
   out_7431833248373995729[23] = 0;
   out_7431833248373995729[24] = 0;
   out_7431833248373995729[25] = 1;
   out_7431833248373995729[26] = 0;
   out_7431833248373995729[27] = 0;
   out_7431833248373995729[28] = 0;
   out_7431833248373995729[29] = 0;
   out_7431833248373995729[30] = 0;
   out_7431833248373995729[31] = 0;
   out_7431833248373995729[32] = 0;
   out_7431833248373995729[33] = 0;
   out_7431833248373995729[34] = 0;
   out_7431833248373995729[35] = 0;
   out_7431833248373995729[36] = 0;
   out_7431833248373995729[37] = 0;
   out_7431833248373995729[38] = 0;
   out_7431833248373995729[39] = 0;
   out_7431833248373995729[40] = 0;
   out_7431833248373995729[41] = 0;
   out_7431833248373995729[42] = 0;
   out_7431833248373995729[43] = 0;
   out_7431833248373995729[44] = 1;
   out_7431833248373995729[45] = 0;
   out_7431833248373995729[46] = 0;
   out_7431833248373995729[47] = 0;
   out_7431833248373995729[48] = 0;
   out_7431833248373995729[49] = 0;
   out_7431833248373995729[50] = 0;
   out_7431833248373995729[51] = 0;
   out_7431833248373995729[52] = 0;
   out_7431833248373995729[53] = 0;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_5211353213920976706) {
  err_fun(nom_x, delta_x, out_5211353213920976706);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_9125851500026127750) {
  inv_err_fun(nom_x, true_x, out_9125851500026127750);
}
void pose_H_mod_fun(double *state, double *out_5103205818133336040) {
  H_mod_fun(state, out_5103205818133336040);
}
void pose_f_fun(double *state, double dt, double *out_8265616698399609189) {
  f_fun(state,  dt, out_8265616698399609189);
}
void pose_F_fun(double *state, double dt, double *out_6282295074003234978) {
  F_fun(state,  dt, out_6282295074003234978);
}
void pose_h_4(double *state, double *unused, double *out_6113181109752145272) {
  h_4(state, unused, out_6113181109752145272);
}
void pose_H_4(double *state, double *unused, double *out_4349044816078623433) {
  H_4(state, unused, out_4349044816078623433);
}
void pose_h_10(double *state, double *unused, double *out_8504819869403414406) {
  h_10(state, unused, out_8504819869403414406);
}
void pose_H_10(double *state, double *unused, double *out_275795050531195524) {
  H_10(state, unused, out_275795050531195524);
}
void pose_h_13(double *state, double *unused, double *out_2405456616869928481) {
  h_13(state, unused, out_2405456616869928481);
}
void pose_H_13(double *state, double *unused, double *out_3261586392238077496) {
  H_13(state, unused, out_3261586392238077496);
}
void pose_h_14(double *state, double *unused, double *out_1182614674018052430) {
  h_14(state, unused, out_1182614674018052430);
}
void pose_H_14(double *state, double *unused, double *out_7431833248373995729) {
  H_14(state, unused, out_7431833248373995729);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
