var cache_tree = null
var now_id = 0

const app = document.querySelector('#app')
// // 打开网页时执行
window.onload = renderNodes
// // 窗口缩放时执行
window.onresize = renderNodes

// 窗口初始化
// function winInit(){
//     if(!root_index)
//         renderNodes()
// }



function renderNodes() {
    // console.log("1")
    window.scrollTo(0, 0)
    if(!window.opener){
        return
    }
    if(!cache_tree){
        // console.log("2")
        // console.log(cache_tree)
        cache_tree = window.opener.cache_tree
    }
    if(!cache_tree){
        // console.log("3")
        // console.log(cache_tree)
        // alert("未找到父窗口树")
        return
    }
    // 统计叶子节点个数
    var left_num = getTreeSonNodeNum(cache_tree)

    // 根据叶子节点数修改宽度
    app.style.width = left_num*70

    const html = `
        <div class="tree-container">${renderNode(cache_tree, null)}</div>
    `
    // 保证根节点 / 节点id正确
    now_id = 0
    app.innerHTML = html
    renderLines()
}

function renderNode(tree, parentId) {
    var node_name = tree["2.kind"]
    var node_children = tree["3.inner"]
    var is_root = tree["1.is_root"]
    now_id++
    var nowID = now_id
    const tree_html =  `
        <div class="tree">
            <div id="${now_id}" parent-id="${parentId}" class="${!is_root ? 'root-node' : 'tree-node'} node">
                ${node_name}
            </div>
            <div class="tree-children">
            ${typeof(node_children)!="undefined"
            ?
            node_children.map(child => {
                return renderNode(child, nowID)
            }).join('')
            :
            ''}
            </div>
        </div>
    `
    return tree_html;
}

function getTreeSonNodeNum(root){
    var node_children = root["3.inner"]
    if(typeof(node_children) == "undefined")
        return 1
    
    var node_num = 0;
    for(var i=0; i<node_children.length; i++){
        node_num+=getTreeSonNodeNum(node_children[i]);
    }
    return node_num;

}

function renderLines() {
    const nodes = document.querySelectorAll('.tree-node')
    let fragment = document.createElement('div')
    for (let i = 0; i < nodes.length; i++) {
        const node = nodes[i]
        const nodeId = node.getAttribute('id')
        const parentId = node.getAttribute('parent-id')
        const line = renderLine(`line-${nodeId}-${parentId}`)
        fragment.appendChild(line)
    }
    const svgContainer = document.querySelector('.svg-container')
    svgContainer.innerHTML = fragment.innerHTML
}

//具体一条边的绘制逻辑
function renderLine(id) {
    const line = document.querySelector(`.${id}`)
    let svg = null,
        path = null
    if (!line) {
        svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg')
        path = document.createElementNS('http://www.w3.org/2000/svg', 'path')
        path.setAttributeNS('http://www.w3.org/2000/svg', 'd', '')
        svg.appendChild(path)
        svg.setAttribute('id', id)
    } else {
        svg = line
        path = svg.querySelector('path')
    }
    const arr = id.split('-')
    const nodeId = arr[1]
    const parentId = arr[2]
    const node = document.getElementById(nodeId)
    const parentNode = document.getElementById(parentId)

    const { x: nx, y: ny } = getNodePosition(node)
    const { w: nw, h: nh } = getNodeSize(node)
    const { x: px, y: py } = getNodePosition(parentNode)
    const { w: pw, h: ph } = getNodeSize(parentNode)

    let width, height, left, top
    let d
    height = (ny + nh / 2) - (py + ph / 2)
    top = py + ph / 2
    if (px > nx) {
        width = (px + pw / 2) - (nx + nw / 2)
        left = nx + nw / 2
        d = `M${width} 0 L0 ${height}` //从右上角至左下角画线
    } else if (px < nx) {
        width = (nx + nw / 2) - (px + pw / 2)
        left = px + pw / 2
        d = `M0 0 L${width} ${height}` //从左上角至右下角画线
    } else {
        width = 2
        left = px + pw / 2
        d = `M ${width / 2} 0 L${width / 2} ${height}` //画一条竖直向下的线
    }

    const length = Math.round(Math.sqrt(Math.pow(width, 2) + Math.pow(height, 2)))
    const val = length - (pw / 2 + nw / 2)
    const height_val = length - (ph / 2 + nh / 2)
    svg.setAttribute('width', width)
    
    path.setAttributeNS('http://www.w3.org/2000/svg', 'd', d)
    if(width == 2){
        svg.setAttribute('height', height)
        path.setAttribute('style', `stroke:black; stroke-dasharray:1 ${ph/2} ${height_val} ${nh/2};`)
    }
    else{
        svg.setAttribute('height', height)
        path.setAttribute('style', `stroke:black; stroke-dasharray:${val};stroke-dashoffset:-${pw / 2}`)
    }
        
    svg.style = `position:absolute;left:${left}px;top:${top}px`
    return svg
}

function getNodePosition(node) {
    const { x, y } = node.getBoundingClientRect()
    return { x, y }
}

function getNodeSize(node) {
    const { width, height } = window.getComputedStyle(node)
    return { w: getNumFromPx(width), h: getNumFromPx(height) }
}

function getNumFromPx(str) {
    return Number(str.substring(0, str.indexOf('p')))
}

/**
 * 提交源代码程序数据
 * @returns 结果保存在cache_tree中
 */
function submitSource(){
    // const codeElement = document.getElementById('sourceInput').value
    const codeElement = window.editor.getValue();
    // 判断不为零
    if(codeElement.length == 0){
        alert("输入不能为空! 请在左侧输入栏输入待分析代码")
        return
    }
    var url = "/getGramParse"
    // get请求:
    var httpRequest = new XMLHttpRequest();	// 第一步：建立所需的对象
    httpRequest.open('POST', url, true);	// 第二步：打开连接，将请求参数写在url中
    var sendData = {data: codeElement}
    httpRequest.setRequestHeader("Content-Type", "application/json")
    httpRequest.send(JSON.stringify(sendData));	// 第三步：发送请求(Body)
    // 启动遮罩
    var hidder = document.getElementById("hidder")
    hidder.style.display = "flex"
    // 获取数据后的处理程序
    httpRequest.onreadystatechange = function() {
        // 取消遮罩
        hidder.style.display = "none"
        if (httpRequest.readyState == 4 && httpRequest.status == 200) {
            var jsonData = JSON.parse(httpRequest.response)
            var status = jsonData["status"];    // 解析结果
            var json_tree = jsonData["res"];    //获取到json字符串，还需解析
            var lex_data = jsonData["lexres"];
            console.log(typeof(status))
            // 解析成功
            if(status == 0){
                cache_tree = JSON.parse(json_tree)  //保存当前树
                var hignLightInput = document.getElementById("hignLightInput");
                hignLightInput.value = lex_data
                alert("语法分析成功! 点击右侧按钮查看语法生成树")
                // 输出语法高亮
            }
            // 解析失败
            else{
                alert("语法错误! 请在右侧输入框查看详情")
                var hignLightInput = document.getElementById("hignLightInput");
                hignLightInput.value = json_tree
                // 输出错误日志
            }
        }
    };

}

// 跳转到tree.html, 并生成一颗语法树
function generateTree(){
    // 查看当前是否有树
    if(!cache_tree){
        alert("请先进行语法分析!")
        return
    }

    // 进行跳转
    window.cache_tree = cache_tree
    // console.log(window.cache_tree)
    subWin = window.open("/tree")
    // subWin.cache_tree
    // subWin.root_index = 
    // console.log(subWin.cache_tree)
    // console.log(subWin.root_index)
    // subWin.root_index = false
}