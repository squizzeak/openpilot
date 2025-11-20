#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_5211353213920976706);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_9125851500026127750);
void pose_H_mod_fun(double *state, double *out_5103205818133336040);
void pose_f_fun(double *state, double dt, double *out_8265616698399609189);
void pose_F_fun(double *state, double dt, double *out_6282295074003234978);
void pose_h_4(double *state, double *unused, double *out_6113181109752145272);
void pose_H_4(double *state, double *unused, double *out_4349044816078623433);
void pose_h_10(double *state, double *unused, double *out_8504819869403414406);
void pose_H_10(double *state, double *unused, double *out_275795050531195524);
void pose_h_13(double *state, double *unused, double *out_2405456616869928481);
void pose_H_13(double *state, double *unused, double *out_3261586392238077496);
void pose_h_14(double *state, double *unused, double *out_1182614674018052430);
void pose_H_14(double *state, double *unused, double *out_7431833248373995729);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}