首先要安装Code::blocks13.12
打开Code::blocks，菜单Settings -> Compiler…->勾选C++11 ISO 开启C++11编译选项。

生成预测分析表程序：
输入为in.txt(文法)  输出为 table.txt(分析表)
在生成预测分析表文件夹->打开gen_AnalyseTable.cbp文件  F9运行即可。

编译器程序：
输入为：in.txt(源程序) table.txt（分析表）
输出为：Asm.txt（汇编语言） VarTable.txt(变量表) resault.txt（语法分析过程）TokenList.txt(词法分析过程) QuadList.txt(四元式)
在编译器文件夹->打开词法-语法LL(1)分析程序.cbp文件  F9运行即可。