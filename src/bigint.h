#ifndef BIGINTCPP
#define BIGINTCPP
#include <iostream>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <format>
#include <cmath>
#include <numeric>
class BigInt
{
public:
    using DataType = std::uint_fast32_t;

    BigInt();
    BigInt(bool negative, const std::vector<DataType> &data);
    BigInt(bool negative, std::vector<DataType> &&data);
    BigInt(bool negative, std::vector<DataType>::const_iterator begin, std::vector<DataType>::const_iterator end);
    BigInt(bool negative, const uint32_t num);
    BigInt(bool negative, std::initializer_list<DataType> list);
    BigInt(const std::string &s);
    BigInt(const BigInt &other);
    BigInt(BigInt &&other);

    bool normal_equal(const BigInt &num) const;
    bool normal_larger(const BigInt &num) const;
    bool normal_smaller(const BigInt &num) const;

    bool operator<(const BigInt &num) const;
    bool operator>(const BigInt &num) const;
    bool operator==(const BigInt &num) const;
    bool operator!=(const BigInt &num) const;
    bool operator<=(const BigInt &num) const;
    bool operator>=(const BigInt &num) const;

    bool is_zero() const;
    std::size_t get_digits() const;

    BigInt operator++(int);
    BigInt operator--(int);

    BigInt &operator=(const BigInt &other);
    BigInt &operator=(BigInt &&other);
    BigInt operator+(const BigInt &num) const;
    BigInt &operator+=(const BigInt &num);
    BigInt operator-() const;
    BigInt operator-(const BigInt &num) const;
    BigInt &operator-=(const BigInt &num);
    BigInt operator*(const BigInt &num) const;
    BigInt &operator*=(const BigInt &num);
    BigInt operator/(const BigInt &num) const;
    BigInt &operator/=(const BigInt &num);
    BigInt operator%(const BigInt &num) const;
    BigInt &operator%=(const BigInt &num);
    BigInt operator^(const BigInt &num) const;
    BigInt &operator^=(const BigInt &num);
    BigInt inverse() const;

    BigInt operator*(const std::uint32_t num) const;
    friend BigInt operator*(const std::uint32_t num, const BigInt &bigint);
    BigInt operator/(const uint32_t num) const;
    friend BigInt operator/(const uint32_t num, const BigInt &bigint);
    BigInt operator/=(const uint32_t num);

    static BigInt from_str(const std::string &s);
    std::string to_str() const;

    friend std::istream &operator>>(std::istream &is, BigInt &num);
    friend std::ostream &operator<<(std::ostream &os, const BigInt &num);

    static void inject_mod(BigInt *mod_tar);

private:
    BigInt normal_add(const BigInt &num, const bool negative) const;
    BigInt &self_normal_add(const BigInt &num, const bool negative);

    BigInt normal_substract(const BigInt &num, const bool negative) const;
    BigInt &self_normal_substract(const BigInt &num, const bool negative);

private:
    void chunk_move_left(const std::size_t sz);
    void chunk_move_right(const std::size_t sz);
    void digit_move_left(const std::size_t sz);
    void digit_move_right(const std::size_t sz);

    void remove_front_zero();

private:
    static const std::uint8_t EXP = 9;
    static const std::uint_fast32_t LIMIT = 1000000000;
    static BigInt *__mod_tar;
    bool negative = false;
    std::vector<DataType> data;
};
#endif
