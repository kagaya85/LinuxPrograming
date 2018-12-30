RM = -rm -rf

# 获取子目录
dirs := $(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
dirs := $(filter-out $(exclude_dirs),$(dirs))
SUBDIRS := $(dirs)

# 当前目录下的源文件
SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)

all : subDirs

subDirs : $(SUBDIRS)
	for dir in $(SUBDIRS);\
	do make -C $$dir all||exit 1;\
	done

.PHONY: clean
clean : 
	for dir in $(SUBDIRS);\
	do make -C $$dir clean||exit 1;\
	done
	$(RM) $(TARGET) $(OBJS)