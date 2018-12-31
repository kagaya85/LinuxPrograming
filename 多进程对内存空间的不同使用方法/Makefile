dirs:=
subdir=$(shell find . -maxdepth 1 -type d)
# 找到当前目录低一级的文件夹
dirs+=$(filter-out ./common,$(subdir))
# 去除$subdir里的./common路径 加入
dirs:=$(basename $(patsubst ./%,%,$(dirs)))
# basename NAMES:
# 从文件名序列“NAMES…”中取出各个文件名的前缀部分
# $(patsubst <pattern>,<replacement>,<text> ) :
# 查找<text>中的单词（单词以“空格”、“Tab”或“回车”“换行”分隔）
# 是否符合模式<pattern>，如果匹配的话，则以<replacement>替换。
# 去除连接库：这里不需要 dirs:=$(filter-out $(exclude_dirs),$(dirs))


.PHONY: $(dirs) clean
$(dirs):
	@for dir in $(dirs); \
	do \
    	$(MAKE) -C $$dir ; \
    done

clean:
	@for dir in $(dirs); \
	do \
		$(MAKE) -C $$dir clean; \
	done
