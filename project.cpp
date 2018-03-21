#include <iostream>
#include <fstream>  
#include <cstring>  
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <list>
#include <queue>
#include <iomanip>  
#include <algorithm>
#include <string.h> 
using namespace std;

struct Edge{
    int from, to, weight;
    Edge(){};
    Edge(int u, int v, int w):from(u), to(v), weight(w){};
};//邊結構

class Via{
	public:
		int x;
		int y;
};

class Shape{
	public:
		int layer;
		int id;
		int x1;
		int y1;
		int x2;
		int y2;
};
class Obstacle
{
	public:
		int layer;
		int id;
		int x1;
		int y1;
		int x2;
		int y2;	
};

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

class GraphMST{
private:
    int num_vertex;
    vector<vector<int> > AdjMatrix;//component 相鄰矩陣
public:
    GraphMST():num_vertex(0){};
    GraphMST(int n):num_vertex(n){
        AdjMatrix.resize(num_vertex);
        for (int i = 0; i < num_vertex; i++) {
            AdjMatrix[i].resize(num_vertex);
        }
    }
    void AddEdge(int from, int to, int weight);

    void KruskalMST();
    void GetSortedEdge(std::vector<struct Edge> &vec);
    friend int FindSetCollapsing(int *subset, int i);
    friend void UnionSet(int *subset, int x, int y);
};//最小生成樹物件

char input_filename[] = "input.txt";
char output_filename[] = "output.txt";
int line_num = 0;
int cnt = 0;
int shape_cnt = 0;
int via_cnt = 0;
int component_num = 0; 
int ccweight[1000][1000];
string line[10000];
string buffer[10000][100];
Shape M[10000];
Via V[1000];
vector<Shape> adjlist[10000];
vector<Shape> component[10000];

void get_shape(int shape_cnt, string s);
void get_distant(vector<Shape> v1, vector<Shape> v2, int ccfrom,int ccto); // 算距離
int position(Shape s1, Shape s2);// 算位置並得到距離
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
            
            component[num_cc-1].push_back(M[i-1]);
            
            for (int j = 0; j < num_vertex; j++) {
                if (predecessor[j] == i && j != 0) {
                    cout << j << " ";
                    component_num = num_cc;
                    component[num_cc-1].push_back(M[j-1]);
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
    cout << endl;
    for (int i = 0; i < num_vertex; i++)
        cout << setw(4) << predecessor[i];
    cout << endl;
}

void Graph::AddEdgeList(int from, int to){
    AdjList[from].push_back(to);
}
bool compare_shape(Shape a,Shape b){//shape is connected or not
	if (a.layer == b.layer){
		return !(a.x2 < b.x1 || a.x1 > b.x2 ||
             	a.y2 < b.y1 || a.y1 > b.y2);
	}
	else{
		for (int i = 0; i < via_cnt; ++i){
			if ((V[i].x > a.x1)&&(V[i].x < a.x2)&&(V[i].y > a.y1)&&(V[i].y < a.y2)){
				if ((V[i].x > b.x1)&&(V[i].x < b.x2)&&(V[i].y > b.y1)&&(V[i].y < b.y2))
					return true;
				else
					return false;
			}
			else
				return false;
		}
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
void get_distant(vector<Shape> v1, vector<Shape> v2, int ccfrom, int ccto){
	ccweight[ccfrom][ccto] = 9999;
	for (int i = 0; i < v1.size(); i++)
	{
		for (int j = 0; j < v2.size(); j++)
		{
			if(position(v1.at(i), v2.at(j)) < ccweight[ccfrom][ccto])
				ccweight[ccfrom][ccto] = position(v1.at(i), v2.at(j));
		}
	}
}
int position(Shape s1, Shape s2){
	int distant = 9487;
	if (s1.layer == s2.layer){
		if (s2.x1 >= s1.x1){
			if (s2.y1 > s1.y2){
				if (s2.x1 >= s1.x2)
					distant = (s2.x1 - s1.x2) + (s2.y1 - s1.y2);
				else
					distant = s2.y1 - s1.y2;
			}
			else if (s2.y2 < s1.y1){
				if (s2.x1 >= s1.x2)
					distant = (s2.x1 - s1.x2) + (s1.y1 - s2.y2);
				else
					distant = s1.y1 - s2.y2;
			}
			else
				distant = s2.x1 - s1.x2;
		}
		else{
			if (s2.y1 > s1.y2){
				if (s1.x1 >= s2.x2)
					distant = (s1.x1 - s2.x2) + (s2.y1 - s1.y2);
				else
					distant = s2.y1 - s1.y2;
			}
			else if (s2.y2 < s1.y1){
				if (s2.x2 <= s1.x1)
					distant = (s1.x1 - s2.x2) + (s1.y1 - s2.y2);
				else
					distant = s1.y1 - s2.y2;
			}
			else
				distant = s1.x1 - s2.x2;
		}
	}
	else{
		if(!(s1.x2 < s2.x1 || s1.x1 > s2.x2 || s1.y2 < s2.y1 || s1.y1 > s2.y2)){
			distant = 20;//ViaCost
		}
		else{
			if (s2.x1 >= s1.x1){
				if (s2.y1 > s1.y2){
					if (s2.x1 >= s1.x2)
						distant = (s2.x1 - s1.x2) + (s2.y1 - s1.y2) + 20;
					else
						distant = s2.y1 - s1.y2 + 20;
				}
				else if (s2.y2 < s1.y1){
					if (s2.x1 >= s1.x2)
						distant = (s2.x1 - s1.x2) + (s1.y1 - s2.y2) + 20;
					else
						distant = s1.y1 - s2.y2 + 20;
				}
				else
					distant = s2.x1 - s1.x2 + 20;
			}
			else{
				if (s2.y1 > s1.y2){
					if (s1.x1 >= s2.x2)
						distant = (s1.x1 - s2.x2) + (s2.y1 - s1.y2) + 20;
					else
						distant = s2.y1 - s1.y2 + 20;
				}
				else if (s2.y2 < s1.y1){
					if (s2.x2 <= s1.x1)
						distant = (s1.x1 - s2.x2) + (s1.y1 - s2.y2) + 20;
					else
						distant = s1.y1 - s2.y2 + 20;
				}
				else
					distant = s1.x1 - s2.x2 + 20;
			}
		}
	}
	return distant;
}
int FindSetCollapsing(int *subset, int i){      // 用遞迴做collapsing

    int root;  // root
    for (root = i; subset[root] >= 0; root = subset[root]);

    while (i != root) {
        int parent = subset[i];
        subset[i] = root;
        i = parent;
    }

    return root;
}
void UnionSet(int *subset, int x, int y){

    int xroot = FindSetCollapsing(subset, x),
        yroot = FindSetCollapsing(subset, y);

    // 用rank比較, 負越多表示set越多element, 所以是值比較小的element比較多
    // xroot, yroot的subset[]一定都是負值
    if (subset[xroot] <= subset[yroot]) {        // x比較多element或是一樣多的時候, 以x作為root
        subset[xroot] += subset[yroot];
        subset[yroot] = xroot;
    }
    else {    //  if (subset[xroot] > subset[yroot]), 表示y比較多element
        subset[yroot] += subset[xroot];
        subset[xroot] = yroot;
    }
}
bool WeightComp(struct Edge e1, struct Edge e2){
    return (e1.weight < e2.weight);
}
void GraphMST::GetSortedEdge(vector<struct Edge> &edgearray){

    for (int i = 0; i < num_vertex-1; i++) {
        for (int j = i+1; j < num_vertex; j++) {
            if (AdjMatrix[i][j] != 0) {
                edgearray.push_back(Edge(i,j,AdjMatrix[i][j]));
            }
        }
    }
    // 用std::sort 排序, 自己定義一個comparison
    sort(edgearray.begin(), edgearray.end(), WeightComp);
}
void GraphMST::KruskalMST(){

    struct Edge *edgesetMST = new struct Edge[num_vertex-1];
    int edgesetcount = 0;

    int subset[num_vertex];
    for (int i = 0; i < num_vertex; i++) {
        subset[i] = -1;
    }

    vector<struct Edge> increaseWeight;
    GetSortedEdge(increaseWeight);              // 得到排好序的edge的vec

    for (int i = 0; i < increaseWeight.size(); i++) {
        if (FindSetCollapsing(subset, increaseWeight[i].from) != FindSetCollapsing(subset, increaseWeight[i].to)) {
            edgesetMST[edgesetcount++] = increaseWeight[i];
            UnionSet(subset, increaseWeight[i].from, increaseWeight[i].to);
        }
    }
    // 以下僅僅是印出vertex與vertex之predecessor
    cout << setw(3) << "v1" << " - " << setw(3) << "v2"<< " : weight\n";
    for (int i = 0; i < num_vertex-1; i++) {
        cout << setw(3) << edgesetMST[i].from + 1 << " - " << setw(3) << edgesetMST[i].to + 1 
                  << " : " << setw(4) << edgesetMST[i].weight << "\n";
    }
}
void GraphMST::AddEdge(int from, int to, int weight){
    AdjMatrix[from][to] = weight;
}

int main(int argc,char const *argv[])
{
	fstream fp;
	fp.open(input_filename,ios::in);
	if(fp.fail()) cout << "fail to open file" << endl;
	else{
		while(getline(fp,line[cnt],'\n')){
			if (line[cnt][0]=='#') continue;
			char spliter[] = " ,()=";
			char cStr[100];
			int x = 0;
			strcpy(cStr, line[cnt].c_str());
			char * pch;
  			pch = strtok(cStr, spliter );
  			while( pch != NULL ){
  				string str;
    			buffer[cnt][x] = str.assign(pch);
    			pch = strtok( NULL, spliter );
    			x++;
  			}
  			if(buffer[cnt][0] == "RoutedShape"){
    			M[shape_cnt].id = shape_cnt+1;
    			M[shape_cnt].layer = atoi(buffer[cnt][1].substr(1,1).c_str());
    			M[shape_cnt].x1 = atoi(buffer[cnt][2].c_str());
    			M[shape_cnt].y1 = atoi(buffer[cnt][3].c_str());
    			M[shape_cnt].x2 = atoi(buffer[cnt][4].c_str());
    			M[shape_cnt].y2 = atoi(buffer[cnt][5].c_str());
    			shape_cnt++;	
			}
			if (buffer[cnt][0] == "RoutedVia"){
				V[via_cnt].x = atoi(buffer[cnt][2].c_str());
				V[via_cnt].y = atoi(buffer[cnt][3].c_str());
				via_cnt++;
			}
			cnt++;
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
			for (int j = 0; j < component[i].size(); j++){
				cout << component[i].at(j).id << " "  ;
			}
			cout << endl;
		}
		cout << endl;
		for (int i = 0; i < component_num; i++)
		{
			for (int j = 0; j < component_num; j++)
			{
				if (i != j)
				{
					get_distant(component[i],component[j],i,j);
				}
				else
					ccweight[i][j] = 0;
				cout << ccweight[i][j] << " ";
			}
			cout << endl;
		}
		GraphMST mst(component_num);
		for(int i = 0; i < component_num;i++){
			for(int j = 0; j < component_num;j++){
				mst.AddEdge(i,j,ccweight[i][j]);
			}
		}   	
		cout << "\nMST found by Kruskal:\n";
    	mst.KruskalMST();
	}
	return 0;
}

