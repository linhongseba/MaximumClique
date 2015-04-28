#define _CRT_SECURE_NO_WARNINGS
#include "Runtimecounter.h"
#include "MCE_TODS.h"

/*@author: linhong (linhong.seba.zhu@gmail.com)
@last update time: 09, May, 2011
EMMCE_nooutput.cpp: implements the external memory algorithm for enumerating all maximal cliques
count the size distribution of cliques
do not write the output to file
*/
// NOTE These next few lines may be win32 specific, you may need to modify them to compile on other platform
// Use CRT Debug facility to dump memory leaks on app exit

//#ifdef WIN32
//  // The following macros set and clear, respectively, given bits
//  // of the C runtime library debug flag, as specified by a bitmask.
//  #ifdef   _DEBUG
//    #define  SET_CRT_DEBUG_FIELD(a) \
//              _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
//    #define  CLEAR_CRT_DEBUG_FIELD(a) \
//              _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
//  #else
//    #define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
//    #define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
//  #endif
//#endif //WIN32


/*
 *  argv[1]: graph_file_name
 *  argv[2]: memory_bound_MB  
 *  argv[3]: B_node_num
 *  argv[4]: average_degree
 *  argv[5]: output_or_not, 0 means NOT output cliques, 1 means output cliques
 *
 * */

int main(int argc, char *argv[]){

	//for debug only
	//argc = 6;
	//argv[1] = "/home/shumo/mpi_dataset/LJ_s/LJ_s.txt";
	//argv[2] = "64";
	//argv[4] = "20";
	//argv[3] = "500";
	//argv[5] = "1";

	if(argc!=6){
		printf("Usage: [graph_file_name] [memory boundMB] [B_node_number] [average degree][output or not]\n");
		exit(0);
	}

	unsigned long memorybound=atol(argv[2]);
	memorybound*=1024;
	memorybound*=1024;
	//memorybound=80; //for debugging small graph only
	Runtimecounter RC;
	Runtimecounter IORC;
	Runtimecounter subRC;
	RC.start();
	/////////////////////////////////////////////////////////////
	////////////initializing memory block (START)////////////////
	////////////////////////////////////////////////////////////
	//Initialize memory blocks (START)
	curBlk=endBlk=0;
	memBlkAr[0]=curMemPos=(char*)malloc(BLK_SZ);
	curMemEnd=curMemPos+BLK_SZ;
	//Initialize memory blocks (END)
	//Initialize memory blocks (START)
	curBlk2=0;
	memBlkAr2[0]=curMemPos2=(char*)malloc(BLK_SZ2);
	curMemEnd2=curMemPos2+BLK_SZ2;
	//Initialize memory blocks (END)
	/////////////////////////////////////////////////////////
	////////////initializing memory block (end)////////////////
	////////////////////////////////////////////////////////
	int nodenum=0;
	int v=0;
	int dv=0;
	int maxdeg=0;
	int i=0;
	int j=0;
	int k=0;
	int minkey=0;
	int cur=0;
	int pos=0;
	int neighbore=0;
	BYTE_TO_READ=60000000;
	allocatetmpmemory(sizeof(char)*600);
	char *infilename=curMemPos;
	curMemPos+=(sizeof(char)*200);
	char *freqfilename=curMemPos;
	curMemPos+=(sizeof(char)*200);
	char *outfilename=curMemPos;
	curMemPos+=(sizeof(char)*200);
	strcpy(infilename,argv[1]);
	strcpy(freqfilename,argv[1]);
	strcat(freqfilename,".freq.txt");
	strcpy(outfilename,argv[1]);
	strcat(outfilename, ".cliquetree.txt");

	FILE *rFile=fopen(infilename,"rb");
	if(rFile==NULL){
		printf("Could not find the data file\n");
		exit(1);
	}
	fscanf(rFile,"%d\n",&nodenum);
	isseednode=(char*)malloc(sizeof(char)*nodenum);
	memset(isseednode,'v',sizeof(char)*nodenum);
	maxdeg=0;
	inputbuffer=(char*)malloc(BYTE_TO_READ);

	scan=0;
	//int Bsize=500;
	avgdeg=atoi(argv[4]);
	int Bsize=atoi(argv[3]);
	outputOrNot = atoi(argv[5]);
    if (outputOrNot) {
		Wfile = fopen(outfilename, "w");
		if (Wfile == NULL) {
			printf("could not write to the file\n");
			exit(0);
		}
	}
	//enable only when output is needed
	if (outputOrNot){
		outputbuffer=(char*)malloc(BYTE_TO_READ);
		outcurpos=outputbuffer;
		outendpos=outputbuffer+BYTE_TO_READ;
	}

	int vmemorynum=0;
	Entry *G=NULL;
	int Umin=nodenum;
	int Umax=-1;
	int sgnodenum=0;
	int finish=-1;
	totalfinish=-1;
	Vmin=nodenum;
	Vmax=-1;
	IOnumber=0;
	Rfreq=(int*)malloc(sizeof(int)*cliquesize);
	memset(Rfreq,0,sizeof(int)*cliquesize);
	int bnode=0; //number of base vertex size
	while(1){
		char *precurMem2=curMemPos2;
		int precurblk2=curBlk2;
		char* preendMem2=curMemEnd2;
		vmemorynum=0;
		ReadGraph(G,vmemorynum,nodenum,memorybound,rFile);
        //printf("finish reading graph\n");
		IOnumber++;
		memset(isseednode,'v',sizeof(char)*nodenum);
		Umin=G[vmemorynum-1].vid;
		Umax=0;
		finish=-1;
		printf("in memory seed set is [%d, %d]\n",Vmin, Vmax);
		while(1){
			///////////////////////////////////////////////////////////////
			//extract a SGinnter for clique computation
			//seed vertex is in the range [Umin,Umax]    (start)///////////
			//////////////////////////////////////////////////////////////
			subRC.start();
			char *precurMem=curMemPos;
			int precurblk=curBlk;
			int preendblk=endBlk;
			char* preendMem=curMemEnd;
			sgnodenum=0;
			maxdeg=0;
			bnode=0;

			//==========================================================
			//////mark seed vertices and their neighbors (start)////////
			//============================================================
			for(i=0;i<vmemorynum;i++){
				v=G[i].vid;
				if(v<=finish)
					continue;
				if(v<Vmin)
					continue;
				if(bnode<Bsize){
					if(isseednode[v]!='t'){
						isseednode[v]='t';
						bnode++;
						sgnodenum++;
						if(v>Umax)
							Umax=v;
						if(v<Umin)
							Umin=v;
					}
					if(G[i].key>maxdeg)
						maxdeg=G[i].key;
					for(j=0;j<G[i].key;j++){
						neighbore=G[i].nbv[j];
						if(isseednode[neighbore]!='t'){
							isseednode[neighbore]='t';
							sgnodenum++;
						}
					}
				}else
					break;
			}
			 //============================================================
			///////mark seed vertices and their neighbors (end)//////////
			//============================================================

			//==========initilize SGinnter (start)================
			Entry *SGinnter=(Entry*)malloc(sizeof(Entry)*sgnodenum);
			if(SGinnter==NULL){
				printf("system could not allocate more memory\n");
				exit(0);
			}
			for(i=0;i<sgnodenum;i++){
				SGinnter[i].vid=-1;
				SGinnter[i].key=0;
				SGinnter[i].nbv=NULL;
			}
			pos=0;
			for(i=0;i<vmemorynum;i++){
				v=G[i].vid;
				if(isseednode[v]!='t'){
					continue;
				}
				if(v>=Umin&&v<=Umax){
					//v is a seed node, all neighbores of v are in SG
					dv=G[i].key;
				}else{
					dv=0;
					for(j=0;j<G[i].key;j++){
						neighbore=G[i].nbv[j];
						if(isseednode[neighbore]=='t')
							dv++;
					}
				}
				if(pos>=sgnodenum){
					sgnodenum+=10;
					SGinnter=(Entry*)realloc(SGinnter,sizeof(Entry)*sgnodenum);
				}
				SGinnter[pos].vid=v;
				if(sizeof(int)*dv>=BLK_SZ){
					SGinnter[pos].nbv=(int*)malloc(sizeof(int)*dv);
				}else{
					allocatetmpmemory(sizeof(int)*dv);
					SGinnter[pos].nbv=(int *)curMemPos;
					curMemPos+=(sizeof(int)*dv);
				}
				if(v>=Umin&&v<=Umax){
					memcpy(SGinnter[pos].nbv,G[i].nbv,sizeof(int)*dv);
					SGinnter[pos].key=dv;
				}else{
					SGinnter[pos].key=0;
					for(j=0;j<G[i].key;j++){
						neighbore=G[i].nbv[j];
						if(isseednode[neighbore]=='t'){
							SGinnter[pos].nbv[SGinnter[pos].key]=neighbore;
							SGinnter[pos].key++;
						}
					}
				}
				pos++;
			}

			sgnodenum=pos;
			
			subRC.stop();

			subRC.start();
			Computeclique(SGinnter,Umin,Umax,sgnodenum,maxdeg);
			subRC.stop();

			memset(isseednode,'v',sizeof(char)*nodenum);
			for(i=0;i<sgnodenum;i++){
				if(SGinnter[i].key<0)
					continue;
				if(sizeof(int)*SGinnter[i].key>=BLK_SZ){
					if(SGinnter[i].nbv!=NULL){
						free(SGinnter[i].nbv);
						SGinnter[i].nbv=NULL;
					}
				}
			}
			if(SGinnter!=NULL){
				free(SGinnter);
				SGinnter=NULL;
			}
			scan++;
			for(j=curBlk;j>precurblk;j--){
				if(memBlkAr[j]!=NULL)
					free(memBlkAr[j]);
			}
			curMemPos=precurMem;
			curMemEnd=preendMem;
			endBlk=preendblk;
			curBlk=precurblk;
			if(Umax==G[vmemorynum-1].vid)
				break;
			Umin=G[vmemorynum-1].vid;
			finish=Umax;
		}//finish current in-memory clique computation
		for(i=0;i<vmemorynum;i++){
			if(G[i].key*sizeof(int)>=BLK_SZ2){
				if(G[i].nbv!=NULL){
					free(G[i].nbv);
					G[i].nbv=NULL;
				}
			}
			G[i].vid=-1;
			G[i].key=0;
			G[i].nbv=NULL;
		}
		for(j=curBlk2;j>precurblk2;j--){
			if(memBlkAr2[j]!=NULL)
				free(memBlkAr2[j]);
		}
		curMemPos2=precurMem2;
		curBlk2=precurblk2;
		curMemEnd2=preendMem2;
		if(G!=NULL){
			free(G);
			G=NULL;
		}
		if(Vmax==nodenum-1)
			break;
		Vmin=nodenum;
		totalfinish=Vmax;
	}
	printf("finished computing\n");
	IORC.start();
	fclose(rFile);
	if(outputOrNot){
		j=outcurpos-outputbuffer;
		fwrite(outputbuffer,sizeof(char),j,Wfile);
		outcurpos=outputbuffer;
		fclose(Wfile);
	}
	FILE *outfile=fopen(freqfilename,"w");
	if(outfile==NULL){
		printf("could not write the clique size distribution to file\n");
		exit(0);
	}
	for(i=1;i<=maxcliquelength;i++){
		fprintf(outfile, "%d\t%d\n",i,Rfreq[i]);
	}
	if(Rfreq!=NULL){
		free(Rfreq);
		Rfreq=NULL;
	}
	fclose(outfile);
	IORC.stop();
	iotime+=IORC.GetRuntime();
	if(inputbuffer!=NULL){
		free(inputbuffer);
		inputbuffer=NULL;
	}
	/*if(outputbuffer!=NULL){
		free(outputbuffer);
		outputbuffer=NULL;
	}*/
	if(isseednode!=NULL){
		free(isseednode);
		isseednode=NULL;
	}
	printf("maximum length of cliques is %d\n",maxcliquelength);
	printf("total number of maximum clique is %d\n",totalcliquenum);
	//printf("total number of IO is %d\n",IOnumber);
	//printf("total number of scanning is %d\n",scan);
	RC.stop();
	printf("total running time is\t%f\t(s)\n",RC.GetRuntime());
        //printf("total IO time is\t%f\t(s)\n",iotime);
	FILE *wfile1=fopen("time.txt","a+");
	if(wfile1==NULL){
		printf("could not write running time statistics to file\n");
		exit(0);
	}
	fprintf(wfile1,"graph\t%s\ttimetotal\t%f\n",infilename,RC.GetRuntime());
	fclose(wfile1);
	while(curBlk2 >= 0){
		if(memBlkAr2[curBlk2]!=NULL){
			free(memBlkAr2[curBlk2]);
			curBlk2--;
		}else
			curBlk2--;
	}
	curBlk=0;
	while(curBlk <= endBlk){
		if(memBlkAr[curBlk]!=NULL){
			free(memBlkAr[curBlk]);
			curBlk++;
		}else
			curBlk++;
	}
	//#ifdef WIN32
//	// Use CRT Debug facility to dump memory leaks on app exit
//	SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );
//#endif //WIN32

//
	return 0;
}
