#include<iostream>
#include<cmath>
#include<cstdio>
using namespace std;
long double C[100][100];
int main(int argc,char ** argv){
    C[1][1]=C[1][0]=1;
    for(int i=2;i<100;i++){
        for(int j=1;j<i;j++)
            C[i][j] = C[i-1][j-1] + C[i-1][j];
        C[i][0] = 1;
    }
    int n = 16;
    int k = 4;
    int N = 256;
    long double cluster_fault = 0;
    long double cluster_correct = 0;
    long double p = 1e-3;
    for(int i=k+1;i<=n;i++)
        cluster_fault += C[n][i]*pow(p,i)*pow(1-p,n-i);
    long double total_fault = 0;
    total_fault = 1-pow(1-cluster_fault,N/n);
    cout<<cluster_fault<<" "<<total_fault<<endl;
    //printf("%.10Lf %.10Lf\n",cluster_fault,total_fault);
    return 0;
}
