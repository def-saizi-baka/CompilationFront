# Lexical_analyzer
编译原理 词法分析器

参数说明

```
-s/--save dfapath 将dfa固化至dfapath
-l/--load dfapath 将dfa模型从dfapath中加载
-h / --help                    : 获得参数说明;
-v / --version                 : 获得版本信息;
-i / --infile  [filepath]      : 设置分析的类C文法的文本文件的路径;
                                   [filepath] 类C文法的文本文件的路径;
-o / --outfile [filepath]      : 设置分析结果的保存路径;
                                   [filepath] 分析结果的保存路径;
-pk / --path_keywords [filepath]      :设置关键字配置文件的路径;
                                       [filepath] 关键字配置文件的路径;
-pd / --path_delimiters [filepath]    : 设置界符配置文件的路径;
                                       [filepath] 界符配置文件的路径;
-po / --path_operator_symbols [filepath]      : 设置操作符配置文件的路径;
                                               [filepath] 操作符配置文件的路径;
-pu / --path_unstop_symbols [filepath]      : 设置非终结符配置文件的路径;
                                               [filepath] 设置非终结符配置文件的路径;
-pg / --path_grammar [filepath]             : 设置语法生成式配置文件的路径;
                                               [filepath] 语法生成式配置文件的路径;
-plog / --path_parserlog [filepath]             : 设置分析过程日志文件保存的路径;
                                               [filepath] 日志文件保存的路径;

例如可以采用下面的命令行方法来运行	
```
`main.exe -s dfamodel test_in.txt`
根目录下配置了`save_for_run.bat`,`load_for_run.bat`
