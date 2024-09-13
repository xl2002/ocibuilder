# Diff Details

Date : 2024-09-13 19:34:50

Directory d:\\BaiduSyncdisk\\OCI

Total : 62 files,  1953 codes, 222 comments, 283 blanks, all 2458 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [execute/buildah/buildah.cpp](/execute/buildah/buildah.cpp) | C++ | 241 | 1 | 19 | 261 |
| [execute/buildah/common.cpp](/execute/buildah/common.cpp) | C++ | 42 | 1 | 6 | 49 |
| [execute/buildah/config.cpp](/execute/buildah/config.cpp) | C++ | 4 | 0 | 1 | 5 |
| [execute/buildah/digester.cpp](/execute/buildah/digester.cpp) | C++ | 36 | 2 | 2 | 40 |
| [execute/buildah/image.cpp](/execute/buildah/image.cpp) | C++ | 5 | 0 | 3 | 8 |
| [execute/buildah/util.cpp](/execute/buildah/util.cpp) | C++ | 24 | 0 | 2 | 26 |
| [execute/digest/algorithm.cpp](/execute/digest/algorithm.cpp) | C++ | 9 | 12 | 4 | 25 |
| [execute/digest/digest.cpp](/execute/digest/digest.cpp) | C++ | 21 | 3 | 2 | 26 |
| [execute/go/string.cpp](/execute/go/string.cpp) | C++ | 24 | 7 | 3 | 34 |
| [execute/imagebuildah/stage_executor.cpp](/execute/imagebuildah/stage_executor.cpp) | C++ | 170 | 4 | 8 | 182 |
| [execute/imagebuilder/builder.cpp](/execute/imagebuilder/builder.cpp) | C++ | 135 | 1 | 16 | 152 |
| [execute/imagebuilder/dispatchers.cpp](/execute/imagebuilder/dispatchers.cpp) | C++ | 281 | 66 | 34 | 381 |
| [execute/manifest/manifest.cpp](/execute/manifest/manifest.cpp) | C++ | 4 | 0 | 3 | 7 |
| [execute/parse/dockerfileparse.cpp](/execute/parse/dockerfileparse.cpp) | C++ | 1 | 0 | 0 | 1 |
| [execute/reference/reference.cpp](/execute/reference/reference.cpp) | C++ | 3 | 0 | 2 | 5 |
| [execute/shell/lex.cpp](/execute/shell/lex.cpp) | C++ | 6 | 0 | 0 | 6 |
| [execute/signature/policy_config.cpp](/execute/signature/policy_config.cpp) | C++ | 29 | 1 | 7 | 37 |
| [execute/signature/policy_eval.cpp](/execute/signature/policy_eval.cpp) | C++ | 6 | 0 | 4 | 10 |
| [execute/signature/policy_types.cpp](/execute/signature/policy_types.cpp) | C++ | 1 | 0 | 0 | 1 |
| [execute/storage/storage_reference.cpp](/execute/storage/storage_reference.cpp) | C++ | 11 | 0 | 2 | 13 |
| [execute/storage/storage_transport.cpp](/execute/storage/storage_transport.cpp) | C++ | 3 | 0 | 1 | 4 |
| [execute/transports/transports.cpp](/execute/transports/transports.cpp) | C++ | 3 | 0 | 0 | 3 |
| [include/buildah/buildah.h](/include/buildah/buildah.h) | C++ | 36 | -1 | 2 | 37 |
| [include/buildah/commit.h](/include/buildah/commit.h) | C++ | 41 | 25 | 26 | 92 |
| [include/buildah/common.h](/include/buildah/common.h) | C++ | 41 | 0 | 5 | 46 |
| [include/buildah/digest.h](/include/buildah/digest.h) | C++ | -22 | -10 | -7 | -39 |
| [include/buildah/digester.h](/include/buildah/digester.h) | C++ | 30 | 10 | 8 | 48 |
| [include/buildah/image.h](/include/buildah/image.h) | C++ | 50 | 1 | 3 | 54 |
| [include/buildah/util.h](/include/buildah/util.h) | C++ | 20 | 0 | 3 | 23 |
| [include/config/config.h](/include/config/config.h) | C++ | 8 | 0 | -1 | 7 |
| [include/copy/copy.h](/include/copy/copy.h) | C++ | 53 | 2 | 12 | 67 |
| [include/define/types.h](/include/define/types.h) | C++ | 25 | 0 | 1 | 26 |
| [include/digest/algorithm.h](/include/digest/algorithm.h) | C++ | 23 | 0 | 3 | 26 |
| [include/digest/digest.h](/include/digest/digest.h) | C++ | 6 | 0 | 1 | 7 |
| [include/digest/digester.h](/include/digest/digester.h) | C++ | 13 | 2 | 4 | 19 |
| [include/docker/container.h](/include/docker/container.h) | C++ | -48 | 64 | 0 | 16 |
| [include/docker/types.h](/include/docker/types.h) | C++ | 96 | 23 | 21 | 140 |
| [include/go/hash.h](/include/go/hash.h) | C++ | 12 | 4 | 6 | 22 |
| [include/go/string.h](/include/go/string.h) | C++ | 3 | 0 | 0 | 3 |
| [include/homedir/homedir_unix.h](/include/homedir/homedir_unix.h) | C++ | 20 | 0 | 6 | 26 |
| [include/imagebuildah/stage_executor.h](/include/imagebuildah/stage_executor.h) | C++ | 15 | 0 | 1 | 16 |
| [include/imagebuilder/builder.h](/include/imagebuilder/builder.h) | C++ | 34 | 1 | -2 | 33 |
| [include/imagebuilder/dispatchers.h](/include/imagebuilder/dispatchers.h) | C++ | 69 | 0 | 11 | 80 |
| [include/imagebuilder/internals.h](/include/imagebuilder/internals.h) | C++ | 103 | 10 | 14 | 127 |
| [include/manifest/docker_schema2.h](/include/manifest/docker_schema2.h) | C++ | 52 | 10 | 6 | 68 |
| [include/manifest/manifest.h](/include/manifest/manifest.h) | C++ | 15 | 0 | 4 | 19 |
| [include/parse/dockerfileparse.h](/include/parse/dockerfileparse.h) | C++ | 20 | -18 | 1 | 3 |
| [include/platforms/default_unix.h](/include/platforms/default_unix.h) | C++ | 39 | 2 | 6 | 47 |
| [include/reference/reference.h](/include/reference/reference.h) | C++ | 1 | -1 | 1 | 1 |
| [include/shell/lex.h](/include/shell/lex.h) | C++ | 1 | 0 | -1 | 0 |
| [include/signature/policy_config.h](/include/signature/policy_config.h) | C++ | 18 | 0 | 5 | 23 |
| [include/signature/policy_eval.h](/include/signature/policy_eval.h) | C++ | 24 | 0 | 5 | 29 |
| [include/signature/policy_types.h](/include/signature/policy_types.h) | C++ | 21 | 0 | 4 | 25 |
| [include/signer/signer.h](/include/signer/signer.h) | C++ | 5 | 0 | 4 | 9 |
| [include/storage/images.h](/include/storage/images.h) | C++ | 27 | 16 | 16 | 59 |
| [include/storage/storage_reference.h](/include/storage/storage_reference.h) | C++ | 12 | 0 | -4 | 8 |
| [include/storage/storage_transport.h](/include/storage/storage_transport.h) | C++ | 4 | 0 | 0 | 4 |
| [include/strslice/strslice.h](/include/strslice/strslice.h) | C++ | 10 | 0 | 3 | 13 |
| [include/transports/transports.h](/include/transports/transports.h) | C++ | 1 | 0 | -1 | 0 |
| [include/types/types.h](/include/types/types.h) | C++ | 2 | -16 | -5 | -19 |
| [include/unshare/unshare.h](/include/unshare/unshare.h) | C++ | 13 | 0 | 3 | 16 |
| [include/v1/config.h](/include/v1/config.h) | C++ | 1 | 0 | 0 | 1 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details