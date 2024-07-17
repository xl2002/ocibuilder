# Diff Details

Date : 2024-07-16 14:29:26

Directory d:\\BaiduSyncdisk\\OCI

Total : 105 files,  409 codes, 163 comments, -707 blanks, all -135 lines

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details

## Files
| filename | language | code | comment | blank | total |
| :--- | :--- | ---: | ---: | ---: | ---: |
| [Makefile](/Makefile) | Makefile | 50 | 30 | 23 | 103 |
| [execute/Config_Json.cpp](/execute/Config_Json.cpp) | C++ | -8 | -10 | -1 | -19 |
| [execute/args.cpp](/execute/args.cpp) | C++ | -10 | -7 | 0 | -17 |
| [execute/build.cpp](/execute/build.cpp) | C++ | -185 | -67 | -16 | -268 |
| [execute/cli/build.cpp](/execute/cli/build.cpp) | C++ | 3 | 0 | 3 | 6 |
| [execute/cli/common.cpp](/execute/cli/common.cpp) | C++ | 29 | 24 | 5 | 58 |
| [execute/cmd/build.cpp](/execute/cmd/build.cpp) | C++ | 206 | 72 | 19 | 297 |
| [execute/cmd/config.cpp](/execute/cmd/config.cpp) | C++ | 15 | 23 | 4 | 42 |
| [execute/cmd/images.cpp](/execute/cmd/images.cpp) | C++ | 15 | 21 | 4 | 40 |
| [execute/cmd/info.cpp](/execute/cmd/info.cpp) | C++ | 11 | 19 | 2 | 32 |
| [execute/cmd/inspect.cpp](/execute/cmd/inspect.cpp) | C++ | 11 | 19 | 2 | 32 |
| [execute/cmd/login.cpp](/execute/cmd/login.cpp) | C++ | 13 | 21 | 3 | 37 |
| [execute/cmd/logout.cpp](/execute/cmd/logout.cpp) | C++ | 13 | 21 | 3 | 37 |
| [execute/cmd/pull.cpp](/execute/cmd/pull.cpp) | C++ | 17 | 21 | 4 | 42 |
| [execute/cmd/push.cpp](/execute/cmd/push.cpp) | C++ | 15 | 21 | 3 | 39 |
| [execute/cmd/rmi.cpp](/execute/cmd/rmi.cpp) | C++ | 11 | 19 | 2 | 32 |
| [execute/cmd/root.cpp](/execute/cmd/root.cpp) | C++ | 49 | 17 | 10 | 76 |
| [execute/cmd/tag.cpp](/execute/cmd/tag.cpp) | C++ | 12 | 19 | 2 | 33 |
| [execute/cmd/version.cpp](/execute/cmd/version.cpp) | C++ | 13 | 21 | 3 | 37 |
| [execute/cobra/args.cpp](/execute/cobra/args.cpp) | C++ | 10 | 7 | 0 | 17 |
| [execute/cobra/command.cpp](/execute/cobra/command.cpp) | C++ | 530 | 295 | 24 | 849 |
| [execute/cobra/error.cpp](/execute/cobra/error.cpp) | C++ | 1 | 0 | 2 | 3 |
| [execute/cobra/flag.cpp](/execute/cobra/flag.cpp) | C++ | 279 | 278 | 12 | 569 |
| [execute/command.cpp](/execute/command.cpp) | C++ | -530 | -295 | -24 | -849 |
| [execute/common.cpp](/execute/common.cpp) | C++ | -26 | -24 | -4 | -54 |
| [execute/config.cpp](/execute/config.cpp) | C++ | -15 | -23 | -4 | -42 |
| [execute/config/config.cpp](/execute/config/config.cpp) | C++ | 8 | 10 | 1 | 19 |
| [execute/define/build.cpp](/execute/define/build.cpp) | C++ | 68 | 12 | 4 | 84 |
| [execute/define/define.cpp](/execute/define/define.cpp) | C++ | 0 | 10 | 1 | 11 |
| [execute/docker/reference.cpp](/execute/docker/reference.cpp) | C++ | 11 | 0 | 2 | 13 |
| [execute/error.cpp](/execute/error.cpp) | C++ | -1 | 0 | -2 | -3 |
| [execute/flag.cpp](/execute/flag.cpp) | C++ | -278 | -278 | -12 | -568 |
| [execute/imagebuildah/build.cpp](/execute/imagebuildah/build.cpp) | C++ | 4 | 0 | 3 | 7 |
| [execute/images.cpp](/execute/images.cpp) | C++ | -15 | -21 | -4 | -40 |
| [execute/info.cpp](/execute/info.cpp) | C++ | -11 | -19 | -2 | -32 |
| [execute/inspect.cpp](/execute/inspect.cpp) | C++ | -11 | -19 | -2 | -32 |
| [execute/login.cpp](/execute/login.cpp) | C++ | -13 | -21 | -3 | -37 |
| [execute/logout.cpp](/execute/logout.cpp) | C++ | -13 | -21 | -3 | -37 |
| [execute/main.cpp](/execute/main.cpp) | C++ | 26 | 26 | 6 | 58 |
| [execute/options.cpp](/execute/options.cpp) | C++ | -67 | -74 | -4 | -145 |
| [execute/pflag/value.cpp](/execute/pflag/value.cpp) | C++ | 165 | 202 | 3 | 370 |
| [execute/pull.cpp](/execute/pull.cpp) | C++ | -17 | -21 | -3 | -41 |
| [execute/push.cpp](/execute/push.cpp) | C++ | -15 | -21 | -3 | -39 |
| [execute/rmi.cpp](/execute/rmi.cpp) | C++ | -11 | -19 | -2 | -32 |
| [execute/root.cpp](/execute/root.cpp) | C++ | -48 | -17 | -10 | -75 |
| [execute/storage/storage.cpp](/execute/storage/storage.cpp) | C++ | 5 | 0 | 1 | 6 |
| [execute/storage/store.cpp](/execute/storage/store.cpp) | C++ | 0 | 0 | 3 | 3 |
| [execute/storage/utils.cpp](/execute/storage/utils.cpp) | C++ | 67 | 74 | 4 | 145 |
| [execute/tag.cpp](/execute/tag.cpp) | C++ | -12 | -19 | -2 | -33 |
| [execute/value.cpp](/execute/value.cpp) | C++ | -165 | -202 | -3 | -370 |
| [execute/version.cpp](/execute/version.cpp) | C++ | -13 | -21 | -3 | -37 |
| [include/Config_Json.h](/include/Config_Json.h) | C++ | -29 | -14 | -18 | -61 |
| [include/args.h](/include/args.h) | C++ | -13 | -11 | -2 | -26 |
| [include/build.h](/include/build.h) | C++ | -200 | -184 | -4 | -388 |
| [include/cli/build.h](/include/cli/build.h) | C++ | 96 | 33 | 7 | 136 |
| [include/cli/common.h](/include/cli/common.h) | C++ | 14 | 10 | 1 | 25 |
| [include/cmd/build.h](/include/cmd/build.h) | C++ | 27 | 15 | 2 | 44 |
| [include/cmd/config.h](/include/cmd/config.h) | C++ | 23 | 15 | 1 | 39 |
| [include/cmd/images.h](/include/cmd/images.h) | C++ | 15 | 16 | 1 | 32 |
| [include/cmd/info.h](/include/cmd/info.h) | C++ | 11 | 10 | 1 | 22 |
| [include/cmd/inspect.h](/include/cmd/inspect.h) | C++ | 11 | 10 | 1 | 22 |
| [include/cmd/login.h](/include/cmd/login.h) | C++ | 17 | 15 | 1 | 33 |
| [include/cmd/logout.h](/include/cmd/logout.h) | C++ | 15 | 16 | 3 | 34 |
| [include/cmd/pull.h](/include/cmd/pull.h) | C++ | 15 | 15 | 1 | 31 |
| [include/cmd/push.h](/include/cmd/push.h) | C++ | 15 | 15 | 1 | 31 |
| [include/cmd/rmi.h](/include/cmd/rmi.h) | C++ | 11 | 10 | 1 | 22 |
| [include/cmd/root.h](/include/cmd/root.h) | C++ | 21 | 22 | 4 | 47 |
| [include/cmd/tag.h](/include/cmd/tag.h) | C++ | 11 | 10 | 1 | 22 |
| [include/cmd/version.h](/include/cmd/version.h) | C++ | 15 | 15 | 1 | 31 |
| [include/cobra/args.h](/include/cobra/args.h) | C++ | 13 | 11 | 2 | 26 |
| [include/cobra/command.h](/include/cobra/command.h) | C++ | 92 | 50 | 7 | 149 |
| [include/cobra/error.h](/include/cobra/error.h) | C++ | 20 | 19 | 5 | 44 |
| [include/cobra/flag.h](/include/cobra/flag.h) | C++ | 75 | 27 | 5 | 107 |
| [include/command.h](/include/command.h) | C++ | -93 | -48 | -7 | -148 |
| [include/common.h](/include/common.h) | C++ | -10 | -10 | -1 | -21 |
| [include/config.h](/include/config.h) | C++ | -23 | -15 | -1 | -39 |
| [include/config/config.h](/include/config/config.h) | C++ | 29 | 14 | 18 | 61 |
| [include/context.h](/include/context.h) | C++ | -9 | -2 | -5 | -16 |
| [include/creat.h](/include/creat.h) | C++ | -7 | -14 | -797 | -818 |
| [include/define/build.h](/include/define/build.h) | C++ | 205 | 236 | 16 | 457 |
| [include/docker/reference.h](/include/docker/reference.h) | C++ | 29 | 0 | 2 | 31 |
| [include/error.h](/include/error.h) | C++ | -20 | -19 | -5 | -44 |
| [include/flag.h](/include/flag.h) | C++ | -76 | -27 | -4 | -107 |
| [include/github/digest.h](/include/github/digest.h) | C++ | 11 | 0 | 4 | 15 |
| [include/imagebuildah/build.h](/include/imagebuildah/build.h) | C++ | 12 | 0 | 3 | 15 |
| [include/images.h](/include/images.h) | C++ | -15 | -16 | -1 | -32 |
| [include/info.h](/include/info.h) | C++ | -11 | -10 | -1 | -22 |
| [include/inspect.h](/include/inspect.h) | C++ | -11 | -10 | -1 | -22 |
| [include/login.h](/include/login.h) | C++ | -17 | -15 | -1 | -33 |
| [include/logout.h](/include/logout.h) | C++ | -15 | -16 | -3 | -34 |
| [include/options.h](/include/options.h) | C++ | -34 | -16 | -4 | -54 |
| [include/pflag/value.h](/include/pflag/value.h) | C++ | 88 | 42 | 2 | 132 |
| [include/pkg/idtools/idtools.h](/include/pkg/idtools/idtools.h) | C++ | 10 | 0 | 4 | 14 |
| [include/pull.h](/include/pull.h) | C++ | -17 | -15 | -1 | -33 |
| [include/push.h](/include/push.h) | C++ | -15 | -15 | -1 | -31 |
| [include/rmi.h](/include/rmi.h) | C++ | -11 | -10 | -1 | -22 |
| [include/root.h](/include/root.h) | C++ | -20 | -22 | -4 | -46 |
| [include/storage/driver/driver.h](/include/storage/driver/driver.h) | C++ | 22 | 3 | 4 | 29 |
| [include/storage/layers.h](/include/storage/layers.h) | C++ | 16 | 0 | 3 | 19 |
| [include/storage/storage.h](/include/storage/storage.h) | C++ | 29 | 17 | 8 | 54 |
| [include/storage/utils.h](/include/storage/utils.h) | C++ | 34 | 16 | 4 | 54 |
| [include/tag.h](/include/tag.h) | C++ | -11 | -10 | -1 | -22 |
| [include/value.h](/include/value.h) | C++ | -88 | -42 | -2 | -132 |
| [include/version.h](/include/version.h) | C++ | -15 | -15 | -1 | -31 |
| [main.cpp](/main.cpp) | C++ | -26 | -26 | -6 | -58 |

[Summary](results.md) / [Details](details.md) / [Diff Summary](diff.md) / Diff Details