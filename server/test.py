import os
import subprocess
print(os.chdir("./src"))
print(os.getcwd())

try:
    p = subprocess.run(
        ['./main.exe', '-i', './test_in.txt', '-o', 'tree.json', "--lex"],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        timeout=10,
    )
except subprocess.TimeoutExpired:
    print('服务器繁忙')

if p.returncode != 0:
    print('程序错误')

try:
    output = p.stdout.decode('utf-8') # 注意这里是win环境
except UnicodeDecodeError:
    print('编码错误')
print(output)