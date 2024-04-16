#include <iostream>
#include <cmath>
#include <utility>
#include <vector>
#include <cmath>





using namespace std;

using pdd = pair<double, double>;
using FunOp = pdd (*)(double, double);
using LL = long long;

pdd QumSwap(double a, double b)
{
    return {b, a};
}


class Node
{
private:
public:
    double val; // 8B
    LL num;     // 4B
    bool deal;  // 1B
    Node *next; // 8B

    Node(double v = 0, LL n = 0, bool d = 0, Node *ne = nullptr)
    {
        val = v;
        num = n;
        deal = d;
        next = ne;
    }

    ~Node(){};
};


class LinkList
{
private:
public:
    LL length;
    Node *head1;
    Node *head2;
    LinkList(LL qnumber)
    {
        head1 = new Node(-999,0,1);
        head2 = new Node(0-9990,1);
        head1->deal = 1;
        head2->deal = 2;
        head1->next = head2;
        length = pow(2, qnumber);
    }


    void Fill(vector<double> a){

        int n = a.size();
        Node* head = head2;
        for(int i=0;i<n;i++){
            LL x = a[i];
            if(head->val==x){
                head->num++;
            }
            else{
                Node *node1 = new Node(x, 1, 0);
                head->next = node1;
                head = head->next;
            }
        }

        
    }

    void DisplayA(){
        cout<<"DisplayA: ";
        Node* node = head2->next;
        while (node != nullptr){
            int k = node->num;
            while(k--){
                cout<<node->val<<" ";
            }
            node = node->next;
        }
        cout<<endl;


    }

    void DisplayB(){
        cout<<"DisplayB: ";
        Node* node = head2->next;
        while(node!=nullptr){
            cout<<node->val<<" "<<node->num<<" ";
            
            node = node->next;
        }
        cout<<endl;

    }



    


    void Operate2not(FunOp gate,LL k){
        /*
        2*2 and not control
        */
        LL inc = 2 * (pow(2, k));
        LL step = pow(2, k);

        Node* hd = head1;
        Node* nd = hd->next;
        Node* td = nullptr;
        //cout<<"1"<<endl;
        while(nd){
            DisplayA();
            DisplayB();
            //cout<<nd->val<<" "<<nd->num<<endl;
            if(nd->deal){
                cout<<"1"<<endl;
                //hd = nd;
                if(nd->val == hd->val){
                    hd->num+=nd->num;
                    nd = nd->next;
                    hd->next = nd;
                    //hd = hd->next;
                }
                else{
                    hd = nd;
                    nd = nd->next;
                }
                
                continue;
            }


            if(nd->num>=inc){
                if(nd->num>inc){
                    cout<<"2"<<endl;
                    LL nn = nd->num/inc;
                    LL mm = nd->num%inc;
                    if(mm){
                        Node* t5 = new Node(nd->val,mm,0);
                        t5->next = nd;
                        nd->next = t5;
                        nd->num-=mm;
                    }
                    //Node* t5 = new Node(nd->val,mm,0);
                    //t5->next = nd;
                    //nd->next = t5;
                    //nd->num-=mm;
                    pdd data = gate(nd->val,nd->val);
                    if(data.first==data.second){
                        nd->val = data.first;
                        nd->deal = 1;
                        hd = nd;
                        nd = nd->next;
                    }
                    else{
                        Node* ttd = hd;
                        td = nd;
                        while(nn--){
                            Node* t1 = new Node(data.first,step,1);
                            Node* t2 = new Node(data.second,step,1);
                            t1->next = t2;
                            t2->next = td;
                            hd->next = t1;
                            ttd = t2;
                            td = t1;

                            
                        }


                        if(hd->val == hd->next->val){
                            hd->num+=hd->next->num;
                            hd->next = hd->next->next;
                        }
                        while(hd->next == nd){
                            hd = hd->next;
                        }
                        hd = nd->next;
                        nd = hd->next;
                        
                        //hd = ttd;

                    }
               

                }
                else{
                    cout<<"3"<<endl;
                    Node* t1 = new Node(nd->val,step,1);
                    nd->num=step;
                    hd->next = t1;
                    t1->next = nd;
                    pdd data = gate(t1->val,nd->val);
                    t1->val = data.first;
                    nd->val = data.second;
                    nd->deal = 1;
                    
                    if(t1->val == hd->val){
                        hd->num+=t1->num;
                        
                        hd->next = nd;
                        

                    }

                    else{
                        hd = t1;
                    }
                    


                }

            }
            else{
                if(nd->num>step){
                    cout<<"4"<<endl;
                    Node* t1 = new Node(nd->val,nd->num-step,1);
                    t1->next = nd->next;
                    nd->next = t1;
                    nd->num = step;
                    pdd data = gate(nd->val,t1->val);
                    

                    if(nd->num>t1->num){
                        Node* t5 = new Node(nd->val,t1->num,0);
                        nd->num -= t1->num;
                        t5->next = nd->next;
                        nd->next = t5;

                    }
                    else{
                        Node* t5 = new Node(t1->val,nd->num,0);
                        t1->num -= nd->num;
                        t5->next = t1->next;
                        t1->next = t5;
                    }

                    nd->val = data.first;
                    t1->val = data.second;
                    nd->deal = 1;
                    t1->deal = 1;
                    if(nd->val == hd->val){
                        hd->num+=nd->num;
                        nd = nd->next;
                        hd->next = nd;
                        hd = hd->next;
                    }

                }
                else{
                    cout<<"5"<<endl;
                    //DisplayA();
                    //DisplayB();
                    //cout<<"1"<<endl;
                    LL tstep = step;
                    td = nd;
                    //Node* xd = hd;
                    while(tstep >= td->num){
                        cout<<tstep<<" "<<td->val<<" "<<td->num<<endl;
                        tstep-=td->num;
                        td = td->next;
                        //xd = xd->next;
                    }
                    //LL x = td->num-tstep;
                    Node* t1 = nullptr;
                    if(tstep==0){
                        t1 = td;
                    }
                    else {
                        t1 = new Node(td->val,td->num-tstep,1);
                        //xd->next = t1;
                        t1->next = td->next;
                        td->next = t1;
                        td->num = tstep;
                    }
                    
                    //td = t1;
                    pdd data = gate(nd->val,t1->val);
                    if(nd->num>t1->num){
                        Node* t5 = new Node(nd->val,nd->num-t1->num,0);
                        nd->num = t1->num;
                        t5->next = nd->next;
                        nd->next = t5;

                    }
                    else{
                        if(nd->num<t1->num){
                            Node* t5 = new Node(t1->val,t1->num-nd->num,0);
                            t1->num = nd->num;
                            t5->next = t1->next;
                            t1->next = t5;
                        }
                        
                    }

                    nd->val = data.first;
                    t1->val = data.second;
                    nd->deal = 1;
                    t1->deal = 1;
                    /*
                    if(nd->val == hd->val){
                        hd->num+=nd->num;
                        nd = nd->next;
                        hd->next = nd;
                        hd = hd->next;
                    }
                    */
                    //hd = nd;
                    //nd = nd->next;
                    


                }
            }
        }
        

    }




};
