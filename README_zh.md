# BigInt 库

**其他语言版本: [English](README.md), [中文](README_zh.md).**

## 概述
`BigInt` 库是一个 C++ 实现，用于处理任意大小的整数，提供广泛的算术运算、比较和实用函数。该库旨在方便进行超出标准整数类型限制的计算。

## 特性
- **任意精度**：支持任意大小的整数，限制仅受系统内存的约束。
- **基本算术运算**：加法、减法、乘法和除法。
- **位运算**：支持按位与、或、异或运算。
- **比较运算符**：便于比较大整数。
- **输入/输出**：从流中读写 `BigInt` 对象。
- **字符串转换**：轻松在 `BigInt` 和字符串表示之间转换。
- **模运算**：支持模运算和求逆计算。

## 安装
要在项目中使用 `BigInt` 库，请按照以下步骤操作：

1. 克隆代码库：
   ```bash
   git clone <repository-url>
   cd BigInt
   ```

2. 将 `BigInt.h` 和 `BigInt.cpp` 文件添加到你的项目中。

3. 在代码中包含该库：
   ```cpp
   #include "BigInt.h"
   ```

## 用法
以下是如何使用 `BigInt` 库的基本示例：

```cpp
#include <iostream>
#include "BigInt.h"

int main() {
    BigInt a("123456789012345678901234567890");
    BigInt b("987654321098765432109876543210");

    BigInt sum = a + b;
    std::cout << "和: " << sum.to_str() << std::endl;

    BigInt product = a * b;
    std::cout << "积: " << product.to_str() << std::endl;

    return 0;
}
```

## API 文档
有关 `BigInt` 类及其方法的详细信息，请参阅头文件 `BigInt.h`。

### 构造函数概述
- `BigInt()`: 默认构造函数。
- `BigInt(bool negative, const std::vector<DataType>& data)`: 使用符号和向量数据初始化。需要注意使用小端序哦。
- `BigInt(const std::string &s)`: 从字符串表示初始化。

### 算术运算
- `BigInt operator+(const BigInt &num) const`: 加法。
- `BigInt operator-(const BigInt &num) const`: 减法。
- `BigInt operator*(const BigInt &num) const`: 乘法。
- `BigInt operator/(const BigInt &num) const`: 除法。
- `BigInt operator%(const BigInt &num) const`: 取模。

### 比较运算符
- `bool operator<(const BigInt &num) const`: 小于。
- `bool operator>(const BigInt &num) const`: 大于。
- `bool operator==(const BigInt &num) const`: 等于检查。

## 贡献
欢迎贡献！欢迎提交 bug 报告、功能请求或改进建议。

## 许可证
本项目遵循 MIT 许可证 - 详情请参见 [LICENSE](LICENSE) 文件。

