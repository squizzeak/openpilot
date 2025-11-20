#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_8092898112889943750);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_7267932490850398471);
void car_H_mod_fun(double *state, double *out_3996584369783644757);
void car_f_fun(double *state, double dt, double *out_2186743974440384671);
void car_F_fun(double *state, double dt, double *out_4137853718826387970);
void car_h_25(double *state, double *unused, double *out_7299825330701741714);
void car_H_25(double *state, double *unused, double *out_5196603856128592480);
void car_h_24(double *state, double *unused, double *out_2087696878449228672);
void car_H_24(double *state, double *unused, double *out_2970896072149723918);
void car_h_30(double *state, double *unused, double *out_7546455252693747336);
void car_H_30(double *state, double *unused, double *out_1720086485363024275);
void car_h_26(double *state, double *unused, double *out_817473480936614770);
void car_H_26(double *state, double *unused, double *out_1892077886367791879);
void car_h_27(double *state, double *unused, double *out_7745495375638094851);
void car_H_27(double *state, double *unused, double *out_454676826437400636);
void car_h_29(double *state, double *unused, double *out_1563531413387605583);
void car_H_29(double *state, double *unused, double *out_2230317829677416459);
void car_h_28(double *state, double *unused, double *out_5714425352689966085);
void car_H_28(double *state, double *unused, double *out_2852081187392114115);
void car_h_31(double *state, double *unused, double *out_1837891953982109468);
void car_H_31(double *state, double *unused, double *out_5165957894251632052);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}