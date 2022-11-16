from flask import Flask, render_template,jsonify, request
import os
import subprocess

app = Flask(__name__)


# def resolve()

@app.route("/")
def index():
    print(os.getcwd())
    return render_template("index.html")

@app.route("/tree")
def tree_index():
    return render_template("tree.html")


@app.route('/getGramParse', methods=['post', 'get'])
def getGramParse():
    # 获取body
    sourceCode = request.get_json()['data']

    input_path = '../src/in.txt'
    output_path = '../src/out.json'
    lexput_path = '../src/lex.txt'
    # 写入test_in.txt
    with open(input_path, 'w') as f:
        f.write(sourceCode)

    # 调用语法分析
    old_cwd = os.getcwd()
    os.chdir("../src")
    print(os.getcwd())
    try:
        p = subprocess.run(
            ['./main.exe', '-i', './in.txt', '-go', './tree.json', '-lo', './lex.txt'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, 
            timeout=10,
        )
    except subprocess.TimeoutExpired:
        print('服务器繁忙')

    os.chdir(old_cwd) # 恢复工作路径
    print(os.getcwd())


    if p.returncode != 0:
        print('程序错误')
    else:
        # 读取语法结果json
        with open(output_path, 'rb') as f:
            tree_data = f.read().decode('utf-8')
        # 读取此法结果txt
        with open(lexput_path, 'rb') as f:
            lex_data = f.read().decode('utf-8')


    try:
        output = p.stderr.decode('utf-8') # 注意这里是win环境
    except UnicodeDecodeError:
        print('编码错误')
    
    print(output)
    

    # 获取结果
    # 成功返回json
    if(p.returncode == 0):

        data = {
            "status": 0,
            "res" : (tree_data),
            "lexres": (lex_data)
        }
        return jsonify(data)
        
    # 失败返回错误信息
    else:
        data = {
            "status": -1,
            "res" : (output)
        }
        return jsonify(data)


if __name__ == '__main__':
    app.run(port=5500, host="localhost", debug=True)
