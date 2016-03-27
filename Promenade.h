#include<iostream>
#include<queue>
using namespace std;

class Person{
public:
	Person();
	Person(int i, int e);
	~Person();

private:
	Person *parent, *child, *sibling; // the pointers to his parent, child, sibling
	int id; // record Person's id
	int energy; // record Person's energy
	int degree;// record how many levels of this Person's children
	friend class Promenade;//you could access all valuables in class Promenade
};

//ctor
Person::Person(){
	parent = child = sibling = NULL;
	id = 0;
	energy = 0;
	degree = 0;
}

//ctor
Person::Person(int i, int e){// define a new Person with id = i and energy = e
	parent = child = sibling = NULL;
	id = i;
	energy = e;
	degree = 0;
}

//dtor
Person::~Person(){

}

class Promenade{

public:
	Promenade();
	~Promenade();
	void mergeTree(Person *, Person*);
	void mergeHeap(Promenade &bh);//把兩個BinomailHeap的BT list串起來
	void MergeHeap(Promenade &bh);//根據這個新的BT list開始進行一系列的合併，如果只有兩個高度相同的BT，就直接合併，如果有三個高度相同的BT，就把後面兩棵合併(維持sorting)
	void one_person_joined(int id, int energy);
	void one_group_joined(Promenade *g);
	void deleteMin();
	void absorb_energy(int id, int e);
	int  calculate_people_below(int e);
	int  get_weakest_person();
	int  size();
private:
	Person *head;
	int num;//caculate how many people in the promenade
};

//ctor
Promenade::Promenade(){
	head = NULL;
	num = 0;
}

//dtor
Promenade::~Promenade(){

}
void Promenade::mergeTree(Person *a, Person* b){//a->energy > b->energy
    a->parent = b;
    a->sibling = b->child;
    b->child = a;
    b->degree++;
}

void Promenade::mergeHeap(Promenade &bh){
    Person* head2 = bh.head;
    Person* head1 = head;

    Person* newHead, *newCurr;
    if(!head1){//如果本身是空的，就不需要合併，直接指向對方即可
     head = head2;
        return ;
    }
    else if(!head2){//對方是空的，也不需要合併
        return ;
    }
    //先行尋找誰的開頭degree比較小，當做新串列的頭
    //newHead記錄新的頭，newCurr紀錄目前的最後一個
    if(head1->degree > head2->degree){
        newHead = newCurr = head2;
        head2 = head2->sibling;
    }
    else {
        newHead = newCurr = head1;
        head1 = head1->sibling;
    }

    while(head1 && head2){
        if(head1->degree < head2->degree){
            newCurr->sibling = head1;
            newCurr = head1;
            head1 = head1->sibling;
        }
        else {
            newCurr->sibling = head2;
            newCurr = head2;
            head2 = head2->sibling;
        }

    }
    while(head1){
        newCurr->sibling = head1;
        newCurr = head1;
        head1 = head1->sibling;
    }
    while(head2){
        newCurr->sibling = head2;
        newCurr = head2;
        head2 = head2->sibling;
    }
    head = newHead;


}

void Promenade::MergeHeap(Promenade &bh){
    mergeHeap(bh);  //先把BT list給重新串接起來
    Person* prev = NULL;
    Person* x = head;
    Person* next = x->sibling;
    while(next){
        if( (x->degree != next->degree) || next->sibling && next->sibling->degree == x->degree){
            prev = x;  //前後兩棵BT的高度不同 或是 後面三棵BT的高度都相同
            x = next;  //下次再合併(三A 6 5樹高度相同->後面兩棵合併)
        }
        else if( x->energy <= next->energy){  //前面BT的值比較小，所以後面的合併進來
            x->sibling = next->sibling;
            mergeTree(next,x);//next>x
        }
        else{ //前面那棵BT的值比較大，要往後合併，視情況也要更新 head指標
            if(!prev){
                head = next;//更新head 指標
            }
            else{
                prev->sibling = next;
            }
            mergeTree(x,next);//合併
            x = next;
        }
        next = next->sibling;
    }


}

void Promenade::one_person_joined(int id, int energy){
	//TODO: fill in the code to simulate a person with his id and energy to join the Promenade
	//build a new BH with only one element, merge it with the original BH
    Promenade bh;
    bh.head = new Person(id, energy);
    MergeHeap(bh);
}

void Promenade::one_group_joined(Promenade *g){
	//TODO: fill in the code to simulate one group to join the full promenade
    MergeHeap(*g);
}

int Promenade::calculate_people_below(int e){
	//TODO: You should calculate how many people's energy are below the number (include the number) and return it
    int cnt=0;
    Person* curr;
    queue<Person*> nodes;
    nodes.push(head);
    while(!nodes.empty()){
        curr = nodes.front();
        if(curr->energy <= e) cnt++;
        nodes.pop();
        if(curr->sibling!=NULL) nodes.push(curr->sibling);
        if(curr->child!=NULL) nodes.push(curr->child);
    }
	return cnt;
}

void Promenade::deleteMin(){
    //if there is only one element left
    if(head->sibling==NULL && head->child==NULL){
        head = NULL;
        return;
    }
    //more than one elements
    int min = head->energy;
    Person* tmp = head;
    Person* minPre = NULL;
    Person* minCurr = head;
    // find the smallest node
    //Since we want to remove that BT, we must record its previous BT
     while(tmp->sibling){
        if(tmp->sibling->energy < min){
            min = tmp->sibling->energy;
            minPre = tmp;
            minCurr = tmp->sibling;
        }
        tmp = tmp->sibling;
    }
    //adjust the pointer(head and sibling)
    if(!minPre && minCurr) //the min tree is the head tree
        head = minCurr->sibling;
    else if(minPre && minCurr)
        minPre->sibling = minCurr->sibling;
    //use three pointer to reverse the tree list under minCurr(minCurr->child and its sibling)
    ///reverse後在MergeHeap中找有沒有相同degree的BT時才不會漏掉
    Person *pre,*curr;
    pre = tmp = NULL;
    curr = minCurr->child;
    while(curr){
        tmp = curr->sibling;
        curr->sibling = pre;
        curr->parent = NULL;//parent 被 delete 掉了
        pre = curr;
        curr = tmp;
    }
    //build a new BH, make the head point to the reversed BT list
    Promenade bh ;
    bh.head = pre;
    //merge the new BH with the original BH
    MergeHeap(bh);
}

void Promenade::absorb_energy(int target_id, int e){
	//TODO: Elizabeth absord the energy of the person with this id, this Person's energy will decrease by this number.
	//		If his energy becomes lower than or equal to zero, he will die and should be thrown out automatically
    Person* curr;
    //Search for the node with target_id, record it as current
    queue<Person*> nodes;
    nodes.push(head);
    bool found=false;
    while(!nodes.empty()){
        curr = nodes.front();
        nodes.pop();
        if(curr->id == target_id) {found=true; break;}
        if(curr->sibling!=NULL) nodes.push(curr->sibling);
        if(curr->child!=NULL) nodes.push(curr->child);
    }
    if(!found) return;//if can not find ->ignore

    curr->energy -= e;
    int new_energy = curr->energy;
    Person* par = curr->parent;//parent of curr
    while(par!=NULL && par->energy > curr->energy){//exchang (curr, par)
        //exchange their values //degree不用交換!!
        int tmp_id=curr->id, tmp_e=curr->energy;
        curr->id = par->id; curr->energy = par->energy;
        par->id = tmp_id;   par->energy = tmp_e;
        //move to next iteration
        curr = par;
        par = par->parent;
    }
    //test
    while(!nodes.empty()) nodes.pop();
    nodes.push(head);
    while(!nodes.empty()){
        curr = nodes.front();
        nodes.pop();
        if(curr->sibling!=NULL){
            nodes.push(curr->sibling);
        }
        if(curr->child!=NULL) {
            nodes.push(curr->child);
        }
    }
    //test
    if(new_energy<=0)
        deleteMin();//delete_min

}

int Promenade::get_weakest_person(){//traverse through the BT root list to get minimum
	//TODO: return the id of the one with the least energy left. If there are more than one solution, just return one of them
    if(!head) return -1;//empty
    int lowest_id;
    int lowest_energy = head->energy;
    lowest_id = head->id;
    Person *node = head;
    while(node){
        if(node->energy < lowest_energy){
            lowest_energy = node->energy;
            lowest_id = node->id;
        }
        node = node->sibling;
    }
	return lowest_id;
}

int Promenade::size(){
    int cnt=0;
    if(!head) return cnt;//empty
    Person *curr;
    queue<Person*> nodes;
    nodes.push(head);
    while(!nodes.empty()){
        curr = nodes.front();
        cnt++;
        nodes.pop();
        if(curr->sibling!=NULL){
            nodes.push(curr->sibling);
            //printf(" %d's sibling is %d\n",curr->id,curr->sibling->id);///test
        }
        if(curr->child!=NULL) {
            nodes.push(curr->child);
            //printf(" %d's child is %d\n",curr->id,curr->child->id);///test
        }
    }
	return cnt;
}
