
#include<iostream>
#include"linklisk.hpp"

using pdd = pair<double,double>;
using namespace std;

/*
pdd QumSwap(double a,double b){
    return {b,a};
}
*/

int main(){
    vector<double> temp = {1.0 ,0.0 ,1.0 ,1.0 ,0.0 ,0.0 ,0.0 ,0.0};
    cd a;
    a.Convert(temp);
    a.DisplayA();
    a.DisplayB();
    a.Operate(QumSwap,1);
    a.DisplayA();
    a.DisplayB();

}