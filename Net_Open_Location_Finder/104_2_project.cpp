#include <iostream>
#include <fstream>  
#include <cstring>  
#include <sstream>
#include <vector>
#include <stdlib.h>

using namespace std;

char input_filename[] = "input.txt";
char output_filename[] = "output.txt";
char nospacefilename[] = "nospace.txt";
int line_num = 0;
int cnt = 0;
int shape_cnt = 0;
string line[1000];
string buffer[1000];
void string_replace(string & strBig, const string & strsrc, const string &strdst);
class Shape{
	public:
		int x1;
		int y1;
		int x2;
		int y2;
};
Shape M[100];
void get_shape(int shape_cnt, string s);
bool compare_shape(Shape a,Shape b);
int main(int argc,char const *argv[])
{
	fstream fp;
	fp.open(input_filename,ios::in);
	if (fp.fail())
	{
		cout << "fail to open file" << endl;
	}
	else{
		while(getline(fp,buffer[line_num],'\n')){
			string_replace(buffer[line_num]," ","");
			line_num++;
		}
		fp.close();
	}
	fp.open(nospacefilename,ios::out);
	if (fp.fail())
	{
		cout << "fail to create file" << endl;	
	}
	else{
		for (int i = 0; i < line_num; ++i){
			if (buffer[i][0]=='#') continue;
			else if(buffer[i]=="") continue;
			else if(buffer[i]=="...") continue;
			else
				fp << buffer[i] << endl;
		}
		fp.close();
	}
	fp.open(nospacefilename,ios::in);
	if(fp.fail()) cout << "fail to open file" << endl;
	else{
		while(getline(fp,line[cnt],'\n')){
			if(line[cnt].substr(0,12) == "RoutedShapeM"){
				get_shape(shape_cnt, line[cnt]);
				shape_cnt++;
			} 
			cnt++;
		}
		for (int i = 0; i < shape_cnt; ++i)
		{
			cout << "shape" << i+1 << " x1 : " << M[i].x1 << endl;
			cout << "shape" << i+1 << " y1 : " << M[i].y1 << endl;
			cout << "shape" << i+1 << " x2 : " << M[i].x2 << endl;
			cout << "shape" << i+1 << " y2 : " << M[i].y2 << endl;
			cout << endl;
		}
		if(compare_shape(M[1],M[2]))
			cout << "yes" <<endl;
		else
			cout << "no" << endl;
	}
	return 0;
}
void string_replace(string & strBig, const string & strsrc, const string &strdst)
{
    string::size_type pos=0;
    string::size_type srclen=strsrc.size();
    string::size_type dstlen=strdst.size();
    while((pos=strBig.find(strsrc, pos))!= string::npos)
    {
        strBig.replace(pos, srclen, strdst);
        pos += dstlen;
    }
}
void get_shape(int shape_cnt, string s){
	int i = 0;
	int j = 0;
	string token[10];
	string token2[10];
	stringstream ss(s); 
	while(getline(ss,token[i],',')){
		i++;
	}
	for (int j = 0; j < 3; j++)
	{
		int x = 0;
		stringstream sss(token[j]);
		while(getline(sss,token2[x],'(')){
			x++;
		}
		if (j == 0)
		{
			M[shape_cnt].x1 = atoi(token2[1].c_str());
		}
		else if (j == 1)
		{
			M[shape_cnt].y1 = atoi(token2[0].substr(0,token2[0].size()-1).c_str());
			M[shape_cnt].x2 = atoi(token2[1].c_str());
		}
		else{
			M[shape_cnt].y2 = atoi(token2[0].substr(0,token2[0].size()-2).c_str());
		}
	}
}
bool compare_shape(Shape a,Shape b){
	if (b.x1 > a.x1 && b.y1 > a.y1)
	{
		if (b.x1 < a.x2 && b.y1 < a.y2)
			return true;
		else
			return false;
	}
	else if (b.x1 > a.x1 && a.y1 > b.y1)
	{
		if (b.x1 < a.x2 && b.y2 > a.y1)
			return true;
		else
			return false;
	}
	else if (a.x1 > b.x1 && b.y1 > a.y1)
	{
		if (b.x2 > a.x1 && a.y2 > b.y1)
			return true;
		else 
			return false;
	}
	else if (a.x1 > b.x1 && a.y1 > b.y1)
	{
		if (b.x2 > a.x1 && a.y2 > b.y1)
			return true;
		else
			return false;
	}
}
