#include "Item.h"



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
	// type ������dotPos��������˲������ж�
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

CFG::CFG()
{
	// ʹ�ÿα��ϵĲ����﷨
	/*
		�ս���� a = 1,b = 2;
		���ս���� S = 1001�� B = 1002
		������ţ� # 0 ,s` = 1000

		�﷨ �� 
			S -> BB
			1001 -> 1002 1002

			B -> aB
			1002 -> 1 1002

			B -> b
			1002 -> 2
		�ع��ķ���
			S`->S
	*/

	// �ķ�����
	int left = 1000;
	vector<int> right3 = { 1001 };
	this->initGram.push_back(Gram(left, right3));
	leftToGramIndex[1000].insert(0);

	left = 1001;
	vector<int> right = { 1002, 1002 };
	Gram tmp(left, right);
	this->initGram.push_back(tmp);
	leftToGramIndex[1001].insert(1);

	left = 1002;
	vector<int> right1 = { 1, 1002 };
	this->initGram.push_back(Gram(left, right1));
	leftToGramIndex[1002].insert(2);

	left = 1002;
	vector<int> right2 = { 2 };
	this->initGram.push_back(Gram(left, right2));
	leftToGramIndex[1002].insert(3);

	this->begState = 1000;//���չ�Լֹͣ�ķ���

    // ���ս��:
    this->nonTerSysboms = {1000, 1001, 1002};
    // �ս��
    this->terSysboms = {Config::end_int, 1, 2};
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
    // �����ʼ״̬
    set<Item> initItemSet{this->getInitItem()};
    queue<Closure> buildQueue;  // ����չ�Ķ���
    queue<pair<int, int>> transQ;   // ���ת�ƹ�ϵ<sourceClosure, transCh>
    vector< set<pair<int, int>> > transRes;
    vector<Closure> visit;  // û�о�����չ��visit����
    buildQueue.push(Closure(initItemSet));
    transQ.push({0, 0});

    while(!buildQueue.empty()){
        Closure waitExtendedClosure = buildQueue.front();
        pair<int, int> closureFrom = transQ.front();
        buildQueue.pop();
        transQ.pop();
        // ��ֹ�ظ�����, ͬʱ��֤���ӹ�ϵ
        auto find_iter = find(visit.begin(), visit.end(), waitExtendedClosure);
        if(find_iter!=visit.end()){
            int index = find_iter - visit.begin();
            transRes[index].insert(closureFrom);
            continue;
        }
        visit.push_back(waitExtendedClosure);
        // ��չ, ����ӵ�CFG����Ŀ����
        Closure builtClosure(*this, waitExtendedClosure.getFamily());

        // debug
        builtClosure.printClosure();

        // ����ת�ƽ��
        this->closures.push_back(builtClosure);
        transRes.push_back(set<pair<int, int>>{closureFrom});
        this->closuresRelation.push_back(set< pair<int, int> >{});

        /* GO */
        int source = this->closures.size()-1;
        // ���ս��
        for(auto nonTerSys: nonTerSysboms){
            set<Item> gotoRes = builtClosure.GO(nonTerSys);
            if(!gotoRes.empty()) {
                buildQueue.push(gotoRes);
                transQ.push({source, nonTerSys});
            }
        }

        // �ս��
        for(auto terSys: terSysboms){
            set<Item> actionRes = builtClosure.GO(terSys);
            if(!actionRes.empty()) {
                buildQueue.push(actionRes);
                transQ.push({source, terSys});
            }
        }

    }

    // ���¹����հ�֮���ϵ
    int i=0;
    for(auto trans : transRes){
        for(auto tranPair : trans){
            this->closuresRelation[tranPair.first].insert({tranPair.second, i});
        }
        i++;
    }
}

void CFG::buildAnalysisTable(){
    // ���ݱհ���ϵ����action / goto��
    for(uint32_t i=0; i<closures.size(); i++){
        // Ϊ�ñհ�״̬�½�һ��
        this->analysisTable.insert({i, vector<pair<int, int>>{}});
        // ����ƽ�
        for(auto transRule : closuresRelation[i]){
            this->analysisTable[i].push_back(transRule);
        }
        // ����Լ��acc
        for(auto item : closures[i].getFamily()){
            if(item.getType() == ACTION_REDUCE){
                // ���ж��Ƿ�Ϊacc
                if(item.left == 1000){
                    this->analysisTable[i].push_back({item.getForward(), parser_config::ACCEPT});
                }
                else{
                    this->analysisTable[i].push_back({item.getForward(), -1*item.getId()});
                }
            }
        }
    }
}

void CFG::showCFG()
{
	/*for (auto& s : initGram)
	{
		s.showGram();
	}*/

    cout << "������ĿΪ: "<< endl;
	for (auto& s : allItem){
		cout << "���������Ŀ:" << endl;
		s.showItem();
	}
    cout << "\n" << "����LR(0)��ĿΪ" << endl;

    for (auto& s : LRItem){
		cout << "���������Ŀ:" << endl;
		s.showItem();
	}
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

// �������е�LR(0)��Ŀ
void CFG::initLRItems(){
    // �Գ�ʼ��Ŀ�е�ÿ���ڵ�
    for(auto item : allItem){
        // ����ܴ��ڵķ�Χ, ���б���
        for(uint32_t dot_pos=0; dot_pos <= item.right.size(); dot_pos++){  
            LRItem.push_back(Item(Gram(item), dot_pos));
        }
    }
}	


void CFG::formFirstSet()
{
	// ����Ҫ�������ű�ģ���ϵ�ʱ��Ҫ�޸�
	// ������ʱ��һ��vector
	// ��Ϊ����FIRST���Ĺ����Ǹ������ظ��Ĺ��̣������Ҫ��FIRST���bool������Ǹü����Ƿ��Ѿ���ȷ��
	// ��ȷ���Ծ߱��ɴ����ԣ�A<-B<-C��C��ȷ������AB��ȷ��
	// ����ķ�����ҪinitGram ����Ҫ���ս������˳�����С�
	vector<int> symbols = { 1,2,1000,1001,1002 };
	bool flag = true;
	while (flag) {
		flag = false;
		for (int i = 0; i < symbols.size(); i++)
		{
			if (!firstSet[symbols[i]].isSure())
			{
				formFirstSet(symbols[i]);
				if (!firstSet[symbols[i]].isSure())
				{
					flag = true;
				}
			}
		}
	}

}

void CFG::formFirstSet(int symbol)
{	
	// �㷨 ��4��PPT��31ҳ
	// 
	// �˴�Ϊ�ս�����жϣ��ڷ�ϵ�ʱ��
	// Ӧ����Ҫ�ӷ��ű��ж�ȡ��
	if (symbol < 1000)
	{
		this->firstSet[symbol].insert(symbol);
		this->firstSet[symbol].makeSure();
	}
	// ��ʱ�����ΪS`
	else if(symbol == this->begState){
		this->firstSet[symbol].insert(0);
		this->firstSet[symbol].makeSure();
	}
	else
	{
		// ���� value �ڲ���ʽ�������
		for (auto iter: leftToGramIndex[symbol])
		{
			// Ϊ�ս���������epsilon��������ȥ��,����������ڿղ���ʽ��Ҳ����д��epsilon
			if (initGram[iter].right[0] < 1000) {
				firstSet[symbol].insert(initGram[iter].right[0]);
				firstSet[symbol].makeSure();
			}
			// ��ʱΪ���ս�� X->Y1Y2
			else if (initGram[iter].right[0] > 1000) {
				bool flag = true;
				int idx = 0;
				while (flag)
				{
					// FIRST[x] = FIRST[Y] - epsilon 
					int x = initGram[iter].right[idx];
					auto tmp = firstSet[x];
					tmp.divEpsilon();
					// �˴���Ȼ���ԸĽ�������Y1ȷ��ʱ��X����Ҫ������ȡ
					firstSet[symbol].insert(tmp);
					firstSet[symbol].transSure(tmp);

					if (idx + 1 == initGram[iter].right.size()) {
						firstSet[symbol].insert(EPSILON);
					}

					// ���FIRST[Y1] �Ƿ���ڿ�ת��
					flag = false;
					for (auto iter1 : leftToGramIndex[x])
					{
						if (initGram[iter1].right[0] == EPSILON)
						{
							flag = true;
							idx++;	// ָ��Y2�������ж�
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
	// �㷨��������PPT 33ҳ
	FIRST first;
	// �ѵ�һ�����ŵ�FIRST/EPSILON ����
	auto tmp = firstSet[gramStr[0]];
	tmp.divEpsilon();
	first.insert(tmp);

	// ���ʣ�µķ���
	int rSize = gramStr.size();
	for (auto iter = ++gramStr.begin(); iter != gramStr.end(); iter++)
	{
		// �������epsilon
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

void FIRST::transSure(const FIRST& front)
{
	this->sure = front.sure;
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
    // ��ʼ������
    queue<Item> item_res;
    for(auto item : items){
        item_res.push(item);
    }

    while(!item_res.empty()){
        // ȡ����Ԫ��
        Item now_item = item_res.front();
        item_res.pop();
        this->family.insert(now_item);
        // showItem(now_item);
        // ������Ƿ��ս��, ������չ, ��S->��.B����ʽ
        if(now_item.getType() != ACTION_REDUCE && now_item.getDotNext()>1000){
            vector<Item> tmp;
            set<int> forward_tmp;
            // ��������B->.XXX�����
            for(auto item_tmp : cfg.allItem){
                if(item_tmp.left == now_item.getDotNext()){
                    tmp.push_back(item_tmp);
                }
            }
            // ��ȡ��, �Ա���Ҧµ�First����
            now_item.dotRightMove();
            // ��Լģʽ, ˵����Ϊ��, forward�ճ�now_item��
            if(now_item.getType() == ACTION_REDUCE){    
                forward_tmp.insert(now_item.getForward());
            }
            // �ƽ�ģʽ, �²�Ϊ��, forward=first(��)
            else{
                vector<int> beta = now_item.getDotNextAll();    // ��ȡ��
                forward_tmp = cfg.getFirstSet(beta);
                // forward_tmp = cfg.getFirstSet(beta[0]);
            }

            // �ñհ�����������Ŀ
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
        // ��ǰ����ֹ(��Լ)״̬, ����ת�Ʒ�������ķ�����ͬ
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

                       