#include "bigint.h"

BigInt *BigInt::__mod_tar = nullptr;

template <>
struct std::formatter<BigInt>
{
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const BigInt &num, std::format_context &ctx) const { return std::format_to(ctx.out(), "{}", num.to_str()); }
};

BigInt::BigInt() : negative(false), data(std::vector<DataType>()) {}

BigInt::BigInt(bool negative, const std::vector<DataType> &data) : negative(negative), data(data) {}

BigInt::BigInt(bool negative, std::vector<DataType> &&data) : negative(negative), data(std::move(data)) {}

BigInt::BigInt(bool negative, std::vector<DataType>::const_iterator begin, std::vector<DataType>::const_iterator end) : negative(negative), data(begin, end) {}

BigInt::BigInt(bool negative, const uint32_t num) : negative(negative) { data.emplace_back(num); }

BigInt::BigInt(bool negative, std::initializer_list<DataType> list) : negative(negative)
{
    const auto sz = list.size();
    data.resize(sz);
    std::size_t cur_i = sz - 1;
    for (const auto element : list)
        data[cur_i--] = element;
}

BigInt::BigInt(const std::string &s) : negative(false)
{
    std::string str = s;
    if (s.front() == '-')
    {
        this->negative = true;
        str.erase(0, 1);
    }
    // 去除前导零
    std::size_t non_zero_pos = str.find_first_not_of('0');
    if (non_zero_pos != std::string::npos)
        str = str.substr(non_zero_pos);

    const std::size_t len = str.length();
    const std::size_t high_left = len % this->EXP;
    const std::size_t counts = len / this->EXP;
    const std::size_t data_size = counts + (high_left != 0);
    this->data.resize(data_size);

    std::size_t pos = 0;
    if (high_left != 0)
    {
        this->data[data_size - 1] = static_cast<DataType>(std::stoull(str.substr(pos, high_left), nullptr));
        pos += high_left;
    }
    for (std::size_t i = (high_left != 0); i < data_size; ++i)
    {
        this->data[data_size - 1 - i] = static_cast<DataType>(std::stoull(str.substr(pos, this->EXP), nullptr));
        pos += this->EXP;
    }
}

BigInt::BigInt(const BigInt &other) : negative(other.negative), data(other.data) {}

BigInt::BigInt(BigInt &&other) : negative(other.negative), data(std::move(other.data)) { other.negative = false; }

bool BigInt::normal_equal(const BigInt &num) const
{
    if (this->data.size() != num.data.size())
        return false;

    for (std::size_t i = 0; i < this->data.size(); ++i)
    {
        if (this->data[i] != num.data[i])
            return false;
    }

    return true;
}
bool BigInt::normal_larger(const BigInt &num) const
{
    if (this->data.size() < num.data.size())
        return false;
    if (this->data.size() > num.data.size())
        return true;

    for (int i = this->data.size() - 1; i >= 0; --i)
    {
        if (this->data[i] < num.data[i])
            return false;
        if (this->data[i] > num.data[i])
            return true;
    }
    return false;
}

bool BigInt::normal_smaller(const BigInt &num) const
{
    if (this->data.size() < num.data.size())
        return true;
    if (this->data.size() > num.data.size())
        return false;

    for (int i = this->data.size() - 1; i >= 0; --i)
    {
        if (this->data[i] < num.data[i])
            return true;
        if (this->data[i] > num.data[i])
            return false;
    }
    return false;
}

bool BigInt::operator<(const BigInt &num) const
{
    if (this->negative != num.negative)
        return this->negative;

    if (this->negative && num.negative)
        return this->normal_larger(num);

    return this->normal_smaller(num);
}

bool BigInt::operator>(const BigInt &num) const
{
    if (this->negative != num.negative)
        return !this->negative;

    if (this->negative && num.negative)
        return this->normal_smaller(num);

    return this->normal_larger(num);
}

bool BigInt::operator==(const BigInt &num) const { return (this->negative == num.negative) && this->normal_equal(num); }
bool BigInt::operator!=(const BigInt &num) const { return !(*this == num); }

bool BigInt::operator<=(const BigInt &num) const { return !(*this > num); }
bool BigInt::operator>=(const BigInt &num) const { return !(*this < num); }

bool BigInt::is_zero() const
{
    for (const auto element : this->data)
    {
        if (element != 0)
            return false;
    }

    return true;
}

std::size_t BigInt::get_digits() const
{
    if (this->is_zero())
        return 1;

    return std::log10(this->data.back()) + 1 + this->EXP * (this->data.size() - 1);
}

BigInt BigInt::normal_add(const BigInt &num, const bool negative) const
{
    BigInt result;
    result.negative = negative;

    std::uint8_t carry = 0;
    const std::size_t either_len = this->data.size(), other_len = num.data.size();

    std::size_t minlen = std::min(either_len, other_len);
    result.data.resize(minlen);
    for (std::size_t i = 0; i < minlen; ++i)
    {
        if (this->data[i] > this->LIMIT - num.data[i] - carry)
        {
            result.data[i] = static_cast<DataType>(this->data[i] + num.data[i] + carry - this->LIMIT);
            carry = 1;
        }
        else
        {
            result.data[i] = static_cast<DataType>(this->data[i] + num.data[i] + carry);
            carry = 0;
        }
    }

    if (either_len == other_len && carry == 0)
    {

        if (BigInt::__mod_tar != nullptr)
            result %= *BigInt::__mod_tar;
        return result;
    }

    if (either_len != other_len)
    {
        const auto &largerInt = (this->data.size() > num.data.size()) ? this->data : num.data;
        const std::size_t len = largerInt.size();
        result.data.resize(len);
        for (std::size_t i = minlen; i < len; ++i)
        {
            result.data[i] = static_cast<DataType>(largerInt[i]) + carry;
            carry = 0;
            if (result.data[i] == 0)
                carry = 1;
        }
        if (BigInt::__mod_tar != nullptr)
            result %= *BigInt::__mod_tar;
        return result;
    }

    result.data.emplace_back(static_cast<DataType>(carry));

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    return result;
}

BigInt &BigInt::self_normal_add(const BigInt &num, const bool negative)
{
    this->negative = negative;

    std::uint8_t carry = 0;
    const std::size_t either_len = this->data.size(), other_len = num.data.size();

    std::size_t minlen = std::min(either_len, other_len);
    this->data.resize(minlen);
    for (std::size_t i = 0; i < minlen; ++i)
    {
        if (this->data[i] > this->LIMIT - num.data[i] - carry)
        {
            this->data[i] = static_cast<DataType>(this->data[i] + num.data[i] + carry - this->LIMIT);
            carry = 1;
        }
        else
        {
            this->data[i] = static_cast<DataType>(this->data[i] + num.data[i] + carry);
            carry = 0;
        }
    }

    if (either_len == other_len && carry == 0)
    {
        if (BigInt::__mod_tar != nullptr)
            *this %= *BigInt::__mod_tar;
        return *this;
    }

    if (either_len != other_len)
    {
        const auto &largerInt = (this->data.size() > num.data.size()) ? this->data : num.data;
        const std::size_t len = largerInt.size();
        this->data.resize(len);
        for (std::size_t i = minlen; i < len; ++i)
        {
            this->data[i] = static_cast<DataType>(largerInt[i]) + carry;
            carry = 0;
            if (this->data[i] == 0)
                carry = 1;
        }
        if (BigInt::__mod_tar != nullptr)
            *this %= *BigInt::__mod_tar;
        return *this;
    }

    this->data.emplace_back(static_cast<DataType>(carry));

    if (BigInt::__mod_tar != nullptr)
        *this %= *BigInt::__mod_tar;

    return *this;
}

BigInt BigInt::normal_substract(const BigInt &num, const bool negative) const
{
    BigInt result(*this);
    result.negative = negative;

    const std::size_t len = num.data.size();
    std::uint8_t borrow = 0;
    for (std::size_t i = 0; i < len; ++i)
    {
        std::uint32_t left = num.data[i] + static_cast<DataType>(borrow);
        if (result.data[i] < left)
        {
            borrow = 1;
            result.data[i] += this->LIMIT - left;
        }
        else
        {
            borrow = 0;
            result.data[i] -= left;
        }
    }
    if (borrow == 1)
        result.data[len] -= 1;

    result.remove_front_zero();

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    if (result.negative)
        result = (result % *BigInt::__mod_tar + *BigInt::__mod_tar) % *BigInt::__mod_tar;

    return result;
}

BigInt &BigInt::self_normal_substract(const BigInt &num, const bool negative)
{
    this->negative = negative;

    const std::size_t len = num.data.size();
    std::uint8_t borrow = 0;
    for (std::size_t i = 0; i < len; ++i)
    {
        std::uint32_t left = num.data[i] + static_cast<DataType>(borrow);
        if (this->data[i] < left)
        {
            borrow = 1;
            this->data[i] += this->LIMIT - left;
        }
        else
        {
            borrow = 0;
            this->data[i] -= left;
        }
    }
    if (borrow == 1)
        this->data[len] -= 1;

    this->remove_front_zero();

    if (BigInt::__mod_tar != nullptr)
        *this %= *BigInt::__mod_tar;

    if (this->negative)
        *this = (*this % *BigInt::__mod_tar + *BigInt::__mod_tar) % *BigInt::__mod_tar;

    return *this;
}

void BigInt::chunk_move_left(const std::size_t sz)
{
    if (sz == 0)
        return;

    const std::size_t prv_sz = this->data.size();
    this->data.resize(prv_sz + sz);

    for (std::size_t i = prv_sz + sz - 1; i >= sz; --i)
        this->data[i] = this->data[i - sz];

    std::fill(this->data.begin(), this->data.begin() + sz, 0);
}

void BigInt::chunk_move_right(const std::size_t sz)
{
    if (sz == 0)
        return;

    const std::size_t prv_sz = this->data.size();
    for (std::size_t i = sz; i < prv_sz; ++i)
        this->data[i - sz] = this->data[i];

    this->data.resize(prv_sz - sz);
}

void BigInt::digit_move_left(const std::size_t sz)
{
    this->chunk_move_left(sz / this->EXP);

    const std::size_t left_sz = sz % this->EXP;
    if (left_sz == 0)
        return;

    DataType carry = 0;
    DataType ncarry = 0;
    DataType divide_helper = std::pow(10, (this->EXP - left_sz));
    DataType multi_helper = std::pow(10, left_sz);
    for (auto &element : this->data)
    {
        ncarry = (element / divide_helper);
        element = (element * multi_helper) % this->LIMIT + carry;
        carry = ncarry;
    }

    if (carry != 0)
        this->data.emplace_back(carry);
}

void BigInt::digit_move_right(const std::size_t sz)
{
    this->chunk_move_right(sz / this->EXP);

    const std::size_t left_sz = sz % this->EXP;
    if (left_sz == 0)
        return;

    DataType carry = 0;
    DataType ncarry = 0;
    DataType mod_divide_helper = std::pow(10, left_sz);
    DataType multi_helper = std::pow(10, (this->EXP - left_sz));
    for (auto it = this->data.rbegin(); it != this->data.rend(); ++it)
    {
        ncarry = (*it % mod_divide_helper) * multi_helper;
        *it = (*it / mod_divide_helper) + carry;
        carry = ncarry;
    }

    if (this->data.back() == 0)
        this->data.resize(this->data.size() - 1);
}

void BigInt::remove_front_zero()
{
    std::size_t zero_chunks = 0;
    for (auto it = this->data.rbegin(); it != this->data.rend(); ++it)
    {
        if (*it != 0)
            break;
        zero_chunks++;
    }
    if (zero_chunks != 0)
        this->data.resize(this->data.size() - zero_chunks);
}

BigInt &BigInt::operator=(const BigInt &other)
{
    this->negative = other.negative;
    this->data = other.data;
    return *this;
}

BigInt &BigInt::operator=(BigInt &&other)
{
    this->negative = other.negative;
    other.negative = false;
    this->data = std::move(other.data);
    return *this;
}

BigInt BigInt::operator+(const BigInt &num) const
{
    if (this->negative == num.negative)
        return this->normal_add(num, this->negative);

    if (this->normal_equal(num))
        return BigInt(std::string("0"));

    if (!this->negative && num.negative)
    {
        if (this->normal_larger(num))
            return this->normal_substract(num, false);

        return num.normal_substract(*this, true);
    }

    if (this->normal_larger(num))
        return this->normal_substract(num, true);

    return num.normal_substract(*this, false);
}

BigInt &BigInt::operator+=(const BigInt &num)
{
    if (this->negative == num.negative)
        return this->self_normal_add(num, this->negative);

    if (this->normal_equal(num))
    {
        *this = BigInt(false, static_cast<uint32_t>(0));
        return *this;
    }

    if (!this->negative && num.negative)
    {
        if (this->normal_larger(num))
            return this->self_normal_substract(num, false);

        *this = std::move(num.normal_substract(*this, true));
        return *this;
    }

    if (this->normal_larger(num))
        return this->self_normal_substract(num, true);

    *this = std::move(num.normal_substract(*this, false));
    return *this;
}

BigInt BigInt::operator-() const { return BigInt(!negative, data); }

BigInt BigInt::operator-(const BigInt &num) const
{
    if (*this == num)
        return BigInt(false, static_cast<uint32_t>(0));

    if (this->negative != num.negative)
        return this->normal_add(num, this->negative);

    if (!this->negative && !num.negative)
    {
        if (this->normal_larger(num))
            return this->normal_substract(num, false);

        return num.normal_substract(*this, true);
    }

    if (this->normal_larger(num))
        return this->normal_substract(num, true);

    return num.normal_substract(*this, false);
}

BigInt &BigInt::operator-=(const BigInt &num)
{
    if (*this == num)
    {
        *this = BigInt(false, static_cast<uint32_t>(0));
        return *this;
    }

    if (this->negative != num.negative)
        return this->self_normal_add(num, this->negative);

    if (!this->negative && !num.negative)
    {
        if (this->normal_larger(num))
            return this->self_normal_substract(num, false);

        *this = std::move(num.normal_substract(*this, true));
        return *this;
    }

    if (this->normal_larger(num))
        return this->self_normal_substract(num, true);

    *this = std::move(num.normal_substract(*this, false));
    return *this;
}

BigInt BigInt::operator*(const BigInt &num) const
{
    const std::size_t chunks_A = this->data.size(), chunks_B = num.data.size();
    // base
    if (chunks_B == 0 || chunks_A == 0)
        return BigInt(false, static_cast<uint32_t>(0));
    if (chunks_B == 1)
    {
        BigInt result(*this * static_cast<uint32_t>(num.data.front()));
        result.negative = this->negative ^ num.negative;
        return result;
    }
    if (chunks_A == 1)
    {
        BigInt result(num * static_cast<uint32_t>(this->data.front()));
        result.negative = this->negative ^ num.negative;
        return result;
    }

    const std::size_t mid = std::min(chunks_A, chunks_B) / 2;
    // divide
    BigInt b(false, this->data.begin(), this->data.begin() + mid), a(false, this->data.begin() + mid, this->data.end());
    BigInt d(false, num.data.begin(), num.data.begin() + mid), c(false, num.data.begin() + mid, num.data.end());

    auto temp2 = a * c;
    const auto temp0 = b * d;
    auto temp1 = (a + b) * (c + d) - temp2 - temp0;
    temp2.chunk_move_left(mid * 2);
    temp1.chunk_move_left(mid);

    BigInt result(temp2 + temp1 + temp0);
    result.negative = this->negative ^ num.negative;

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    return result;
}

BigInt &BigInt::operator*=(const BigInt &num)
{
    *this = *this * num;
    return *this;
}

BigInt BigInt::operator/(const BigInt &num) const
{
    if (num.is_zero())
        throw std::runtime_error("divided by zero");

    // 如果*this < num -> return 0;
    if (*this < num)
        return BigInt(false, 0);
    if (*this == num)
        return BigInt(false, 1);

    BigInt origin(*this);
    BigInt other(num);
    BigInt result(false, std::move(std::vector<DataType>(this->data.size(), 0)));
    other.digit_move_left(this->get_digits() - num.get_digits());

    while (origin >= num)
    {
        std::uint_fast32_t count = 0;
        while (origin >= other)
        {
            count++;
            origin -= other;
        }

        result.data[other.data.size() - 1] += (count * std::pow(10, (other.get_digits() - 1) % BigInt::EXP));
        other.digit_move_right(1);
    }

    result.remove_front_zero();
    result.digit_move_right(num.get_digits() - 1);

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    return result;
}

BigInt &BigInt::operator/=(const BigInt &num)
{
    if (num.is_zero())
        throw std::runtime_error("divided by zero");

    // 如果*this < num -> return 0;
    if (*this < num)
    {
        *this = BigInt(false, 0);
        return *this;
    }
    if (*this == num)
    {
        *this = BigInt(false, 1);
        return *this;
    }

    BigInt origin(*this);
    BigInt other(num);
    other.digit_move_left(origin.get_digits() - other.get_digits());
    std::fill(this->data.begin(), this->data.end(), 0);

    while (origin >= num)
    {
        std::uint_fast32_t count = 0;
        while (origin >= other)
        {
            count++;
            origin -= other;
        }

        this->data[other.data.size() - 1] += (count * std::pow(10, (other.get_digits() - 1) % BigInt::EXP));
        other.digit_move_right(1);
    }

    this->remove_front_zero();
    this->digit_move_right(num.get_digits() - 1);

    if (BigInt::__mod_tar != nullptr)
        *this %= *BigInt::__mod_tar;

    return *this;
}

BigInt BigInt::operator%(const BigInt &num) const
{
    if (num.is_zero())
        throw std::runtime_error("mod by zero");

    if (*this < num)
        return *this;
    if (*this == num)
        return BigInt(false, static_cast<uint32_t>(0));

    BigInt origin(*this);
    BigInt other(num);
    other.digit_move_left(this->get_digits() - num.get_digits());

    while (origin >= num)
    {
        std::uint_fast32_t count = 0;
        while (origin >= other)
        {
            count++;
            origin -= other;
        }

        other.digit_move_right(1);
    }

    return origin;
}

BigInt &BigInt::operator%=(const BigInt &num)
{
    if (num.is_zero())
        throw std::runtime_error("mod by zero");

    if (*this < num)
        return *this;
    if (*this == num)
    {
        *this = BigInt(false, static_cast<uint32_t>(0));
        return *this;
    }

    BigInt other(num);
    other.digit_move_left(this->get_digits() - num.get_digits());

    while (*this >= num)
    {
        std::uint_fast32_t count = 0;
        while (*this >= other)
        {
            count++;
            *this -= other;
        }

        other.digit_move_right(1);
    }

    return *this;
}

BigInt BigInt::operator^(const BigInt &num) const
{
    if (num.is_zero())
        return BigInt(false, 1);

    BigInt result(false, static_cast<uint32_t>(1));
    BigInt a(*this);
    BigInt b(num);
    BigInt helper(false, static_cast<uint32_t>(2));
    while (!b.is_zero())
    {
        if ((b.data.front() & 0x1) == 1)
            result *= a;

        a *= a;
        b /= helper;
    }

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    return result;
}

BigInt BigInt::inverse() const
{
    BigInt r0 = *this, r1 = *BigInt::__mod_tar, r2(false, static_cast<uint32_t>(0)), r11(false, static_cast<uint32_t>(0)), q(false, static_cast<uint32_t>(0));
    BigInt s0(false, static_cast<uint32_t>(1)), s1(false, static_cast<uint32_t>(0));
    BigInt s00(false, static_cast<uint32_t>(0)), s11(false, static_cast<uint32_t>(0));

    while (true)
    {
        q = r0 / r1;
        r2 = r0 % r1;

        if (r2.is_zero())
            break;

        r0 = r1;
        r1 = r2;

        s00 = s0;
        s11 = s1;

        s0 = s11;
        s1 = s00 - q * s11;
    }

    if (s1.negative)
        s1 = (s1 % *BigInt::__mod_tar + *BigInt::__mod_tar) % *BigInt::__mod_tar;

    return s1;
}

BigInt BigInt::operator*(const std::uint32_t num) const
{
    if (num == 0)
        return BigInt(false, static_cast<uint32_t>(0));

    BigInt result;

    uint32_t carry = 0;
    uint64_t temp = 0;

    const auto &data = this->data;
    const auto LIMIT = this->LIMIT;
    auto &rdata = result.data;

    rdata.resize(data.size());

    const std::size_t prv_sz = data.size();
    for (std::size_t i = 0; i < prv_sz; ++i)
    {
        temp = data[i] * num + carry;
        carry = temp / LIMIT;
        rdata[i] = temp % LIMIT;
    }

    if (carry != 0)
        rdata.emplace_back(carry);

    result.negative = this->negative;

    if (BigInt::__mod_tar != nullptr)
        result %= *BigInt::__mod_tar;

    return result;
}

BigInt operator*(const std::uint32_t num, const BigInt &bigint) { return bigint * num; }

BigInt BigInt::operator/(const uint32_t number) const
{
    BigInt num(false, number);
    return *this / num;
}

BigInt operator/(const uint32_t num, const BigInt &bigint) { return bigint / num; }

BigInt BigInt::operator/=(const uint32_t number)
{
    BigInt num(false, number);
    *this /= num;
    return *this;
}

void BigInt::inject_mod(BigInt *mod_tar) { BigInt::__mod_tar = mod_tar; }

BigInt BigInt::from_str(const std::string &s) { return BigInt(s); }

std::string BigInt::to_str() const
{
    if (this->data.empty())
        return std::string("0");

    std::string s;
    if (this->negative)
        s += '-';

    s += std::to_string(*this->data.rbegin());
    if (this->data.size() == 1)
        return s;

    for (auto it = this->data.rbegin() + 1; it != this->data.rend(); ++it)
    {
        std::string str = std::to_string(*it);
        int zeros = 9 - str.length();
        s += std::string(zeros, '0') + str;
    }

    return s;
}

std::istream &operator>>(std::istream &is, BigInt &num)
{
    std::string str;
    std::cin >> str;
    num = BigInt(str);

    return is;
}

std::ostream &operator<<(std::ostream &os, const BigInt &num)
{
    os << num.to_str();
    return os;
}
