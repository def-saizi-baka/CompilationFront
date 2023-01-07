#include "../include/Gram.h"

extern config con;

Gram::Gram(const Gram& tmp)
{
	this->left = tmp.left;
	this->right.assign(tmp.right.begin(), tmp.right.end());
}

Gram::Gram(int left, const vector<int>& right)
{
	this->left = left;
	this->right.assign(right.begin(), right.end());
}

void Gram::showGram()
{
	cout << this->left << "\t->\t";
	for (auto iter : right)
	{
		cout << iter << ",";
	}
	
	cout << "\t\t";
	cout << con.get_name(this->left) << "\t->\t";
	for (auto iter : right)
	{
		cout << con.get_name(iter) << ",";
	}
	cout << endl;
}

Item::Item(int left, const vector<int>& right,uint32_t dot_pos):Gram(left,right)
{
	this->dotPos = dot_pos;
	if (dot_pos == this->right.size()){
		this->type = ACTION_REDUCE;
	}
	else {
		this->type = ACTION_MOVE;
	}
}

Item::Item(const Gram& gram,uint32_t dot_pos) :Gram(gram)
{
    
	this->dotPos = dot_pos;
	if (dot_pos == this->right.size()){
		this->type = ACTION_REDUCE;
	}
	else {
		this->type = ACTION_MOVE;
	}
}	


void Item::dotRightMove(){
    this->dotPos++;
    if (dotPos == this->right.size()){
		this->type = ACTION_REDUCE;
	}
	else {
		this->type = ACTION_MOVE;
	}
}

void Item::showItem()
{
	cout << this->left << "("<<this->dotPos<<") \t->\t";
	int rSize = right.size();
	for (int i = 0; i < rSize; i++)
	{
		if (i == dotPos)
		{
			cout << "." << " ";
		}
		cout << right[i] << " ";

	}
	if (type == ACTION_REDUCE)
	{
		cout << "." << " ";
	}
	cout << endl;
}

vector<int> Item::getDotNextAll(){
    vector<int> res;
    for(uint32_t src_pos = this->dotPos; src_pos < this->right.size(); src_pos++){
        res.push_back(this->right[src_pos]);
    }
    return res;
}

bool Item::operator == (const Item& other) const
{
	if (this->left != other.left)
		return false;
	else if (this->right != other.right)
		return false;
	else if (this->dotPos != other.dotPos)
		return false;
	else if(this->forward != other.forward)
		return false;
	// type 本质由dotPos决定，因此不必再判断
	return true;
	
}	

bool Item::operator!=(const Item& other) const{
    return !(*this == other);
}

bool Item::operator<(const Item& other) const{
    if(this->id == other.id){
        if(this->forward == other.forward){
            return this->dotPos > other.dotPos;
        }
        else
            return this->forward < other.forward; 
    }
    else
        return this->id < other.id;
}

CFG::CFG(){
    vector<std::pair<int, std::vector<int> > > gramVec = con.get_grammar();
    this->terSysboms = con.get_stop_symbols();
    this->nonTerSysboms = con.get_unstop_symbols();
    
    // 初始状态
    this->begState = gramVec[0].first;
    // 构建语法表
    int _ = 0;
    for(auto grammar : gramVec){
        if(grammar.second.size()>0){
            this->initGram.push_back(Gram(grammar.first, grammar.second));
            leftToGramIndex[grammar.first].insert(_++);
        }
        else{
            // cout << "[WARN]不存在的生成式"+to_string(grammar.first) << endl;
            con.log("[WARN]不存在的生成式"+to_string(grammar.first));
        }
    }
	// 使用课本上的测试语法
	/*
		终结符： a = 1,b = 2;
		非终结符： S = 1001， B = 1002
		额外符号： # 0 ,s` = 1000

		语法 ： 
			S -> BB
			1001 -> 1002 1002

			B -> aB
			1002 -> 1 1002

			B -> b
			1002 -> 2
		拓广文法：
			S`->S
	*/
    // this->begState = 

	// 文法定义
	// int left = 1000;
	// vector<int> right3 = { 1001 };
	// this->initGram.push_back(Gram(left, right3));
	// leftToGramIndex[1000].insert(0);

	// left = 1001;
	// vector<int> right = { 1002, 1002 };
	// Gram tmp(left, right);
	// this->initGram.push_back(tmp);
	// leftToGramIndex[1001].insert(1);

	// left = 1002;
	// vector<int> right1 = { 1, 1002 };
	// this->initGram.push_back(Gram(left, right1));
	// leftToGramIndex[1002].insert(2);

	// left = 1002;
	// vector<int> right2 = { 2 };
	// this->initGram.push_back(Gram(left, right2));
	// leftToGramIndex[1002].insert(3);

	// this->begState = 1000;//最终归约停止的符号

    // // 非终结符:
    // this->nonTerSysboms = {1000, 1001, 1002};
    // // 终结符
    // this->terSysboms = {Config::end_int, 1, 2};
}

CFG::CFG(const string& grammerFile)
{
}

void CFG::initItems(){
    for(uint32_t i=0; i<initGram.size(); i++){
        Item item = Item(initGram[i], 0);
        item.setId(i);
        this->allItem.push_back(item);
    }
}

Item CFG::getInitItem(){
    Item res = this->allItem[0];
    res.setForward(Config::end_int);
    return res;
}


void CFG::buildClosures(){

	if(this->debug)
	{
		cout << "******************************************************************************************************" <<endl;
        cout << "                         This is the Closure :                                                          " <<endl;
        cout << "******************************************************************************************************" <<endl;
		cout << endl;
	}

    // 构造初始状态
    set<Item> initItemSet{this->getInitItem()};
    queue<Closure> buildQueue;  // 待扩展的队列
    queue<pair<int, int>> transQ;   // 存放转移关系<sourceClosure, transCh>
    vector< set<pair<int, int>> > transRes;
    vector<Closure> visit;  // 没有经过扩展的visit数组
    buildQueue.push(Closure(initItemSet));
    transQ.push({0, 0});

    while(!buildQueue.empty()){
        Closure waitExtendedClosure = buildQueue.front();
        pair<int, int> closureFrom = transQ.front();
        buildQueue.pop();
        transQ.pop();
        // 防止重复查找, 同时保证链接关系
        auto find_iter = find(visit.begin(), visit.end(), waitExtendedClosure);
        if(find_iter!=visit.end()){
            int index = find_iter - visit.begin();
            transRes[index].insert(closureFrom);
            continue;
        }
        visit.push_back(waitExtendedClosure);
        // 扩展, 并添加到CFG的项目集中
        Closure builtClosure(*this, waitExtendedClosure.getFamily());

        // debug
        int source = this->closures.size();
        if(this->debug){
            cout << "ClosureID: " << source << endl;
            builtClosure.printClosure();
        }

        // 保存转移结果
        this->closures.push_back(builtClosure);
        transRes.push_back(set<pair<int, int>>{closureFrom});
        this->closuresRelation.push_back(set< pair<int, int> >{});

        // 非终结符
        for(auto nonTerSys: nonTerSysboms){
            set<Item> gotoRes = builtClosure.GO(nonTerSys.second);
            if(!gotoRes.empty()) {
                buildQueue.push(gotoRes);
                transQ.push({source, nonTerSys.second});
            }
        }

        // 终结符
        for(auto terSys: terSysboms){
            set<Item> actionRes = builtClosure.GO(terSys.second);
            if(!actionRes.empty()) {
                buildQueue.push(actionRes);
                transQ.push({source, terSys.second});
            }
        }

    }

    // 重新构建闭包之间关系
    int i=0;
    for(auto trans : transRes){
        for(auto tranPair : trans){
            if(tranPair.first!=0 || tranPair.second!=0)
                this->closuresRelation[tranPair.first].insert({tranPair.second, i});
        }
        i++;
    }

    // 输出闭包之间关系
    if(this->debug){
        cout << "闭包关系: " << endl;
        for(uint32_t i = 0; i<this->closuresRelation.size(); i++){
            cout << i << ": ";
            for(auto tran : this->closuresRelation[i]){
                cout << "<" << tran.first << ", " << tran.second << "> ";
            }
            cout << endl;
        }
    }
    // cout << endl;
}


void CFG::buildAnalysisTable(){
    // 根据闭包关系构建action / goto表
    for(uint32_t i=0; i<closures.size(); i++){
        // 为该闭包状态新建一行
        this->analysisTable.insert({i, vector<pair<int, int>>{}});
        // 检查移进
        for(auto transRule : closuresRelation[i]){
            this->analysisTable[i].push_back(transRule);
        }
        // 检查规约和acc
        for(auto item : closures[i].getFamily()){
            if(item.getType() == ACTION_REDUCE){
				bool is_conflick = false;
				for (auto par : this->analysisTable[i]) {
					if (par.first == item.getForward()) {
						is_conflick = true;
						break;
					}
				}
                // 先判断是否为acc
                if(item.left == 1000){
                    this->analysisTable[i].push_back({item.getForward(), parser_config::ACCEPT});
                }
                else{
                    this->analysisTable[i].push_back({item.getForward(), -1*item.getId()});
                }
				// 打印冲突语句
				if (is_conflick) {
					cout << item.getForward() << " " << con.get__symbols()[item.getForward()];
					item.showItem();
				}
            }
        }
    }

    // debug

    if(this->debug){
        cout << endl;
        for(auto analysisItem : this->analysisTable){
            cout << "State: " << analysisItem.first << endl;
            sort(analysisItem.second.begin(), analysisItem.second.end());
            cout << "\taction: ";
            uint32_t i = 0;
            for(; i<analysisItem.second.size(); i++){
                if(analysisItem.second[i].first>1000) break;
                cout << "<" << analysisItem.second[i].first << ", " << analysisItem.second[i].second << "> ";
            }
            cout << endl;
            cout << "\tgoto: ";
            if(i<analysisItem.second.size()){
                for(; i<analysisItem.second.size(); i++){
                    cout << "<" << analysisItem.second[i].first << ", " << analysisItem.second[i].second << "> ";
                }
            }
            cout<<"\n"<<endl;
        }
    }

}

void CFG::showCFG()
{
	/*for (auto& s : initGram)
	{
		s.showGram();
	}*/

    cout << "所有项目为: "<< endl;
	for (auto& s : allItem){
		cout << "输出所有项目:" << endl;
		s.showItem();
	}
    // cout << "\n" << "所有LR(0)项目为" << endl;

    // for (auto& s : LRItem){
	// 	cout << "输出所有项目:" << endl;
	// 	s.showItem();
	// }
	/*for (auto& iter: leftToGramIndex)
	{
		cout << iter.first<<"\t:\t";

		for (auto i : iter.second)
		{
			cout << i << "->\t";
		}
		cout << endl;
	}*/
}

// 创建所有的LR(0)项目
void CFG::initLRItems(){
    // 对初始项目中的每个节点
    for(auto item : allItem){
        // 点可能存在的范围, 进行遍历
        for(uint32_t dot_pos=0; dot_pos <= item.right.size(); dot_pos++){  
            LRItem.push_back(Item(Gram(item), dot_pos));
        }
    }
}	


void CFG::formFirstSet()
{
	// 因为生成FIRST集的过程是个不断重复的过程，因此需要用FIRST类的bool变量标记该集合是否已经被确定
	// 不确定性具备可传递性，A<-B<-C，C不确定，则AB不确定
	map<std::string, int> symbols = con.get_symbols();
    
	bool flag = true;
	//int times = 0;
	int beforeMakeSure = 0;
	int afterMakeSure = 0;
	beforeMakeSure = symbols.size();
	while (flag) {
		flag = false;
		afterMakeSure = 0;
        for(auto symbol : symbols){
			if(leftToGramIndex[symbol.second].size()!=0)
			{
				if (!firstSet[symbol.second].isSure()){
					formFirstSet(symbol.second);
					if (!firstSet[symbol.second].isSure()){
						flag = true;
						afterMakeSure++; // 记录当前这一轮筛选剩下的元素
					}
				} 
			}           
        }
		if(beforeMakeSure == afterMakeSure)
			break;
		else
			beforeMakeSure = afterMakeSure;

		// cout<<times<<"轮"<<endl;
		// for(auto symbol : symbols)
		// {
		// 	if(!firstSet[symbol.second].isSure() && leftToGramIndex[symbol.second].size()!=0)
		// 	{
		// 		cout<<symbol.second<<"\t: \t"<<endl;
		// 		for (auto iter: leftToGramIndex[symbol.second])
		// 		{
		// 			cout<<"\t\t";
		// 			initGram[iter].showGram();
		// 		}
		// 		cout<<endl;
		// 		firstSet[symbol.second].showFIRST();
		// 	}
		// }
		// times++;
	}

	if(this->debug)
	{
		cout << "******************************************************************************************************" <<endl;
        cout << "                         This is the grammer :                                                        " <<endl;
        cout << "******************************************************************************************************" <<endl;
		cout << endl;
		for(auto &i:initGram)
		{
			i.showGram();
		}
		cout<<endl;
		cout << "******************************************************************************************************" <<endl;
        cout << "                         This is the FIRST :                                                          " <<endl;
        cout << "******************************************************************************************************" <<endl;
		cout << endl;
		this->showFirstSet();

		cout <<endl;
	}

}

void CFG::formFirstSet(int symbol)
{	
	// 算法 第4章PPT第31页

	// 先开启为确定，若所有子产生式为确定，则最终确定
	this->firstSet[symbol].makeSure();
	if (symbol < 1000)
	{
		this->firstSet[symbol].insert(symbol);
		this->firstSet[symbol].makeSure();
	}
	// 此时的情况为S`
	else if(symbol == this->begState){
		this->firstSet[symbol].insert(Config::end_int);
		this->firstSet[symbol].makeSure();
	}
	else
	{
		// 遍历 value 在产生式左侧的情况
		for (auto iter: leftToGramIndex[symbol])
		{
			// 为终结符，这里把epsilon给包含进去了,就是如果存在空产生式，也可以写入epsilon
			if (initGram[iter].right[0] < 1000) {
				firstSet[symbol].insert(initGram[iter].right[0]);
				firstSet[symbol].transSure(true);
			}
			// 此时为非终结符 X->Y1Y2
			else if (initGram[iter].right[0] > 1000) {
				bool flag = true;
				int idx = 0;
				//firstSet[symbol].makeSure();
				while (flag)
				{
					// FIRST[x] = FIRST[Y] - epsilon 
					int x = initGram[iter].right[idx];
					// 防止出现左递归锁住确定关系
					if(x != symbol)
					{
						auto tmp = firstSet[x];
						tmp.divEpsilon();
						// 此处仍然可以改进，即当Y1确定时，X不需要反复读取
						firstSet[symbol].insert(tmp);
						firstSet[symbol].transSure(tmp);

						if (idx + 1 == initGram[iter].right.size()) {
							firstSet[symbol].insert(EPSILON);
						}
					}

					// 检查FIRST[Y1] 是否存在空转移
					flag = false;
					for (auto iter1 : leftToGramIndex[x])
					{
						if (initGram[iter1].right[0] == EPSILON)
						{
							flag = true;
							idx++;	// 指向Y2，继续判断
							break;
						}
					}
				}
			}
		}
	}	
}


set<int> CFG::getFirstSet(int value){
	return firstSet[value].terminals;
}

set<int> CFG::getFirstSet(vector<int> gramStr)
{
	// 算法：第四章PPT 33页
	FIRST first;
	// 把第一个符号的FIRST/EPSILON 加入
	auto tmp = firstSet[gramStr[0]];
	tmp.divEpsilon();
	first.insert(tmp);

	// 检查剩下的符号
	int rSize = gramStr.size();
	for (auto iter = ++gramStr.begin(); iter != gramStr.end(); iter++)
	{
		// 假设存在epsilon
		if (firstSet[*iter].count(EPSILON))
		{
			auto tmp = firstSet[*iter];
			tmp.divEpsilon();
			first.insert(tmp);

			auto next = iter++;
            iter--;
			if (next == gramStr.end())
			{
				first.insert(EPSILON);
			}
		}
		else
		{
			break;
		}
	}
	return first.terminals;
}

void CFG::showFirstSet()
{
	for (auto iter = this->firstSet.begin(); iter != this->firstSet.end(); iter++)
	{
		cout << iter->first << "\t:\t";
		iter->second.showFIRST();
	}

}

void CFG::save(string path)
{
	try {
		ofstream fout;
		fout.open(path, ios::out | ios::binary);
		assert(fout.is_open());
		unsigned int size = 0;
		vector<int> sizes;
		for (auto it = this->analysisTable.begin(); it != this->analysisTable.end(); it++) {
			unsigned int meta_size = sizeof(int) + it->second.size() * sizeof(pair<int, int>);
			size += meta_size;
			sizes.push_back(it->second.size());
		}
		size += sizes.size() * sizeof(int) + sizeof(int);

		char* buffer = new char[size] { 0 };
		unsigned int cnt = 0;

		int sizes_size = sizes.size();
		memcpy(buffer + cnt, (void*)(&sizes_size), sizeof(int));
		cnt += sizeof(int);

		memcpy(buffer + cnt, sizes.data(), sizes.size() * sizeof(int));
		cnt += sizes.size() * sizeof(int);

		for (const auto& t : this->analysisTable) {
			memcpy(buffer + cnt, (void*)(&t.first), sizeof(int));
			cnt += sizeof(int);
			memcpy(buffer + cnt, (void*)t.second.data(), t.second.size() * sizeof(pair<int, int>));
			cnt += t.second.size() * sizeof(pair<int, int>);
		}
		fout.write(buffer, size);
		fout.close();
		con.log("[INFO] 当前进行action表和goto表的保存，具体保存路径为" + path + "，保存的表大小为" + to_string(size) + "字节");
	}
	catch (const std::bad_alloc& e) {
		cout << e.what() << std::endl;
		con.log(e.what());
	}
}

void CFG::load(string path)
{
	ifstream fin;
	fin.open(path, ios::in | ios::binary);
	assert(fin.is_open());
	unsigned int cnt = 0;
	vector<int>sizes;
	int sizes_size;
	fin.read((char*)(&sizes_size), sizeof(int));
	for (int i = 0; i < sizes_size; i++) {
		int temp;
		fin.read((char*)(&temp), sizeof(int));
		sizes.push_back(temp);
	}

	while (fin.peek() != EOF) {
		int first;
		fin.read((char*)(&first), sizeof(int));
		vector<pair<int, int>> secondp;
		for (int i = 0; i < sizes[cnt]; i++) {
			int second, third;
			fin.read((char*)(&second), sizeof(int));
			fin.read((char*)(&third), sizeof(int));
			secondp.push_back(make_pair(second, third));
		}
		cnt++;
		this->analysisTable[first] = secondp;
	}
	fin.close();
	con.log("[INFO] 已经成功将action表和goto表加载入内存");
}

void CFG::load(bool simpleLoad, string path)
{
	if (simpleLoad) { //true代表直接加载模型
		load(path);
	}
	else { // false就不直接加载，需要完成计算
		initItems();
		initLRItems();
		formFirstSet();
		buildClosures();
		buildAnalysisTable();
		save();
	}
}

FIRST::FIRST()
{
	this->sure = false;
}

void FIRST::insert(int termial)
{
	this->terminals.insert(termial);
}

void FIRST::insert(const FIRST& other)
{
	this->terminals.insert(other.terminals.begin(), other.terminals.end());
}

void FIRST::divEpsilon()
{
	this->terminals.erase(EPSILON);
}

int FIRST::count(int terminal)
{
	return this->terminals.count(terminal);
}

void FIRST::showFIRST()
{
	for (auto iter = this->terminals.begin(); iter != this->terminals.end(); iter++)
	{
		if (iter != this->terminals.begin())
			cout << "->" << (*iter);
		else
			cout << (*iter);
	}
	cout << endl;
}

void FIRST::makeSure()
{
	this->sure = true;
}

void FIRST::notSure()
{
	this->sure = false;
}

bool FIRST::isSure()
{
	return this->sure;
}

void FIRST::transSure(bool subSure)
{
	this->sure = this->sure && subSure;
}

void FIRST::transSure(const FIRST& front)
{
	this->sure = this->sure && front.sure;
}

void Closure::showItem(Item& item){
    cout << item.getId() << " " << item.left << " --> ";
    int rSize = item.right.size();
	for (int i = 0; i < rSize; i++)
	{
		if (i == item.getDotPos())
		{
			cout << "." << " ";
		}
		cout << item.right[i] << " ";

	}
	if (item.getType() == ACTION_REDUCE)
	{
		cout << "." << " ";
	}
    cout << "  " << item.getForward();
	cout << endl;
}

Closure::Closure(CFG& cfg,const set<Item>& items){
    // 初始化队列
    queue<Item> item_res;
    for(auto item : items){
        item_res.push(item);
    }

    while(!item_res.empty()){
        // 取队首元素
        Item now_item = item_res.front();
        item_res.pop();
        // 防止重复
        uint32_t old_size = this->family.size();
        this->family.insert(now_item);
        if(old_size == this->family.size()){
            // cout << "重复的Item" << endl;
            // now_item.showItem();
            continue;
        }
        // showItem(now_item);
        // 点后面是非终结符, 进行拓展, 即S->α.Bβ形式
        if(now_item.getType() != ACTION_REDUCE && now_item.getDotNext()>1000){
            vector<Item> tmp;
            set<int> forward_tmp;
            // 查找所有B->.XXX的情况
            for(auto item_tmp : cfg.allItem){
                if(item_tmp.left == now_item.getDotNext()){
                    tmp.push_back(item_tmp);
                }
            }
            // 获取β, 以便查找β的First集合
            now_item.dotRightMove();
            // 规约模式, 说明β为空, forward照抄now_item的
            if(now_item.getType() == ACTION_REDUCE){    
                forward_tmp.insert(now_item.getForward());
            }
            // 移进模式, β不为空, forward=first(β)
            else{
                vector<int> beta = now_item.getDotNextAll();    // 获取β
                if(beta[0] < 1000){
                    forward_tmp.insert(beta[0]);
                }
                else
                    forward_tmp = cfg.getFirstSet(beta);
            }

            // 该闭包构造其他项目
            for(auto forward : forward_tmp){
                for(auto item : tmp){
                    item.setForward(forward);
                    item_res.push(item);
                }
            }
        }
    }
}

Closure::Closure(set<Item>& items){
    this->family = items;
}

void Closure::printClosure(){
    cout << "===============================" << endl;
    for(auto item : this->family){
        this->showItem(item);
    }
    cout << "===============================\n" << endl;
}

set<Item> Closure::GO(int input){
    set<Item> res;
    for(auto item : this->family){
        // 当前非终止(规约)状态, 并且转移符号与点后的符号相同
        if(item.getType()!=ACTION_REDUCE && item.getDotNext() == input){
            item.dotRightMove();
            res.insert(item);
        }
    }
    return res;
}


bool Closure::operator==(const Closure& other) const{
    if(other.family.size()!=this->family.size()){
        return false;
    }
    else{
        auto iter_this = this->family.begin();
        auto iter_other = other.family.begin();
        while(iter_this != this->family.end()){
            if((*iter_this)!=(*iter_other)){
                return false;
            }
            iter_this++;
            iter_other++;
        }
        return true;
    }
}

                       