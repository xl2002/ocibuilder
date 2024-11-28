#include "image/types/internal/types.h"

// std::string GetTempDir() {
//     try {
//         std::string tmpdir = GetEnvVar("TMPDIR");
//         if (!tmpdir.empty()) {
//             try {
//                 return GetAbsolutePath(tmpdir);
//             } catch (const std::exception &e) {
//                 std::cerr << "Ignoring TMPDIR from environment, evaluating it: " << e.what() << std::endl;
//             }
//         }

//         std::shared_ptr<Config> containerConfig = Config::Default();
//         try {
//             return containerConfig->ImageCopyTmpDir();
//         } catch (const std::exception &e) {
//             std::cerr << "Error getting ImageCopyTmpDir: " << e.what() << std::endl;
//         }
//     } catch (const std::exception &e) {
//         std::cerr << "Error getting default config: " << e.what() << std::endl;
//     }

//     return "/var/tmp";
// }
