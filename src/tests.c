#include <check.h>
#include <stdlib.h>

#include "helpers.c"
#include "s21_decimal.c"
#include "s21_decimal.h"

START_TEST(test_s21_add_basic) {
  s21_decimal a = {{3333333330, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3333333333);
}
END_TEST

START_TEST(test_s21_add_with_diff_signs) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{4, 0, 0, 0}};
  s21_decimal b = {{70, 0, 0, sign_bit}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 66);
  ck_assert_int_eq(result.bits[3], sign_bit);
}
END_TEST

START_TEST(test_s21_add_positive) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_add_negative) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{5, 0, 0, sign_bit}};
  s21_decimal b = {{10, 0, 0, sign_bit}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 15);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_add_opposite_numbers) {
  s21_decimal a = {{123456789, 0, 0, 0}};
  s21_decimal b = {{123456789, 0, 0, 0x80000000}};
  s21_decimal result = {{1, 1, 1, 1}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_add_with_zero) {
  s21_decimal a = {{42, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 42);
}
END_TEST

START_TEST(test_s21_add_negative_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0x80000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_add_complex_case) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0x80000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_sub_basic) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 2);
}
END_TEST

START_TEST(test_s21_sub_underflow) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, sign_bit}};
  s21_decimal b = {{1, 0, 0, 1}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_sub_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 2);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_sub_with_diff_signs_basic) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, sign_bit}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 8);
}
END_TEST

START_TEST(test_s21_sub_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFE);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
}
END_TEST

START_TEST(test_s21_sub_same_numbers) {
  s21_decimal a = {{123456789, 0, 0, 0}};
  s21_decimal b = {{123456789, 0, 0, 0}};
  s21_decimal result = {{1, 1, 1, 1}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_s21_sub_negative_zero) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0x80000000}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_s21_sub_overflow_negative) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_s21_sub_carry_propagation) {
  s21_decimal a = {{0, 0, 1, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_same_scale) {
  s21_decimal value_1 = {{123, 0, 0, (2 << 16)}};
  s21_decimal value_2 = {{456, 0, 0, (2 << 16)}};
  int error = 0;

  s21_equalize_scales(&value_1, &value_2, &error);

  ck_assert_int_eq(s21_get_scale(value_1), 2);
  ck_assert_int_eq(s21_get_scale(value_2), 2);
  ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_increase_scale_1) {
  s21_decimal value_1 = {{123, 0, 0, (2 << 16)}};
  s21_decimal value_2 = {{456, 0, 0, (3 << 16)}};
  int error = 0;

  s21_equalize_scales(&value_1, &value_2, &error);

  ck_assert_int_eq(s21_get_scale(value_1), 3);
  ck_assert_int_eq(s21_get_scale(value_2), 3);
  ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_increase_scale_2) {
  s21_decimal value_1 = {{123, 0, 0, (3 << 16)}};
  s21_decimal value_2 = {{456, 0, 0, (2 << 16)}};
  int error = 0;

  s21_equalize_scales(&value_1, &value_2, &error);

  ck_assert_int_eq(s21_get_scale(value_1), 3);
  ck_assert_int_eq(s21_get_scale(value_2), 3);
  ck_assert_int_eq(error, 0);
}
END_TEST

START_TEST(test_s21_equalize_scales_multiply_overflow) {
  s21_decimal value_1 = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, (2 << 16)}};
  s21_decimal value_2 = {{456, 0, 0, (3 << 16)}};
  int error = 0;

  s21_equalize_scales(&value_1, &value_2, &error);

  ck_assert_int_eq(error, 1);
}
END_TEST

START_TEST(test_s21_shift_left_basic) {
  s21_decimal value = {{1, 0, 0, 0}};
  s21_shift_left(&value);
  ck_assert_int_eq(value.bits[0], 2);
  ck_assert_int_eq(value.bits[1], 0);
  ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_s21_shift_left_carry) {
  s21_decimal value = {{0x80000000, 0, 0, 0}};
  s21_shift_left(&value);
  ck_assert_int_eq(value.bits[0], 0);
  ck_assert_int_eq(value.bits[1], 1);
  ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_s21_shift_left_multi_bit) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_shift_left(&value);
  ck_assert_int_eq(value.bits[0], 0xFFFFFFFE);
  ck_assert_int_eq(value.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(value.bits[2], 1);
}
END_TEST

START_TEST(test_s21_shift_left_top_bit) {
  s21_decimal value = {{0, 0, 0x80000000, 0}};
  s21_shift_left(&value);
  ck_assert_int_eq(value.bits[0], 0);
  ck_assert_int_eq(value.bits[1], 0);
  ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_s21_shift_left_zero) {
  s21_decimal value = {{0, 0, 0, 0}};
  s21_shift_left(&value);
  ck_assert_int_eq(value.bits[0], 0);
  ck_assert_int_eq(value.bits[1], 0);
  ck_assert_int_eq(value.bits[2], 0);
}
END_TEST

START_TEST(test_div_support_basic) {
  s21_decimal remainder = {{0, 0, 0, 0}};
  s21_decimal divisible = {{100, 0, 0, 0}};
  s21_decimal divisor = {{10, 0, 0, 0}};
  s21_decimal quotient = {{0, 0, 0, 0}};

  int res = s21_div_support(&remainder, divisible, divisor, &quotient);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(quotient.bits[0], 10);
  ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_with_remainder) {
  s21_decimal remainder = {{0, 0, 0, 0}};
  s21_decimal divisible = {{105, 0, 0, 0}};
  s21_decimal divisor = {{10, 0, 0, 0}};
  s21_decimal quotient = {{0, 0, 0, 0}};

  int res = s21_div_support(&remainder, divisible, divisor, &quotient);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(quotient.bits[0], 10);
  ck_assert_int_eq(remainder.bits[0], 5);
}
END_TEST

START_TEST(test_div_support_large_number) {
  s21_decimal remainder = {{0, 0, 0, 0}};
  s21_decimal divisible = {{1000000, 0, 0, 0}};
  s21_decimal divisor = {{1000, 0, 0, 0}};
  s21_decimal quotient = {{0, 0, 0, 0}};

  int res = s21_div_support(&remainder, divisible, divisor, &quotient);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(quotient.bits[0], 1000);
  ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_zero_divisible) {
  s21_decimal remainder = {{0, 0, 0, 0}};
  s21_decimal divisible = {{0, 0, 0, 0}};
  s21_decimal divisor = {{123, 0, 0, 0}};
  s21_decimal quotient = {{123, 0, 0, 0}};

  int res = s21_div_support(&remainder, divisible, divisor, &quotient);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(quotient.bits[0], 0);
  ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_div_support_divide_by_one) {
  s21_decimal remainder = {{0, 0, 0, 0}};
  s21_decimal divisible = {{98765, 0, 0, 0}};
  s21_decimal divisor = {{1, 0, 0, 0}};
  s21_decimal quotient = {{0, 0, 0, 0}};

  int res = s21_div_support(&remainder, divisible, divisor, &quotient);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(quotient.bits[0], 98765);
  ck_assert_int_eq(remainder.bits[0], 0);
}
END_TEST

START_TEST(test_s21_div_by_10_basic) {
  s21_decimal a = {{1000, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  s21_div_by_10(1, a, &result);

  ck_assert_int_eq(result.bits[0], 100);
}
END_TEST

START_TEST(test_s21_div_by_10_multiple) {
  s21_decimal a = {{1000, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  s21_div_by_10(2, a, &result);

  ck_assert_int_eq(result.bits[0], 10);
}
END_TEST

START_TEST(test_s21_div_by_10_rounding) {
  s21_decimal a = {{105, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  s21_div_by_10(1, a, &result);

  ck_assert_int_eq(result.bits[0], 10);
}
END_TEST

START_TEST(test_s21_div_by_10_negative) {
  s21_decimal a = {{100, 0, 0, 0x80000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  s21_div_by_10(1, a, &result);
  ck_assert_int_eq(result.bits[0], 10);
  ck_assert_int_eq(result.bits[3], 0x80000000 | (uint32_t)1 << 16);
}
END_TEST

START_TEST(test_s21_div_by_10_zero) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal result = {{123, 0, 0, 0}};

  s21_div_by_10(3, a, &result);

  ck_assert_int_eq(result.bits[0], 0);
}
END_TEST

START_TEST(test_s21_div_by_10_large_number) {
  s21_decimal a = {{1000000, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  s21_div_by_10(3, a, &result);

  ck_assert_int_eq(result.bits[0], 1000);
}
END_TEST

START_TEST(test_s21_add_overflow) {
  s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  s21_decimal b = {{UINT32_MAX, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_mul_basic) {
  s21_decimal a = {{2, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_mul(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 6);
}
END_TEST

START_TEST(test_s21_mul_overflow) {
  s21_decimal a = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_mul(a, b, &result);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_mul_negative_result) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{2, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, sign_bit}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_mul(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 6);
  ck_assert_int_eq((uint32_t)result.bits[3] & sign_bit, sign_bit);
}
END_TEST

START_TEST(test_s21_div_basic) {
  s21_decimal a = {{6, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_div_by_zero) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 3);
}
END_TEST

START_TEST(test_s21_div_negative_result) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{6, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, sign_bit}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
  ck_assert_int_eq((uint32_t)result.bits[3] & sign_bit, sign_bit);
}
END_TEST

START_TEST(test_s21_is_less_basic) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};

  int res = s21_is_less(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_less_equal) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{1, 0, 0, 0}};

  int res = s21_is_less_or_equal(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_greater) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};

  int res = s21_is_greater(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_greater_or_equal) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};

  int res = s21_is_greater_or_equal(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_equal) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{5, 0, 0, 0}};

  int res = s21_is_equal(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_is_not_equal) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};

  int res = s21_is_not_equal(a, b);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_floor) {
  s21_decimal a = {{314, 0, 0, (2 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_floor(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_floor_negative) {
  s21_decimal a = {{314, 0, 0, (2 << 16)}};
  s21_set_sign(&a, 1);
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_floor(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 4);
  uint32_t sign_bit = (uint32_t)1 << 31;
  ck_assert_int_eq(result.bits[3], sign_bit);
}
END_TEST

START_TEST(test_s21_round) {
  s21_decimal a = {{314, 0, 0, (2 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_round(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_round_to_higher) {
  s21_decimal a = {{367, 0, 0, (2 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_round(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 4);
}
END_TEST

START_TEST(test_s21_round_for_negative) {
  s21_decimal a = {{367, 0, 0, (2 << 16)}};
  s21_set_sign(&a, 1);
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_round(a, &result);
  uint32_t sign_bit = (uint32_t)1 << 31;
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 4);
  ck_assert_int_eq(result.bits[3], sign_bit);
}
END_TEST

START_TEST(test_s21_truncate) {
  s21_decimal a = {{314, 0, 0, (2 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_truncate(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 3);
}
END_TEST

START_TEST(test_s21_negate) {
  uint32_t sign_bit = (uint32_t)1 << 31;
  s21_decimal a = {{314, 0, 0, (3 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_negate(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 314);
  ck_assert_int_eq((uint32_t)result.bits[3] & sign_bit, sign_bit);
}
END_TEST

START_TEST(test_s21_from_int_to_decimal) {
  int a = 123456789;
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_from_int_to_decimal(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 123456789);
}
END_TEST

START_TEST(test_s21_from_negative_int_to_decimal) {
  int a = -123456789;
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_from_int_to_decimal(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 123456789);
  ck_assert_int_eq(result.bits[3] >> 31, 1);
}
END_TEST

START_TEST(test_s21_from_float_to_decimal) {
  float a = 123.456f;
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_from_float_to_decimal(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 123456);
  ck_assert_int_eq(result.bits[3] >> 16, 3);
}
END_TEST

START_TEST(test_s21_from_decimal_to_int) {
  s21_decimal a = {{123456789, 0, 0, 0}};
  int result = 0;

  int res = s21_from_decimal_to_int(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result, 123456789);
}
END_TEST

START_TEST(test_s21_from_fractional_decimal_to_int) {
  s21_decimal a = {{123456789, 0, 0, 5 << 16}};
  s21_set_sign(&a, 1);
  int result = 0;

  int res = s21_from_decimal_to_int(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result, -1234);
}
END_TEST

START_TEST(test_s21_from_decimal_to_float) {
  s21_decimal a = {{123456789, 0, 0, 0}};
  float result = 0.0f;

  int res = s21_from_decimal_to_float(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_float_eq(result, 123456789.0f);
}
END_TEST

START_TEST(test_s21_from_fractional_decimal_to_float) {
  s21_decimal a = {{123456789, 0, 0, 5 << 16}};
  s21_set_sign(&a, 1);
  float result = 0.0f;

  int res = s21_from_decimal_to_float(a, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_float_eq(result, -1234.56789f);
}
END_TEST

START_TEST(test_s21_div_fraction) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 5);
  ck_assert_int_eq(s21_get_scale(result), 1);
}
END_TEST

START_TEST(test_s21_div_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0x00000000);
  ck_assert_int_eq(result.bits[1], 0x00000000);
  ck_assert_int_eq(result.bits[2], 0x80000000);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_div_negative) {
  s21_decimal a = {{100, 0, 0, 0x80000000}};
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 10);
  ck_assert_int_eq(result.bits[3], 0x80000000);
}
END_TEST

START_TEST(test_s21_div_two_negatives) {
  s21_decimal a = {{100, 0, 0, 0x80000000}};
  s21_decimal b = {{10, 0, 0, 0x80000000}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 10);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_div_scale_adjustment) {
  s21_decimal a = {{12345, 0, 0, (3 << 16)}};
  s21_decimal b = {{100, 0, 0, (2 << 16)}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 12345);
  ck_assert_int_eq(s21_get_scale(result), 3);
}
END_TEST

START_TEST(test_s21_multiply_by_10_basic) {
  s21_decimal value = {{5, 0, 0, 0}};
  int res = s21_multiply_by_10(&value);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(value.bits[0], 50);
}
END_TEST

START_TEST(test_s21_multiply_by_10_large_number) {
  s21_decimal value = {{0x20000000, 0, 0, 0}};
  int res = s21_multiply_by_10(&value);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(value.bits[0], 0x40000000);
}
END_TEST

START_TEST(test_s21_multiply_by_10_overflow) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  int res = s21_multiply_by_10(&value);
  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_multiply_by_10_middle_overflow) {
  s21_decimal value = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  int res = s21_multiply_by_10(&value);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(value.bits[2], 9);
}
END_TEST

START_TEST(test_s21_mul_carry_propagation) {
  s21_decimal a = {{0, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{0, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_mul(a, b, &result);

  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_bank_rounding_1) {
  s21_decimal a = {{2, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};

  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0x0AAAAAAB);
  ck_assert_int_eq(result.bits[1], 0x296E0196);
  ck_assert_int_eq(result.bits[2], 0x158A8994);
  ck_assert_int_eq(s21_get_scale(result), 28);
}
END_TEST

START_TEST(test_s21_div_large_result_with_fraction) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0x55555555);
  ck_assert_int_eq(result.bits[1], 0x55555555);
  ck_assert_int_eq(result.bits[2], 0x55555555);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_div_infinite_fraction) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_div(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0x05555555);
  ck_assert_int_eq(result.bits[1], 0x14B700CB);
  ck_assert_int_eq(result.bits[2], 0xAC544CA);
  ck_assert_int_eq(result.bits[3], (28 << 16));
}
END_TEST

START_TEST(test_s21_div_two_bits) {
  s21_decimal a = {{2, 2, 2, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_div(a, b, &result);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq(result.bits[1], 1);
  ck_assert_int_eq(result.bits[2], 1);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_sub_three_bits) {
  s21_decimal a = {{0, 0, 1, 0}};
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_sub(a, b, &result);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 1);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_bank_rounding_sum) {
  s21_decimal a = {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{5, 0, 0, 1 << 16}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFE);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 0);
}

START_TEST(test_add_rounding_max) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{5, 0, 0, 1 << 16}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);
  ck_assert_int_eq(res, 1);
}

START_TEST(test_add_rounding_min) {
  uint32_t sign = (uint32_t)1 << 31;
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, sign}};
  s21_decimal b = {{5, 0, 0, sign | scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);
  ck_assert_int_eq(res, 2);
}

START_TEST(test_add_rounding_with_extra_one) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{9, 0, 0, scale}};
  s21_decimal b = {{9, 0, 0, scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 18);
  ck_assert_int_eq(result.bits[1], 0);
  ck_assert_int_eq(result.bits[2], 0);
  ck_assert_int_eq(result.bits[3], scale);
}

START_TEST(test_s21_add_banking_1) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{6, 0, 0, 0x80000000 | scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFE);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_add_banking_2) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{2, 0, 0, 0x80000000 | scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST
START_TEST(test_s21_add_banking_3) {
  uint32_t scale = (uint32_t)2 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{1, 0, 0, scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 0x80000000);
}
END_TEST

START_TEST(test_s21_add_banking_4) {
  uint32_t scale = (uint32_t)28 << 16;
  s21_decimal a = {{5, 0, 0, scale}};
  s21_decimal b = {{0xFFFFFFF0, 0xFFFFFFFF, 0xFFFFFFFF, 28 << 16}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_add(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFF5);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], scale);
}
END_TEST

START_TEST(test_s21_sub_banking_1) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{6, 0, 0, 0x80000000 | scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_sub(a, b, &result);
  ck_assert_int_eq(res, 1);
}
END_TEST

START_TEST(test_s21_sub_banking_2) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{2, 0, 0, scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(result.bits[0], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[1], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[2], 0xFFFFFFFF);
  ck_assert_int_eq(result.bits[3], 0);
}
END_TEST

START_TEST(test_s21_sub_banking_3) {
  uint32_t scale = (uint32_t)1 << 16;
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80000000}};
  s21_decimal b = {{5, 0, 0, scale}};
  s21_decimal result = {{0, 0, 0, 0}};
  int res = s21_sub(a, b, &result);

  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(div_8) {
  s21_decimal x = {{11}};
  s21_set_scale(&x, 28);
  s21_decimal y = {{2}};
  s21_decimal result = {{0}};
  s21_decimal correct = {{6}};
  s21_set_scale(&correct, 28);
  s21_div(x, y, &result);
  ck_assert_int_eq(result.bits[0], correct.bits[0]);
  ck_assert_int_eq(result.bits[1], correct.bits[1]);
  ck_assert_int_eq(result.bits[2], correct.bits[2]);
  ck_assert_int_eq(result.bits[3], correct.bits[3]);
}
END_TEST

START_TEST(div_10) {
  s21_decimal x = {{10}};
  s21_set_sign(&x, 1);
  s21_decimal y = {{1}};
  s21_set_scale(&y, 28);
  s21_decimal result = {{0}};
  ck_assert_int_eq(s21_div(x, y, &result), 2);
}
END_TEST

START_TEST(round_6) {
  s21_decimal a = {{0xffffffff, 0xffffffff, 0xffffffff, 0}};
  s21_decimal b;
  s21_set_scale(&a, 28);
  ck_assert_int_eq(s21_round(a, &b), 0);
}
END_TEST

START_TEST(simple_truncate) {
  s21_decimal dec = {{0, 0, 0, 0}};
  dec.bits[0] = 178532910;
  dec.bits[1] = 4081246736;
  dec.bits[2] = 2119837296;
  dec.bits[3] = 2149253120;

  s21_decimal ans = {{0, 0, 0, 0}};
  s21_decimal true_ans = {{0, 0, 0, 0}};
  true_ans.bits[0] = 39;
  s21_set_sign(&true_ans, 1);

  int status = s21_truncate(dec, &ans);
  int true_status = 0;

  ck_assert_int_eq(1, s21_is_equal(ans, true_ans));
  ck_assert_int_eq(status, true_status);
}
END_TEST

START_TEST(sub_4) {
  s21_decimal x = {{3, 0, 0, 0x80000000}};
  s21_decimal y = {{2, 0, 0, 0x80000000}};
  s21_decimal z = {{0, 0, 0, 0}};
  s21_decimal res = {{1, 0, 0, 0x80000000}};
  ck_assert_int_eq(s21_sub(x, y, &z), 0);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(z.bits[i], res.bits[i]);
  }
}
END_TEST

START_TEST(sub_5) {
  s21_decimal x = {{3, 0, 0, 0}};
  s21_decimal y = {{2, 0, 0, 0x80000000}};
  s21_decimal z = {{0, 0, 0, 0}};
  s21_decimal res = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_sub(x, y, &z), 0);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(z.bits[i], res.bits[i]);
  }
}
END_TEST

START_TEST(sub_6) {
  s21_decimal x = {{2, 0, 0, 0}};
  s21_decimal y = {{0xfffffffe, 0xffffffff, 0xffffffff, 0x80000000}};
  s21_decimal z = {{0, 0, 0, 0}};
  s21_decimal res = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_sub(x, y, &z), 1);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(z.bits[i], res.bits[i]);
  }
}
END_TEST

START_TEST(sub_7) {
  s21_decimal x = {{2, 0, 0, 0x80000000}};
  s21_decimal y = {{0xfffffffe, 0xffffffff, 0xffffffff, 0}};
  s21_decimal z = {{0, 0, 0, 0}};
  ck_assert_int_eq(s21_sub(x, y, &z), 2);
}
END_TEST

START_TEST(sub_8) {
  s21_decimal x = {{0x00000037, 0x00000000, 0x00000000, 0x001C0000}};
  s21_decimal y = {{0x00000002, 0x00000000, 0x00000000, 0}};
  s21_decimal z = {{0, 0, 0, 0}};
  s21_decimal res = {{0x1FFFFFC9, 0x7C4A04C2, 0x409F9CBC, 0x801C0000}};
  ck_assert_int_eq(s21_sub(x, y, &z), 0);
  for (int i = 3; i >= 0; i--) {
    ck_assert_int_eq(z.bits[i], res.bits[i]);
  }
}
END_TEST

START_TEST(sub_10) {
  s21_decimal x = {{0x00000037, 0x00000000, 0x00000000, 0x001C0000}};
  s21_decimal y = {{2, 0, 0, 0x80000000}};
  s21_decimal z = {{0, 0, 0, 0}};
  s21_decimal res = {{0x020000037, 0x7C4A04C2, 0x409F9CBC, 0x001C0000}};
  ck_assert_int_eq(s21_sub(x, y, &z), 0);
  for (int i = 0; i < 4; i++) {
    ck_assert_int_eq(z.bits[i], res.bits[i]);
  }
}
END_TEST

START_TEST(sub_21) {
  s21_decimal dec1;
  s21_decimal dec2;
  int tmp1 = -100;
  int tmp2 = -99999;
  int res_s21 = 0;
  s21_decimal res1;
  int res = 99899;
  s21_from_int_to_decimal(tmp1, &dec1);
  s21_from_int_to_decimal(tmp2, &dec2);
  s21_sub(dec1, dec2, &res1);
  s21_from_decimal_to_int(res1, &res_s21);
  ck_assert_int_eq(res_s21, res);
}
END_TEST

START_TEST(sub_18) {
  s21_decimal val1 = {{4, 0, 0, 0}};
  s21_decimal val2 = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, ~(UINT32_MAX / 2)}};
  s21_decimal res;
  ck_assert_int_eq(1, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_14) {
  s21_decimal val1 = {{4, 0, 0, ~(UINT32_MAX / 2)}};
  s21_decimal val2 = {{UINT32_MAX, UINT32_MAX, UINT32_MAX, 0}};
  s21_decimal res;
  ck_assert_int_eq(2, s21_sub(val1, val2, &res));
}
END_TEST

START_TEST(sub_int_1) {
  s21_decimal decimal_1 = {{20, 0, 0, 0}};
  s21_decimal decimal_2 = {{17, 0, 0, 0}};
  s21_set_sign(&decimal_2, 1);
  s21_decimal result = {{0, 0, 0, 0}};
  int state = s21_sub(decimal_1, decimal_2, &result);
  ck_assert_int_eq(state, 0);
  int dst = 0;
  s21_from_decimal_to_int(result, &dst);
  ck_assert_int_eq(dst, 37);
}
END_TEST

START_TEST(sub_int_3) {
  s21_decimal decimal_1 = {{10, 0, 0, 0}};
  s21_decimal decimal_2 = {{15, 0, 0, 0}};
  s21_set_sign(&decimal_1, 1);
  s21_set_sign(&decimal_2, 1);
  s21_decimal result = {{0, 0, 0, 0}};
  int state = s21_sub(decimal_1, decimal_2, &result);
  ck_assert_int_eq(state, 0);
  int dst = 0;
  s21_from_decimal_to_int(result, &dst);
  ck_assert_int_eq(dst, (-10 - (-15)));
}
END_TEST

START_TEST(sub_float_1) {
  s21_decimal decimal_1 = {{132, 0, 0, 0}};
  s21_set_scale(&decimal_1, 1);
  s21_decimal decimal_2 = {{10, 0, 0, 0}};
  s21_decimal result = {{0, 0, 0, 0}};
  int state = s21_sub(decimal_1, decimal_2, &result);
  ck_assert_int_eq(state, 0);
  float dst = 0;
  s21_from_decimal_to_float(result, &dst);
  ck_assert_float_eq(dst, (13.2 - 10));
}
END_TEST

START_TEST(sub_float_3) {
  s21_decimal decimal_1 = {{93493321, 0, 0, 0}};
  s21_set_scale(&decimal_1, 2);
  s21_decimal decimal_2 = {{12349321, 0, 0, 0}};
  s21_set_scale(&decimal_2, 4);
  s21_decimal result = {{0, 0, 0, 0}};
  int state = s21_sub(decimal_1, decimal_2, &result);
  ck_assert_int_eq(state, 0);
  float dst = 0;
  s21_from_decimal_to_float(result, &dst);
  ck_assert_float_eq(dst, (934933.21 - 1234.9321));
}
END_TEST

START_TEST(sub_float_5) {
  s21_decimal decimal_1 = {{93493321, 0, 0, 0}};
  s21_set_sign(&decimal_1, 1);
  s21_set_scale(&decimal_1, 2);
  s21_decimal decimal_2 = {{12349321, 0, 0, 0}};
  s21_set_sign(&decimal_2, 1);
  s21_set_scale(&decimal_2, 4);
  s21_decimal result = {{0, 0, 0, 0}};
  int state = s21_sub(decimal_1, decimal_2, &result);
  ck_assert_int_eq(state, 0);
  float dst = 0;
  s21_from_decimal_to_float(result, &dst);
  ck_assert_float_eq(dst, (-934933.21 + 1234.9321));
}
END_TEST

START_TEST(test_s21_sub_7) {
  s21_decimal src1, src2, result;
  src1.bits[3] = 0b00000000000000010000000000000000;
  src1.bits[2] = 0;
  src1.bits[1] = 0;
  src1.bits[0] = 0b00000000000000000110011011110000;
  src2.bits[3] = 0b00000000000000110000000000000000;
  src2.bits[2] = 0;
  src2.bits[1] = 0;
  src2.bits[0] = 0b00000000000001000101000111010110;
  s21_decimal result_origin;
  result_origin.bits[3] = 0b00000000000000110000000000000000;
  result_origin.bits[2] = 0;
  result_origin.bits[1] = 0;
  result_origin.bits[0] = 0b00000000001000111110001111101010;
  s21_sub(src1, src2, &result);
  ck_assert_int_eq(result_origin.bits[3], result.bits[3]);
  ck_assert_int_eq(result_origin.bits[2], result.bits[2]);
  ck_assert_int_eq(result_origin.bits[1], result.bits[1]);
  ck_assert_int_eq(result_origin.bits[0], result.bits[0]);
}
END_TEST

Suite *s21_decimal_suite(void) {
  Suite *s = suite_create("s21_decimal");
  TCase *tc_core = tcase_create("Core");

  tcase_add_test(tc_core, sub_14);
  tcase_add_test(tc_core, sub_18);
  tcase_add_test(tc_core, sub_21);
  tcase_add_test(tc_core, sub_4);
  tcase_add_test(tc_core, sub_5);
  tcase_add_test(tc_core, sub_6);
  tcase_add_test(tc_core, sub_7);
  tcase_add_test(tc_core, sub_8);
  tcase_add_test(tc_core, sub_10);
  tcase_add_test(tc_core, round_6);
  tcase_add_test(tc_core, div_8);
  tcase_add_test(tc_core, div_10);
  tcase_add_test(tc_core, simple_truncate);
  tcase_add_test(tc_core, sub_int_1);
  tcase_add_test(tc_core, sub_int_3);
  tcase_add_test(tc_core, sub_float_1);
  tcase_add_test(tc_core, sub_float_3);
  tcase_add_test(tc_core, sub_float_5);
  tcase_add_test(tc_core, test_s21_sub_7);
  tcase_add_test(tc_core, test_s21_sub_banking_1);
  tcase_add_test(tc_core, test_s21_sub_banking_2);
  tcase_add_test(tc_core, test_s21_sub_banking_3);
  tcase_add_test(tc_core, test_add_rounding_max);
  tcase_add_test(tc_core, test_add_rounding_min);
  tcase_add_test(tc_core, test_add_rounding_with_extra_one);
  tcase_add_test(tc_core, test_s21_add_banking_1);
  tcase_add_test(tc_core, test_s21_add_banking_2);
  tcase_add_test(tc_core, test_s21_add_banking_3);
  tcase_add_test(tc_core, test_s21_add_banking_4);
  tcase_add_test(tc_core, test_s21_mul_carry_propagation);
  tcase_add_test(tc_core, test_s21_equalize_scales_same_scale);
  tcase_add_test(tc_core, test_s21_equalize_scales_increase_scale_1);
  tcase_add_test(tc_core, test_s21_equalize_scales_increase_scale_2);
  tcase_add_test(tc_core, test_s21_equalize_scales_multiply_overflow);

  tcase_add_test(tc_core, test_s21_shift_left_zero);
  tcase_add_test(tc_core, test_s21_shift_left_basic);
  tcase_add_test(tc_core, test_s21_shift_left_carry);
  tcase_add_test(tc_core, test_s21_shift_left_multi_bit);
  tcase_add_test(tc_core, test_s21_shift_left_top_bit);

  tcase_add_test(tc_core, test_div_support_basic);
  tcase_add_test(tc_core, test_div_support_with_remainder);
  tcase_add_test(tc_core, test_div_support_large_number);
  tcase_add_test(tc_core, test_div_support_zero_divisible);
  tcase_add_test(tc_core, test_div_support_divide_by_one);
  tcase_add_test(tc_core, test_s21_div_by_10_basic);
  tcase_add_test(tc_core, test_s21_div_by_10_rounding);
  tcase_add_test(tc_core, test_s21_div_by_10_negative);
  tcase_add_test(tc_core, test_s21_div_by_10_zero);
  tcase_add_test(tc_core, test_s21_div_by_10_multiple);
  tcase_add_test(tc_core, test_s21_div_by_10_large_number);

  tcase_add_test(tc_core, test_s21_add_basic);
  tcase_add_test(tc_core, test_s21_add_positive);
  tcase_add_test(tc_core, test_s21_add_negative);
  tcase_add_test(tc_core, test_s21_add_opposite_numbers);
  tcase_add_test(tc_core, test_s21_add_overflow);
  tcase_add_test(tc_core, test_s21_add_with_zero);
  tcase_add_test(tc_core, test_s21_add_negative_overflow);
  tcase_add_test(tc_core, test_s21_add_complex_case);
  tcase_add_test(tc_core, test_s21_add_with_diff_signs);
  tcase_add_test(tc_core, test_s21_sub_with_diff_signs_basic);
  tcase_add_test(tc_core, test_s21_from_fractional_decimal_to_float);
  tcase_add_test(tc_core, test_s21_from_fractional_decimal_to_int);
  tcase_add_test(tc_core, test_s21_from_negative_int_to_decimal);
  tcase_add_test(tc_core, test_s21_round_for_negative);
  tcase_add_test(tc_core, test_s21_round_to_higher);
  tcase_add_test(tc_core, test_s21_sub_basic);
  tcase_add_test(tc_core, test_s21_sub_underflow);
  tcase_add_test(tc_core, test_s21_sub_negative_result);
  tcase_add_test(tc_core, test_s21_sub_large_numbers);
  tcase_add_test(tc_core, test_s21_sub_same_numbers);
  tcase_add_test(tc_core, test_s21_sub_negative_zero);
  tcase_add_test(tc_core, test_s21_sub_overflow_negative);
  tcase_add_test(tc_core, test_s21_sub_carry_propagation);

  tcase_add_test(tc_core, test_s21_mul_basic);
  tcase_add_test(tc_core, test_s21_mul_overflow);
  tcase_add_test(tc_core, test_s21_mul_negative_result);
  tcase_add_test(tc_core, test_s21_div_basic);
  tcase_add_test(tc_core, test_s21_div_by_zero);
  tcase_add_test(tc_core, test_s21_div_negative_result);
  tcase_add_test(tc_core, test_s21_is_less_basic);
  tcase_add_test(tc_core, test_s21_is_less_equal);
  tcase_add_test(tc_core, test_s21_is_greater);
  tcase_add_test(tc_core, test_s21_is_greater_or_equal);
  tcase_add_test(tc_core, test_s21_is_equal);
  tcase_add_test(tc_core, test_s21_is_not_equal);
  tcase_add_test(tc_core, test_s21_floor);
  tcase_add_test(tc_core, test_s21_floor_negative);
  tcase_add_test(tc_core, test_s21_round);
  tcase_add_test(tc_core, test_s21_truncate);
  tcase_add_test(tc_core, test_s21_negate);
  tcase_add_test(tc_core, test_s21_from_int_to_decimal);
  tcase_add_test(tc_core, test_s21_from_float_to_decimal);
  tcase_add_test(tc_core, test_s21_from_decimal_to_int);
  tcase_add_test(tc_core, test_s21_from_decimal_to_float);

  tcase_add_test(tc_core, test_s21_div_fraction);
  tcase_add_test(tc_core, test_s21_div_large_numbers);
  tcase_add_test(tc_core, test_s21_div_negative);
  tcase_add_test(tc_core, test_s21_div_two_negatives);
  tcase_add_test(tc_core, test_s21_div_scale_adjustment);

  tcase_add_test(tc_core, test_s21_multiply_by_10_basic);
  tcase_add_test(tc_core, test_s21_multiply_by_10_large_number);
  tcase_add_test(tc_core, test_s21_multiply_by_10_overflow);
  tcase_add_test(tc_core, test_s21_multiply_by_10_middle_overflow);

  tcase_add_test(tc_core, test_bank_rounding_1);
  tcase_add_test(tc_core, test_s21_div_large_result_with_fraction);
  tcase_add_test(tc_core, test_s21_div_infinite_fraction);
  tcase_add_test(tc_core, test_s21_div_two_bits);
  tcase_add_test(tc_core, test_s21_sub_three_bits);
  tcase_add_test(tc_core, test_bank_rounding_sum);
  suite_add_tcase(s, tc_core);
  return s;
}

int main(void) {
  int number_failed;
  Suite *s = s21_decimal_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
