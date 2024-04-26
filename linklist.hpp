#include<iostream>
#include<cmath>
#include<utility>
#include<vector>
#include<cmath>

using namespace std;


//MaxMemory = 16G


using pdd = pair<double,double>;
using FunOp = pdd(*)(double,double);
using LL = long long;

pdd QumSwap(double a,double b){
    return {b,a};
}


class Node
{
private:
    
public:
    double val;   //8B
    int    num;   //4B
    bool   deal;  //1B
    Node* next;   //8B

    Node(double v=0,int n=0,bool d = 0,Node* ne = nullptr){
        val = v;
        num = n;
        deal = d;
        next = ne;

    }
   
    ~Node(){};
};


class cd
{
private:
    
public:
    Node* head;
    int  length;
    cd(){
        length = 0;
        head = new Node();

    }

    int Convert(vector<double> sv ){
        int n = sv.size();
        int i=0;
        Node* node = head;
        
        while(i<n){
            if(node == head || sv[i] != node->val){
                Node* node1 = new Node(sv[i],1,0);
                node->next = node1;
                //node1->num = 1;
                node = node1;
            }
            else{
                node->num++;
            }
            i++;

        }

        return 1;
    }


    void DisplayA(){
        cout<<"DisplayA: ";
        Node* node = head->next;
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
        Node* node = head->next;
        while(node!=nullptr){
            cout<<node->val<<" "<<node->num<<" ";
            
            node = node->next;
        }
        cout<<endl;

    }



    void Operate(FunOp gate,int n){
        LL step = 2*(pow(2,n));
        LL inc = pow(2,n);

        Node* node = head->next;

        while (node){
            if(node->deal){
                node = node->next;
                continue;
            }
            int x = node->num;
            int y = node->val;
            cout<<x<<" "<<y<<" "<<node->deal<<endl;
            if(x>=step){
                int a = x/step;
                int b = x%step;
                pdd temp = gate(y,y);
                int c = temp.first;
                int d = temp.second;
                if(c == d){
                    node->val = c;
                    node->num = x-b;
                    if(b>0){
                        Node* node1 = new Node(y,b,0);
                        node1->next = node->next;
                        node->next = node1;
                    }
                    node = node->next;

                }else{
                    node->val = c;
                    node->num = step/2;
                    Node* node1 = new Node(d,step/2,1);
                    node1->next = node->next;
                    node->next = node1;
                    node = node->next;
                    int e = a;
                    e--;
                    while(e--){
                        node1 = new Node(c,step/2,1);
                        node1->next = node->next;
                        node->next = node1;
                        node = node->next;
                        node1 = new Node(d,step/2,1);
                        node1->next = node->next;
                        node->next = node1;
                        node = node->next;
                    }
                    if(!b){
                        Node* node1 = new Node(y,b,0);
                        node1->next = node->next;
                        node->next = node1;
                    }
                    node = node->next;
                    
                }
            }else{
                if(x<=inc){
                    int find = inc;
                    find -= node->num;
                    Node* node1 = node;
                    while(find>=0){
                        node1 = node1->next;
                        
                        find -= node1->num;
                        
                    
                    }
                    cout<<node1->num<<" "<<node1->val<<" "<<node1->deal<<endl;
                    int aa = node->num;
 w                   int bb = node1->num;
                    if(aa>bb){
                        Node* node2 = new Node(node->val,aa-bb,0);
                        node->num = bb;
                        node2->next = node->next;
                        node->next = node2;

                    }else if(bb>aa){
                        Node* node2 = new Node(node1->val,bb-aa,0);
                        node1->num = aa;
                        node2->next = node1->next;
                        node1->next = node2;
                    }

                    pdd temp = gate(y,node1->val);
                    int c = temp.first;
                    int d = temp.second;
                    cout<<"swap"<<c<<" "<<d<<endl;
                    node->deal = 1;
                    node1->deal = 1;
                    node->val = c;
                    node1->val = d;
                    node = node->next;
                    DisplayB();

                }else{
                    int rema = x-inc;
                    Node* node1 = new Node(y,inc-rema,0);
                    Node* node2 = new Node(y,rema,1);
                    pdd temp = gate(y,y);
                    int c = temp.first;
                    int d = temp.second;
                    node->val = c;
                    node2->val = d;
                    node->num = rema;
                    node->deal = 1;
                    node2->next = node->next;
                    node->next = node1;
                    node1->next = node2;
                    node = node->next;

                }



            }
        }
        

    }

    ~cd(){};
};
