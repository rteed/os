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
  // fclose (stderr);
	// stderr = fopen ("tee2.txt", "w");
    //printf("\ncmd: %s",cmd);
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

    else{
			cmd = cmd + 3; //shifting cd command by 3 characters
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
	concat_two_strings(cmd, " 2>&1");
	FILE *fcmd = popen(cmd, "r");
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
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    while(1)
    {


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
      if (strcmp(buffer, "exit") == 0) {
          printf("breaking\n\n");
          exit(0);
          //break;
      }
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
