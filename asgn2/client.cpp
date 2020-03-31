// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include<iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include<cstring>
#include <sys/wait.h>
#define PORT 8080
using namespace std;


// cd /mnt/d/F/mp/asgn2


#define BUFF_SIZE1 102400
#define BUFF_SIZE2 1024

char prev_loc[BUFF_SIZE2];
char temp[BUFF_SIZE2];
void parse_input_string(char *input_string, char **parsed_strings, int &count, char delim){
	int i=0,k=0;
	count=0;

  while(input_string[i]!='\0'){

    k=0;
    char *temp_word = new char[BUFF_SIZE2];
		// This loop copies the first word until space into the temp word variable
    while(input_string[i]!=delim && input_string[i]!='\0'){
      temp_word[k] = input_string[i];
      k++;
      i++;
    }
    temp_word[k] = '\0';
		// copies the temp word variable to the parsed arguments
    parsed_strings[count] = temp_word ;
    count++;
		// At this point we may get either the delim or the end of string
		// If equal to delim then go to next word
    if (input_string[i]==delim){
      i++;
    }

  }
	// This ensures we have a NULL terminated array of strings
  parsed_strings[count] = NULL ;

}

int findLastIndex(const char* str)
{   int index=-1;
    int len =strlen(str);
    for(int i=0;i<len;i++)
    {
        if(str[i]=='/')          // cannot be put in double quotes
            index=i;
    }
    return index;
}
char* getApproFileName(char* filePath,char* dirName,const char* cmd)
{
    string delim="/";
    string str(filePath);
    int index=str.find(delim);

    if(strcmp(cmd,"put")==0){
        if(index==0)        //absolute path case
            return filePath;
        int lIndex=findLastIndex(filePath);
        char * combFileName =new char[std::strlen(dirName)+delim.size()+strlen(filePath) +1];
         std::strcpy(combFileName,dirName);
         std::strcat(combFileName,delim.c_str());
        std::strcat(combFileName,filePath);
        return combFileName;
    }
    else if(strcmp(cmd,"get")==0)
    {
                int lIndex=findLastIndex(filePath);
        string tempFileName;
        if(lIndex==-1)
            {
                tempFileName=str;
                char * combFileName =new char[std::strlen(dirName)+delim.size()+tempFileName.size() +1];
                std::strcpy(combFileName,dirName);
                std::strcat(combFileName,delim.c_str());
                std::strcat(combFileName,tempFileName.c_str());
                   return combFileName;
            }
        else
        {
            tempFileName=str.substr(lIndex,-1);
        char * combFileName =new char[std::strlen(dirName)+tempFileName.size() +1];
         std::strcpy(combFileName,dirName);
        std::strcat(combFileName,tempFileName.c_str());
           return combFileName;
        }


    }
}
void sendFileContent(char* dirName,char* fileName,int output_socket)
{   char * combFileName = getApproFileName(fileName,dirName,"put");
    //cout<<"combFileName: "<<combFileName<<std::endl;
    const char* fs_name=combFileName;
    char buffer[1024];
        FILE *file_ = fopen(fs_name, "r");
       if(file_ == NULL)
        {
            printf("ERROR: File %s not found.\n", fs_name);
            snprintf(buffer, sizeof(buffer), "%d", -1);
            //printf ("decimal: %s\n",buffer);
            write(output_socket,buffer,sizeof(buffer));
            return;
        }    // Read data into buffer.  We may not have enough to fill up buffer, so we
    // store how many bytes were actually read in bytes_read.

    fseek(file_, 0, SEEK_END);
    long FileSize = ftell(file_);
    std::cout<<"\nFileSize"<<FileSize<<std::endl;
    snprintf(buffer, sizeof(buffer), "%d", FileSize);

  //printf ("decimal: %s\n",buffer);
    write(output_socket,buffer,sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));
    rewind(file_);
    long SizeCheck = 0;
    while (!feof(file_)) // to read file
    {
        // function used to read the contents of file
        fread(buffer, sizeof(char),sizeof(buffer), file_);
        long fr_block_sz=write(output_socket,buffer,sizeof(buffer));
       // cout << buffer<<endl;
       memset(buffer, 0, sizeof(buffer));
       SizeCheck+=fr_block_sz;
        if(SizeCheck>=FileSize)
        break;
    }

    std::cout<<"File transfer done"<<std::endl;
    fclose(file_);
    return;
}

void receiveFileContent(char* dirName,char* fileName,int output_socket)
{
    char * combFileName = getApproFileName(fileName,dirName,"get");
    cout<<"combFileName: "<<combFileName<<std::endl;
    int fr_block_sz;
    int LENGTH=1024;
    const char* fs_name=combFileName;
    char revbuf[1024];

        long FileSize=0;
        recv(output_socket,revbuf, LENGTH, 0);
        FileSize=atoi(revbuf);
        std::cout<<"fileSize: "<<FileSize<<std::endl;
        if(FileSize==-1)
        {
            std::cout<<"File not found on server"<<std::endl;
            return;
        }
        FILE *fr = fopen(fs_name, "w");
       if(fr == NULL)
        {
            printf("ERROR: File %s cannot be created on client.\n", fs_name);
            return;
        }

         std::cout<<"File created on client"<<std::endl;
        long SizeCheck = 0;
         memset(revbuf, 0, sizeof(revbuf));
while((fr_block_sz = recv(output_socket, revbuf, LENGTH, 0)) > 0)
        {

            //std::cout<<"in while"<<std::endl;
            //std::cout<<"block_sz"<<fr_block_sz<<std::endl;
             //std::cout<<"\n"<<revbuf<<std::endl;
            int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
            if(write_sz < fr_block_sz)
            {
                std::cout<<"File write failed"<<std::endl;
                perror("File write failed.\n");
            }
            //bzero(revbuf, LENGTH);
            //std::cout<<"SizeCheck"<<SizeCheck;
            memset(revbuf, 0, sizeof(revbuf));
            SizeCheck+=fr_block_sz;
            if(SizeCheck>=FileSize)
                break;        }
       // std::cout<<"While ended"<<std::endl;
        if(fr_block_sz < 0)
        {
            if (errno == EAGAIN)
            {
                std::cout<<"in if "<<errno<<std::endl;
                printf("recv() timed out.\n");
            }
            else
            {
                std::cout<<"in if "<<stderr<<" "<<errno<<std::endl;
                fprintf(stderr, "recv() failed due to errno = %d\n", errno);
            }
        }
    fclose(fr);
		if(SizeCheck<FileSize)
	    cout<<"File transfer incomplete"<<endl;
    else
    	    std::cout<<"File transfer done"<<std::endl;
    return;

}





int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	struct sockaddr_in serv_addr;

	char buffer[BUFF_SIZE1] = {0};
  char *input_string = new char[BUFF_SIZE2];
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		// return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	//192.168.56.1

	// int inet_pton(int af, const char *src, void *dst);  // convert IPv4 and IPv6 addresses from text to binary form
	// This function converts the character string src into a network
  //  address structure in the address family, then copies the network
       // address structure to dst.
			 //  dst is written in network byte order.
	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		// return -1;
	}
//   int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
// The connect() system call connects the socket referred to by the file
       // descriptor sockfd to the address specified by addr.  The addrlen
       // argument specifies the size of addr.


	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
  memset(input_string, 0, strlen(input_string));  // initializing input_string to null
  char *output = new char[BUFF_SIZE1];
  char *pwd = new char[BUFF_SIZE2];  // present working directory of server
  int arg_count;
  char cwd[BUFF_SIZE2];  // holds the current working directory info
  char **parsed_args = new char*[BUFF_SIZE2];  // null terminated array of strings of the space limited parsed arguments
  do {
		memset(input_string, 0, strlen(input_string));
		memset(output, 0, strlen(output));
		memset(pwd, 0, strlen(pwd));
		memset(cwd, 0, strlen(cwd));
		arg_count = 0;

    send(sock , "pwd" , 3 , 0 );
    valread = read( sock , pwd, BUFF_SIZE2);
		if(strcmp(pwd,"")==0)
    {
			printf("\n@@@@@@@@@Connection error\n");
    	break;
    }
    printf("--- server location --- %s >>\n", pwd );

    getcwd(cwd, sizeof(cwd));
    printf("--- client location --- %s >>\n\n", cwd);



    cin.getline(input_string, BUFF_SIZE2);
		if (strlen(input_string)==0){
			continue;
		}
    parse_input_string(input_string,  parsed_args, arg_count,' '); //space as delim
		if (strcmp(parsed_args[0], "lls") == 0  ) {

				//input_string = input_string + 1;
				parsed_args[0] = parsed_args[0] + 1;

				printf("\n\n");
				pid_t pc = fork();
      	if(pc==0){ // new process
					cout << endl;
	        if (execvp("ls",parsed_args) < 0) {
						cout << endl << "Could not execute command" << endl << endl;
	    		}
					exit(0);  // exit in case the command does not execute
      	}
      	else{
       		wait(NULL);
      	}
				printf("\n\n");
				continue;
      }



		  if (strcmp(parsed_args[0], "lcd") == 0) {


		    if (arg_count == 1 || strcmp(parsed_args[1], "~") == 0) {
					getcwd(prev_loc, sizeof(prev_loc)); // store current location in prev_loc
		      chdir(getenv("HOME"));
					printf("\n\n%s\n\n", "Changed to hme" );

		    }
				else if (strcmp(parsed_args[1], "/") == 0) {
					getcwd(prev_loc, sizeof(prev_loc));
					chdir("/");
					printf("\n\n%s\n\n", "Changed to root" );

				}
				else if (strcmp(parsed_args[1], "-") == 0) {
					getcwd(temp, sizeof(prev_loc));
					chdir(prev_loc);
					strcpy(prev_loc, temp);
					printf("\n\n%s\n\n", "Changed to previous" );

				}
				else if (strcmp(parsed_args[1], "--") == 0) {
					// getcwd(temp, sizeof(prev_loc));
					// chdir(prev_loc);
					// strcpy(prev_loc, temp);
					printf("\n\n%s\n\n",  prev_loc);

				}

		    else{
					// lcd is removed - cd to a directory path
					input_string = input_string + 4;
					getcwd(temp, sizeof(prev_loc));
					int status = chdir(input_string);
		      if (status) {
						printf("\n\n%s\n\n", "Enter proper directory value" );
		      }
					else{
						strcpy(prev_loc, temp);
						printf("\n\n%s\n\n", "Directory changed" );

					}
		    }
		    continue;
		  }

		if (strcmp(parsed_args[0], "lchmod") == 0  ) {
			parsed_args[0] = parsed_args[0] + 1;
			printf("\n\n");
			pid_t pc = fork();
			if(pc==0){
				cout << endl;
				if (execvp("chmod",parsed_args) < 0) {
					cout << endl << "Could not execute command" << endl << endl;
				}
				exit(0);  // exit in case the command does not execute
			}
			else{
				wait(NULL);
			}
			printf("\n\n");
			continue;
		}
		if(strcmp(parsed_args[0],"get")==0)
        {
            //printf("args[1]: %s",parsed_args[2]);
						if(arg_count!=2)
			          {
			              std::cout<<"Incorrect command format"<<std::endl;
			              continue;
			          }
            send(sock , input_string , strlen(input_string) , 0 );
            receiveFileContent(cwd,parsed_args[1],sock);
            continue;
        }
        if(strcmp(parsed_args[0],"put")==0)
        {
            //printf("args[1]: %s",parsed_args[1]);
						if(arg_count!=2)
			          {
			              std::cout<<"Incorrect command format"<<std::endl;
			              continue;
			          }
            send(sock , input_string , strlen(input_string) , 0 );
            sendFileContent(cwd,parsed_args[1],sock);
            continue;
        }

		// if the command is any of the other client commands, it must be a server command
		// and sent to the server
    send(sock , input_string , strlen(input_string) , 0 );
    if (strcmp(input_string, "close") == 0) {
        // send(newConnSock, buffer, sizeof(buffer), 0);
        printf("breaking\n\n");
        break;
    }
    valread = read( sock , output, BUFF_SIZE1);
    printf("\n\n%s\n\n", output);

    // break;
  } while(1);


	return 0;
}
