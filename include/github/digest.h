#if !defined(GITHUB_DIGEST_H)
#define GITHUB_DIGEST_H
#include <string>
#include <vector>
using std::string;
using std::vector;

class Digest{
    public:
    string digest;
     // 验证 digest 的有效性
    bool Validate() const {
        // 这里是示例，实际验证逻辑需要根据实际要求实现
        return !digest.empty();
    }

    // 重载小于运算符以便在 std::set 中使用
    bool operator<(const Digest& other) const {
        return digest < other.digest;
    }

};

#endif // GITHUB_DIGEST_H
