#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/types.h>
#include<stdlib.h>

	//game harbours
	int home[10][10]; 
	int away[10][10];
	// connection buffers
	int readbfr;
	int postbfr;
	// socket 
	int sock;
	int csock;
	// socket details
	struct sockaddr_in listeningsocket;
	struct sockaddr_in clisock;

/*
	network protocall for the game 
	message = 00000
	ones position = position status /occupied/not occupied/failed hit/successful hit
	tens position = y coord
	hundreds position = x coord
	thousands position = ship type
	ten thousands position = misc message;
*/

// function prototypes
	int checkfill(int x,int y,int orientation,int size);
	int gshow(char *query);
	void setz();
	int post();
	int recieve();
	int addship(int x,int y,int orientation,int size);
	int uphit();
	int upstrike();
	void status(int value);
	int placescheckhome();
	int placescheckaway();
    
    //Execution Begin !!

int main(int argc, char *argv[]){
	if (argc != 2)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        return 0;
    }

    unsigned short port = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        printf("\nsocket creation failed");
		return 0;
		}
    else {
	printf("Green socket \n");
	}      
    /* Construct local address structure */
    memset(&listeningsocket, 0, sizeof(listeningsocket));   /* Zero out structure */
    listeningsocket.sin_family = AF_INET;                /* Internet address family */
    listeningsocket.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    listeningsocket.sin_port = htons(port);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &listeningsocket, sizeof(listeningsocket)) < 0){
        printf("\nbind failed");
		return 0;
		}	
    else {
	printf("Green bind \n");
	}
    // listening for a request from the server
	if(listen(sock,1)<0){
		printf("\nattempts to connect to tihs pc failed aka listen failed");
		return 0;
		}
	else {
		printf("incoming connecction");
		}
	// accepting a connection request from client and setting up a dedicated socket apart from the listening socket
	socklen_t rsize=sizeof(clisock);

	csock=accept(sock,(struct sockaddr *)&clisock,&rsize);
	if(csock<0){
		printf("\n error in accept function");
		return 0;
		}
	else{
		printf("\nnow connected to %s",inet_ntoa(clisock.sin_addr));
		}
	//awesome
	int a,b,c,d=1;

	//start the game get filling
	setz();
	gshow("");
	while(d<=5){
		printf("Enter The x coordinate for placement of ship (size==|%2d:",d);
		scanf("%d",&a);
		printf("Enter The y coordinate for placement of ship (size==|%2d:",d);
		scanf("%d",&b);		
		printf("Enter The orientation for placement of ship \n1= vertical , 0=horizontal(size==|%2d:",d);
		scanf("%d",&c);		
		if((checkfill((a-1),(b-1),c,d))==1){
			addship((a-1),(b-1),c,d);
			d++;		
			}
		else{
			printf("invalid position");	
			}
		gshow("");
		}
	printf("Waiting for the other player");
	//start
	post(0);
	while(recieve()!=0){
		} 
	int t;
	t=0;
	printf("\ninitiating game");
	a=0;
	int x;
	int y;
	int p;
	
	//the game 
	while(a==0){
		
		if (t%2==0){
			printf("opponents turn %%.%%.%%..");
			uphit();
			t++;	
			}
		else{
			gshow("enter The coordinates for the strike");
			printf("The X coordinate:");
			scanf("%d",&x);
			printf("The Y coordinate:");
			scanf("%d",&y);
			upstrike(x-1,y-1);
			t++;
			}
		if (placescheckhome() == 0){
			gshow("You LOST . Better luck next time :( ");
			break;
			}
		else if(placescheckaway() ==0){
			gshow("You WON !! Awesome !! ,keep it up :) ");
			break;
			}
		}
	if(shutdown(csock,2)==0){
		printf("closed Client Socket");
		}
	if(shutdown(sock,2)==0){
		printf("closed Listening Socket");
		}
	printf("Hope you Enjoyed the Game (c) Paul George For GCI LABS \n Bye !");
	return 0;
	}
// places left
int placescheckhome(){
	int i,j,k=0;
	for (i=0;i<10;i++){
		for(j=0;j<10;j++){
			if ((home[i][j]%10)==1){
				k++;
				}
			}
		}
	return k;
	}
// places left on the away 	
int placescheckaway(){
	int i,j,k=0;
	for (i=0;i<10;i++){
		for(j=0;j<10;j++){
			if ((away[i][j]%10)==4){
				k++;
				}
			}
		}
	return (15-k);
	}
//update when hit by the enemy
int uphit (){
	int a = recieve();
	int x =(a%10);
	int y =((a/10)%10); 
	if (((home[x][y])%10)==1||((home[x][y]%10)==4)){
		home[x][y]=4;
		post(4);
		gshow ("you have been HIT");		
		return 1;
		}
	else if((home[x][y]%10)!=1){
		home[x][y]=3;
		gshow("the opponent missed");
		post(3);
		return 1;
		}
	return 8;
	}
//update when attacking the enemy

int upstrike(int x, int y){
	post(x+(10*y));		
	int a = recieve();
	if ((a)==4){
		away[x][y]=4;
		gshow("hit successful");
		}
	else{
		away[x][y]=3;
		gshow("failed hit");
		}
	return 1;
	}

//checking if the ship can be inserted in the given position
	
int checkfill(int x,int y,int orientation,int size){
	int i,j,k=0,l;	
	if(orientation ==1){
		for(i=0;((x+i)<10)&&(i<size);i++){
			if (home[x+i][y]==0){
				k++;
				}
			}		
		
		}
	else if(orientation ==0) {
		for(i=0;((y+i)<10)&&(i<size);i++){
			if (home[x][y+i]==0){
				k++;
				}
			}
		}
	if (k==size){
		return 1;
		}
	else{
		return 8;
		}	
	}



// displaying the array s and related information

int gshow(char message[1024]){	
	system("clear");	
	int i=-1,j=-1,k,l;
	printf("\n");
	printf("\nNETOWRK BATTLESHIP V 1.0.1 Beta (c) Paul George For GCI LABS\n");
	for (i=-1;i<10;i++){
		for(j=-1;j<10;j++){
			if(i==-1&&j==-1){
				printf(" x\\y ");
				}
			else if (i==-1){
				printf(" %2d  ",j+1);
				}
			else if(j==-1){
				printf(" %2d  ",i+1);
				}
			else{
				status(home[i][j]);
				}
			}
		printf("\n");		
		}
	printf("ships remaing to attack: %2d",placescheckaway());
	printf("\tships left : %2d",placescheckhome());
	printf("\n");
	for (i=-1;i<10;i++){
		for(j=-1;j<10;j++){
			if(i==-1&&j==-1){
				printf(" x\\y ");
				}
			else if (i==-1){
				printf(" %2d  ",j+1);
				}
			else if(j==-1){
				printf(" %2d  ",i+1);
				}
			else{
				status(away[i][j]);
				}
			}
		printf("\n");		
		}	
	puts(message);
	}
	
// char graphics later upgrade to open gl
void status(int value){
		if (value%10==0){
			printf(" ~~~ ");			
			}
		else if (value%10==3){
			printf(" ~*~ ");			
			}
		else if (value%10==4){
			printf(" /O/ ");			
			}
		else if (value%10==1){
			printf(" /#/ ");			
			}
		else {
			printf("error");			
			}
	}
// setting the arrays to zero in the begining
void setz(){
	int i=0,j=0;
	for (i=0;i<10;i++){
		for(j=0;j<10;j++){
			home[i][j]=0;
			}
		}
	for (i=0;i<10;i++){
		for(j=0;j<10;j++){
			away[i][j]=0;
			}
		}
	}
// posting the message integer
int post(int32_t num){
        postbfr = htonl(num);
        send(csock, &postbfr, sizeof(int32_t), 0);
        return  0;
	}

// recieve ing the sent integer

int recieve(){
	recv(csock,&readbfr, sizeof(int32_t), 0);
	readbfr = ntohl(readbfr);
	return readbfr;
	}
// insert ship
int addship(int x,int y,int orientation,int size){
	int i=0,j=0;
	if(orientation ==1){
		for(i=0;i<size;i++){
			home[x+i][y]=10*(x+i)+100*(y)+1000*(size)+1;
			}		
		}
	else{
		for(i=0;i<size;i++){
			home[x][y+i]=10*(x)+100*(y+i)+1000*(size)+1;
			}
		}
	}
