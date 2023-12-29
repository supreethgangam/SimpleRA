#include<bits/stdc++.h>
using namespace std;

int main(void)
{
    ofstream fout("A1.csv", ios::out);
    //create a file named A1.csv and write 10000x10000 number strings to it similar to A.csv
    for(int i=0;i<10000;i++)
    {
        for(int j=0;j<10000;j++)
        {
            fout<<i*10000+j<<",";
        }
        fout<<endl;
    }
    fout.close();
    return 0;
}