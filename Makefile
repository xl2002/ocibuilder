# 主Makefile

ifeq ($(MF), M1)
# Makefile1不能增量编译
    include Makefile1
else
# Makefile2可以增量编译
    include Makefile2
endif

# 默认目标（可以选择执行主Makefile中的默认目标）
# all:
# 	@echo "Executing"
