//GROUP #6
//EE658 Project Submission

/*=======================================================================
  A simple parser for "self" format

  The circuit format (called "self" format) is based on outputs of
  a ISCAS 85 format translator written by Dr. Sandeep Gupta.
  The format uses only integers to represent circuit information.
  The format is as follows:

1        2        3        4           5           6 ...
------   -------  -------  ---------   --------    --------
0 GATE   outline  0 IPT    #_of_fout   #_of_fin    inlines
                  1 BRCH
                  2 XOR(currently not implemented)
                  3 OR
                  4 NOR
                  5 NOT
                  6 NAND
                  7 AND

1 PI     outline  0        #_of_fout   0
2 FB     outline  1 BRCH   inline
3 PO     outline  2 - 7    0           #_of_fin    inlines




                                    Author: Chihang Chen,Anu Siriyal, Swetha Babu
                                    Date: 12/4/16

=======================================================================*/

/*=======================================================================
  - Write your program as a subroutine under main().
    The following is an example to add another command 'lev' under main()

enum e_com {READ, PC, HELP, QUIT, LEV};
#define NUMFUNCS 5
int cread(), pc(), quit(), lev();
struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread, EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   {"LEV", lev, CKTLD},
};

lev()
{
   ...
}
=======================================================================*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#define MAXLINE 81               /* Input buffer size */
#define MAXNAME 31               /* File name size */
#define Upcase(x) ((isalpha(x) && islower(x))? toupper(x) : (x))
#define Lowcase(x) ((isalpha(x) && isupper(x))? tolower(x) : (x))

enum e_com {READ, PC, HELP, QUIT, LEV, LS, FL,PFS,DFS,IMPLYHK};
enum e_state {EXEC, CKTLD};         /* Gstate values */
enum e_ntype {GATE, PI, FB, PO};    /* column 1 of circuit format */
enum e_gtype {IPT, BRCH, XOR, OR, NOR, NOT, NAND, AND};  /* gate types */

struct cmdstruc {
   char name[MAXNAME];        /* command syntax */
   int (*fptr)();             /* function pointer of the commands */
   enum e_state state;        /* execution state sequence */
};
struct CFL //node
{
    int NodeNumber; //data;
	bool FaultValue;
	int ANDMASK;
	int ORMASK;
    struct CFL *nextFault; //node *next;
}*headFault; //*head;
struct FaultList //node
{
    int NoNu; //data;
	bool FaVa;
    struct FaultList *nF; //node *next;
}*hF; //*head;
struct NodeList //node
{
    int NodNum; //data;
	bool FauVal;
    struct NodeList *neFa; //node *next;
	
}*heFa;
typedef struct n_struc {
   unsigned indx;             /* node index(from 0 to NumOfLine - 1 */
   unsigned num;              /* line number(May be different from indx */
   enum e_gtype type;         /* gate type */
   unsigned fin;              /* number of fanins */
   unsigned fout;             /* number of fanouts */
   struct n_struc **unodes;   /* pointer to array of up nodes */
   struct n_struc **dnodes;   /* pointer to array of down nodes */
   int level;                 /* level of the gate output */
   int value;
   bool SA0;
   bool SA1;
   int AM;
   int OM;
   int outputs;
   struct FaultList *gau; 
int noofele;
int nodesofele[1000]; 
int nodesofeleval[1000];   
   } NSTRUC;

/*----------------- Command definitions ----------------------------------*/
#define NUMFUNCS 10
int cread(), pc(), help(), quit(), lev(), logsim(), powe(int,int);
int notgate(int), evaluate(int,int,int), branch1(int),preprocessor(),ParallelFaultSimulation(),DeductiveFaultSimulation();
void append(int,bool,bool,bool), add(int,bool,bool,bool), addafter(int,bool,bool,bool,int),insert(int,bool,bool,bool);
int delete(int,bool);
void display(struct CFL *r);
int count();
//void appendFL(int,bool), addFL(int,bool), addafterFL(int,bool,int),insertFL(int,bool);
//int deleteFL(int,bool);
void displayFL(struct FaultList *r);
int countFL();
void appendNL(int,bool), addNL(int,bool), addafterNL(int,bool,int),insertNL(int,bool);
int deleteNL(int,bool);
void displayNL(struct NodeList *r);
int countNL();
int complement(int);
NSTRUC* J_frontier(NSTRUC*,int, int);
NSTRUC* D_frontier(NSTRUC*,int, int)
void Imply_and_check(NSTRUC*,bool);
void imply();
//void evaluateFault();
struct cmdstruc command[NUMFUNCS] = {
   {"READ", cread, EXEC},
   {"PC", pc, CKTLD},
   {"HELP", help, EXEC},
   {"QUIT", quit, EXEC},
   {"LEV", lev, CKTLD},
   {"LS", logsim, CKTLD},
   {"FL", preprocessor, CKTLD},
   {"PFS", ParallelFaultSimulation, CKTLD},
   {"DFS", DeductiveFaultSimulation, CKTLD},
   {"Implyhk",imply, CKTLD},
};

/*------------------------------------------------------------------------*/
enum e_state Gstate = EXEC;     /* global exectution sequence */
NSTRUC *Node;                   /* dynamic array of nodes */
NSTRUC **Pinput;                /* pointer to array of primary inputs */
NSTRUC **Poutput;               /* pointer to array of primary outputs */
int Nnodes;                     /* number of nodes */
int Npi;                        /* number of primary inputs */
int Npo;                        /* number of primary outputs */
int Done = 0;                   /* status bit to terminate program */
int MaxLevel;
int *Fault_List;
int *Good_Circuit_Simulation;
int **Fault_Circuit_Simulation;
int NoOfFaultsToBeDetected;
int PlaceFaultAt;
int GoodCircuitANDMASK=0xFFFFFFFF;
int GoodCircuitORMASK=0x00000000;
int AANDMASKSA1;
int flag11;
int AANDMASKSA0;
int AORMASK;
int anu;
int onu;
int flaggy1;
int flaggy2;
int blabby;
int MaxList[2000],MaxListVal[2000];
int complimente[1000];
int complimenteval[1000];
/*------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: shell
description:
  This is the main program of the simulator. It displays the prompt, reads
  and parses the user command, and calls the corresponding routines.
  Commands not reconized by the parser are passed along to the shell.
  The command is executed according to some pre-determined sequence.
  For example, we have to read in the circuit description file before any
  action commands.  The code uses "Gstate" to check the execution
  sequence.
  Pointers to functions are used to make function calls which makes the
  code short and clean.
-----------------------------------------------------------------------*/
NSTRUC* D_frontier_array[100];
NSTRUC* J_frontier_array[100];
main()
{
	
	int hk;
	for(hk=0;hk<100;hk++)
	{
	D_frontier_array[hk]=NULL;	
	J_frontier_array[hk]=NULL;
	}
   enum e_com com;
   char cline[MAXLINE], wstr[MAXLINE], *cp;
	
  while(!Done) {
      printf("\nCommand>");
      fgets(cline, MAXLINE, stdin);
      if(sscanf(cline, "%s", wstr) != 1) continue;
      cp = wstr;
      while(*cp){
	*cp= Upcase(*cp);
	cp++;
      }
      cp = cline + strlen(wstr);
      com = READ;
      while(com < NUMFUNCS && strcmp(wstr, command[com].name)) com++;
      if(com < NUMFUNCS) {
         if(command[com].state <= Gstate) (*command[com].fptr)(cp);
         else printf("Execution out of sequence!\n");
      }
      else system(cline);
   }
}

/*-----------------------------------------------------------------------
input: circuit description file name
output: nothing
called by: main
description:
  This routine reads in the circuit description file and set up all the
  required data structure. It first checks if the file exists, then it
  sets up a mapping table, determines the number of nodes, PI's and PO's,
  allocates dynamic data arrays, and fills in the structural information
  of the circuit. In the ISCAS circuit description format, only upstream
  nodes are specified. Downstream nodes are implied. However, to facilitate
  forward implication, they are also built up in the data structure.
  To have the maximal flexibility, three passes through the circuit file
  are required: the first pass to determine the size of the mapping table
  , the second to fill in the mapping table, and the third to actually
  set up the circuit information. These procedures may be simplified in
  the future.
-----------------------------------------------------------------------*/
cread(cp)
char *cp;
{
   char buf[MAXLINE];
   int ntbl, *tbl, i, j, k, nd, tp, fo, fi, ni = 0, no = 0;
   FILE *fd;
   NSTRUC *np;

   sscanf(cp, "%s", buf);
   if((fd = fopen(buf,"r")) == NULL) {
      printf("File %s does not exist!\n", buf);
      return;
   }
   if(Gstate >= CKTLD)  clear();
   Nnodes = Npi = Npo = ntbl = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) {
         if(ntbl < nd) ntbl = nd;
         Nnodes ++;
         if(tp == PI) Npi++;
         else if(tp == PO) Npo++;
      }
   }
   tbl = (int *) malloc(++ntbl * sizeof(int));

   fseek(fd, 0L, 0);
   i = 0;
   while(fgets(buf, MAXLINE, fd) != NULL) {
      if(sscanf(buf,"%d %d", &tp, &nd) == 2) tbl[nd] = i++;
   }
   allocate();

   fseek(fd, 0L, 0);
   while(fscanf(fd, "%d %d", &tp, &nd) != EOF) {
      np = &Node[tbl[nd]];
      np->num = nd;
      if(tp == PI) Pinput[ni++] = np;
      else if(tp == PO) Poutput[no++] = np;
      switch(tp) {
         case PI:
         case PO:
         case GATE:
            fscanf(fd, "%d %d %d", &np->type, &np->fout, &np->fin);
            break;

         case FB:
            np->fout = np->fin = 1;
            fscanf(fd, "%d", &np->type);
            break;

         default:
            printf("Unknown node type!\n");
            exit(-1);
         }
      np->unodes = (NSTRUC **) malloc(np->fin * sizeof(NSTRUC *));
      np->dnodes = (NSTRUC **) malloc(np->fout * sizeof(NSTRUC *));
      for(i = 0; i < np->fin; i++) {
         fscanf(fd, "%d", &nd);
         np->unodes[i] = &Node[tbl[nd]];
         }
      for(i = 0; i < np->fout; np->dnodes[i++] = NULL);
      }
   for(i = 0; i < Nnodes; i++) {
      for(j = 0; j < Node[i].fin; j++) {
         np = Node[i].unodes[j];
         k = 0;
         while(np->dnodes[k] != NULL) k++;
         np->dnodes[k] = &Node[i];
         }
      }
   fclose(fd);
   Gstate = CKTLD;
   printf("==> OK\n");
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  The routine prints out the circuit description from previous READ command.
-----------------------------------------------------------------------*/
pc(cp)
char *cp;
{
   int i, j;
   NSTRUC *np;
   char *gname();

   printf(" Node   Type \tIn     \t\t\tOut\t\tLevel\n");
   printf("------ ------\t-------\t\t\t-------\t\t--------\n");
   for(i = 0; i<Nnodes; i++) {
      np = &Node[i];
      printf("\t\t\t\t\t");

      for(j = 0; j<np->fout; j++) printf("%d ",np->dnodes[j]->num);//Fanouts
	  printf("\t");
	  printf("   %d",np->level);
	  printf("\r%5d  %s\t", np->num, gname(np->type));
	  for(j = 0; j<np->fin; j++) printf("%d ",np->unodes[j]->num);//Fanins
	  printf("\n");
   }
   printf("Primary inputs:  ");
   for(i = 0; i<Npi; i++) printf("%d ",Pinput[i]->num);
   printf("\n");
   printf("Primary outputs: ");
   for(i = 0; i<Npo; i++) printf("%d ",Poutput[i]->num);
   printf("\n\n");
   printf("Number of nodes = %d\n", Nnodes);
   printf("Number of primary inputs = %d\n", Npi);
   printf("Number of primary outputs = %d\n", Npo);
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  The routine prints out help information for each command.
-----------------------------------------------------------------------*/
help()
{
   printf("READ filename - ");
   printf("read in circuit file and creat all data structures\n");
   printf("PC - ");
   printf("print circuit information\n");
   printf("HELP - ");
   printf("print this help information\n");
   printf("QUIT - ");
   printf("stop and exit\n");
   printf("PFS - ");
   printf("performs Parallel Fault Simulation on the circuit\n");
   printf("DFS - ");
   printf("performs Deductive Fault Simulation on the circuit\n");
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: main
description:
  Set Done to 1 which will terminates the program.
-----------------------------------------------------------------------*/
quit()
{
   Done = 1;
}

/*======================================================================*/

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine clears the memory space occupied by the previous circuit
  before reading in new one. It frees up the dynamic arrays Node.unodes,
  Node.dnodes, Node.flist, Node, Pinput, Poutput, and Tap.
-----------------------------------------------------------------------*/
clear()
{
   int i;

   for(i = 0; i<Nnodes; i++) {
      free(Node[i].unodes);
      free(Node[i].dnodes);
   }
   free(Node);
   free(Pinput);
   free(Poutput);
   Gstate = EXEC;
}

/*-----------------------------------------------------------------------
input: nothing
output: nothing
called by: cread
description:
  This routine allocatess the memory space required by the circuit
  description data structure. It allocates the dynamic arrays Node,
  Node.flist, Node, Pinput, Poutput, and Tap. It also set the default
  tap selection and the fanin and fanout to 0.
-----------------------------------------------------------------------*/
allocate()
{
   int i;

   Node = (NSTRUC *) malloc(Nnodes * sizeof(NSTRUC));
   Pinput = (NSTRUC **) malloc(Npi * sizeof(NSTRUC *));
   Poutput = (NSTRUC **) malloc(Npo * sizeof(NSTRUC *));
   for(i = 0; i<Nnodes; i++) {
      Node[i].indx = i;
      Node[i].fin = Node[i].fout = 0;
   }
}

/*-----------------------------------------------------------------------
input: gate type
output: string of the gate type
called by: pc
description:
  The routine receive an integer gate type and return the gate type in
  character string.
-----------------------------------------------------------------------*/
char *gname(tp)
int tp;
{
   switch(tp) {
      case 0: return("PI");
      case 1: return("BRANCH");
      case 2: return("XOR");
      case 3: return("OR");
      case 4: return("NOR");
      case 5: return("NOT");
      case 6: return("NAND");
      case 7: return("AND");
   }
}

lev()
{
	int i, j, total, max;
	NSTRUC *np;
	char *gname();
	for(i = 0; i<Nnodes; i++)
	{
		np = &Node[i];
		np->level=-1;
	//	printf("i %d level %d\n",i,np->level);
	}
	for(i = 0; i<Npi; i++)
	{
		Pinput[i]->level=0;
	}
	total=Nnodes;i=0;
	while(total!=0)
	{
		np = &Node[i];
		{
			if((np->fin)>0)
			{
				max=-1;
				for(j = 0; j<np->fin; j++)
					{
						if((np->unodes[j]->level)>max)
							{
								max=np->unodes[j]->level;
							}
					}
				np->level=max+1;
			}
			if((np->fout)>0)
			{
				for(j = 0; j<np->fout; j++)
					{
						np->dnodes[j]->level=np->level+1;
						if(MaxLevel<np->level+1)
							MaxLevel=np->level+1;
					}
			}
		}
		total=total-1;
		i=i+1;
	}
}
int *T;
logsim()
{
	int i,j,k,z,count,x;

	NSTRUC *np;
	Good_Circuit_Simulation=(int *)malloc(Npo*sizeof(int));
	T=(int *)malloc(Npi*sizeof(int));
		for(j=0;j<Nnodes;j++)
		{
			np=&Node[j];
			np->value=-1;
		}
			printf("\nEnter the test pattern:\n");
		for(i=0;i<Npi;i++)
			scanf("%d",&T[i]);
		
		for(k=0;k<Npi;k++)
			{
				Pinput[k]->value=T[k];
				printf("value = %d\n",Pinput[k]->value);
			}
		x=0;
		
		while(x<=MaxLevel)
		{
			for(j=0;j<Nnodes;j++)
			{
				np=&Node[j];
				if((np->level)==x)
				{
					switch(np->type)
					{
						case 0: //printf("PI\n");
								/*for(z = 0; z<np->fout; z++)
									np->dnodes[z]->value = np->value;*/
								break;
						case 1: //printf("BRANCH\n");
								branch1(j);
								break;
						case 2: //printf("XOR\n");
								break;
						case 3: //printf("OR\n");
								evaluate(j,1,0);
								break;
						case 4: //printf("NOR\n");
								evaluate(j,1,1);
								break;
						case 5: //printf("NOT\n");
								notgate(j);
								break;
						case 6: //printf("NAND\n");
								evaluate(j,0,1);
								break;
						case 7: //printf("AND\n");
								evaluate(j,0,0);
								break;
					}
				}
			}

			x=x+1;
		}
		for(j=0;j<Npo;j++)
		{	
			printf("Primary outputs= %d \n",Poutput[j]->value);
			Good_Circuit_Simulation[j]=Poutput[j]->value;
		}
		printf("\n");
	
}

int powe(int base, int exp1)
{
    int result = 1;
    while(exp1)
    {
        result = result * base;
        exp1--;
    }
    return result;
}

branch1(int j)
{
	int b, i;
	NSTRUC *np;
	np = &Node[j];
	
	for(i = 0; i<np->fin;i++)
		np->value=np->unodes[i]->value;
		//printf("%d\n",np->value);
}

evaluate(int input,int c,int i)
{
	int a, j;
	bool r;
	int flag = 0;
	NSTRUC *np;
	NSTRUC *np1;
	np = &Node[input];
	for(j = 0; j<np->fin; j++)
	{
		a = np->unodes[j]->value;
		if(flag==0)
		{
		if(a==c)
		{
			r=((bool)c^(bool)i);
			flag = 1;
		}
		else
		{	r=((!(bool)c)^(bool)i);}
		}
	}
	np->value =(int)r;
	//printf("%d\n",np->value);
}

notgate(int i)
{
	int a, r,j;
	NSTRUC *np;
	np = &Node[i];
	for(j = 0; j<np->fin; j++)
	{
		a = np->unodes[j]->value;
		if(a==0)
			r = 1;
		else
			r = 0;
	}
	for(j = 0; j<np->fout; j++)
		np->dnodes[j]->value = r;
}
preprocessor()
{
	NSTRUC *np;
	int num;
    struct CFL *n;
    headFault=NULL;
	int i,val,j,k,flag;
	Fault_List=(int *)malloc(2*Nnodes*sizeof(int));
	printf("Fault list before fault collapsing:\n");
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];
		Fault_List[i]=np->num;
		printf("Node Number %d SA%d\n",Fault_List[i],0);		
	}
	for(i=0;i<Nnodes;i++)
	{
		//Nnodes+i is SA1 just i is SA0
		np=&Node[i];
		Fault_List[Nnodes+i]=np->num;
		printf("Node Number %d SA%d\n",Fault_List[Nnodes+i],1);
	}
	printf("\n");
	//printf("\n");
	//Checkpoint
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];
		if((np->type != 0)&&(np->type != 1))
		{
			Fault_List[i]=-1;
			Fault_List[Nnodes+i]=-1;
			
		}
	}
	/*printf("Fault list after checkpint:\n\n");
	for(i=0;i<2*Nnodes;i++)
	{	
		printf("%d\t",Fault_List[i]);		
	}
	printf("\n");*/
	//Equivalence and dominance
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];flag=0;
		
		if(np->type == 3)
		{
			//OR gate
			for(j = 0; j<np->fin; j++)
			{
				val=np->unodes[j]->num;
				for(k=0;k<Nnodes;k++)
				{
					if((val==Fault_List[k])&&(flag==0)&&(Fault_List[k]!=-1))
					{
						flag=1;
					}
					else if((val==Fault_List[k])&&(Fault_List[k]!=-1)&&(flag==1))	
					{
						Fault_List[Nnodes+k]=-1;
					}
				}
			}
			
		}
		if(np->type == 4)
		{
			//NOR gate
			for(j = 0; j<np->fin; j++)
			{
				val=np->unodes[j]->num;
				for(k=0;k<Nnodes;k++)
				{
					if((val==Fault_List[k])&&(flag==0)&&(Fault_List[k]!=-1))
					{
						flag=1;
					}
					else if((val==Fault_List[k])&&(Fault_List[k]!=-1)&&(flag==1))	
					{
						Fault_List[Nnodes+k]=-1;
					}
				}
			}
			
		}
		if(np->type == 6)
		{
			//NAND gate
			for(j = 0; j<np->fin; j++)
			{
				val=np->unodes[j]->num;
				for(k=0;k<Nnodes;k++)
				{
					if((val==Fault_List[k])&&(flag==0)&&(Fault_List[k]!=-1))
					{
						flag=1;
					}
					else if((val==Fault_List[k])&&(Fault_List[k]!=-1)&&(flag==1))	
					{
						Fault_List[k]=-1;
					}
				}
			}
			
		}
		if(np->type == 7)
		{
			//AND gate
			for(j = 0; j<np->fin; j++)
			{
				val=np->unodes[j]->num;
				for(k=0;k<Nnodes;k++)
				{
					if((val==Fault_List[k])&&(flag==0)&&(Fault_List[k]!=-1))
					{
						flag=1;
					}
					else if((val==Fault_List[k])&&(Fault_List[k]!=-1)&&(flag==1))	
					{
						Fault_List[k]=-1;
					}
				}
			}
			
		}
		
				
	}
	PlaceFaultAt=0;
	flag11=0;
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];
		np->AM=0xFFFFFFFF;
		np->OM=0x00000000;
		
	}
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];
		flaggy1=0;
		flaggy2=0;
		blabby=0;
		if(Fault_List[i]==-1)

			//PlaceFaultAt=PlaceFaultAt+1;
			np->SA0=false;
		else
		{
			np->SA0=true;
			//PlaceFaultAt=PlaceFaultAt+1;
			//printf("Node Number %d SA%d\t",np->num,0);
			flag11=1;
			insert(np->num,false,0,0);
			flaggy1=1;
		}
		
		if(Fault_List[Nnodes+i]==-1)
			np->SA1=false;
		else
		{
			np->SA1=true;
			if(flaggy1==0)
			flag11=1;//PlaceFaultAt=PlaceFaultAt+1;
			insert(np->num,true,1,1);
			//printf("Node Number %d SA%d\t",np->num,1);
			flaggy2=1;
			blabby=1;
		}
		if((flaggy1==1)&&(flaggy2==1))
		{
			np->AM=~(AANDMASKSA0^AANDMASKSA1);
		//	printf("Swetha");
		}
		else if((flaggy1==1)&&(flaggy2==0))
		{
			np->AM=AANDMASKSA0;
			//printf("Shashank");
		}
		else if((flaggy2==1)&&(flaggy1==0))
		{
			np->AM=AANDMASKSA1;
			//printf("Anu");
		}
		else
			np->AM=0xFFFFFFFF;
		if(blabby==1)
			np->OM=AORMASK;
		else
			np->OM=0x00000000;
		
	}
	printf("Fault list after fault collapsing:\n\n");
	
	for(i=0;i<Nnodes;i++)
	{
		np=&Node[i];
		anu=np->AM;
		onu=np->OM;
	
		//for(j = 31; j >= 0; j--) putchar('0' + ((anu >> j) & 1));
		//printf("\t%d %d\t",np->num,np->outputs);
		//for(j = 31; j >= 0; j--) putchar('0' + ((onu >> j) & 1));
		//printf("\n");
	}
	
	display(n);
	NoOfFaultsToBeDetected=count();
	//printf("Going off\n");
	
}
/*******************************************************************************/

void append(int num,bool FV, bool ANDinp, bool ORinp)
{
    struct CFL *temp,*right;
	int Val,ValOR,ValAND;
    temp= (struct CFL *)malloc(sizeof(struct CFL));
    temp->NodeNumber=num;
	temp->FaultValue=(bool)FV;
	PlaceFaultAt=PlaceFaultAt+1;
	//printf("%d\n",PlaceFaultAt);
	Val=1<<PlaceFaultAt;
	ValAND=Val^0xFFFFFFFF;
	if(FV==1)
	{
		ValOR=Val^0x00000000;
		
	}
	else
		ValOR=0x00000000;
	temp->ANDMASK=ValAND;
	if(flag11==0)
	{
		AANDMASKSA0=ValAND;
		flag11=1;
	}
	else
	{
		AANDMASKSA1=ValAND;
		flag11=0;
	}
	
	AORMASK=ValOR;
	temp->ORMASK=ValOR;
    right=(struct CFL *)headFault;
    while(right->nextFault != NULL)
    right=right->nextFault;
    right->nextFault =temp;
    right=temp;
    right->nextFault=NULL;
}
void add( int num, bool FV, bool ANDinp, bool ORinp)
{
    struct CFL *temp;
	int Val,ValOR,ValAND;
    temp=(struct CFL *)malloc(sizeof(struct CFL));
    temp->NodeNumber=num;
	temp->FaultValue=(bool)FV;
	PlaceFaultAt=PlaceFaultAt+1;
	//printf("%d\n",PlaceFaultAt);
	Val=1<<PlaceFaultAt;
	ValAND=Val^0xFFFFFFFF;
	//flaggy=ValAND;
	if(FV==1)
		ValOR=Val^0x00000000;
	else
		ValOR=0x00000000;
	temp->ANDMASK=ValAND;
	if(flag11==0)
	{
		AANDMASKSA0=ValAND;
		flag11=1;
	}
	else
	{
		AANDMASKSA1=ValAND;
		flag11=0;
	}
	AORMASK=ValOR;
	temp->ORMASK=ValOR;
    if (headFault== NULL)
    {
    headFault=temp;
    headFault->nextFault=NULL;
    }
    else
    {
    temp->nextFault=headFault;
    headFault=temp;
    }
}
void addafter(int num, bool FV, bool ANDinp, bool ORinp, int loc)
{
    int i;
	int Val,ValOR,ValAND;
    struct CFL *temp,*left,*right;
    right=headFault;
    for(i=1;i<loc;i++)
    {
    left=right;
    right=right->nextFault;
    }
    temp=(struct CFL *)malloc(sizeof(struct CFL));
    temp->NodeNumber=num;
	temp->FaultValue=(bool)FV;
	PlaceFaultAt=PlaceFaultAt+1;
//	printf("%d\n",PlaceFaultAt);
	Val=1<<PlaceFaultAt;
	ValAND=Val^0xFFFFFFFF;
	//flaggy=ValAND;
	if(FV==1)
	{
		ValOR=Val^0x00000000;
	}
	else
		ValOR=0x00000000;
	temp->ANDMASK=ValAND;
	temp->ORMASK=ValOR;
	if(flag11==0)
	{
		AANDMASKSA0=ValAND;
		flag11=1;
	}
	else
	{
		AANDMASKSA1=ValAND;
		flag11=0;
	}
	AORMASK=ValOR;
    left->nextFault=temp;
    left=temp;
    left->nextFault=right;
    return;
}
 
 
 
void insert(int num,bool FV, bool ANDinp, bool ORinp)
{
    int c=0;
    struct CFL *temp;
    temp=headFault;
    if(temp==NULL)
    {
    add(num,FV,ANDinp,ORinp);
    }
    else
    {
    while(temp!=NULL)
    {
        if(temp->NodeNumber<num)
        c++;
        temp=temp->nextFault;
    }
    if(c==0)
        add(num,FV,ANDinp,ORinp);
    else if(c<count())
        addafter(num,FV,ANDinp,ORinp,++c);
    else
        append(num,FV,ANDinp,ORinp);
    }
}
int delete(int num,bool FaultValues)
{
    struct CFL *temp, *prev;
    temp=headFault;
    while(temp!=NULL)
    {
    if((temp->NodeNumber==num)&&(temp->FaultValue==FaultValues))
    {
        if(temp==headFault)
        {
        headFault=temp->nextFault;
        free(temp);
        return 1;
        }
        else
        {
        prev->nextFault=temp->nextFault;
        free(temp);
        return 1;
        }
    }
    else
    {
        prev=temp;
        temp= temp->nextFault;
    }
    }
    return 0;
}
 
 
void  display(struct CFL *r)
{
    r=headFault;int x1=0;
	int ANDs,ORs,i;
    if(r==NULL)
    {
		return;
    }
	printf("\n");
    while(r!=NULL)
    {
		x1=x1+1;
		ANDs=r->ANDMASK;
		ORs=r->ORMASK;
		printf("Node Number %d SA%d\n",r->NodeNumber,(int)r->FaultValue);
		// for(i = 31; i >= 0; i--) putchar('0' + ((ANDs >> i) & 1));
		 //printf("\t");
		  //for(i = 31; i >= 0; i--) putchar('0' + ((ORs >> i) & 1));
		//printf("\n");
		r=r->nextFault;
	}
	printf("\nNo of faults in the collapsed fault list is %d ",x1);
    printf("\n");
	return;
}
 

 
int count()
{
    struct CFL *n;
    int c=0;
    n=headFault;
    while(n!=NULL)
    {
    n=n->nextFault;
    c++;
    }
    return c;
}


imply()
{
	NSTRUC * Np;
	int i;
	printf("Enter gate\n");
	scanf("%d",&i);
	Np=&Node[i];
	Imply_and_check(Np,0);
}
Imply_and_check(NSTRUC* np,bool D_propogate)
{
	bool sa0,sa1,c,NOT_GATE,fault;
	NSTRUC* fp;
	NSTRUC* imply_check_array[101];
	int no_of_branch_outs,k,m,j,line_value,gate_type,no_of_inputs;
	int index=0;
	int count=0;
	//propogate the D to PO///////////////////////////////////////////////////////////////////////////////////////

	while(D_propogate==0)
	{
		if(np->level==MaxLevel)
		{
			printf("PO");
			D_propogate=1;			
		}
		else
		{
			no_of_branch_outs=np->fout;
			np->dnodes[0]->val=2;//assign value D to the first branch out node
			for(k=1;k<no_of_branch_outs;k++)
			{
				D_frontier(np->dnodes[k],1,0);// Push the other branch outs to D frontier
			}
			fp=np->dnodes[0]->dnodes[0];// Go to the gate output node of the selected node
			
			switch(fp->type)
			{
	
				case 3: //printf("OR\n");
						c=1;
						break;
				case 4: //printf("NOR\n");
						c=1;
						break;
				case 5: //printf("NOT\n");
						c=0;
						NOT_GATE=1;
						break;
				case 6: //printf("NAND\n");
						c=0;
						break;
				case 7: //printf("AND\n");
						c=0;
						break;
			}
			if(fp->val!=4)
			{
			D_propogate=1;	
			}
			else
			{
				if(fp->level==MaxLevel)
				{
					D_propogate=1;
				}
				
				if(NOT_GATE==1)
				{
					fp->val=3;
	
				}
				else
				{
					no_of_ins=fp->fin;
					for(m=0;m<no_of_ins-1;m++)
					{
						if(fp->unodes[m]->val==c)
						{
							printf("Fall back to other gates in D Frontier");
							D_propogate=1;		
						}
						else
						{
							switch(fp->unodes[m]->val)
							{
								case 2: //printf("value is already D\n");
										//value is already D
										break;
								case 3: //printf("value is already Dbar\n");
										//value is already Dbar
										break;
								case 4: //printf("NAND\n");
										//value is already Dbar
										fp->unodes[m]->val=!c;
										imply_check_array[index]=fp->unodes[m];
										index=index+1;
										break;
							}
						}
					}
				}
			}	
		}
		np=fp;
	}
	
	
	
	
}

NSTRUC* D_frontier(NSTRUC* Pointer,int PUSH, int POP)
{
	int k;
	
	NSTRUC* pgate,rgate;
	if(PUSH==1)
	{
		for(k=0;k<100;k++)
		{
			if(D_frontier_array[k]==NULL)
			{
				D_frontier_array[k]=Pointer;
				return NULL;
			}
		}	
	}
	else if(POP==1)
	{
		for(k=0;k<100;k++)
		{
			pgate=D_frontier_array[k];
			if(pgate!=NULL)
			{
				rgate=pgate;
				pgate=NULL;
				return rgate;
			}
		}

	}
	else 
	{
		return NULL;
	}
}


NSTRUC* J_frontier(NSTRUC* Pointer,int PUSH, int POP)
{
	int k;
	
	NSTRUC* pgate,rgate;
	if(PUSH==1)
	{
		for(k=0;k<100;k++)
		{
			if(J_frontier_array[k]==NULL)
			{
				J_frontier_array[k]=Pointer;
				return NULL;
			}
		}	
	}
	else if(POP==1)
	{
		for(k=0;k<100;k++)
		{
			pgate=J_frontier_array[k];
			if(pgate!=NULL)
			{
				rgate=pgate;
				pgate=NULL;
				return rgate;
			}
		}

	}
	else 
	{
		return NULL;
	}	
}
