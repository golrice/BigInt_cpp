# BigInt Library

**Read this in other languages: [English](README.md), [中文](README_zh.md).**

## Overview
The `BigInt` library is a C++ implementation for handling arbitrarily large integers, providing a wide range of arithmetic operations, comparisons, and utility functions. This library is designed to facilitate computations that exceed the limits of standard integer types.

## Features
- **Arbitrary Precision**: Supports integers of any size, limited only by system memory.
- **Basic Arithmetic Operations**: Addition, subtraction, multiplication, and division.
- **Bitwise Operations**: Supports bitwise AND, OR, XOR operations.
- **Comparison Operators**: Easy comparison of large integers.
- **Input/Output**: Read and write `BigInt` objects from/to streams.
- **String Conversion**: Easily convert between `BigInt` and string representations.
- **Modular Arithmetic**: Support for modular operations and inverse calculations.

## Installation
To include the `BigInt` library in your project, follow these steps:

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd BigInt_cpp
   ```

2. Add the `BigInt.h` and `BigInt.cpp` files to your project.

3. Include the library in your code:
   ```cpp
   #include "BigInt.h"
   ```

## Usage
Here is a basic example of how to use the `BigInt` library:

```cpp
#include <iostream>
#include "BigInt.h"

int main() {
    BigInt a("123456789012345678901234567890");
    BigInt b("987654321098765432109876543210");

    BigInt sum = a + b;
    std::cout << "Sum: " << sum.to_str() << std::endl;

    BigInt product = a * b;
    std::cout << "Product: " << product.to_str() << std::endl;

    return 0;
}
```

## API Documentation
For detailed information about the `BigInt` class and its methods, please refer to the header file `BigInt.h`. 

### Constructor Overview
- `BigInt()`: Default constructor.
- `BigInt(bool negative, const std::vector<DataType>& data)`: Initialize with sign and vector data. Be careful when using this constructor, as it assumes that the input data is little-endian and in the correct order.
- `BigInt(const std::string &s)`: Initialize from a string representation.

### Arithmetic Operations
- `BigInt operator+(const BigInt &num) const`: Addition.
- `BigInt operator-(const BigInt &num) const`: Subtraction.
- `BigInt operator*(const BigInt &num) const`: Multiplication.
- `BigInt operator/(const BigInt &num) const`: Division.
- `BigInt operator%(const BigInt &num) const`: Modulus.

### Comparison Operators
- `bool operator<(const BigInt &num) const`: Less than.
- `bool operator>(const BigInt &num) const`: Greater than.
- `bool operator==(const BigInt &num) const`: Equality check.

## Testing
To ensure the accuracy of the library, comprehensive unit tests are included. You can run the tests using your preferred C++ testing framework.

## Contributing
Contributions are welcome! Please feel free to submit a pull request or open an issue for any feature requests or bug reports.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
