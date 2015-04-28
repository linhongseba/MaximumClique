#include<stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
/*@author: linhong (linhong.seba.zhu@gmail.com)
@last update time: 12, May, 2011
 define all the global variables and functions that are used in MCE_TODS.cpp
*/
///////////////////////////////////////////////////////////////////////////////
/////////////////Global variables Declaration (START)/////////////////////////
///////////////////////////////////////////////////////////////////////////////
/*
this  part keeps those frequently used global variables
and frequently used global functions
*/

/////////////////////////////////////////////////////////////////////////////
////////////////STRUCTURE AND VARIABLES RELATED TO GRAPH (START)///
/////////////////////////////////////////////////////////////////////////////
struct Entry{
	int key;
	int vid;
	int *nbv;
};
struct DegEntry{
	int key;
	int vid;
};
/////////////////////////////////////////////////////////////////////////////
////////////////STRUCTURE AND VARIABLES RELATED TO GRAPH (END)///////////////
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//////////////class related to Heap (START)///////////////////////////////
//////////////////////////////////////////////////////////////////////////
class Heap{
public:
	Entry *cont;
	int hsize;        // the heap size
	int used;         // number of used places
	int maxused;
	Heap();
	~Heap();
	inline void clean(int min);
	inline int cleankeys(int min);//return the sum of key cleaned
	inline void enter(Entry *_he, int _pos);
	inline void insert(Entry *_he);
	inline void insertbyvid(Entry *_he);
	inline bool remove(Entry *_he);
};

Heap::Heap(){
	this->used=0;
	cont = NULL;
}
Heap::~Heap(){
	if(cont!=NULL)
		free(cont);
}
inline void Heap::clean(int min){
	//this function cleans those entries with keys smaller or equal than min
	//in the memory
	for (int i = 0; i < used; i ++){
		if (cont[i].key <= min){
			used = i;
			break;
		}
	}
}
inline int Heap::cleankeys(int min){
	int cpmused=used;
	int summin=0;
	int i=0;
	for (i = 0; i < used; i ++){
		if (cont[i].key <= min){
			used = i;
			break;
		}
	}
	for(i=cpmused;i>=used;i--){
		summin+=cont[i-1].key;
	}
	return summin;
}
inline void Heap::enter(Entry *_he, int _pos){
	if(_pos>=hsize){
		hsize=_pos;
		hsize+=100;
		cont=(Entry*)realloc(cont,sizeof(Entry)*(hsize+1));
		if(cont==NULL){
			printf("system could not allocate more memory\n");
			exit(0);
		}
	}
	cont[_pos].key=_he->key;
	cont[_pos].vid=_he->vid;
	cont[_pos].nbv=_he->nbv;
	used ++;
	if (maxused<used)
		maxused=used;
}
inline void Heap::insert(Entry *_he){
	int low=0;
	int high=used;
	int mid=0;
	while(low<high){
		mid=low+((high-low)/2);
		if(this->cont[mid].key>_he->key)
			low=mid+1;
		else
			high=mid;
	}
	for(int i=used;i>low;i--){
		cont[i].key=cont[i-1].key;
		cont[i].nbv=cont[i-1].nbv;
		cont[i].vid=cont[i-1].vid;
	}
	cont[low].key=_he->key;
	cont[low].vid=_he->vid;
	cont[low].nbv=_he->nbv;
	used++;
	if (maxused<used)
		maxused=used;
	if (used > hsize) {
		//this is why the heap size is initiated with one more place than hsize (to facilitate
		//coding)
		hsize+=1000;
		cont=(Entry*)realloc(cont,sizeof(Entry)*(hsize+1));
		if(cont==NULL){
			printf("system could not allocate more memory\n");
			printf("heap exceeded...\n");
			exit(1);
		}

	}
}
inline void Heap::insertbyvid(Entry *_he){
	int low=0;
	int high=used;
	int mid=0;
	while(low<high){
		mid=low+((high-low)/2);
		if(this->cont[mid].vid>_he->vid)
			high=mid;
		else
			if(this->cont[mid].vid<_he->vid)
				low=mid+1;
	}
	for(int i=used;i>low;i--){
		cont[i].key=cont[i-1].key;
		cont[i].nbv=cont[i-1].nbv;
		cont[i].vid=cont[i-1].vid;
	}
	cont[low].key=_he->key;
	cont[low].vid=_he->vid;
	cont[low].nbv=_he->nbv;
	used++;
	if (maxused<used)
		maxused=used;
	if (used > hsize) {
		//this is why the heap size is initiated with one more place than hsize (to facilitate
		//coding)
		hsize+=1000;
		cont=(Entry*)realloc(cont,sizeof(Entry)*(hsize+1));
		if(cont==NULL){
			printf("system could not allocate more memory\n");
			printf("heap exceeded...\n");
			exit(1);
		}

	}
}
inline bool Heap::remove(Entry *_he){
	//this function deheaps an entry. the return value indicates whether successful: false
		//means heap is already empty
	int low=0;
	int high=used;
	while(low<high){
		int mid=low+((high-low)/2);
		if(this->cont[mid].key>_he->key)
			low=mid+1;
		else
			high=mid;
	}
	if((low<used)&&(cont[low].key==_he->key)){
		for(int i=low;i<used;i++){
			cont[i].key=cont[i+1].key;
			cont[i].nbv=cont[i+1].nbv;
			cont[i].vid=cont[i+1].vid;
		}
		used--;
		return true;
	}
	else
		return false;
}

///////////////////////////////////////////////////////////////////////////
//////////////class related to Heap (END)///////////////////////////////
//////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
///////Other globe variables (start)//////////////////////////
///////////////////////////////////////////////////////////////
//const long int blocksize=3000000;
//const long int blocksize=1048576;
//const long int blocksize=524288;
//const long int blocksize=262144;
FILE * Wfile;
int outputOrNot = 0;
const long int blocksize=131072;
int totalcliquenum=0; //total number of cliques
int maxcliquelength=0; //maximum length(size) of cliques
int cliquesize=100;
int *R=NULL;          //the array to store clique
int *Rfreq=NULL;      //the array where Rfreq[i] counts number of cliques with length i
//int **Rmax=NULL;      //the array to store maximal
int Rcur=0;          //the current cursor for R array
int scan=0;          //number of innter iterations
int IOnumber=0;      //number of IO times
//int Rmaxnum=0;       //number of maximum liques
int avgdeg=0;          //average degree of the graph
char *isseednode=NULL;  //an array where each entry indicates whether a node is seed or not
int BYTE_TO_READ=6000000; //number of bytes that read into input buffer and output buffer
char *inputbuffer=NULL;   //input buffer
char *outputbuffer=NULL;  //output buffer
char *outcurpos=NULL;     //current cursor of outputbuffer
char *outendpos=NULL;     //end cursor of outputbuffer
char *curpos=NULL;       //current cursor of inputbuffer
char *endpos=NULL;      //current end cursor of inputbuffer
char *endpos0=NULL;
int byteread=0;        //an tempal variable
//char *readbuffer=NULL;
//int readbuffersize=10000;
float iotime=0;
int totalfinish=0;
int Vmin=0;
int Vmax=0;
char temp_string[33];// used for store value of inline function itostring
////////////////////////////////////////////////////////////////
///////Other globle variables (end)//////////////////////////
///////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (START)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int BLK_NUM=1024;
const int BLK_SZ=4194304;	//1024*1024*4

char *memBlkAr[BLK_NUM];	//An array of pointers to allocated blocks
char *curMemPos=NULL;	//The first free pos (startpos) in the first free block for writing
char *curMemEnd=NULL;	//The end pos of the first free block for writing
int curBlk=0;
int endBlk=0;	//curBlk: the ID of the current block
									//endBlk: the ID of the last allocated block
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for tmp memory management (END)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for permanent memory management (START)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int BLK_NUM2=1024;
const int BLK_SZ2=4194304;	//1024*1024*4

char *memBlkAr2[BLK_NUM2];	//An array of pointers to allocated blocks
char *curMemPos2=NULL;	//The first free pos (startpos) in the first free block for writing
char *curMemEnd2=NULL;	//The end pos of the first free block for writing
int curBlk2=0;	//curBlk: the ID of the current block
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Parameters for permanent memory management (END)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//////////global functions declaration (start)//////////////////
////////////////////////////////////////////////////////////////
inline int binarysearch(int *&arrays, int l, int h, int v);
inline int binarysearch(Entry *&G,int l, int h, int v);
inline int binarysearch2(Entry *&G,int l, int h, int v);
//perform binarysearch at arrays, find v's position in arrays
//if not found, return the maximum value l such that arrays[l]<v
inline void insert(int *&arrays, int pos, int v, int length);
//insert v into arrays at pos
//note that length++ and checking of out of range is done after and before insert function
inline void insert(int **&arrays,int pos, int *v,int length);
inline int interset(int *arrays1, int u, int l1, Entry *G);
inline void FillInputBuffer(FILE *inFile);
inline void GetData(char *dest,int byte2read, FILE *inFile);
inline void allocatetmpmemory(unsigned int size);
inline void allocatepermemory(unsigned int size);
inline void updateaddr(int *&addrmap, int &vmin, int &vmax,int &addrsize);
inline void ReadGraph(Entry *&G, int &gnodenum, int nodenum, unsigned long memorybound, FILE *inFile);


///////////////////////////////////////////////////////////////////
//////////global functions declaration (end)///////////////////////
///////////////////////////////////////////////////////////////////


//binarysearch: put BST as an array, for any given v,
//search it against arrays, if found, return its position
//otherwise, record the largest position low that arrays[low]<v;
//return -low-1;
inline int binarysearch(int *&arrays, int l, int h, int v){
	int cur=l;
	int low=l;
	int high=h;
	while(low<high){
		cur=low+((high-low)/2);
		if(arrays[cur]==v)
				return cur;
			else{
				if(arrays[cur]>v)
					high=cur;
				else
					low=cur+1;
			}
		}
	return -low-1;
}
inline int binarysearch(Entry *&G,int l, int h, int v){
	int cur=l;
	int low=l;
	int high=h;
	while(low<high){
		cur=low+((high-low)/2);
		if(G[cur].key==v)
			return cur;
		else
			if(G[cur].key>v)
				low=cur+1;
			else
				high=cur;
	}
	return -low-1;
}
inline int binarysearch2(Entry *&G,int l, int h, int v){
	int cur=l;
	int low=l;
	int high=h;
	while(low<high){
		cur=low+((high-low)/2);
		if(G[cur].vid==v)
			return cur;
		else
			if(G[cur].vid>v)
				high=cur;
			else
				low=cur+1;
	}
	return -low-1;
}
//========================================================================//
//========================end of binary search functions==================//
//========================================================================//
//================================================================================//
//insert v into arrays, make sure the index of arrays is not out of range (start)
//===============================================================================//
inline void insert(int *&arrays, int pos, int v, int length){
	int k=0;
	for(k=length;k>pos;k--)
		arrays[k]=arrays[k-1];
	arrays[pos]=v;
}

inline void insert(int **&arrays,int pos, int *v,int length){
	int k=0;
	for(k=length;k>pos;k--){
		arrays[k]=arrays[k-1];
	}
	arrays[pos]=v;
}

//================================================================================//
//insert v into arrays, make sure the index of arrays is not out of range (end)
//===============================================================================//
inline void remove(int *&arrays, int pos, int v, int length){
	int k=0;
	for(k=pos;k<length-1;k++)
		arrays[k]=arrays[k+1];
}

//return the size of arrays1 intersets G[u].nbv
inline int interset(int *arrays1, int u, int l1, Entry *G){
	int j=0;
	int k=0;
	int l=0;
	int m=0;
	////l keeps the size of intersect
	////  i/e, G[u].nbv intersect arrays1
	while(j<l1&&k<G[u].key){
		m=arrays1[j];
		if(m<0||m==u){
			j++;
			continue;
		}
		if(G[m].vid>G[u].nbv[k])
			k++;
		else
			if(G[m].vid<G[u].nbv[k])
				j++;
			else{
				l++;
				k++;
				j++;
			}
	}//end of while
	return l;
}

//return arrays1 intersets G[u].nbv

inline void intersetarray(int *&result, int &rsize, int *arrays, int l, int u, Entry *G){
	rsize=0;
	int k=0;
	int cur=0;
	while(k<l&&cur<G[u].key){
		if(arrays[k]<0)
			k++;
		else{
			if(G[arrays[k]].vid==G[u].nbv[cur]){
				result[rsize]=arrays[k];
				rsize++;
				k++;
				cur++;
			}else
				if(G[arrays[k]].vid>G[u].nbv[cur])
					cur++;
				else
					k++;
				}
			}
}
//load a part of infile into a buffer, which is stored as "char" format
inline void FillInputBuffer(FILE *inFile){
	int i=0;
	endpos=endpos0;
	if(endpos > curpos){
		i=endpos-curpos;
		//this essentially moves data after curpos to the start of the buffer
		//and then call GetData to fill data to the position starting from endpos
		memmove(inputbuffer,curpos,i);
		endpos=inputbuffer+i;
		//get data from Input file
		GetData(endpos,BYTE_TO_READ-i,inFile);	//new end position
	}
	else{
		endpos=inputbuffer;
		//get data from Input file
		GetData(endpos,BYTE_TO_READ,inFile);	//new end position
	}

	endpos+=byteread;

	if(!feof(inFile)){
		//we seek back to the end of the last complete line
		endpos0=endpos--;

		while(*endpos != '\n'){
			--endpos;
		}
		++endpos;	//endpos is one pos after '\n'
	}

	curpos=inputbuffer;
}
inline void GetData(char *dest,int byte2read, FILE *inFile){
	if(!feof(inFile)){
		byteread=fread(dest,1,byte2read,inFile);
//		printf("inputbufferB[0] = %d, inputbufferB[1] = %d, inputbufferB[2] = %d, inputbufferB[3] = %d, inputbufferB[4] = %d, inputbufferB[5] = %d \n", inputbufferB[0],inputbufferB[1],inputbufferB[2],  inputbufferB[3],inputbufferB[4],inputbufferB[5]);
//		exit(1);
		if(byteread < byte2read){
			//end of file or more to read
			if(!feof(inFile)){
				//Tests if a error has occurred in the last reading or writing operation with the given stream,
				//returning a non-zero value in case of error.
				if(ferror(inFile)){
					perror("Error reading from input file. Abort!\n");
					exit(1);
				}
			}
		}
	}
}

//check whether tmpblock can allocate a memory block with length size
void inline allocatetmpmemory(unsigned int size){
		if(size>=(size_t)(curMemEnd - curMemPos)||(curMemPos==NULL)||(curMemEnd - curMemPos)>BLK_SZ){//free mem in cur block is not enough
			if(curBlk < endBlk){
				//we have already allocated free blocks
				curMemPos=memBlkAr[++curBlk];
				curMemEnd=curMemPos+BLK_SZ;
			}else{
				//allocate a new block
				++endBlk;
				if(endBlk>=BLK_NUM){
					printf("system is unable to allocate more temporal memory\n");
					printf("number of block is %d\n",endBlk);
					exit(0);
				}
				curMemPos=(char*)malloc(BLK_SZ);
				if(curMemPos==NULL){
					printf("system is unable to allocate more temporal memory\n");
					printf("number of block is %d\n",curBlk);
					printf("number of static block is %d\n",curBlk2);
					exit(0);
				}
				memBlkAr[++curBlk]=curMemPos;
				curMemEnd=curMemPos+BLK_SZ;
			}
		}//end of if free mem is not enough
	}

//check whether the pemblock can allocate a memory block with length size
void inline allocatepermemory(unsigned int size){
		if(size>=(size_t)(curMemEnd2 - curMemPos2)||(curMemPos2==NULL)||(curMemEnd2 - curMemPos2)>BLK_SZ2){//free mem in cur block is not enough
			//free mem in cur block is not enough
			//allocate a new block
			if(curBlk2<BLK_NUM2-1){
				++curBlk2;
				curMemPos2=(char*)malloc(BLK_SZ2);
				if(curMemPos2==NULL){
					printf("system is unable to allocate more static memory\n");
					printf("number of block is %d\n",curBlk);
					printf("number of static block is %d\n",curBlk2);
					exit(0);
				}
				memBlkAr2[curBlk2]=curMemPos2;
				curMemEnd2=curMemPos2+BLK_SZ2;
			}else{
				printf("system is unable to allocate more static memory\n");
				exit(0);
			}
		}
	}

//parse an integer to string
inline int itostring(int value, char *& string, int radix){
	char *tp = temp_string;
	int i;
	unsigned vm;
	int sign;
	char *sp;
	int length=0;
	if (radix > 36 || radix <= 1){
		return 0;
	}
	sign = (radix == 10 && value < 0);
	if (sign)
		vm = -value;
	else
		vm = (unsigned)value;
	while (vm || tp == temp_string){
		i = vm % radix;
		vm = vm / radix;
		if (i < 10)
			*tp++ = i+'0';
		else
			*tp++ = i + 'a' - 10;
	}
	if (string == NULL)
		string = (char *)malloc((tp-temp_string)+sign+1);
	sp = string;
	if (sign){
		*sp++ = '-';
		length++;
	}
	while (tp > temp_string){
		*sp++ = *--tp;
		length++;
	}
	//*sp++='\0';
	//*sp = 0;
	return length;
}

//update the offset of each node in the local file
inline void updateaddr(int *&addrmap, int &vmin, int &vmax, int &addrsize){
	int v;
	char *tmppos=curpos;
	v=0;
	int pos=0;
	int j=0;
	while(1){
		v=0;
		do{
			v=(10 * v)+int(*tmppos)-48;
		}while(*(++tmppos) != ',');
		if(v<vmin)
			vmin=v;
		if(v>vmax)
			vmax=v;
		pos=tmppos-inputbuffer;
		if(pos<0){
			printf("error in update addr\n");
			exit(0);
		}
		if(j>=addrsize){
			addrmap=(int*)realloc(addrmap,sizeof(int)*(addrsize+1000));
			if(addrmap==NULL){
				printf("system could not allocate more memory\n");
				exit(0);
			}
			addrsize+=1000;
		}
		addrmap[j]=pos;
		j++;
		//printf("%d,%d\n",v,pos);
		do{
			++tmppos;
		}while(*tmppos != '\n');
		if(++tmppos >= endpos){
			//printf("testing updating address\n");
			break;
		}
	}
}

//read a subgraph of the input graph file into the memory
//remember to exclude those finish nodes (the finished H in previous round)
inline void ReadGraph(Entry *&G, int &gnodenum, int nodenum, unsigned long memorybound, FILE *inFile){
	Runtimecounter IORC;
	IORC.start();
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	int i=0;
	int v=0;
	int dv=0;
	int neighbore=0;
	int j=0;
	long sumSsize=0;
	/////////////////////////////////////////////////////////////////////////////
	//////Select base node/ seed node///////////////////////////////////////////
	/////make base node and its neighbores as 't'(start)///////////////////////
	/////base node is in the range of vmin and vmax///////////////////////////
	while(1){
		////////////////////////////////////
		//starting base node/seed node selection
		v=0;
		do{
			v=(10 * v)+int(*curpos)-48;
		}while(*(++curpos) != ',');
		//graphs that has been processed before
		//just skip
		if(v<=totalfinish){
			while(*curpos!='\n')
				curpos++;
			//when adjacent list is not in inputbuffer
			//reload a portion of infile into inputbuffer
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip
		if(sumSsize<memorybound){
			if(isseednode[v]!='t'){
				isseednode[v]='t';
				gnodenum++;
				if(v<Vmin)
					Vmin=v;
				if(v>Vmax)
					Vmax=v;
			}
			dv=0;
			if(*(++curpos) != '0'){
				dv=0;
				do{
					dv=(10 * dv)+int(*curpos)-48;
				}while(*(++curpos) != ':');
				if(dv<0||dv>nodenum){
					printf("error in graph degree\n");
					exit(0);
				}
				j=0;
				while(j<dv){
					neighbore=0;
					++curpos;
					while(*curpos > '/' && *curpos < ':'){
						neighbore=(10 * neighbore)+int(*curpos)-48;
						curpos++;
					}
					if(neighbore<0||neighbore>=nodenum){
						printf("error,please check line 721\n");
						exit(0);
					}
					if(isseednode[neighbore]!='t'){
						isseednode[neighbore]='t';
						gnodenum++;
					}
					j++;
				}
				while(*curpos!='\n')
					curpos++;
				sumSsize+=sizeof(Entry);
				sumSsize+=(sizeof(int)*dv*avgdeg);
			}//end: if(*(++curpos) != '0')
			else{
				while(*curpos!='\n')
					curpos++;
				sumSsize+=sizeof(Entry);
			}
		}else
			break;
		//when adjacent list is not in inputbuffer
		//reload a portion of infile into inputbuffer
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);
		}//end of reload
	}
	rewind(inFile);//back to the start position of inFile
	fscanf(inFile,"%d\n",&nodenum);
	/////////////////////////////////////////////////////////////////////////////
	//////Select base node/ seed node///////////////////////////////////////////
	/////make base node and its neighbores as 't'(end)/////////////////////////
	/////base node is in the range of vmin and vmax///////////////////////////
	//============================================================================//

	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(start)
	//////////////////////////////////////////////////////////////////////////////////////////////
    G=(Entry*)malloc(sizeof(Entry)*gnodenum);
	if(G==NULL){
		printf("system could not allocate more memory\n");
		exit(0);
	}
	for(i=0;i<gnodenum;i++){
		G[i].vid=-1;
		G[i].key=0;
		G[i].nbv=NULL;
	}
	curpos=endpos0=inputbuffer;	//initialize the data position pointers to the start of the buffer
	FillInputBuffer(inFile);
	//starting scanning
	int pos=0;
	while(1){
		v=0;
		do{
			v=(10 * v)+int(*curpos)-48;
		}while(*(++curpos) != ',');
		//skip the lines where the format of graph has some problem
		if(v<0||v>=nodenum){
			while(*curpos!='\n')
				curpos++;
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}//end of skip "bad" lines

		//skip the line where vid is smaller than vmin, i.e., the nodes that
		//have been processed
		//skip the nodes that are not S and S+ nodes
		if(isseednode[v]!='t'){
			while(*curpos!='\n')
				curpos++;
			if(++curpos >= endpos){
				if(feof(inFile))
					break;
				FillInputBuffer(inFile);
			}
			continue;
		}
		if(pos>=gnodenum){
                        G=(Entry*)realloc(G,sizeof(Entry)*(gnodenum+10));
			if(G==NULL){
				printf("could not allocate more memory\n");
				exit(0);
			}
			for(i=gnodenum;i<gnodenum+10;i++){
				G[i].vid=-1;
				G[i].key=0;
				G[i].nbv=NULL;
			}
			gnodenum+=10;
		}
		G[pos].vid=v;
		dv=0;
		if(*(++curpos) != '0'){
			dv=0;
			do{
				dv=(10 * dv)+int(*curpos)-48;
			}while(*(++curpos) != ':');
			if(dv<0||dv>nodenum){
				printf("error in graph degree\n");
				exit(0);
			}
			/////////////////////////////////////////////////////////////
			//for each node v,
			//allocate memory space to store a heap entry (dv,v,nb(v)) (START)
			///////////////////////////////////////////////////////////
			if(sizeof(int)*dv>=BLK_SZ2){
				G[pos].nbv=(int*)malloc(sizeof(int)*dv);
			}else{
				allocatepermemory(sizeof(int)*dv);
				G[pos].nbv=(int *)curMemPos2;
				curMemPos2+=(sizeof(int)*dv);
			}
			j=0;
			G[pos].key=0;
			while(j<dv){
				neighbore=0;
				++curpos;
				while(*curpos > '/' && *curpos < ':'){
					neighbore=(10 * neighbore)+int(*curpos)-48;
					curpos++;
				}
				if(neighbore<0||neighbore>=nodenum){
					printf("error,please check line 845\n");
					exit(0);
				}
				if(isseednode[neighbore]=='t'){
					G[pos].nbv[G[pos].key]=neighbore;
					G[pos].key++;
				}
				j++;
			}
			if(sizeof(int)*dv>=BLK_SZ2&&sizeof(int)*G[pos].key<BLK_SZ2){
				allocatepermemory(sizeof(int)*G[pos].key);
				int *tempnbv=(int*)curMemPos2;
				curMemPos2+=(sizeof(int)*G[pos].key);
				memcpy(tempnbv,G[pos].nbv,sizeof(int)*G[pos].key);
				if(G[pos].nbv!=NULL)
					free(G[pos].nbv);
				G[pos].nbv=tempnbv;
				tempnbv=NULL;
			}
			while(*curpos!='\n')
				curpos++;
		}//end: if(*(++curpos) != '0')
		else{
			G[pos].key=0;
			G[pos].nbv=NULL;
			while(*curpos!='\n')
				curpos++;
		}
		pos++;
		if(++curpos >= endpos){
			if(feof(inFile))
				break;
			FillInputBuffer(inFile);

		}
	}//end: while(i)
        gnodenum=pos;
	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////scanning the graph again to initializing the S and Splus nodes' adjacent lists(end)
	//////////////////////////////////////////////////////////////////////////////////////////////
	rewind(inFile);
	fscanf(inFile,"%d\n",&nodenum);
	IORC.stop();
	iotime+=IORC.GetRuntime();
}

//================================================================================================================//
//=====a sub procedure for maximal clique enumeration base on Bron-Kerbosch      (function start)
//=====algorithm with pivot selection and limitation on the start node of expansion =============================//
//this version the pivot is selected based on the size of v's neighbore intesects
//the P and X
inline void Subclique(Entry *&G,int *P, int *&X,int Pcur, int &Xcur,int Umin, int Umax, int Xmax){
	int i=0;
	int k=0;
	int l=0;
	int u=0;
	int up=-1;
	int j = 0;
	unsigned int size=0;
	char *precurMem=curMemPos;
	int precurblk=curBlk;
	int preendblk=endBlk;
	char* preendMem=curMemEnd;
	int cur=0;
	Runtimecounter IORC;
	if(Pcur==0&&Xcur==0){
		if(Rcur>=cliquesize){
			Rfreq=(int*)realloc(Rfreq,sizeof(int)*(Rcur+20));
			if(Rfreq==NULL){
				printf("could not allocate more memory\n");
				exit(0);
			}
			memset(Rfreq+cliquesize,0,sizeof(int)*(Rcur-cliquesize+20));
			cliquesize=Rcur+20;
		}
		Rfreq[Rcur]++;
		if(Rcur>maxcliquelength)
			maxcliquelength=Rcur;

		//outputControl
		if (outputOrNot) {
			for (i = 0; i < Rcur; i++) {
				////===================================================================//
				////=======output clique (start)======================================//
				////=================================================================//
				//printf("%d ",G[R[i]].vid);
				if (outendpos - outcurpos <= 33) {
					j = outcurpos - outputbuffer;
					IORC.start();
					fwrite(outputbuffer, sizeof(char), j, Wfile);
					IORC.stop();
					iotime += IORC.GetRuntime();
					outcurpos = outputbuffer;
				}
				j = itostring(G[R[i]].vid, outcurpos, 10);
				outcurpos += j;
				*outcurpos = ' ';
				outcurpos++;
			}

			if (outendpos - outcurpos <= 33) {
				j = outcurpos - outputbuffer;
				IORC.start();
				fwrite(outputbuffer, sizeof(char), j, Wfile);
				outcurpos = outputbuffer;
				IORC.stop();
				iotime += IORC.GetRuntime();
			}
			//printf("\n");
			*outcurpos = '\n';
			outcurpos++;

		}

		totalcliquenum++;
	//===================================================================//
	//=======output clique (end)======================================//
	//=================================================================//
	}else{
		int *cpyP=NULL;
		size=sizeof(int)*Pcur;
		if(size>=BLK_SZ){
			cpyP=(int*)malloc(size);
			if(cpyP==NULL){
				printf("could not allocate memory\n");
				exit(0);
			}
		}else{
			allocatetmpmemory(size);
			cpyP=(int*)curMemPos;
			curMemPos+=size;
		}
		//========================================================//
		//==============do pivot node selection (start)==========//
		//======================================================//
		if(Rcur>0){
			//now the pivot up is selected from (P union X)
			//which maximizing the size of P intersects G[up].nbv
			int maxintersect=0;
			for(i=0;i<Pcur;i++){
				u=P[i];
				if(G[u].key<maxintersect)
					continue;
				l=interset(P,u,Pcur,G);
				if(l>=maxintersect){
					maxintersect=l;
					up=u;
				}
			}//end of enumerating P
			//do the same thing in X
			for(i=0;i<Xcur;i++){
				u=X[i];
				if(G[u].key<maxintersect)
					continue;
				l=interset(P,u,Pcur,G);
				if(l>=maxintersect){
					maxintersect=l;
					up=u;
				}
			}//end of enumerating X
			if(up==-1){
				printf("please check line 989\n");
				exit(0);
			}
		}
		//========================================================//
		//==============do pivot node selection (end)==========//
		//======================================================//
		memcpy(cpyP,P,sizeof(int)*Pcur);
		//cpyP denotes u1 to uk in P, which will be change
		for(i=0;i<Pcur;i++){//for each u1 to uk in P
			u=P[i];
			if(Rcur==0){
				if(G[u].vid<Umin||G[u].vid>Umax)
					continue;
				if(G[u].vid>Vmax)
					continue;
			}else{
				if(G[u].vid<Umin){
					continue;
				}
				if(u!=up){
					cur=binarysearch(G[up].nbv,0,G[up].key,G[u].vid);
					if(cur>=0)
						continue;
				}
			}
			//starting from here
			//if it is a child of root, G[u].vid is inside [Umin, Umax]
			//otherwise, it is not a neighbore of pivot node up
			//also, it is not a preprocessed node
			cpyP[i]=-u-1; //remove u from cpyP
			R[Rcur]=u;
			Rcur++;
			char *precurMeminner=curMemPos;
			int precurblkinner=curBlk;
			int preendblkinner=endBlk;
			char* preendMeminner=curMemEnd;
			///////////////////////////////////////////////////////////
			///////////allocate memory to newP and newX  (start)//////
			///////////////////////////////////////////////////////////
			int *Pnew=NULL;
			int *Xnew=NULL;
			if(Pnew==NULL){
				size=sizeof(int)*G[u].key;
				if(size<BLK_SZ){
					allocatetmpmemory(size);
					Pnew=(int*)curMemPos;
					curMemPos+=(size);
				}else
					Pnew=(int*)malloc(size);
			}//end of NULL
			if(Xnew==NULL){
				size=sizeof(int)*G[u].key;
				if(size<BLK_SZ){
					allocatetmpmemory(size);
					Xnew=(int*)curMemPos;
					curMemPos+=(size);
				}else
					Xnew=(int*)malloc(size);
			}
			///////////////////////////////////////////////////////////
			///////////allocate memory to newP and newX  (end)///
			///////////////////////////////////////////////////////////

			///////////////////////////////////////////////////////////
			////////////Pnew=localp intersects nb(u)   (start)//////////
			//////////////////////////////////////////////////////////
			int Pnewcur=0;
			intersetarray(Pnew,Pnewcur,cpyP,Pcur,u,G);
			////////////////////////////////////////////////////////////
			///////////////Pnew=localp intersects nb(u)   (end)/////////
			///////////////////////////////////////////////////////////

			//======================================================//
			///////////////Xnew=X intersects nb(u)   (start)///////
			///====================================================//
			int Xnewcur=0;
			intersetarray(Xnew,Xnewcur,X,Xcur,u,G);
			//========================================================///
			//////////////////Xnew=X intersets nb(u)    (end)//////////
			//========================================================//
			Subclique(G,Pnew,Xnew,Pnewcur,Xnewcur,Umin,Umax,G[u].key);
			Rcur--;

			////////////////////////////////////////////////////////////
			////////////////release memory for Pnew, Xnew, cpyP (start)////
			////////////////////////////////////////////////////////////
			if(Pnew!=NULL){
				if(sizeof(int)*G[u].key>=BLK_SZ){
					free(Pnew);
					Pnew=NULL;
					Pnewcur=0;
				}
			}
			if(Xnew!=NULL){
				if(sizeof(int)*G[u].key>=BLK_SZ){
					free(Xnew);
					Xnew=NULL;
					Xnewcur=0;
				}
			}
			for(k=curBlk;k>precurblkinner;k--){
				if(memBlkAr[k]!=NULL)
					free(memBlkAr[k]);
			}
			curMemPos=precurMeminner;
			curMemEnd=preendMeminner;
			endBlk=preendblkinner;
			curBlk=precurblkinner;
			Pnew=NULL;
			Xnew=NULL;
			Pnewcur=0;
			Xnewcur=0;
			/////////////////////////////////////////////////////////////
			////////////////release memory for Pnew and Xnew (end)//////
			////////////////////////////////////////////////////////////
			if(Xcur>=Xmax){
				printf("testing out of range of X vector\n");
				exit(0);
			}
			cur=binarysearch(X,0,Xcur,u);
			if(cur<0){
				insert(X,-cur-1,u,Xcur);
				Xcur++;
			}
		}//end of enumerating P;
		if(cpyP!=NULL){
			if(sizeof(int)*Pcur>=BLK_SZ){
				free(cpyP);
				cpyP=NULL;
			}
		}
		cpyP=NULL;
	}//end of P is not empty or X is not empty
	for(i=curBlk;i>precurblk;i--){
		if(memBlkAr[i]!=NULL)
			free(memBlkAr[i]);
	}
	curMemPos=precurMem;
	curMemEnd=preendMem;
	endBlk=preendblk;
	curBlk=precurblk;
}
//================================================================================================================//
//=====a sub procedure for maximal clique enumeration base on Bron-Kerbosch      (function end)
//=====algorithm with pivot selection and limitation on the start node of expansion =============================//

//===============================================================================================//
//======
inline void Computeclique(Entry *&G, int Umin,int Umax, int nodenum, int maxdeg){
	int i;
	char *precurMem=curMemPos;
	int precurblk=curBlk;
	int preendblk=endBlk;
	char* preendMem=curMemEnd;
	int *P=NULL;
	int *X=NULL;
	int Pcur=0;
	int Xcur=0;
	unsigned int size;
	int j=0;
	Runtimecounter IORC;
	if(P==NULL){
		size=sizeof(int)*nodenum;
		if(size<BLK_SZ){
			allocatetmpmemory(size);
			P=(int*)curMemPos;
			curMemPos+=(size);
		}
		else
			P=(int*)malloc(size);
	}
	if(X==NULL){
		size=sizeof(int)*nodenum;
		if(size<BLK_SZ){
			allocatetmpmemory(size);
			X=(int*)curMemPos;
			curMemPos+=(size);
		}
		else
			X=(int*)malloc(size);
	}
	if(R==NULL){
		size=sizeof(int)*maxdeg;
		if(size<BLK_SZ){
			allocatetmpmemory(size);
			R=(int*)curMemPos;
			curMemPos+=(size);
		}
		else
			R=(int*)malloc(size);
	}
	for(i=0;i<nodenum;i++){
		if(G[i].vid<0||G[i].key<0)
			continue;
		if(G[i].key==0){
			if(G[i].vid>=Umin&&G[i].vid<=Umax){
				Rfreq[1]++;
				//outputControl
				if (outputOrNot) {
					//fprintf(Wfile, "%d\n", G[i].vid);
					if (outendpos - outcurpos <= 33) {
						j = outcurpos - outputbuffer;
						IORC.start();
						fwrite(outputbuffer, sizeof(char), j, Wfile);
						outcurpos = outputbuffer;
						IORC.stop();
						iotime += IORC.GetRuntime();
					}
					j = itostring(G[i].vid, outcurpos, 10);
					outcurpos += j;
					*outcurpos = '\n';
					outcurpos++;
				}


				totalcliquenum++;
			}
		}else{
			P[Pcur]=i;
			Pcur++;
		}
	}
	Xcur=0;
	Rcur=0;
	Subclique(G,P,X,Pcur,Xcur,Umin,Umax,nodenum);
	/////////////////////////////////////////////////////////////////
	//////////free temporal memory used inside function(start)//////
	////////////////////////////////////////////////////////////////
	if(R!=NULL){
		if(sizeof(int)*maxdeg>=BLK_SZ){
			free(R);
			R=NULL;
		}
	}
	if(P!=NULL){
		if(sizeof(int)*nodenum>=BLK_SZ){
			free(P);
			P=NULL;
		}
	}
	if(X!=NULL){
		if(sizeof(int)*nodenum>=BLK_SZ){
			free(X);
			X=NULL;
		}
	}
	for(i=curBlk;i>precurblk;i--){
		if(memBlkAr[i]!=NULL)
			free(memBlkAr[i]);
	}
	curMemPos=precurMem;
	curMemEnd=preendMem;
	endBlk=preendblk;
	curBlk=precurblk;
	/////////////////////////////////////////////////////////////////
	//////////free temporal memory used inside the function(end)////
	////////////////////////////////////////////////////////////////
	P=NULL;
	R=NULL;
	X=NULL;
	Rcur=0;
	Pcur=0;
	Xcur=0;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////Global variables Declaration (END)/////////////////////////
///////////////////////////////////////////////////////////////////////////////
