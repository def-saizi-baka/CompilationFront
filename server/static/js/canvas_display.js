var cur_img = "/static/upload/99999/2021-11-0719-08-33.703471/1_t0.jpg";

function clear_canvas(id = "myCanvas") {
    var c = document.getElementById(id);
    var cxt = c.getContext("2d");
    cxt.clearRect(0, 0, c.width, c.height);
}

function handle_size(cv, img) {
    var scale = 1;
    var width, height;
    while (!(img.width / scale <= cv.width && img.height / scale <= cv.height)) {
        scale += 1;
    }
    width = img.width;
    height = img.height;
    return [width, height];
}

function handle_start_point(cv, size) {
    var ltop_x, ltop_y;
    ltop_x = cv.width / 2 - size[0] / 2;
    ltop_y = cv.height / 2 - size[1] / 2;

    return [ltop_x, ltop_y];
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function display_pic(id = "myCanvas") {

    //1. 获取canvas元素
    var cv = document.getElementById(id);
    //2. 获取2D上下文
    var ctx = cv.getContext('2d');
    //3. 新建一个Image对象
    var img = new Image();
    //4. 设置Image的src

    img.src = cur_img;
    //alert(cur_img);
    console.log("display " + cur_img);

    var size = handle_size(cv, img);

    var start_point = handle_start_point(cv, size);


    //5. 确定Image加载完毕后将Image画到canvas上
    img.onload = function() {
        ctx.drawImage(img, start_point[0], start_point[1], size[0], size[1]);
    }
}