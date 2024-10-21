# Diff Details

Date : 2024-10-14 18:21:09

Directory e:\\BaiduSyncdisk\\OCI

Total : 73 files,  820 codes, 309 comments, 151 blanks, all 1280 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [Dockerfile](/Dockerfile) | Docker | -2 | 15 | 3 | 16 |
| [README.md](/README.md) | Markdown | 2 | 0 | 0 | 2 |
| [app/my_script.sh](/app/my_script.sh) | Shell Script | 1 | 2 | 2 | 5 |
| [execute/blobcache/blobcache.cpp](/execute/blobcache/blobcache.cpp) | C++ | 4 | 0 | 3 | 7 |
| [execute/buildah/buildah.cpp](/execute/buildah/buildah.cpp) | C++ | 7 | 0 | 0 | 7 |
| [execute/buildah/new.cpp](/execute/buildah/new.cpp) | C++ | 43 | 2 | 1 | 46 |
| [execute/buildah/push.cpp](/execute/buildah/push.cpp) | C++ | 16 | 0 | 1 | 17 |
| [execute/cli/build.cpp](/execute/cli/build.cpp) | C++ | 0 | 1 | 0 | 1 |
| [execute/cli/common.cpp](/execute/cli/common.cpp) | C++ | 7 | 0 | -1 | 6 |
| [execute/config/config.cpp](/execute/config/config.cpp) | C++ | 1 | 0 | 0 | 1 |
| [execute/config/default_linux.cpp](/execute/config/default_linux.cpp) | C++ | -1 | 2 | 0 | 1 |
| [execute/config/defaut.cpp](/execute/config/defaut.cpp) | C++ | 1 | -2 | 0 | -1 |
| [execute/config/new.cpp](/execute/config/new.cpp) | C++ | 6 | 3 | -2 | 7 |
| [execute/define/pull.cpp](/execute/define/pull.cpp) | C++ | 13 | 1 | 1 | 15 |
| [execute/docker/docker_transport.cpp](/execute/docker/docker_transport.cpp) | C++ | 54 | 2 | 5 | 61 |
| [execute/go/string.cpp](/execute/go/string.cpp) | C++ | 6 | 2 | 0 | 8 |
| [execute/graphdriver/driver.cpp](/execute/graphdriver/driver.cpp) | C++ | 0 | 2 | 1 | 3 |
| [execute/imagebuildah/build.cpp](/execute/imagebuildah/build.cpp) | C++ | 3 | 1 | 0 | 4 |
| [execute/imagebuildah/executor.cpp](/execute/imagebuildah/executor.cpp) | C++ | 5 | 1 | -2 | 4 |
| [execute/imagebuildah/stage_executor.cpp](/execute/imagebuildah/stage_executor.cpp) | C++ | 69 | 3 | 3 | 75 |
| [execute/imagebuilder/builder.cpp](/execute/imagebuilder/builder.cpp) | C++ | -6 | 22 | 0 | 16 |
| [execute/init/init_global.cpp](/execute/init/init_global.cpp) | C++ | 30 | 0 | -5 | 25 |
| [execute/libimage/pull.cpp](/execute/libimage/pull.cpp) | C++ | 1 | 0 | 2 | 3 |
| [execute/libimage/runtime.cpp](/execute/libimage/runtime.cpp) | C++ | 27 | 0 | 4 | 31 |
| [execute/lockfile/lastwrite.cpp](/execute/lockfile/lastwrite.cpp) | C++ | 19 | 7 | 6 | 32 |
| [execute/lockfile/lockfile.cpp](/execute/lockfile/lockfile.cpp) | C++ | -4 | 7 | -5 | -2 |
| [execute/logrus/entry.cpp](/execute/logrus/entry.cpp) | C++ | -2 | 5 | 0 | 3 |
| [execute/main/build.cpp](/execute/main/build.cpp) | C++ | 9 | -7 | 0 | 2 |
| [execute/parse/dockerfileparse.cpp](/execute/parse/dockerfileparse.cpp) | C++ | -3 | -9 | -2 | -14 |
| [execute/parse/parse.cpp](/execute/parse/parse.cpp) | C++ | 10 | 1 | -2 | 9 |
| [execute/parse/split_command.cpp](/execute/parse/split_command.cpp) | C++ | -1 | 1 | 0 | 0 |
| [execute/pflag/value.cpp](/execute/pflag/value.cpp) | C++ | 0 | 1 | 0 | 1 |
| [execute/reference/normalize.cpp](/execute/reference/normalize.cpp) | C++ | 48 | 13 | 13 | 74 |
| [execute/reference/reference.cpp](/execute/reference/reference.cpp) | C++ | 38 | 6 | 0 | 44 |
| [execute/reference/regexp.cpp](/execute/reference/regexp.cpp) | C++ | 0 | 0 | 1 | 1 |
| [execute/regexp/regexp.cpp](/execute/regexp/regexp.cpp) | C++ | 5 | 10 | 2 | 17 |
| [execute/storage/storage.cpp](/execute/storage/storage.cpp) | C++ | 0 | 8 | -1 | 7 |
| [execute/toml/decode.cpp](/execute/toml/decode.cpp) | C++ | 1 | 0 | 1 | 2 |
| [execute/transports/transports.cpp](/execute/transports/transports.cpp) | C++ | -1 | 1 | 0 | 0 |
| [execute/types/options.cpp](/execute/types/options.cpp) | C++ | -3 | 33 | 2 | 32 |
| [include/blobcache/blobcache.h](/include/blobcache/blobcache.h) | C++ | 15 | 0 | 4 | 19 |
| [include/buildah/buildah.h](/include/buildah/buildah.h) | C++ | 71 | 63 | 0 | 134 |
| [include/buildah/new.h](/include/buildah/new.h) | C++ | 7 | 0 | 4 | 11 |
| [include/buildah/push.h](/include/buildah/push.h) | C++ | 9 | 0 | 3 | 12 |
| [include/bytes/buffer.h](/include/bytes/buffer.h) | C++ | 2 | 1 | 0 | 3 |
| [include/config/new.h](/include/config/new.h) | C++ | 2 | -2 | 0 | 0 |
| [include/define/pull.h](/include/define/pull.h) | C++ | 2 | 0 | -1 | 1 |
| [include/docker/docker_transport.h](/include/docker/docker_transport.h) | C++ | 22 | 1 | 5 | 28 |
| [include/go/string.h](/include/go/string.h) | C++ | 1 | 0 | 0 | 1 |
| [include/graphdriver/driver.h](/include/graphdriver/driver.h) | C++ | 5 | 1 | 1 | 7 |
| [include/imagebuildah/executor.h](/include/imagebuildah/executor.h) | C++ | 3 | -2 | 0 | 1 |
| [include/imagebuildah/stage_executor.h](/include/imagebuildah/stage_executor.h) | C++ | 5 | 0 | 0 | 5 |
| [include/imagebuilder/builder.h](/include/imagebuilder/builder.h) | C++ | 3 | 0 | 0 | 3 |
| [include/init/init_global.h](/include/init/init_global.h) | C++ | 6 | 0 | -1 | 5 |
| [include/libimage/copier.h](/include/libimage/copier.h) | C++ | 50 | 43 | 44 | 137 |
| [include/libimage/events.h](/include/libimage/events.h) | C++ | 31 | 17 | 2 | 50 |
| [include/libimage/image.h](/include/libimage/image.h) | C++ | 26 | 16 | 22 | 64 |
| [include/libimage/manifest.h](/include/libimage/manifest.h) | C++ | 11 | 0 | 2 | 13 |
| [include/libimage/pull.h](/include/libimage/pull.h) | C++ | 7 | 0 | 1 | 8 |
| [include/libimage/runtime.h](/include/libimage/runtime.h) | C++ | 19 | 4 | 4 | 27 |
| [include/lockfile/lastwrite.h](/include/lockfile/lastwrite.h) | C++ | 14 | 0 | -2 | 12 |
| [include/lockfile/lockfile.h](/include/lockfile/lockfile.h) | C++ | 1 | 0 | 0 | 1 |
| [include/parse/dockerfileparse.h](/include/parse/dockerfileparse.h) | C++ | 2 | 0 | 0 | 2 |
| [include/reference/normalize.h](/include/reference/normalize.h) | C++ | 9 | 0 | 3 | 12 |
| [include/reference/reference.h](/include/reference/reference.h) | C++ | 40 | 2 | 2 | 44 |
| [include/reference/regexp.h](/include/reference/regexp.h) | C++ | 13 | 0 | 3 | 16 |
| [include/signer/signer.h](/include/signer/signer.h) | C++ | 8 | 0 | -1 | 7 |
| [include/storage/storage.h](/include/storage/storage.h) | C++ | 1 | 0 | -1 | 0 |
| [include/toml/decode.h](/include/toml/decode.h) | C++ | 3 | 0 | 4 | 7 |
| [include/transports/transports.h](/include/transports/transports.h) | C++ | 2 | 0 | -1 | 1 |
| [include/types/options.h](/include/types/options.h) | C++ | 0 | 1 | 0 | 1 |
| [include/types/types.h](/include/types/types.h) | C++ | 31 | 17 | 15 | 63 |
| [test.cpp](/test.cpp) | C++ | -4 | 13 | 8 | 17 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details