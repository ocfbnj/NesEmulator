#ifndef LITERALS_H
#define LITERALS_H

constexpr unsigned long long operator"" _b(unsigned long long n) {
    return n;
}

constexpr unsigned long long operator"" _kb(unsigned long long n) {
    return 1024 * n;
}

#endif
