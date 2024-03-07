
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
        head1 = new Node();
        head2 = new Node();
        head1->next = head2;
        length = pow(2, qnumber);
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
    vector<LinkList> ListTable;
    ;

    CD(LL all, LL GN)
    {
        GroupNumberB = GN;
        AllLengthB = all;
        GroupLengthB = all - GN;
        GroupLength = pow(2, GroupLengthB);
        AllLength = pow(2, AllLengthB);
        GroupNumber = pow(2, GroupNumberB);
        for (LL i = 0; i < GroupNumber; i++)
        {
            ListTable.push_back(LinkList(GroupNumberB));
        }
    }

    void FillGroup(vector<double> state)
    {
        LL G1 = GroupLength;
        LL G2 = GroupNumber;
        LL i = 0;
        LL j = 0;
        while (i < G2)
        {
            Node *head = ListTable[i].head2;
            while (G1--)
            {
                double x = state[j++];
                if (head == ListTable[i].head2 || head->val != x)
                {
                    Node *node1 = new Node(x, 1, 0);
                    head->next = node1;
                    head = head->next;
                }
                else
                {
                    head->num++;
                }
            }
            G1 = GroupLength;
            i++;
        }
    }

    void CircuitConvert()
    {
    }

    void OperateBlockSingle2(FunOp gate, LL k, LL first, LL second)
    {
        Node *hd11 = ListTable[first].head1;
        Node *hd1 = hd11->next;
        Node *hd21 = ListTable[second].head1;
        Node *hd2 = hd21->next;
        while (hd1->next)
        {
            LL a = hd1->next->num;
            LL b = hd2->next->num;
            if (a > b)
            {
                Node *temp = new Node(hd1->next->val, a - b);
                temp->next = hd1->next->next;
                hd1->next->next = temp;
            }
            else if (b > a)
            {
                Node *temp = new Node(hd2->next->val, b - a);
                temp->next = hd2->next->next;
                hd2->next->next = temp;
            }
            double v1 = hd1->next->val;
            double v2 = hd2->next->val;

            pdd temp = gate(v1, v2);
            hd1->next->val = temp.first;
            hd2->next->val = temp.second;

            if (hd1->val == hd1->next->val)
            {
                hd1->num += hd1->next->num;
                hd1->next = hd1->next->next;
            }

            if (hd2->val == hd2->next->val)
            {
                hd2->num += hd2->next->num;
                hd2->next = hd2->next->next;
            }

            hd1 = hd1->next;
            hd2 = hd2->next;
        }
    }

    void OperateBlockSingle1(FunOp gate, LL k, LL first)
    {
        LL step = 2 * (pow(2, k));
        LL inc = pow(2, k);

        Node *hd = ListTable[first].head2;
        while (hd->next)
        {
            if (hd->next->deal)
            {
                hd = hd->next;
                continue;
            }
            Node *nd = hd->next;
            if (step < nd->num)
            {
                pdd tt = gate(nd->val, nd->val);
                if (tt.first != tt.second)
                {
                    Node *h1 = new Node(tt.first, step / 2, 1);
                    Node *h2 = new Node(tt.second, step / 2, 1);
                    h1->next = h2;
                    h2->next = nd;
                    hd->next = h1;
                    nd->num -= step;
                    hd = h2;
                }
                else
                {
                    Node *h1 = new Node(tt.first, step, 1);

                    h1->next = nd;
                    hd->next = h1;
                    nd->num -= step;
                    hd = h1;
                }
            }
            else if (step == nd->num)
            {
                pdd tt = gate(nd->val, nd->val);
                if (tt.first != tt.second)
                {
                    Node *h1 = new Node(tt.first, step / 2, 1);
                    Node *h2 = new Node(tt.second, step / 2, 1);
                    h1->next = h2;
                    h2->next = nd->next;
                    hd->next = h1;
                    delete nd;
                    hd = h2;
                }
                else
                {
                    Node *h1 = new Node(tt.first, step, 1);

                    h1->next = nd->next;
                    hd->next = h1;
                    delete (nd);
                    hd = h1;
                }
            }
            else
            {
                LL sub = step - nd->num + 1;
                Node *temp = nd->next;
                while (temp->num < sub)
                {
                    sub -= temp->num;
                    temp = temp->next;
                }
                if (sub != 0)
                {
                    // sub+=temp->num;
                    Node *temp1 = new Node(temp->val, temp->num - sub);
                    temp->num = sub;
                    temp1->next = temp->next;
                    temp->next = temp1;
                }
                pdd tt = gate(nd->val, temp->val);
                nd->val = tt.first;
                temp->val = tt.second;
            }

            if (hd->next->val == hd->val)
            {
                hd->num += hd->next->num;
                Node *nd = hd->next;
                hd->next = hd->next->next;
                delete nd;
            }

            hd = hd->next;
        }
    }

    void DisplayLink()
    {
        for (LL i = 0; i < GroupNumber; i++)
        {
            cout << i << ":" << endl;
            Node *head = ListTable[i].head2->next;
            while (head)
            {
                cout << head->val << " " << head->num << "--";
                head = head->next;
            }
            cout << endl;
        }
        return;
    }
};
