# 编译原理大作业
## repo

https://github.com/def-saizi-baka/Lexical_analyzer

## 视频演示

https://space.bilibili.com/22761084

## 命令行

编译运行

进入src/ 目录，在windows下使用make.bat

参数说明

```
usage :
-h / --help                                    : 帮助信息
-v / --version                                 : 获得版本信息
-s / --save    [filepath]                      : 保存当前生成的DFA为模型文件
                                                   [filepath] 保存DFA模型的文件路径
-l / --load    [filepath]                      : 加载DFA模型，能够实现更快冷启动
                                                   默认使用${cwd}/dfamodel文件作为加载文件
                                                   [filepath] DFA模型路径
--lex                                          : 仅完成词法分析，输出为单词表示序列
-i / --infile  [filepath]                      : 输入的待分析文件
                                                   [filepath] 输入文件路径
-go / --gram_outfile [filepath]                : 输出的语法分析结果文件
                                                   [filepath] 输出文件路径
-lo / --lex_outfile [filepath]                 : 输出的词法分析结果文件
                                                    [filepath] 输出文件路径
-co / --code_outfile [filepath]				   : 输出的中间代码生成文件
													[filepath] 输出文件路径
-d / --debug                                   : 启动debug模式，会在命令行输出有效的中间数据
-pk / --path_keywords [filepath]               : 设置关键字的配置文件路径
                                                   [filepath] 的配置文件路径
-pd / --path_delimiters [filepath]             : 设置界符的配置文件路径
                                                   [filepath] 界符的配置文件路径
-po / --path_operator_symbols [filepath]       : 设置运算符的配置文件路径
                                                   [filepath] 运算符的配置文件路径             
-pu / --path_unstop_symbols [filepath]         : 设置非终结符的配置文件路径
                                                   [filepath] 非终结符的配置文件路径 
-pg / --path_grammar [filepath]                : 设置运算符的配置文件路径
                                                   [filepath] 运算符的配置文件路径 
-plog / --path_parserlog [filepath]            : 设置运行日志的配置文件路径
                                                   [filepath] 运行日志的配置文件路径 
```
以下是一些示例：
```
# 只做词法分析，将结果输出到lex_res.txt
main.exe --lex -i test_in.txt -lo lex_res.txt
# 语法分析，将结果输出
main.exe -i test_in.txt -go grammar.json
main.exe -i test_in.txt -go grammar.json -lo lex_res.txt
# 中间代码生成
main.exe -i test_in.txt -go grammar.json -lo lex_res.txt -co interCode.txt
```
