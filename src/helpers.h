#ifndef S21_DECIMAL_HELPERS_H
#define S21_DECIMAL_HELPERS_H
#include "s21_decimal.h"

int s21_divide_core(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *quotient, uint32_t *initial_scale,
                    int *fractional_scale);
void s21_adjust_scale_div(s21_decimal *quotient, uint32_t initial_scale,
                          int fractional_scale, s21_decimal value_1,
                          s21_decimal value_2);
void s21_decrease_scale(s21_decimal *value);
bool s21_is_null(s21_decimal value);
int s21_divide_by_10(uint64_t temp[6]);
void s21_add_one(uint64_t temp[6]);
void s21_compute_frac_sum(s21_decimal value_1, s21_decimal value_2, int *error,
                          s21_decimal *int_frac_part,
                          s21_decimal *frac_remainder);
void s21_adjust_main_by_frac(s21_decimal *value, s21_decimal int_frac_part,
                             int *error);
void s21_sum_integer_parts(s21_decimal value_1, s21_decimal value_2, int *error,
                           s21_decimal *int_sum);
void s21_apply_bank_rounding(s21_decimal *result, s21_decimal frac_remainder,
                             int *error);
void s21_add_with_rounding(s21_decimal value_1, s21_decimal value_2, int *error,
                           s21_decimal *result);
s21_decimal s21_get_fr_part(s21_decimal value);
void s21_div_bank_round(s21_decimal *quotient, s21_decimal *fractional,
                        int *fractional_scale, s21_decimal divisor,
                        s21_decimal remainder);
void s21_add_with_equal_signs(int *error, s21_decimal *result,
                              s21_decimal value_1, s21_decimal value_2);
void s21_add_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2);
void s21_sub_with_equal_signs(s21_decimal *result, s21_decimal value_1,
                              s21_decimal value_2);
void s21_sub_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2);
int s21_get_bit(s21_decimal value, int index);
void s21_set_bit(s21_decimal *value, int index, int bit);
void s21_ostatok(s21_decimal value, s21_decimal *result);
int s21_div_support(s21_decimal *remainder, s21_decimal divisible,
                    s21_decimal divisor, s21_decimal *quotient);
void s21_shift_left(s21_decimal *value);
void s21_equalize_scales(s21_decimal *value_1, s21_decimal *value_2,
                         int *error);
uint32_t s21_get_sign(s21_decimal value);
uint32_t s21_get_scale(s21_decimal value);
int s21_multiply_by_10(s21_decimal *value);
void s21_decimal_zero(s21_decimal *dst);
void s21_set_scale(s21_decimal *decimal, uint32_t scale);
void s21_set_sign(s21_decimal *dst, uint32_t sign);
void s21_dec_assignment(s21_decimal value, s21_decimal *result);
bool s21_is_valid_decimal(s21_decimal value);
bool s21_are_all_bits_zero(s21_decimal value);
s21_decimal s21_trim_trailing_zeros(s21_decimal value);
void s21_div_by_10(uint32_t power, s21_decimal value, s21_decimal *result);
#endif
