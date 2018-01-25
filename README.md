## 开发环境：
Code::blocks13.12

打开Code::blocks，菜单Settings -> Compiler…->勾选C++11 ISO 开启C++11编译选项。

## 生成预测分析表程序：
输入为in.txt(文法)  输出为 table.txt(分析表)

在生成预测分析表文件夹->打开gen_AnalyseTable.cbp文件  F9运行即可。

## 编译器程序：
输入为：

in.txt(源程序) table.txt（分析表）

输出为：

Asm.txt（汇编语言） VarTable.txt(变量表) resault.txt（语法分析过程）TokenList.txt(词法分析过程) QuadList.txt(四元式)

在编译器文件夹->打开词法-语法LL(1)分析程序.cbp文件  F9运行即可。

## 主要功能

|  功能	| 说明 |
| :--------| :----|
| 可识别的关键字 | "int", "double", "bool", "string", "if", "else", "while", "for", "include", "read", "write","return", "true", "false"|
|可识别的运算符	|"!", "!=", "%", "*", "+", "++", "-", "--", "/", "<","<=", "=", "==", ">", ">=", "&&", "\|\|" |
|可识别的界符 | '(', ')', ',', ';', '[', ']', '{', '}', |
|支持浮点数和科学计数法 | 如-1.84652、1.45e-5|
|支持的数据类型 | int, double, bool, string|
|字符串赋值操作| str = “hello world\n”|
|支持循环、判断、分支的程序设计 | 支持if,if...else,for while, do...while	使用时都需要需要用｛｝包裹函数块
|支持语法错误提示 |	如：Error in Line [10] : 语句成分不完整：缺少"{"|
|支持语义分析和类型检查|	如：语义错误in line[6]：x该变量未定义。     语义错误in line[6]：int与string类型不匹配。|
|可依据文法自动生成预测分析表|	自动求First、Follow、Selcet并判断文法是否满足LL1|
