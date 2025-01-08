#if !defined(IMAGE_DIGEST_DIGEST_H)
#define IMAGE_DIGEST_DIGEST_H
#include <string>
#include <vector>
#include "image/digest/hash.h"
#include "image/digest/algorithm.h"
#include "image/digest/verifier.h"
#include <boost/json.hpp>
using std::string;
using std::vector;
class Algorithm_sha256;
struct Verifier_interface;
class Digest{
    public:
    string digest;
    Digest()=default;
    Digest(string d):digest(d){};
    std::string Encoded() const;
    std::size_t sepIndex() const;
    std::string Hex();
    std::string String();
    std::shared_ptr<Algorithm_sha256> Algorithm();
    std::shared_ptr<Verifier_interface> Verifier();
    void Validate();
    bool operator< (const Digest& other) const {
        return digest < other.digest;
    }
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Digest& d) {
        jv = boost::json::object{
            {"digest", d.digest}
        };
    }
    friend Digest tag_invoke(boost::json::value_to_tag<Digest>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Digest d;
        d.digest = obj.at("digest").as_string().c_str();
        return d;
    }
};
std::shared_ptr<Digest> FromString(std::string s);
std::shared_ptr<::Digest> FromBytes(std::vector<uint8_t> p);
std::shared_ptr<Digest> Fromfile(const std::string& filepath);
std::shared_ptr<Digest> NewDigest(std::shared_ptr<Algorithm_sha256> alg,std::shared_ptr<Hash_256> hash);
std::shared_ptr<Digest> NewDigestFromBytes(std::shared_ptr<Algorithm_sha256> alg,std::vector<uint8_t> p);
std::shared_ptr<Digest> NewDigestFromEncoded(std::shared_ptr<Algorithm_sha256> alg,std::string p);

extern std::regex DigestRegexp;
extern std::regex DigestRegexpAnchored;



#endif // GITHUB_DIGEST_H
