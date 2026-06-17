// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Wesley Atwell.
// See LICENSE.md for repository split licensing.

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Row {
    int m;
    int T;
    int g;
};

class BigInt {
public:
    BigInt(std::uint64_t value = 0) {
        if (value == 0) {
            digits_.push_back(0);
        } else {
            while (value > 0) {
                digits_.push_back(static_cast<std::uint32_t>(value % BASE));
                value /= BASE;
            }
        }
    }

    void add(const BigInt& other) {
        const std::size_t n = std::max(digits_.size(), other.digits_.size());
        digits_.resize(n, 0);
        std::uint64_t carry = 0;
        for (std::size_t i = 0; i < n; ++i) {
            const std::uint64_t rhs = i < other.digits_.size() ? other.digits_[i] : 0;
            const std::uint64_t sum = static_cast<std::uint64_t>(digits_[i]) + rhs + carry;
            digits_[i] = static_cast<std::uint32_t>(sum % BASE);
            carry = sum / BASE;
        }
        if (carry) {
            digits_.push_back(static_cast<std::uint32_t>(carry));
        }
    }

    void mul_uint(std::uint64_t factor) {
        if (factor == 0 || is_zero()) {
            digits_.assign(1, 0);
            return;
        }
        std::uint64_t carry = 0;
        for (std::uint32_t& digit : digits_) {
            const std::uint64_t product = static_cast<std::uint64_t>(digit) * factor + carry;
            digit = static_cast<std::uint32_t>(product % BASE);
            carry = product / BASE;
        }
        while (carry) {
            digits_.push_back(static_cast<std::uint32_t>(carry % BASE));
            carry /= BASE;
        }
    }

    bool is_zero() const {
        return digits_.size() == 1 && digits_[0] == 0;
    }

    friend BigInt operator*(const BigInt& lhs, const BigInt& rhs) {
        BigInt out;
        out.digits_.assign(lhs.digits_.size() + rhs.digits_.size(), 0);
        for (std::size_t i = 0; i < lhs.digits_.size(); ++i) {
            std::uint64_t carry = 0;
            for (std::size_t j = 0; j < rhs.digits_.size() || carry; ++j) {
                const std::uint64_t rhs_digit = j < rhs.digits_.size() ? rhs.digits_[j] : 0;
                const std::uint64_t cur =
                    static_cast<std::uint64_t>(out.digits_[i + j]) +
                    static_cast<std::uint64_t>(lhs.digits_[i]) * rhs_digit +
                    carry;
                out.digits_[i + j] = static_cast<std::uint32_t>(cur % BASE);
                carry = cur / BASE;
            }
        }
        out.normalize();
        return out;
    }

    BigInt& operator*=(const BigInt& other) {
        *this = *this * other;
        return *this;
    }

    friend bool operator<(const BigInt& lhs, const BigInt& rhs) {
        if (lhs.digits_.size() != rhs.digits_.size()) {
            return lhs.digits_.size() < rhs.digits_.size();
        }
        for (std::size_t i = lhs.digits_.size(); i-- > 0;) {
            if (lhs.digits_[i] != rhs.digits_[i]) {
                return lhs.digits_[i] < rhs.digits_[i];
            }
        }
        return false;
    }

    friend bool operator>(const BigInt& lhs, const BigInt& rhs) {
        return rhs < lhs;
    }

    friend bool operator<=(const BigInt& lhs, const BigInt& rhs) {
        return !(rhs < lhs);
    }

private:
    static constexpr std::uint64_t BASE = 1000000000ULL;
    std::vector<std::uint32_t> digits_;

    void normalize() {
        while (digits_.size() > 1 && digits_.back() == 0) {
            digits_.pop_back();
        }
    }
};

static BigInt ipow(BigInt base, int exp) {
    BigInt out(1);
    while (exp > 0) {
        if (exp & 1) {
            out *= base;
        }
        exp >>= 1;
        if (exp) {
            base *= base;
        }
    }
    return out;
}

static BigInt power_tail_sum(int x, int k) {
    BigInt sum(0);
    BigInt term(1);
    for (int j = 1; j <= k + 1; ++j) {
        term.mul_uint(static_cast<std::uint64_t>(x));
        sum.add(term);
    }
    return sum;
}

static std::vector<int> primes_up_to(int limit) {
    std::vector<bool> composite(limit + 1, false);
    std::vector<int> primes;
    for (int n = 2; n <= limit; ++n) {
        if (!composite[n]) {
            primes.push_back(n);
            if (static_cast<std::int64_t>(n) * n <= limit) {
                for (std::int64_t m = static_cast<std::int64_t>(n) * n; m <= limit; m += n) {
                    composite[static_cast<std::size_t>(m)] = true;
                }
            }
        }
    }
    return primes;
}

static bool contains_prime(const std::vector<int>& primes, int n) {
    return std::binary_search(primes.begin(), primes.end(), n);
}

static int next_prime_after(const std::vector<int>& primes, int n) {
    auto it = std::upper_bound(primes.begin(), primes.end(), n);
    if (it == primes.end()) {
        throw std::runtime_error("prime table too small");
    }
    return *it;
}

static int expected_gap(int m) {
    return std::min({3 * m + 1, 2 * m + 21, 83});
}

static void strip_trailing_cr(std::string& line) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
}

static int parse_int_field(const std::string& field, const std::string& context) {
    if (field.empty()) {
        throw std::runtime_error("empty integer field in " + context);
    }
    std::size_t consumed = 0;
    int value = 0;
    try {
        value = std::stoi(field, &consumed);
    } catch (const std::exception&) {
        throw std::runtime_error("malformed integer field in " + context + ": " + field);
    }
    if (consumed != field.size()) {
        throw std::runtime_error("malformed integer field in " + context + ": " + field);
    }
    return value;
}

static std::vector<Row> read_certificate(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw std::runtime_error("could not open certificate: " + path);
    }

    std::string line;
    if (!std::getline(in, line)) {
        throw std::runtime_error("missing certificate header");
    }
    strip_trailing_cr(line);
    if (line != "m,T_m,g_m") {
        throw std::runtime_error("unexpected certificate header");
    }

    std::vector<Row> rows;
    std::uint64_t line_number = 1;
    while (std::getline(in, line)) {
        ++line_number;
        strip_trailing_cr(line);
        if (line.empty()) {
            continue;
        }
        if (std::count(line.begin(), line.end(), ',') != 2) {
            throw std::runtime_error("certificate row must contain exactly three columns");
        }
        std::istringstream parts(line);
        std::string m_field;
        std::string T_field;
        std::string g_field;
        std::getline(parts, m_field, ',');
        std::getline(parts, T_field, ',');
        std::getline(parts, g_field, ',');
        const std::string context = "certificate row " + std::to_string(line_number);
        Row row{};
        row.m = parse_int_field(m_field, context);
        row.T = parse_int_field(T_field, context);
        row.g = parse_int_field(g_field, context);
        rows.push_back(row);
    }
    return rows;
}

static void verify_handoff_row(const Row& row) {
    const int a = row.m + 1;
    const BigInt lhs =
        ipow(BigInt(36), 4 * a) * ipow(BigInt(64), 4) * ipow(BigInt(row.T), 20 + a);
    const BigInt rhs = ipow(BigInt(row.T - row.g), 4 * a);
    if (lhs > rhs) {
        std::ostringstream msg;
        msg << "handoff inequality failed for m=" << row.m;
        throw std::runtime_error(msg.str());
    }
}

static int cutoff_for_level(const std::vector<int>& primes, const BigInt& p4, int k) {
    int cutoff = 0;
    for (int q : primes) {
        if (power_tail_sum(q, k) < p4) {
            cutoff = q;
        } else if (q > cutoff) {
            return cutoff;
        }
    }
    throw std::runtime_error("prime table too small for cutoff verification");
}

static void verify_finite_prefix_row(const Row& row, const std::vector<int>& primes) {
    std::uint64_t checked = 0;
    std::int64_t worst_margin = 0;
    int worst_t = 0;

    for (int t : primes) {
        if (t < 83) {
            continue;
        }
        if (t >= row.T) {
            break;
        }
        const int successor = next_prime_after(primes, t);
        const BigInt p4 = power_tail_sum(successor, 4);

        std::int64_t tail_sum = 0;
        for (int k = row.m; power_tail_sum(2, k) < p4; ++k) {
            tail_sum += cutoff_for_level(primes, p4, k);
            if (tail_sum > t) {
                break;
            }
        }

        const std::int64_t margin =
            static_cast<std::int64_t>(t) - row.g - tail_sum;
        if (checked == 0 || margin < worst_margin) {
            worst_margin = margin;
            worst_t = t;
        }
        if (margin < 0) {
            std::ostringstream msg;
            msg << "finite prefix failed for m=" << row.m
                << ", t=" << t
                << ", tail_sum=" << tail_sum
                << ", margin=" << margin;
            throw std::runtime_error(msg.str());
        }
        ++checked;
    }

    std::cout << "row ok: m=" << row.m
              << " T_m=" << row.T
              << " g_m=" << row.g
              << " checked_primes=" << checked
              << " worst_t=" << worst_t
              << " worst_margin=" << worst_margin
              << "\n";
}

int main(int argc, char** argv) {
    try {
        const std::string certificate_path = argc >= 2 ? argv[1] : "certificate.csv";
        const std::vector<Row> rows = read_certificate(certificate_path);
        if (rows.size() != 19) {
            throw std::runtime_error("certificate must contain exactly 19 rows");
        }

        int max_T = 0;
        for (std::size_t i = 0; i < rows.size(); ++i) {
            const Row& row = rows[i];
            const int expected_m = 13 + static_cast<int>(i);
            if (row.m != expected_m) {
                throw std::runtime_error("certificate rows must be ordered by m=13..31");
            }
            if (row.g != expected_gap(row.m)) {
                throw std::runtime_error("gap does not match min(3m+1,2m+21,83)");
            }
            if (row.T <= row.g || row.T < 256) {
                throw std::runtime_error("handoff threshold is outside the certified range");
            }
            max_T = std::max(max_T, row.T);
        }

        const std::vector<int> primes = primes_up_to(max_T + 4096);
        for (const Row& row : rows) {
            if (!contains_prime(primes, 83)) {
                throw std::runtime_error("internal prime table failure");
            }
            verify_handoff_row(row);
            verify_finite_prefix_row(row, primes);
        }

        std::cout << "ACCEPT threshold83 tail certificate\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "REJECT threshold83 tail certificate: " << ex.what() << "\n";
        return 1;
    }
}
