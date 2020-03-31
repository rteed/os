#include<iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include<cstring>
#define PORT 8080
using namespace std;




// cd /mnt/d/F/mp/asgn2



#define BUFF_SIZE1 102400
#define BUFF_SIZE2 1024
char prev_loc[BUFF_SIZE2];
char temp[BUFF_SIZE2];

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

    if(strcmp(cmd,"get")==0){
        if(index==0)        //absolute path case
            return filePath;
        int lIndex=findLastIndex(filePath);
        char * combFileName =new char[std::strlen(dirName)+delim.size()+strlen(filePath) +1];
         std::strcpy(combFileName,dirName);
         std::strcat(combFileName,delim.c_str());
        std::strcat(combFileName,filePath);
        return combFileName;
    }
    else if(strcmp(cmd,"put")==0)
    {int lIndex=findLastIndex(filePath);
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
{   char * combFileName =getApproFileName(fileName,dirName,"get");
    cout<<"combFileName: "<<combFileName<<std::endl;
    char* fs_name=combFileName;
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

 // printf ("decimal: %s\n",buffer);
    write(output_socket,buffer,sizeof(buffer));
    memset(buffer, 0, sizeof(buffer));
    rewind(file_);
    long SizeCheck = 0;
    while (!feof(file_)) // to read file
    {
        // fucntion used to read the contents of file
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
    char * combFileName = getApproFileName(fileName,dirName,"put");
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
            std::cout<<"File not found on client"<<std::endl;
            return;
        }
        FILE *fr = fopen(fs_name, "w");
       if(fr == NULL)
        {
            printf("ERROR: File %s cannot be created on client.\n", fs_name);
            return;
        }
        std::cout<<"File created on server"<<std::endl;
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
void concat_two_strings(char *str1, const char *str2){
  int c=0;
	while (str1[c] != '\0') {
		c++;
	}
	int c2=0;

	while(str2[c2]!='\0'){
		str1[c++] = str2[c2++];
	}

  str1[c] = '\0';
}


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




void execute_cmd(char *cmd, char *buff,int new_socket){

    std::cout<<"cmd: "<<cmd<<std::endl;
  char **parsed_args = new char*[BUFF_SIZE2];
  int arg_count;
	char cwd[BUFF_SIZE2];

  parse_input_string(cmd,  parsed_args, arg_count,' '); //space as delim

	// ** Change Directory ** //
	if (strcmp(parsed_args[0], "cd") == 0) {


    if (arg_count == 1 || strcmp(parsed_args[1], "~") == 0) {
			getcwd(prev_loc, sizeof(prev_loc));
      chdir(getenv("HOME"));
			strcpy(buff, "Changed to hme");
    }
		else if (strcmp(parsed_args[1], "/") == 0) {
			getcwd(prev_loc, sizeof(prev_loc));
			chdir("/");
			strcpy(buff, "Changed to root");
		}
		else if (strcmp(parsed_args[1], "-") == 0) {
			getcwd(temp, sizeof(prev_loc));
			chdir(prev_loc);
			strcpy(prev_loc, temp);
			strcpy(buff, "Changed to previous");
		}
    else if (strcmp(parsed_args[1], "--") == 0) {
			strcpy(buff, prev_loc); // showing what the previous location was
		}

    else{
			cmd = cmd + 3; //shifting cd command by 3 characters to get the path directory
			getcwd(temp, sizeof(prev_loc));
			int status = chdir(cmd);
      if (status) {
				strcpy(buff, "Enter proper Cd value");
      }
			else{
				strcpy(prev_loc, temp);
				strcpy(buff, "Directory changed");
			}
    }
    return;
  }

	// ** Current Working Directory ** //
  cout << "getcwd command :  "  <<  getcwd(cwd, sizeof(cwd)) << '\n';
  if(strcmp(cmd, "pwd") == 0){
    strcpy(buff, cwd);

    return;
  }

    if(strcmp(parsed_args[0], "get") == 0){
               strcpy(buff, cwd);
  std::cout<<"\nin get buffer: "<<std::endl;
  std::cout<<":::"<<parsed_args[1]<<std::endl;
     sendFileContent(buff,parsed_args[1],new_socket);
     return;
   }
     if(strcmp(parsed_args[0], "put") == 0){

         strcpy(buff, cwd);
  std::cout<<"\nin put buffer: "<<std::endl;
  std::cout<<":::"<<parsed_args[1]<<std::endl;
     receiveFileContent(buff,parsed_args[1],new_socket);
     return;
   }
	concat_two_strings(cmd, " 2>&1"); // redirect stderr(2) to stdout (1)
	FILE *fcmd = popen(cmd, "r"); // executes the command and stores the contents of both stdout and stderr in the file pointer
	// static char buff[1024];
	size_t n;
	if (fcmd == NULL) {
			fprintf(stderr, "popen(3) error");
			exit(EXIT_FAILURE);
	}

	while ((n = fread(buff, 1, BUFF_SIZE1-1, fcmd)) > 0) {
			buff[n] = '\0';
	}
	if(strlen(buff) == 0){
		strcpy(buff, "\n\nno output\n\n");
	}
	if (pclose(fcmd) < 0)
			perror("pclose(3) error");

	// printf("\n\n buff content is\n %s\n\n", buff);

}

int main(int argc, char const *argv[]){

  	int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFF_SIZE1] = {0};


    // Creating socket file descriptor
    // Returns an end point of communication and returns a file descriptor
    //socket(int domain, int type, int protocol);
    // domain : indicates the protocol family used for comm  AF_INET      IPv4 Internet protocols
    // type SOCK_STREAM     Provides sequenced, reliable, two-way, connection-
                       // based byte streams.  An out-of-band data transmission
                       // mechanism may be supported - TCP
    // type indicates signs and symbols used for communication.
    // Valid socket types are SOCK_STREAM to open a tcp(7) socket,
       // SOCK_DGRAM to open a udp(7) socket, or SOCK_RAW to open a raw(7)
       // socket to access the IP protocol directly.
       // 0 indicates to select the default protocol in the family

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    // setting the options of the socket
    // int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
    // set socket options
    // socket : what file descriptor
    // level : The level argument specifies the protocol level at which the option
               // resides. To set options at the socket level, specify the level
               // argument as SOL_SOCKET.
    // option_name  :  SO_REUSEADDR - reuse of local addresses
                    // SO_REUSEPORT - Permits multiple AF_INET or AF_INET6 sockets to be bound to an
                    // identical socket address.

    // *option_value : the value in here will be used to set the socket name option
    // option_len  : len of the option
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE); // system wide os constant non zero
    }
    address.sin_family = AF_INET;  // IPv4 Internet protocols
    address.sin_addr.s_addr = INADDR_ANY;   // INADDR_ANY  (0.0.0.0) means any address for binding
                                            // assigns all/any of the local network addresses at that port
    address.sin_port = htons( PORT );  // /* port in network byte order */
              // The htons() function converts the unsigned short integer hostshort
       //from host byte order to network byte order (big endian).

      //  When a socket is created with socket(2), it exists in a name space
          // (address family) but has no address assigned to it.  bind() assigns
          // the address specified by addr to the socket referred to by the file
          // descriptor sockfd.
    // attaching socket to the port 8080
    //  bind() assigns the address specified by addr to the socket referred to by the file
    //   descriptor sockfd.
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // int listen(int sockfd, int backlog);
    //  listen() marks the socket referred to by sockfd as a passive socket,
       // that is, as a socket that will be used to accept incoming connection
       // requests using accept(2). The backlog argument defines the maximum length to which the queue of
       //   pending connections for sockfd may grow.
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    while(1)
    {


// int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// The accept() system call is used with connection-based socket types
       // (SOCK_STREAM, SOCK_SEQPACKET).  It extracts the first connection
       // request on the queue of pending connections for the listening socket,
       // sockfd, creates a new connected socket, and returns a new file
       // descriptor referring to that socket.  The newly created socket is not
       // in the listening state.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    if(fork()==0)
    {

    std::cout<<"In child process"<<std::endl;
    char *output = new char[BUFF_SIZE1];
    do {
      memset(buffer, 0, strlen(buffer));
      memset(output, 0, strlen(output));

      valread = read( new_socket , buffer , BUFF_SIZE1);
        //printf("buffer: %s",buffer);
        std::cout<<"buffer: "<<buffer<<std::endl;

      if (strcmp(buffer, "close") == 0) {
          // send(newConnSock, buffer, sizeof(buffer), 0);
          std::cout << "\nclosing" << '\n';
          exit(0);
      }
      // executing the command and storing the result in the output string variable
      execute_cmd(buffer, output,new_socket);


      if(strlen(output) == 0){
        strcpy(output, "\n\nno output\n\n");
      }
      if(strstr(buffer,"get")==nullptr&&strstr(buffer,"put")==nullptr)
      {
          //std::cout<<"Not get or put"<<std::endl;
          send(new_socket , output , strlen(output) , 0 );

      }

      // break;

    } while(1);
    }
    else{
    //parent process
      std::cout<<"In parent process"<<std::endl;
    }
    }
    return 0;

}
