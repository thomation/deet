# deet
斯坦福CS110L的项目，用C重写了它的框架。

https://reberhardt.com/cs110l/spring-2020/assignments/project-1/
# ptrace
学习了祁祁不正经的文章和示例代码。

https://zhuanlan.zhihu.com/p/653385264

# dwarf
地址和代码的对应关系，就得从dwarf中找，使用库来解析爱麻烦了，暂时用linux命令
## 根据地址获得函数名和所在文件行
addr2line -e samples/segfault -f -C -p 0x401186
## 所有源码和地址关系
objdump -S -l samples/segfault