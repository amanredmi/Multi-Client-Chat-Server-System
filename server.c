#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>	/* Include this to use semaphores */
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

#define P(s) semop(s, &pop, 1)  /* pop is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &vop, 1)  /* vop is the structure we pass for doing the V(s) operation */
int clientids[10],count=0,sockids[10],group_count=0;
struct group *groups;
/*Define all the desired data structures as asked in the question*/

/*Function to handle ^C*/
struct group
{
	int count,admin[10],id,state[10],broadcast;
};
int helper(char * buffer)
{
	int pid=fork();
	if(pid==0)
	{
		execlp("python","python","tester.py",buffer,NULL);
	}
	else
	{
		wait(NULL);
		FILE *fp=fopen("temp.txt","r");
		int i;
		fscanf(fp,"%d",&i);
		fclose(fp);
		return i;
	}
}
int removegroup(int k)
{
	char buffer[256];
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"Group having id %d is deleted",groups[k].id);
	for(int i=0;i<(count-1);i++)
	{
		if(groups[k].state[i]==2)
		{
			send(sockids[i],buffer,strlen(buffer),0);

		}
	}
	if(group_count>1)
	{
		int i;
		for(i=0;i<count;i++)
		{
			groups[k].admin[i]=groups[group_count-1].admin[i];
			groups[k].state[i]=groups[group_count-1].state[i];
			groups[k].broadcast=groups[group_count-1].broadcast;
		}

	}
	group_count--;
//	printf("%d: groups remained\n",group_count);
	return group_count;
}
int getSocketid(int dest)
{
	int result=0,i;
	for(i=0;i<count;i++)
	{
		if(clientids[i]==dest)
			return(sockids[i]);

	}
	return -1;
}
int generate()
{
	int result,i,flag=0,lower=10000,higher=99999;
	while(1)
	{
		result= (rand() %
        (higher- lower + 1)) + lower;
		for(i=0;i<count;i++)
		{
			if(clientids[i]==result)
				break;
		}
		if(i==count)
			return result;

	}
}
void sigCHandler() 
{ 
	//WRITE YOUR CODE HERE
} 

/*Function to handle ^Z*/
void sigZhandler() 
{ 
	//WRITE YOUR CODE HERE
}

/*Function to handle errors*/
void error(char *msg)
{
	//WRITE YOUR CODE HERE
}

/*Some utility functions to take care of client-server connections*/

/*function to initialize Shared Memory Segment for storing list of active clients*/
void initializeRec(){
	//WRITE YOUR CODE HERE
}

/*function to get the first empt index from Shared Memory Segment for storing list of active clients*/
int getEmptyClient(){
	//WRITE YOUR CODE HERE
}

/*function to add a new client entry to Shared Memory Segment for storing list of active clients*/
void addRecClient(){
	//WRITE YOUR CODE HERE
	int num=generate();
	clientids[count]=num;
	count++;
}
int printgroup(int i)
{
	for(int k=0;k<group_count;k++)
	{
	printf("\nGroup %d \nstates= : ",k+1);
	for(int j=0;j<count;j++)
		printf("%d  ",groups[k].state[j]);
	printf("\n admin status : ");
	for(int j=0;j<count;j++)
		printf("%d  ",groups[k].admin[j]);
	printf("\n");
	}
	return 0;
}

/*function to remove a disconnected client entry from Shared Memory Segment for storing list of active clients*/
void removeRecClient(int i,fd_set *master){
	//WRITE YOUR CODE HERE
	int j;
	for(j=0;j<count;j++)
	{
		if(sockids[j]==i)
			break;
	}
	//printf("index is %d\n",j);
	int k;
	
	if(count>1)
	{
		sockids[j]=sockids[count-1];	
		clientids[j]=clientids[count-1];
		
		for(k=0;k<group_count;k++)
		{
			groups[k].state[j]=groups[k].state[count-1];
			groups[k].admin[j]=groups[k].admin[count-1];
			
			int l,c=0;
			for(l=0;l<(count-1);l++)
			{
				if(groups[k].admin[l]==1)
					break;
			}

			//printf("\n%d,%d\n",l,count-1);
			//printgroup(0);
			if(l==(count-1))
				removegroup(k);
			//printgroup(0);
			
		}
	}
	
	
	count--;
	close(i);
	FD_CLR(i, master); 
}

/*function to print the contents of Shared Memory Segment for storing list of active clients*/
void printRecClient(char *buf){
	//WRITE YOUR CODE HERE
	int i;
	char id[7];
	memset(buf,'\0',256);
	if(count==1)
	{
		strcpy(buf,"No active clients except you");

	}
	else
	{
		sprintf(id,"%d",clientids[0]);
		
		//printf("%s\n",id);
		strcpy(buf,id);
		//printf("%d\n",count);
		for(i=1;i<count;i++)
		{
			strcat(buf," ");
			memset(id,'\0',sizeof(id));

			sprintf(id,"%d",clientids[i]);
			//printf("%s\n",id);
			strcat(buf," ");
			strcat(buf,id);

		}
		

	}

}

/*function to get sockfd mapped to the client_id in Shared Memory Segment for storing list of active clients*/
int getClientsd(int newsockfd){
	//WRITE YOUR CODE HERE	
	sockids[count]=newsockfd;
	return 1;
}

/*function to get unique client id mapped to sockfd from the Shared Memory Segment for storing list of active clients*/
int getClientId(int i){
	//WRITE YOUR CODE HERE
	int result=0,j;
	for(j=0;j<count;j++)
	{
		if(sockids[j]==i)
			return(clientids[j]);
	}
	return -1;
}


/*FUNCTIONS BELOW HANLES ALL SORT OF MESSAGE QUERIES FROM CLIENT*/

/* function to initializeShared Memory Segment for storing list of messages from clients*/
void initializeMsgEntry(){
	//WRITE YOUR CODE HERE
}

 /* function to get the first empty location form  Shared Memory Segment for storing list of messages from clients*/
int getEmptyMsgEntry(){
	//WRITE YOUR CODE HERE	
}

/* function to add a new msg to the Shared Memory Segment for storing list of messages from clients*/
void addMsgEntry(){
	//WRITE YOUR CODE HERE
}

/* function to remove an entry from Shared Memory Segment for storing list of messages from clients*/
void removeMsgEntry(){
	//WRITE YOUR CODE HERE
}

/* function to processs messages from clients*/
void sendMsg(int id,char *buf,fd_set master){
	//WRITE YOUR CODE HERE
//	printf("insided send\n");
	

		send(id, buf, strlen(buf), 0);

		//printf("successfully sent\n");
		}
								
					


/*FUCTION TO HANDLE BROADCAST REQUEST*/
void broadcast(int fdmax,char*command,fd_set *master,int i, int sockfd){
	int j;//printf("line 204\n");printf("%s\n",command);
	int id=getClientId(i);//printf("205\n");
	char *buffer=(char*)malloc(256*sizeof(char)),*token;//printf("206\n");
	memset(buffer,'\0',sizeof(buffer));//printf("207\n");
	sprintf(buffer,"message from client %d : ",id);//printf("208\n");
	token=strtok(command," ");//printf("209\n");
	token=strtok(NULL," ");//printf("210\n");printf("%s\n",token);
	strcat(buffer,token);//printf("211\n");
	printf("%s ",token);//printf("212\n");
	token=strtok(NULL," ");//printf("line 213\n");
	while(token!=NULL)
	{
		//printf("%s ",token);
		strcat(buffer," ");
		strcat(buffer,token);
		token=strtok(NULL," ");
	}//printf("line 220\n");
	strcat(buffer,"\n");
	if(helper(buffer)==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"%s","Sending abusive message is not allowed");
		send(i,buffer,strlen(buffer),0);
		return;
	}
	for(j = 0; j <= fdmax; j++) {
		if (FD_ISSET(j, master)) {
			if (j != sockfd && j != i) {
				if (send(j, buffer, strlen(buffer), 0) == -1) {
					perror("send");
									}
								}
							}
						}
//printf("done\n");
	//WRITE YOUR CODE HERE
}
int joingroup(int gid,int i)
{
	int j;
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==gid)
			break;
	}
	
	char buffer[256];
	if(j==group_count)
	{
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,"no group with given id");
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	int k;
	for(k=0;k<count;k++)
	{
		//printf("line 254: %d")
		if(sockids[k]==i)
			break;
	}
	if(groups[j].state[k]==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"Request for joining group is not sent to you");
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	else if(groups[j].state[k]==1)
	{
		groups[j].state[k]=2;
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"welcome you are added to a new group with id %d !",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 2;

	}
	else if(groups[j].state[k]==2)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"you are already in the  group with id %d !",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 3;
	}
	else if(groups[j].state[k]==3)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"joining not possible group with id %d !",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 4;
	}

	return 1;

}

int makegroup(int*groupids,int it,int creator)
{

	groups[group_count].count=it+1;
	int i,j;
	for(i=0;i<10;i++)
	{
		groups[group_count].state[i]=0;
		groups[group_count].admin[i]=0;
	}
	groups[group_count].broadcast=0;
	for(i=0;i<=it;i++)
	{
		for(j=0;j<count;j++)
		{
			if(clientids[j]==groupids[i])
				break;
		}

		if(j==count)
		{
			char buffer[256];
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"No active client with id %d . Group formed with remaining valid clients\n",groupids[i]);
			send(creator,buffer,strlen(buffer),0);

		}
		else

			groups[group_count].state[j]=2;
		 
	}
	groups[group_count].admin[j]=1;
	groups[group_count].id=rand();
	char buffer[256];
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"welcome you are added to a new group with id %d !",groups[group_count].id);
	for(i=0;i<count;i++)
	{
		if(groups[group_count].state[i]==2)
			send(sockids[i],buffer,strlen(buffer),0);
	}
	group_count++;
	return groups[group_count-1].id;
	
}

int sendgroup(int g, char *buffer,int i)
{
	int j;
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==g)
			break;
	}
	if(j==group_count)
	{
		char m[256];
		memset(m,'\0',sizeof(m));
		sprintf(m,"no active group with given id");
		send(i,m,strlen(m),0);
		return 1;
	}
	int k;
	for(k=0;k<count;k++)
	{
		if(sockids[k]==i)
			break;
	}
	if(groups[j].state[k]!=2)
	{
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,"sending not possible as you are not present in the group");
		send(i,buffer,strlen(buffer),0);
		return 3;
	}
	if((groups[j].broadcast==1)&&(groups[j].admin[k]==0))
	{
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,"only admin can send message");
		send(i,buffer,strlen(buffer),0);
		return 1;

	
	}
	if(helper(buffer)==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"%s","Sending abusive message is not allowed");
		send(i,buffer,strlen(buffer),0);
		return 2;
	}
	for(k=0;k<count;k++)
	{
		if(groups[j].state[k]==2)
			send(sockids[k],buffer,strlen(buffer),0);
	}
	return 0;


}

int makeadmin(int g,int admin ,int newadmin)
{
	int i,j,k;
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==g)
			break;
	}
	char buffer[256];
	if(j==group_count)
	{
		strcpy(buffer,"invalid group id");
		send(admin,buffer,strlen(buffer),0);
		return 1;
	}
	else
	{
		
		for(i=0;i<count;i++)
		{
			if(sockids[i]==admin)
				break;
		}
		for(k=0;k<count;k++)
		{
			if(clientids[k]==newadmin)
				break;
		}
		if(k==count)
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"No client with id : %d",newadmin);
			send(admin,buffer,strlen(buffer),0);
			return 1;
		}
		else if(groups[j].state[k]!=2)
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"Client with id %d  not present in the group",newadmin);
			send(admin,buffer,strlen(buffer),0);
			return 1;
		}
		if((groups[j].state[i]==0)||(groups[j].state[i]==3))
		{
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,"You are not even present in the group");
			send(admin,buffer,strlen(buffer),0);
			return 1;
		}
		else if(groups[j].state[i]==1)
		{
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,"You have not accepted group joining request yet");
			send(admin,buffer,strlen(buffer),0);
			return 1;
	
		}
		else if(groups[j].admin[i]==0)
		{
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,"Operation not possibe since you are not admin");
			send(admin,buffer,strlen(buffer),0);
			return 1;
		}
		else
		{
			groups[j].admin[k]=1;
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"You are made admin of the group with id %d",groups[j].id);
			send(sockids[k],buffer,strlen(buffer),0);
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,"operation successfull");
			send(admin,buffer,strlen(buffer),0);
			return 1;
			
		}
		
	}
	return 4;

}
int send_request(int groupids[],int it)
{

	groups[group_count].count=it+1;
	int i,j;
	for(i=0;i<count;i++)
	{
		groups[group_count].state[i]=0;
		groups[group_count].admin[i]=0;
	}
	groups[group_count].broadcast=0;
	for(i=0;i<=it;i++)
	{
		for(j=0;j<count;j++)
		{
			if(clientids[j]==groupids[i])
				break;
		}

		groups[group_count].state[j]=1;
		 
	}
	groups[group_count].admin[j]=1;
	groups[group_count].state[j]=2;
	
	groups[group_count].id=rand();
	char buffer[256];
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"welcome you are added to a new group with id %d !",groups[group_count].id);
	for(i=0;i<count;i++)
	{
		if(groups[group_count].state[i]==2)
			send(sockids[i],buffer,strlen(buffer),0);
	}
	memset(buffer,'\0',sizeof(buffer));
	sprintf(buffer,"Request to join  a new group with id %d !",groups[group_count].id);
	for(i=0;i<count;i++)
	{
		if(groups[group_count].state[i]==1)
			send(sockids[i],buffer,strlen(buffer),0);
	}



	group_count++;
	return groups[group_count-1].id;
	
}
int declinegroup(int i,int gid)
{
	int j,k;
	char buffer[256];
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==gid)
			break;
	}
	if(j==group_count)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"No active group with given id %d ",gid);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	for(k=0;k<count;k++)
	{
		if(sockids[k]==i)
			break;
	}
	if(groups[j].state[k]==1)
	{
		groups[j].state[k]=3;
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not added in group with id %d ",gid);
		send(i,buffer,strlen(buffer),0);
		return 1;

	}
	else
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"Operation not possible since joining request is not sent to you");
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	return 2;

}
int addtogroup(int i,int g,int groupids[],int it)
{
	int j,k,l;
	char buffer[256];
	//checking if group id is valid or not
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==g)
			break;
	}
	if(j==group_count)
	{//group is not present
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"No active group with given id %d",g);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	//finding information of requesting client
	for(k=0;k<count;k++)
	{
		if(sockids[k]==i)
			break;
	}
	if((groups[j].state[k]==2)&&(groups[j].admin[k]==1))//if client is the admin
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"you are added to group with group id %d",groups[j].id);
		for(l=0;l<=it;l++)//for every clients to be joined
		{
			for(k=0;k<count;k++)//find information of client
			{
				if(clientids[k]==groupids[l])
					break;
			}
			groups[j].state[k]=2;
			send(sockids[k],buffer,strlen(buffer),0);
			

		}
		return 1;
	}
	if(groups[j].state[k]!=2)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not present in the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	if(groups[j].admin[k]==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not admin of the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;

	}
	return 1;



}
int removefromgroup(int i,int g,int groupids[],int it)
{
	int j,k,l;
	char buffer[256];
	//checking if group id is valid or not
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==g)
			break;
	}
	if(j==group_count)
	{//group is not present
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"No active group with given id %d",g);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	//finding information of requesting client
	for(k=0;k<count;k++)
	{
		if(sockids[k]==i)
			break;
	}
	if((groups[j].state[k]==2)&&(groups[j].admin[k]==1))//if client is the admin
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"you are removed from group with group id %d",groups[j].id);
		for(l=0;l<=it;l++)//for every clients to be removed
		{
			for(k=0;k<count;k++)//find information of client
			{
				if(clientids[k]==groupids[l])
					break;
			}
			groups[j].state[k]=3;
			send(sockids[k],buffer,strlen(buffer),0);
			

		}
		return 1;
	}
	if(groups[j].state[k]!=2)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not present in the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	if(groups[j].admin[k]==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not admin of the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;

	}
	return 1;



}

int makegroupbroadcast(int g,int i)
{
	int j,k,l;
	char buffer[256];
	//checking if group id is valid or not
	for(j=0;j<group_count;j++)
	{
		if(groups[j].id==g)
			break;
	}
	if(j==group_count)
	{//group is not present
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"No active group with given id %d",g);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	//finding information of requesting client
	for(k=0;k<count;k++)
	{
		if(sockids[k]==i)
			break;
	}
	
	if((groups[j].state[k]==2)&&(groups[j].admin[k]==1))//if client is the admin
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"group is made broadcastonly type %d",groups[j].id);
		groups[j].broadcast=1;
		send(i,buffer,strlen(buffer),0);
		
		return 1;
	}
	if(groups[j].state[k]!=2)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not present in the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	if(groups[j].admin[k]==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"You are not admin of the group with id %d",groups[j].id);
		send(i,buffer,strlen(buffer),0);
		return 1;

	}
	return 1;



}
int printactivegroups(int i)
{
	int j,k,l,c=0;//printf("inside activegroups function");
	char buffer[256];
	char id[15];
	//finding the client information
	for(j=0;j<count;j++)
	{
		if(sockids[j]==i)
			break;
	}

	//for every group in which client is present
	for(k=0;k<group_count;k++)
	{
		if(groups[k].state[j]==2)
		{
			c++;
			memset(buffer,'\0',sizeof(buffer));//printf("group contains client");
			sprintf(buffer,"Group id : %d ,",groups[k].id);
			strcat(buffer,"admin ids :");
			//printf("buffer after adding group id : %s\n",buffer);
			//finding client ids of all the admin
			for(l=0;l<count;l++)
			{
				if((groups[k].state[l]==2)&&(groups[k].admin[l]==1))//if client is admin
				{
					memset(id,'\0',sizeof(id));
					sprintf(id,"  %d",clientids[l]);
					strcat(buffer,id);
				}
			}
			//printf("buffer content after adding admin ids : %s\n",buffer);
			strcat(buffer," , group memembers client ids :");
			for(l=0;l<count;l++)//finding client ids of all group memmbers
			{
				if(groups[k].state[l]==2)
				{
					memset(id,'\0',sizeof(id));
					sprintf(id,"  %d",clientids[l]);
					strcat(buffer,id);
				}
			}
			strcat(buffer,"\n");
			//printf("final message sent is %s\n",buffer);
			send(i,buffer,strlen(buffer),0);
			


		}
		
	}
	if(c==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,"No active groups in which you are member");
		send(i,buffer,strlen(buffer),0);
	}
	return 1;
	


}




/*Function to handle all the commands as entered by the client*/ 
int performAction(int fdmax,char *command,fd_set *master,int i, int sockfd){
	/*You are instructed to implement the following utilities:
	  -/active
	  -/send <client id> <Message>
	  -/broadcast <Message>
	  -/quit
	*/
	//WRITE YOUR CODE HERE
	//printf("123");
//	printf("command is %s\n",command);
	char *buffer =(char *)malloc(256*sizeof(char));
	if(strcmp(command,"/active")==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		printRecClient(buffer);//printf("line 195");
		send(i,buffer,strlen(buffer),0);
		return 1;
	}
	else if(strcmp(command,"/quit")==0)
	{
		memset(buffer,'\0',sizeof(buffer));
		strcpy(buffer,"connection terminated");
		send(i,buffer,strlen(buffer),0);
		removeRecClient(i,master);
		return 2;

	}
	else if(strcmp(command,"/activegroups")==0)
	{
		printactivegroups(i);//printf("activegroups command detected\n");
		return 2;
	}
	char *token=strtok(command," ");
	if(strcmp(token,"/send")==0)
	{
		int id=getClientId(i),isempty=0;
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"message from client %d : ",id);
		token=strtok(NULL," ");
		int dest=atoi(token);
		token=strtok(NULL," ");
		if(token==NULL)
			isempty=1;
		else
		{
			strcat(buffer,token);
		//	printf("%s ",token);
			token=strtok(NULL," ");

		}
		
		while(token!=NULL)
		{
		//	printf("%s ",token);
			strcat(buffer," ");
			strcat(buffer,token);
			token=strtok(NULL," ");
		}
		if(isempty==1)
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"Empty message sending is not allowed");
			send(i,buffer,strlen(buffer),0);
			return 1;
		}
		strcat(buffer,"\n");
		//printf("message sent is %s\n",buffer);
		id=getSocketid(dest);
		//printf("%d\n",id);
	
	if(id<0)
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"client %d is not active",dest);
			send(i,buffer,strlen(buffer),0);


		}
	else if(helper(buffer)==0)
	{
		memset(buffer,'\0',sizeof(buffer));

		sprintf(buffer,"%s","Sending abusive message is not allowed");
		send(i,buffer,strlen(buffer),0);
		return 1;
		 
	}

		else if(FD_ISSET(id,master))
		{
			if(send(id, buffer, strlen(buffer), 0)==-1)
			
			{
				memset(buffer,'\0',sizeof(buffer));
				sprintf(buffer,"client %d is not active",dest);
				send(i,buffer,strlen(buffer),0);

			}
		}
			
		else
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"client %d his not active",dest);
			send(i,buffer,strlen(buffer),0);
		}


		
	}
	else if(strcmp(token,"/broadcast")==0)
	{
		int id=getClientId(i),isempty=0;
		memset(buffer,'\0',sizeof(buffer));
		sprintf(buffer,"message from client %d : ",id);
		token=strtok(NULL," ");
		if(token==NULL)
			isempty=1;
		else
		{
			strcat(buffer,token);
			//printf("%s ",token);
			token=strtok(NULL," ");

		}
		
		while(token!=NULL)
		{
			//printf("%s ",token);
			strcat(buffer," ");
			strcat(buffer,token);
			token=strtok(NULL," ");
		}
		if(isempty==1)
		{
			memset(buffer,'\0',sizeof(buffer));
			sprintf(buffer,"empty message !");
			send(i,buffer,strlen(buffer),0);
			return 1;
		}
		strcat(buffer,"\n");
		//printf("message sent is %s\n",buffer);
		int j;
		for(j = 0; j <= fdmax; j++) {
			if(FD_ISSET(j, master)) {
				if(j != sockfd && j != i){
					if(send(j, buffer, strlen(buffer), 0) == -1) {
						perror("send");
								}
								}
							}
						}
		
	}
	else if(strcmp(token,"/makegroup")==0)
	{
		int *groupids=(int*)malloc(5*sizeof(int)),it=0;
		token=strtok(NULL," ");
		while(token!=NULL)
		{
			groupids[it]=atoi(token);
			it++;
			token=strtok(NULL," ");
		}
		groupids[it]=getClientId(i);
		makegroup(groupids,it,i);

	}
	else if(strcmp(token,"/makegroupreq")==0)
	{
		int groupids[5],it=0;
	
		//int k=makegroup(groupids,0);
		token=strtok(NULL," ");
		char buffer[256];
		//memset(buffer,'\0',sizeof(buffer));
		//sprintf(buffer,"request to join group having id %d ",k);
		while(token!=NULL)
		{
			groupids[it]=atoi(token);
			it++;
			token=strtok(NULL," ");
		}
		groupids[it]=getClientId(i);
		send_request(groupids,it);
	}
	else if(strcmp(token,"/joingroup")==0)
	{
		token=strtok(NULL," ");
		joingroup(atoi(token),i);
	}
	else if(strcmp(token,"/declinegroup")==0)
	{
		token=strtok(NULL," ");
		declinegroup(i,atoi(token));
		

	}
	else if(strcmp(token,"/sendgroup")==0)
	{
		int g;
		memset(buffer,'\0',sizeof(buffer));
		token=strtok(NULL," ");
		g=atoi(token);
		token=strtok(NULL," ");
		sprintf(buffer,"message from client %d : ",getClientId(i));
		strcat(buffer,token);
		token=strtok(NULL," ");
		while(token!=NULL)
		{
			strcat(buffer," ");
			strcat(buffer,token);
			token=strtok(NULL," ");
		}
		sendgroup(g,buffer,i);
	}
	else if(strcmp(token,"/makeadmin")==0)
	{
		int g;
		memset(buffer,'\0',sizeof(buffer));
		token=strtok(NULL," ");
		g=atoi(token);
		int j;
		token=strtok(NULL," ");
		while(token!=NULL)
		{
			j=atoi(token);
			makeadmin(g,i,j);
			token=strtok(NULL," ");
		}
	}
	else if(strcmp(token,"/addtogroup")==0)
	{
		int g,j,it=0;
		token=strtok(NULL," ");
		g=atoi(token);
		token=strtok(NULL," ");
		int groupids[5];
		while(token!=NULL)
		{
			groupids[it]=atoi(token);
			it++;
			token=strtok(NULL," ");
		}
		it--;
		addtogroup(i,g,groupids,it);

	}
	else if(strcmp(token,"/removefromgroup")==0)//removefromgroup group id clientids
	{
		int g,j,it=0;
		token=strtok(NULL," ");
		g=atoi(token);
		token=strtok(NULL," ");
		int groupids[5];
		while(token!=NULL)
		{
			groupids[it]=atoi(token);
			it++;
			token=strtok(NULL," ");
		}
		it--;
		removefromgroup(i,g,groupids,it);

	}
	else if(strcmp(token,"/makegroupbroadcast")==0)
	{
		int g;
		token=strtok(NULL," ");
		if(token==NULL)
		{
			memset(buffer,'\0',sizeof(buffer));
			strcpy(buffer,"Invalid command: send some group id also");
			send(i,buffer,strlen(buffer),0);
		}
		else
		{
			g=atoi(token);
			makegroupbroadcast(g,i);

		}
		
	}
	
	
	else
	{
		memset(buffer,'\0',sizeof(buffer));printf("line 232");
		strcpy(buffer,"invalid command");
		send(i,buffer,strlen(buffer),0);
	}
	
}


/*Call this function from the while loop to initiate the process as requested by the client. In this function you should call performAction which handles requests in a modular fashion.*/
void dostuff()
{
	//WRITE YOUR CODE HERE	
}

void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
	return &(((struct sockaddr_in*)sa)->sin_addr);
	}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	groups=(struct group*)malloc(500*sizeof(struct group));

	struct sockaddr_storage remoteaddr;
	fd_set master; // master file descriptor list
	fd_set read_fds; // temp file descriptor list for select()
	int fdmax; 
	srand(time(0)); /*seeding the rand() function*/
	socklen_t addrlen;
	
	//signal(SIGINT, sigCHandler);  // handles ^C
	//signal(SIGTSTP, sigZhandler);    //handles ^Z
	
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	FD_ZERO(&master);
	FD_ZERO(&read_fds);


	int sockfd, newsockfd, portno, clilen, pid,client_id,flags,i,j;
	struct sockaddr_in serv_addr, cli_addr;
	int nbytes;
	char buf[256],remoteIP[INET6_ADDRSTRLEN];

	  
	sockfd = socket(AF_INET, SOCK_STREAM, 0);  /*getting a sockfd for a TCP connection*/
	if (sockfd < 0)  error("ERROR opening socket");

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
	{ 
		error("can't get flags to SCOKET!!");
	} 


	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) 
	{ 
		error("fcntl failed.");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);
	serv_addr.sin_family = AF_INET; /*symbolic constant for IPv4 address*/
	serv_addr.sin_addr.s_addr = INADDR_ANY; /*symbolic constant for holding IP address of the server*/
	serv_addr.sin_port = htons(portno);

	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		error("ERROR on binding");
 	}
    if(listen(sockfd,10)==-1)
    {
        perror("listen");
        exit(3);
    }
	FD_SET(sockfd, &master);
	fdmax = sockfd;
 	
 	/*Initialize all the data structures and the semaphores you will need for handling the client requests*/
 	
 	
	while (1) {
		
		/*Write appropriate code to set up fd_set to be used in the select call. Following is a rough outline of what needs to be done here	
			-Calling FD_ZERO
			-Calling FD_SET
			-identifying highest fd for using it in select call
		*/
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			error("select");
			exit(4);
			}
		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == sockfd) {
					addrlen = sizeof(remoteaddr);
					newsockfd = accept(sockfd,(struct sockaddr *)&remoteaddr,&addrlen);
					if (newsockfd== -1) {
						perror("accept");
					}
                    else if(count==10)
                    {
                        sprintf(buf,"connection limit exceeded!!");
                        send(newsockfd,buf,strlen(buf),0);
                        close(newsockfd);

                    }
					 else {

                         
						 FD_SET(newsockfd, &master);
						 if (newsockfd > fdmax) {
							 fdmax = newsockfd;
						 }
						 getClientsd(newsockfd);
						 addRecClient();
						 
                         sprintf(buf,"welcome your id is %d ",clientids[count-1]);
                         send(newsockfd,buf,strlen(buf),0);
						 printf("selectserver: new connection from %s on socket %d\n",inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP, INET6_ADDRSTRLEN),newsockfd);
					 }

				}
				else
				{
					memset(buf,'\0',sizeof(buf));
					if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
						if (nbytes == 0) {
							printf("selectserver: socket %d hung up\n", i);
						}
						else{
							perror("recv");
						}
						removeRecClient(i,&master);
                        //count--;
						//close(i);
						//FD_CLR(i, &master); 
					}
					else {
						//printf("command recieved from client is %s\n",buf);
						performAction(fdmax,buf,&master,i,sockfd);
						/*for(j = 0; j <= fdmax; j++) {
							if (FD_ISSET(j, &master)) {
								if (j != sockfd && j != i) {
									if (send(j, buf, nbytes, 0) == -1) {
										perror("send");
									}
								}
							}
						}*/
					}

				}
			}
		}
		
		
		/*activity = select(); //give appropriate parameters */
		/*
		if((activity<___)&&(errno!=________)) //fill appropriate parameters here
		{
			error("!!ERROR ON SELECT!!");
		}*/
		
		/*After successful select call you can now monitor these two scenarios in a non-blocking fashion:
			- A new connection is established, and
			- An existing client has made some request
		  You have to handle both these scenarios after line 191.
		*/

	} /* end of while */
	
	return 0; /* we never get here */
}
