#include "helpers.h"

int s21_divide_core(s21_decimal value_1, s21_decimal value_2,
                    s21_decimal *quotient, uint32_t *initial_scale,
                    int *fractional_scale) {
  int error = 0;
  s21_decimal divisible = value_1;
  s21_decimal divisor = value_2;
  if (s21_get_scale(value_1) < 28) {
    s21_equalize_scales(&divisible, &divisor, &error);
    if (error != 0) {
      s21_set_scale(&divisible, 28);
    }
  }
  *initial_scale = s21_get_scale(divisible) - s21_get_scale(divisor);
  divisible.bits[3] = 0;
  divisor.bits[3] = 0;
  s21_decimal remainder = {{0}}, digit = {{0}}, temp = {{0}};
  s21_decimal fractional = {{0}}, q = {{0}};
  s21_div_support(&remainder, divisible, divisor, &q);
  *fractional_scale = 0;
  while ((remainder.bits[0] || remainder.bits[1] || remainder.bits[2]) &&
         *fractional_scale < 28) {
    s21_multiply_by_10(&remainder);
    s21_div_support(&temp, remainder, divisor, &digit);
    remainder.bits[0] = temp.bits[0];
    remainder.bits[1] = temp.bits[1];
    remainder.bits[2] = temp.bits[2];
    s21_multiply_by_10(&fractional);
    s21_add(fractional, digit, &fractional);
    (*fractional_scale)++;
  }
  s21_div_bank_round(&q, &fractional, fractional_scale, divisor, remainder);
  *quotient = q;
  if (error == 1 && (s21_get_sign(value_1) ^ s21_get_sign(value_2))) {
    error++;
  }
  return error;
}

void s21_adjust_scale_div(s21_decimal *quotient, uint32_t initial_scale,
                          int fractional_scale, s21_decimal value_1,
                          s21_decimal value_2) {
  uint32_t total_scale = initial_scale + fractional_scale;
  while (total_scale > 28) {
    int q_int = 0;
    s21_from_decimal_to_int(*quotient, &q_int);
    int r = q_int % 10;
    q_int /= 10;
    if (r > 5 || (r == 5 && (q_int % 2 != 0))) {
      q_int++;
    }
    s21_from_int_to_decimal(q_int, quotient);
    total_scale--;
  }
  s21_set_scale(quotient, total_scale);
  s21_set_sign(quotient,
               ((value_1.bits[3] >> 31) ^ (value_2.bits[3] >> 31)) & 1);
}

bool s21_is_null(s21_decimal value) {
  bool res = false;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0) {
    res = true;
  }
  return res;
}

void s21_decrease_scale(s21_decimal *value) {
  uint64_t r = 0;
  for (int i = 2; i >= 0; i--) {
    uint64_t t = (r << 32) | value->bits[i];
    value->bits[i] = t / 10;
    r = t % 10;
  }
  s21_set_scale(value, s21_get_scale(*value) - 1);
}

int s21_divide_by_10(uint64_t temp[6]) {
  uint64_t remainder = 0;
  for (int i = 5; i >= 0; i--) {
    uint64_t current = (remainder << 32) | (temp[i] & 0xFFFFFFFF);
    temp[i] = current / 10;
    remainder = current % 10;
  }
  return (int)remainder;
}

void s21_add_one(uint64_t temp[6]) {
  uint64_t carry = 1;
  for (int i = 0; i < 6 && carry; i++) {
    uint64_t sum = temp[i] + carry;
    temp[i] = sum & 0xFFFFFFFF;
    carry = sum >> 32;
  }
}

void s21_compute_frac_sum(s21_decimal value_1, s21_decimal value_2, int *error,
                          s21_decimal *int_frac_part,
                          s21_decimal *frac_remainder) {
  s21_decimal fr_part_1 = s21_get_fr_part(value_1);
  s21_decimal fr_part_2 = s21_get_fr_part(value_2);
  s21_decimal sum_fr_part = {{0, 0, 0, 0}};
  if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
    s21_add_with_equal_signs(error, &sum_fr_part, fr_part_1, fr_part_2);
  } else {
    s21_add_with_diff_signs(error, &sum_fr_part, fr_part_1, fr_part_2);
  }
  *frac_remainder = s21_get_fr_part(sum_fr_part);
  s21_truncate(sum_fr_part, int_frac_part);
}

void s21_adjust_main_by_frac(s21_decimal *value, s21_decimal int_frac_part,
                             int *error) {
  if (s21_get_sign(*value) == s21_get_sign(int_frac_part)) {
    s21_add_with_equal_signs(error, value, *value, int_frac_part);
  } else {
    s21_add_with_diff_signs(error, value, *value, int_frac_part);
  }
}

void s21_sum_integer_parts(s21_decimal value_1, s21_decimal value_2, int *error,
                           s21_decimal *int_sum) {
  s21_decimal int_part_1 = {{0, 0, 0, 0}};
  s21_decimal int_part_2 = {{0, 0, 0, 0}};
  s21_truncate(value_1, &int_part_1);
  s21_truncate(value_2, &int_part_2);
  if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
    s21_add_with_equal_signs(error, int_sum, int_part_1, int_part_2);
  } else {
    s21_add_with_diff_signs(error, int_sum, int_part_1, int_part_2);
  }
}

void s21_apply_bank_rounding(s21_decimal *result, s21_decimal frac_remainder,
                             int *error) {
  uint32_t signBit = (uint32_t)1 << 31;
  uint32_t halfScale = (uint32_t)1 << 16;

  if (!(*error) &&
      (s21_is_greater(frac_remainder, (s21_decimal){{5, 0, 0, halfScale}}) ||
       (s21_is_equal(frac_remainder, (s21_decimal){{5, 0, 0, halfScale}}) &&
        (result->bits[0] % 2) == 1))) {
    if (s21_get_sign(*result)) {
      s21_add_with_diff_signs(error, result, *result,
                              (s21_decimal){{1, 0, 0, 0}});
    } else {
      s21_add_with_equal_signs(error, result, *result,
                               (s21_decimal){{1, 0, 0, 0}});
    }
  } else if (!(*error) &&
             (s21_is_less(frac_remainder,
                          (s21_decimal){{5, 0, 0, signBit | halfScale}}) ||
              (s21_is_equal(frac_remainder,
                            (s21_decimal){{5, 0, 0, signBit | halfScale}}) &&
               (result->bits[0] % 2) == 1))) {
    if (s21_get_sign(*result)) {
      s21_add_with_equal_signs(error, result, *result,
                               (s21_decimal){{1, 0, 0, signBit}});
    } else {
      s21_add_with_diff_signs(error, result, *result,
                              (s21_decimal){{1, 0, 0, signBit}});
    }
  }
}

void s21_add_with_rounding(s21_decimal value_1, s21_decimal value_2, int *error,
                           s21_decimal *result) {
  s21_decimal int_frac_part = {{0, 0, 0, 0}};
  s21_decimal frac_remainder = {{0, 0, 0, 0}};
  s21_decimal int_sum = {{0, 0, 0, 0}};

  s21_compute_frac_sum(value_1, value_2, error, &int_frac_part,
                       &frac_remainder);
  s21_adjust_main_by_frac(&value_1, int_frac_part, error);
  s21_sum_integer_parts(value_1, value_2, error, &int_sum);
  s21_apply_bank_rounding(&int_sum, frac_remainder, error);
  s21_dec_assignment(int_sum, result);
}

s21_decimal s21_get_fr_part(s21_decimal value) {
  s21_decimal result = {{0, 0, 0, 0}};
  uint32_t scale = s21_get_scale(value);
  uint32_t sign = s21_get_sign(value);
  if (scale) {
    s21_decimal int_part = {{0, 0, 0, 0}};
    s21_truncate(value, &int_part);
    s21_sub(value, int_part, &result);
  }
  s21_set_scale(&result, scale);
  s21_set_sign(&result, sign);
  return result;
}

void s21_add_with_equal_signs(int *error, s21_decimal *result,
                              s21_decimal value_1, s21_decimal value_2) {
  uint64_t carry = 0;
  uint32_t sign1 = s21_get_sign(value_1);
  uint32_t max_scale = s21_get_scale(value_1) > s21_get_scale(value_2)
                           ? s21_get_scale(value_1)
                           : s21_get_scale(value_2);
  for (int i = 0; i < 3; i++) {
    uint64_t sum = (uint64_t)value_1.bits[i] + value_2.bits[i] + carry;
    result->bits[i] = (uint32_t)(sum & 0xFFFFFFFF);
    carry = sum >> 32;
  }
  if (carry && !max_scale) {
    *error = sign1 == 0 ? 1 : 2;
  }
  if (sign1) {
    s21_set_sign(result, 1);
  }
  s21_set_scale(result, max_scale);
}

void s21_add_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2) {
  s21_decimal abs_value_1 = value_1;
  s21_decimal abs_value_2 = value_2;
  s21_set_sign(&abs_value_1, 0);
  s21_set_sign(&abs_value_2, 0);
  s21_decimal max_value =
      s21_is_greater_or_equal(abs_value_1, abs_value_2) ? value_1 : value_2;
  if (s21_is_greater_or_equal(abs_value_1, abs_value_2)) {
    value_2.bits[3] ^= 0x80000000;
    *error = s21_sub(value_1, value_2, result);
    s21_set_sign(result, s21_get_sign(max_value));
  } else {
    value_1.bits[3] ^= 0x80000000;
    *error = s21_sub(value_2, value_1, result);
    s21_set_sign(result, s21_get_sign(max_value));
  }
  uint32_t max_scale = s21_get_scale(value_1) > s21_get_scale(value_2)
                           ? s21_get_scale(value_1)
                           : s21_get_scale(value_2);
  s21_set_scale(result, max_scale);
}

void s21_sub_with_equal_signs(s21_decimal *result, s21_decimal value_1,
                              s21_decimal value_2) {
  s21_decimal abs_value_1 = value_1;
  s21_decimal abs_value_2 = value_2;
  s21_set_sign(&abs_value_1, 0);
  s21_set_sign(&abs_value_2, 0);
  int borrow = 0;
  *result = (s21_decimal){{0, 0, 0, 0}};
  if (s21_is_greater_or_equal(abs_value_1, abs_value_2)) {
    for (int i = 0; i < 3; i++) {
      int64_t diff = (int64_t)value_1.bits[i] - value_2.bits[i] - borrow;
      if (diff < 0) {
        borrow = 1;
        diff += (1LL << 32);
      } else {
        borrow = 0;
      }
      result->bits[i] = (uint32_t)diff;
    }
    s21_set_sign(result, s21_get_sign(value_1));
  } else {
    for (int i = 0; i < 3; i++) {
      int64_t diff = (int64_t)value_2.bits[i] - value_1.bits[i] - borrow;
      if (diff < 0) {
        borrow = 1;
        diff += (1LL << 32);
      } else {
        borrow = 0;
      }
      result->bits[i] = (uint32_t)diff;
    }
    s21_set_sign(result, !s21_get_sign(value_1));
  }
  s21_set_scale(result, s21_get_scale(value_1));
}

void s21_sub_with_diff_signs(int *error, s21_decimal *result,
                             s21_decimal value_1, s21_decimal value_2) {
  if (s21_is_greater_or_equal(value_1, value_2)) {
    value_2.bits[3] ^= 0x80000000;
    *error = s21_add(value_1, value_2, result);
  } else {
    value_1.bits[3] ^= 0x80000000;
    *error = s21_add(value_2, value_1, result);
    result->bits[3] ^= 0x80000000;
  }
  s21_set_scale(result, s21_get_scale(value_1));
  if (*error == 1 && s21_get_sign(*result)) {
    (*error)++;
  }
}

int s21_get_bit(s21_decimal value, int index) {
  uint32_t bit_position = index % 32;
  uint32_t word_index = index / 32;
  return (value.bits[word_index] >> bit_position) & 1;
}

void s21_set_bit(s21_decimal *value, int index, int bit) {
  int bit_position = index % 32;
  int word_index = index / 32;
  if (bit)
    value->bits[word_index] |= (1U << bit_position);
  else
    value->bits[word_index] &= ~(1U << bit_position);
}

int s21_div_support(s21_decimal *remainder, s21_decimal divisible,
                    s21_decimal divisor, s21_decimal *quotient) {
  *remainder = (s21_decimal){{0, 0, 0, 0}};
  *quotient = (s21_decimal){{0, 0, 0, 0}};

  for (int i = 95; i >= 0; i--) {
    s21_shift_left(remainder);
    if (s21_get_bit(divisible, i)) {
      s21_set_bit(remainder, 0, 1);
    }

    if (s21_is_greater_or_equal(*remainder, divisor)) {
      s21_sub(*remainder, divisor, remainder);
      s21_set_bit(quotient, i, 1);
    }
  }
  return 0;
}

void s21_div_bank_round(s21_decimal *quotient, s21_decimal *fractional,
                        int *fractional_scale, s21_decimal divisor,
                        s21_decimal remainder) {
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal one = {{1, 0, 0, 0}};
  s21_decimal two = {{2, 0, 0, 0}};
  s21_decimal five = {{5, 0, 0, 0}};
  uint64_t chislo = (uint64_t)quotient->bits[0] + (uint64_t)fractional->bits[0];
  s21_decimal rem_for_round = remainder, scaled_int = *quotient;
  int error = 0;
  if (chislo <= UINT32_MAX) {
    for (int i = 0; i < *fractional_scale; i++) {
      s21_multiply_by_10(&scaled_int);
    }
    s21_add_with_equal_signs(&error, quotient, scaled_int, *fractional);
  } else {
    (*fractional_scale)--;
  }
  if ((*fractional_scale == 28 && !s21_is_equal(remainder, zero)) ||
      chislo > UINT32_MAX) {
    if (s21_is_equal(remainder, zero) &&
        (s21_is_greater(*fractional, five) ||
         (s21_is_equal(*fractional, five) && quotient->bits[0] % 2 == 1))) {
      s21_add_with_equal_signs(&error, quotient, *quotient, one);
    } else {
      s21_mul(rem_for_round, two, &rem_for_round);
      if (s21_is_greater(rem_for_round, divisor) ||
          (s21_is_equal(rem_for_round, divisor) &&
           quotient->bits[0] % 2 == 1)) {
        s21_add_with_equal_signs(&error, quotient, *quotient, one);
      }
    }
  }
}

void s21_shift_left(s21_decimal *val) {
  uint32_t carry = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t temp = ((uint64_t)val->bits[i]) << 1 | carry;
    val->bits[i] = (uint32_t)temp;
    carry = (uint32_t)(temp >> 32);
  }
}

void s21_equalize_scales(s21_decimal *value_1, s21_decimal *value_2,
                         int *error) {
  uint32_t scale1 = s21_get_scale(*value_1);
  uint32_t scale2 = s21_get_scale(*value_2);
  int flag = 0;
  while (scale1 != scale2 && !(*error) && !flag) {
    if (scale1 < scale2) {
      if (scale1 == 28) {
        flag = 1;
      }
      if (s21_multiply_by_10(value_1) && !flag) {
        *error = 1;
        flag = 1;
      }
      if (flag != 1) {
        scale1++;
      }
    } else {
      if (scale2 == 28) {
        flag = 1;
      }
      if (s21_multiply_by_10(value_2) && !flag) {
        *error = 1;
        flag = 1;
      }
      if (flag != 1) {
        scale2++;
      }
    }
  }
  s21_set_scale(value_1, scale1);
  s21_set_scale(value_2, scale2);
}

uint32_t s21_get_sign(s21_decimal value) {
  decimal_bit3 db3;
  db3.i = value.bits[3];
  return db3.parts.sign;
}

uint32_t s21_get_scale(s21_decimal value) {
  decimal_bit3 db3;
  db3.i = value.bits[3];
  return db3.parts.scale;
}

int s21_multiply_by_10(s21_decimal *value) {
  s21_decimal tmp = {{0, 0, 0, 0}};
  tmp.bits[0] = (uint32_t)value->bits[0];
  tmp.bits[1] = (uint32_t)value->bits[1];
  tmp.bits[2] = (uint32_t)value->bits[2];
  uint64_t carry = 0;
  int res = 0;
  for (int i = 0; i < 3; i++) {
    uint64_t ans = (uint64_t)tmp.bits[i] * 10 + carry;

    if (ans > 0xFFFFFFFF) {
      carry = ans >> 32;
      tmp.bits[i] = (uint32_t)(ans & 0xFFFFFFFF);
    } else {
      carry = 0;
      tmp.bits[i] = (uint32_t)ans;
    }
  }
  if (carry != 0) {
    res = 1;
  } else {
    value->bits[0] = (uint32_t)tmp.bits[0];
    value->bits[1] = (uint32_t)tmp.bits[1];
    value->bits[2] = (uint32_t)tmp.bits[2];
  }

  return res;
}

void s21_decimal_zero(s21_decimal *dst) {
  for (int i = 0; i < 4; i++) {
    dst->bits[i] = 0;
  }
}

void s21_set_sign(s21_decimal *dst, uint32_t sign) {
  uint32_t sign_bit = (uint32_t)1 << 31;

  if (sign) {
    dst->bits[3] |= sign_bit;
  } else {
    dst->bits[3] &= ~sign_bit;
  }
}

void s21_set_scale(s21_decimal *decimal, uint32_t scale) {
  decimal_bit3 db3;
  db3.i = decimal->bits[3];
  db3.parts.scale = scale;
  decimal->bits[3] = db3.i;
}

void s21_dec_assignment(s21_decimal value, s21_decimal *result) {
  result->bits[0] = value.bits[0];
  result->bits[1] = value.bits[1];
  result->bits[2] = value.bits[2];
  result->bits[3] = value.bits[3];
}

bool s21_is_valid_decimal(s21_decimal value) {
  bool result = true;
  decimal_bit3 db3;
  db3.i = value.bits[3];
  if (db3.parts.empty2 != 0 || db3.parts.empty1 != 0 || db3.parts.scale > 28 ||
      db3.parts.sign > 1) {
    result = false;
  }
  return result;
}

bool s21_are_all_bits_zero(s21_decimal value) {
  bool result = false;
  if (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0 &&
      value.bits[3] == 0) {
    result = true;
  }
  return result;
}

s21_decimal s21_trim_trailing_zeros(s21_decimal value) {
  s21_decimal result = value;
  uint32_t scale = s21_get_scale(value);
  if (scale > 0 && s21_is_valid_decimal(value)) {
    s21_decimal ost = {{0, 0, 0, 0}};
    s21_decimal tmp = value;
    tmp.bits[3] = 0;
    s21_decimal DECIMAL_TEN = {{10, 0, 0, 0}};
    while (scale > 0 && s21_are_all_bits_zero(ost)) {
      s21_div(tmp, DECIMAL_TEN, &tmp);
      s21_ostatok(tmp, &ost);
      if (s21_are_all_bits_zero(ost)) {
        --scale;
        result = tmp;
      }
    }
  }
  result.bits[3] = value.bits[3];
  s21_set_scale(&result, scale);
  return result;
}

void s21_div_by_10(uint32_t power, s21_decimal value, s21_decimal *result) {
  for (uint32_t i = 0; i < power; i++) {
    s21_decrease_scale(&value);
  }
  *result = value;
  s21_set_scale(result, power);
}

void s21_ostatok(s21_decimal value, s21_decimal *result) {
  uint32_t power = s21_get_scale(value);
  int num = value.bits[0] % (uint32_t)pow(10, power);
  s21_from_int_to_decimal(num, result);
}