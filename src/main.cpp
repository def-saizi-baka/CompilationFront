#include <iostream>
#include "Item.h"
#include "Parser.hpp"

int main()
{
	CFG cfg;
    // 初始化项目
    cfg.initItems();
    // 初始化LR0项目
    cfg.initLRItems();
    // 展示文法
	// cfg.showCFG();

	cfg.formFirstSet();
	// cfg.showFirstSet();
    // set<Item> initItemSet{cfg.getInitItem()};
    // Closure closure0(cfg, initItemSet);
    // set<Item> goRes = closure0.GO(1001);
    // Closure closure1(cfg, goRes);

    cfg.buildClosures();
    cfg.buildAnalysisTable();

    
	return 0;

}