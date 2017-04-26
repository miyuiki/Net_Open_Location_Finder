#include <iostream>
#include <fstream>  
#include <cstring>  
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <list>
#include <queue>
#include <iomanip>  
using namespace std;

class Shape{
	public:
		int layer;
		int id;
		int x1;
		int y1;
		int x2;
		int y2;
};
/*class Component{
	private:
		vector<Shape> shape_vec[100];
	public:
		void addShape(Shape s,vector<Shape> v);
};*/

class Graph{
private:
    int num_vertex;
    vector< list<int> > AdjList;
    int *color,             // 0:white, 1:gray, 2:black
        *predecessor,
        *discover,
        *finish;
public:
	Graph():num_vertex(0){};
    Graph(int N):num_vertex(N){
        // initialize Adj List
        AdjList.resize(num_vertex);
    };
    void AddEdgeList(int from, int to);
    void DFS(int Start);
    void DFSVisit(int vertex, int &time);
    
    void CCDFS(int vertex);                
    void SetCollapsing(int vertex);
    void PrintPredecessor();              
};

char input_filename[] = "input.txt";
char output_filename[] = "output.txt";
char nospacefilename[] = "nospace.txt";
int line_num = 0;
int cnt = 0;
int shape_cnt = 0;
int component_num = 0; 
string line[1000];
string buffer[1000];
Shape M[100];
vector<Shape> adjlist[100];
vector<Shape> shape_vec[100];
void get_shape(int shape_cnt, string s);
bool compare_shape(Shape a,Shape b);
void create_adj();
void string_replace(string & strBig, const string & strsrc, const string &strdst);

void Graph::DFS(int Start){
    color = new int[num_vertex];           
    discover = new int[num_vertex];
    finish = new int[num_vertex];
    predecessor = new int[num_vertex];

    int time = 0;                          
    for (int i = 0; i < num_vertex; i++) { 
        color[i] = 0;
        discover[i] = 0;
        finish[i] = 0;
        predecessor[i] = -1;
    }

    int i = Start;
    for (int j = 0; j < num_vertex; j++) { 
        if (color[i] == 0) {               
            DFSVisit(i, time);
        }
        i = j;                             
    }
}

void Graph::DFSVisit(int vertex, int &time){   
    color[vertex] = 1;                        
    discover[vertex] = ++time;                
    for (std::list<int>::iterator itr = AdjList[vertex].begin();   
         itr != AdjList[vertex].end(); itr++) {                    
        if (color[*itr] == 0) {               
            predecessor[*itr] = vertex;        
            DFSVisit(*itr, time);             
        }
    }
    color[vertex] = 2;                        
    finish[vertex] = ++time;                 
}

void Graph::SetCollapsing(int current){
    int root;  // root
    for (root = current; predecessor[root] >= 0; root = predecessor[root]);

    while (current != root) {
        int parent = predecessor[current];
        predecessor[current] = root;
        current = parent;
    }
}

void Graph::CCDFS(int vertex = 0){

    DFS(vertex);      // 
    PrintPredecessor();
    for (int i = 0; i< num_vertex; i++){
        SetCollapsing(i);
    }
    PrintPredecessor();

    int num_cc = 0;
    for (int i = 0; i < num_vertex; i++) {
    	
        if (predecessor[i] < 0 && i != 0) {
            cout << "Component#" << ++num_cc << ": " << i << " ";
            
            shape_vec[num_cc-1].push_back(M[i-1]);
            
            for (int j = 0; j < num_vertex; j++) {
                if (predecessor[j] == i && j != 0) {
                    cout << j << " ";
                    component_num = num_cc;
                    shape_vec[num_cc-1].push_back(M[j-1]);
                }
            }
            cout << endl;
        }
    }
}

void Graph::PrintPredecessor(){
    cout << "predecessor:" << endl;
    for (int i = 0; i < num_vertex; i++)
        cout << setw(4) << i;
    cout << std::endl;
    for (int i = 0; i < num_vertex; i++)
        cout << setw(4) << predecessor[i];
    cout << endl;
}

void Graph::AddEdgeList(int from, int to){
    AdjList[from].push_back(to);
}

/*void Component::addShape(Shape s,vector<Shape> v){
	v.push_back(s);
}*/

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
			M[shape_cnt].y2 = atoi(token2[0].substr(0,token2[0].size()-1).c_str());
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
void create_adj(){
	
	for(int i = 0; i < shape_cnt; i++){
		adjlist[i].push_back(M[i]);
	}
	for(int i = 0; i < shape_cnt; i++){
		for(int j = 0;j < shape_cnt ; j++){
			if(compare_shape(M[i],M[j]) && M[i].id != M[j].id)
				adjlist[i].push_back(M[j]);
		}
	}
}
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
				M[shape_cnt].layer = int(line[cnt].at(12))-48;
				M[shape_cnt].id = shape_cnt+1;
				shape_cnt++;
			} 
			cnt++;
		}
		for (int i = 0; i < shape_cnt; ++i)
		{
			cout << "shape" << i+1 << " ID : : " << M[i].id << endl;
			cout << "shape" << i+1 << " Layer : " << M[i].layer << endl;
			cout << "shape" << i+1 << " x1 : " << M[i].x1 << endl;
			cout << "shape" << i+1 << " y1 : " << M[i].y1 << endl;
			cout << "shape" << i+1 << " x2 : " << M[i].x2 << endl;
			cout << "shape" << i+1 << " y2 : " << M[i].y2 << endl;
			cout << endl;
		}
		create_adj();
		cout << "AdjList" << endl;
		for(int i = 0;i < shape_cnt; i++){
			for(int j = 0; j < adjlist[i].size(); j++)
				cout << adjlist[i].at(j).id <<" ";
			cout << endl;
		}
		Graph obj(shape_cnt+1);   
		for(int i = 1; i < shape_cnt+1;i++){
			for(int j = 0; j < adjlist[i-1].size();j++){
				if(adjlist[i-1].at(j).id != i)
					obj.AddEdgeList(i,adjlist[i-1].at(j).id);
			}
		}   	
		obj.CCDFS();
		cout << endl;
		for (int i = 0; i < component_num; i++)
		{
			for (int j = 0; j < shape_vec[i].size(); j++){
				cout << shape_vec[i].at(j).id << " "  ;
			}
			cout << endl;
		}
		cout << "component_num " << component_num << endl;
		
		
	}
	return 0;
}

