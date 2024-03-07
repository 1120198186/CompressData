
#include<iostream>
#include<cmath>
#include<utility>
#include<vector>
#include<cmath>

using namespace std;


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
    LL    num;   //4B
    bool   deal;  //1B
    Node* next;   //8B

    Node(double v=0,LL n=0,bool d = 0,Node* ne = nullptr){
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
        Node* head1;
        Node* head2;
        LinkList(LL qnumber){
            head1 = new Node();
            head2 = new Node();
            head1->next = head2;
            length = pow(2,qnumber);
        }
};  


class CD
{
    private:

    public:
        LL AllLengthB;
        LL GroupNumberB;
        LL GroupLengthB;
        LL AllLength;
        LL GroupNumber;
        LL GroupLength;
        vector<LinkList> ListTable;;

        CD(LL all,LL GN){
            GroupNumberB = GN;
            AllLengthB = all;
            GroupLengthB = all-GN;
            GroupLength = pow(2,GroupLengthB);
            AllLength = pow(2,AllLengthB);
            GroupNumber = pow(2,GroupNumberB);
            for(LL i=0;i<GroupNumber;i++){
                ListTable.push_back(LinkList(GroupNumberB));
            }

        }


        int FillGroup(vector<double> state){
            LL G1 = GroupLength;
            LL G2 = GroupNumber;
            LL i=0;
            LL j=0;
            while(i<G2){
                Node* head = ListTable[i].head2; 
                while(G1--){
                    double x = state[j++];
                    if(head == ListTable[i].head2 || head->val!=x){
                        Node* node1 = new Node(x,1,0);
                        head->next = node1;
                        head = head->next;
                    }
                    else{
                        head->num++;
                    }
                }
                G1 = GroupLength;
                i++;
            }
        }


        void CircuitConvert(){
            
        }


        void OperateBlockSingle(FunOp gate,LL k){
          

        }


        void DisplayLink(){
            for(LL i=0;i<GroupNumber;i++){
                cout<<i<<":"<<endl;
                Node* head = ListTable[i].head2->next;
                while(head){
                    cout<<head->val<<" "<<head->num<<"--";
                    head = head->next;
                }
                cout<<endl;
            }
            return ;

        }
        
};
