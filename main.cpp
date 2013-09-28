#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

//base 1
class CB1
{
public:
	CB1(){printf("CB1, this=%p,sizeof(*this)=%d \n",this, sizeof(*this)); }
	~CB1(){printf("CB1 delete\n");	}
	virtual void f1(){printf("CB1::f1 \n");}
	virtual void fcb1(){printf("CB1::fcb1 \n");}

	int m_a;
};
//base 2
class CB2
{
public:
	CB2(){printf("CB2, this=%p,sizeof(*this)=%d \n",this, sizeof(*this)); }
	~CB2(){printf("CB2 delete\n");	}
	virtual void f1(){printf("CB2::f1 \n");}
	virtual void fcb2(){printf("CB2::fcb2 \n");}
	int m_a;
};
//derived 1
class CD1 : public CB1,public CB2
{
public:
	CD1(){printf("CD1, this=%p,sizeof(*this)=%d \n",this, sizeof(*this));}
	~CD1(){printf("CD1 delete\n");}
	virtual void f1(){printf("CD1:f1\n");}
	virtual void fcd1(){printf("CD1:fcd1\n");}
};
//derived 2
class CD2 : public CB1
{
public:
	CD2(){printf("CD2, this=%p,sizeof(*this)=%d \n",this, sizeof(*this));}
	~CD2(){printf("CD2 delete\n");}
	virtual void f1(){printf("CD2:f1\n");}
	virtual void fcd2(){printf("CD2:fcd2\n");}
};

//derived 3
class CD3 : public CD2
{
public:
	CD3(){printf("CD3, this=%p,sizeof(*this)=%d \n",this, sizeof(*this));}
	~CD3(){printf("CD3 delete\n");}
	virtual void f1(){printf("CD3:f1\n");}
	virtual void fcd3(){printf("CD3:fcd3\n");}
};

//derived 2
class CD4 : public CB1
{
public:
	CD4(){printf("CD4, this=%p,sizeof(*this)=%d \n",this, sizeof(*this));}
	~CD4(){printf("CD4 delete\n");}
	virtual void f1(){printf("CD4:f1\n");}
	virtual void fcd4(){printf("CD4:fcd4\n");}
};

class CD5 : public CD2, public CD4
{
public:
	CD5(){printf("CD5, this=%p,sizeof(*this)=%d \n",this, sizeof(*this));}
	~CD5(){printf("CD5 delete\n");}
	virtual void f1(){printf("CD5:f1\n");}
	virtual void fcd5(){printf("CD5:fcd5\n");}
};

typedef void(*FUNC)(void);

int main()
{
	
	printf("-------------CD1::CB1,CB2----------\n");
	//多重继承D1继承B1，B2.
	//构造先B1,B2,构造函数之前空间已申请好，this指针已指向。D1内存布局如下
	//---------
	//vptab---->|CD1:f1|CB1:fcb1|CD1:fcd1|null| CB1的this同对象的this，this前4个字节是vtab
	//---------
	//CB1成员
	//---------
	//CB2成员
	//---------
	CD1 d1;
	printf("&d1=%p,sizeof(d1)=%d,vpfunc=%p\n",&d1,sizeof(d1),*(int*)&d1);
	CB1* pb1 = &d1;
	CB2* pb2 = &d1;
	CD1* pd1 = &d1;
	printf("pb1=%p, pb2=%p, pd1=%p\n", pb1, pb2, pd1);
	pb1->f1();
	pb2->f1();
	pd1->f1();
	int** pV = (int**)&d1;
	printf("virtual func list: \n");
	//0:CD1:f1(override) 1:CB1:fcb1 2:CD2:f2
	for (int i = 0; i < 3; i++)
	{
		FUNC ff = (FUNC)pV[0][i];
		ff();
	}
	
	
	printf("-------------CD3::CD2, CD2:CB1----------\n");
	//多层继承，CD3继承CD2, CD2继承CB1
	//构造D2，在B1.3个类this指针指向同一个地址，内存结构
	//---------
	//vptab---->|CD3:f1|CB1:fcb1|CD2:fcd2|CD3:fcd3| CB1,CD2的this同对象的this，this前4个字节是vtab
	//---------
	//CD2成员
	//---------
	//CB1成员
	//---------
	CD3 d3;
	printf("&d3=%p,sizeof(d3)=%d,vpfunc=%p\n",&d3,sizeof(d3),*(int*)&d3);
	pb1 = &d3;
	CD2* pd2 = &d3;
	CD3* pd3 = &d3;
	printf("pb1=%p, pd2=%p, pd3=%p\n", pb1, pd2, pd3);
	pb1->f1();
	pd2->f1();
	pd3->f1();
	pV = (int**)&d3;
	printf("virtual func list: \n");
	//0:CD3:f1(override) 1:CB1:fcb1 2:CD2:fcd2 3：CD3:fcd3
	for (int i = 0; i < 1; i++)
	{
		FUNC ff = (FUNC)pV[0][i];
		ff();
	}
	
	
	printf("-------------CD5::CD2, CD4, CD2:CB1, CD4:CB1----------\n");
	//多层，重继承?
	//构造D5，先D2,后D4，CB1存在2份不同的对象，CD5对象地址同CD2（CB1），CD4（CB1）地址不同
	//---------
	//vptab---->|CD3:f1|CB1:fcb1|CD2:fcd2|CD5:fcd5|
	//---------
	//CD2成员
	//---------
	//CB1成员
	//---------
	CD5 d5;
	printf("&d5=%p,sizeof(d5)=%d,vpfunc=%p\n",&d5,sizeof(d5),*(int*)&d5);
	CD4* pb4 = &d5;
	pd2 = &d5;
	CD5* pd5 = &d5;
	pb2->m_a = 10;
	pb4->m_a = 11;
	//pb2->m_a, pb4->m_a 不同，如果把CD2:CB1, CD4:CB1改成虚继承则只有一份CB1.
	//并且虚函数1:CB1:fcb1不存在了。
	printf("pb4=%p, pd2=%p, pd5=%p，pb2::m_a=%d, pb4::m_a=%d\n", pb1, pd2, pd5, pb2->m_a, pb4->m_a);
	pb4->f1(); 	
	pd2->f1();
	pd5->f1();
	
	pV = (int**)&d5;
	printf("virtual func list: \n");
	//0:CD3:f1(override) 1:CB1:fcb1 2:CD2:fcd2 3：CD3:fcd3
	for (int i = 0; i < 4; i++)
	{
		FUNC ff = (FUNC)pV[0][i];
		ff();
	}
	
	//system("PAUSE");
	return 0;
}
