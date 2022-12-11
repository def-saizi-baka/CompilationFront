import os
import chardet

def decectCode(filePath):
    with open(filePath,"rb") as f:
        return chardet.detect(f.read())['encoding']
        

def ReadFile(filePath,encoding):
    with open(filePath,"r",encoding = encoding) as f:
        return f.read()
 
def WriteFile(filePath,u,encoding):
    with open(filePath,"w",encoding = encoding) as f:
        f.write(u)
 
def UTF8_2_GBK(src,dst):
    print(src)
    content = ReadFile(src,encoding="utf-8")
    WriteFile(dst,content,encoding="gb18030")

def gci(filepath):
#遍历filepath下所有文件，包括子目录
    files = os.listdir(filepath)
    # print(files)
    for fi in files:
        fi_d = os.path.join(filepath,fi)           
        if os.path.isdir(fi_d):
            # if('Debug' not in fi_d):
            #     print("--------------" + fi_d + "--------------------")
            gci(fi_d)                  
        else:
            if('.cpp' in fi_d or '.h' in fi_d or '.txt' in fi_d):
                a = 1
                # print(fi_d)
                if(decectCode(fi_d) in ['utf-8','None']):
                    UTF8_2_GBK(fi_d, fi_d)

if __name__ == "__main__":
    gci('./')