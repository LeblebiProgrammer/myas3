
#include "helperFunctions.h"

#define SA struct sockaddr

typedef struct stringListObject{
  char *value;
  char *fileName;
  struct stringListObject *next;
}stringList;

// Function designed for chat between client and server.

int functionDeterminer(char *str){
    char *functionString = NULL;
		int type = -1;
    char *ptr = strchr(str, ':');
    if(ptr) {
       int index = ptr - str;
       int i = 0;
       functionString = (char*)malloc(sizeof(char)*(index+1));

       //printf("portlen %d\t",portLen );
       for(i = 0; i< index; i++){
           functionString[i] = str[i];
       }
       functionString[i] = '\0';
   }

	 if(strcmp(functionString, "checkout") == 0){
		 type = 1;
	 }else if(strcmp(functionString, "update") == 0){
		  type = 2;
	 }else if(strcmp(functionString, "upgrade") == 0){
		  type = 3;
	 }else if(strcmp(functionString, "commit") == 0){
		  type = 4;
	 }else if(strcmp(functionString, "push") == 0){
		  type = 5;
	 }else if(strcmp(functionString, "create") == 0){
		  type = 6;
	 }else if(strcmp(functionString, "destroy") == 0){
		  type = 7;
	 }else if(strcmp(functionString, "currentversion") == 0){
		  type = 10;}
	else if(strcmp(functionString, "rollback") == 0){
 		  type = 11;
	 }else if(strcmp(functionString, "rollback") == 0){
		  type = 12;
	 }

	 free(functionString);
   return type;
}


int recursiveRead(char *path, stringList **head, char *parentPath)
{
   DIR *_dir = opendir(path);
   int rvalue = -1;
   if (_dir)
   {
      struct dirent *pdirent;
      rvalue = 0;
      while ((pdirent=readdir(_dir)) != NULL)
      {
          int rvalue2 = -1;

          if (!strcmp(pdirent->d_name, ".") || !strcmp(pdirent->d_name, ".."))
          {
             continue;
          }
           struct stat statbuf;
           char *fpath = concat(path, pdirent->d_name, '/');
           char *ppath = concat(parentPath, pdirent->d_name, '/');
           if (!stat(fpath, &statbuf)){
              if (S_ISDIR(statbuf.st_mode)){

                recursiveRead(fpath, head, ppath);

              }
              else{
                 char *str = fileReader(fpath);
                 if((*head) == NULL){
                   (*head) = (stringList *)malloc(sizeof(stringList));
                   (*head)->value = str;
                   (*head)->fileName = ppath;
                   (*head)->next = NULL;
                 }
                 else{
                   stringList *nextNode = (stringList*)malloc(sizeof(stringList));
                   nextNode->value = str;
                   nextNode->fileName = ppath;
                   nextNode->next = NULL;
                   stringList *ptr = *head;
                   do{
                     if(ptr->next == NULL){
                       ptr->next = nextNode;
                       break;
                     }
                     else{
                       ptr = ptr->next;
                     }
                   }while(ptr != NULL);
                 }
              }
           }

         //free(fpath);
         rvalue = rvalue2;
      }
      closedir(_dir);
   }

   return rvalue;
}


char * checkoutFunction(char *fname){
  char *message = NULL;
  struct stat stats;
  char *numpath = NULL;
  if (stat( fname, &stats) != -1){
    DIR *_dir = opendir(fname);
    struct dirent *de;
    int highest = 0;
    char hasManifest = '0';
    while((de = readdir(_dir)) != NULL) {
      if(strcmp(de->d_name, ".manifest") == 0){
        hasManifest = '1';
      }
      if(!isdigit(*de->d_name))
          continue;
      int version = atoi(de->d_name);
      if(highest < version){
        highest = version;
        numpath = de->d_name;
      }
    }
    if(highest == 0 && hasManifest == '1'){
      char *st = "success><";
      char *stpt2 = concat(st, fname, '\0');
      char *path_ = concat(stpt2, ".manifest", '/');
      message = concat(path_, "><version:0\n", '\0');
      free(stpt2);
      free(path_);
    }
    else{
      char *path = concat(fname, numpath, '/');
      stringList *head = NULL ;//= (stringList*)malloc(sizeof(stringList));
      recursiveRead(path, &head, fname);
      char *mms = NULL;
      int count = 0;
      while(head != NULL){
        char *thead = concat("<", head->fileName, '\0');
        char *temp = concat(thead, "><", '\0');
        char *messagePrep1 = concat(temp, head->value, '\0');
        char *messagePrep2;
        if(head->next != NULL){
          messagePrep2 = concat(messagePrep1, "", '>');
        }
        else{
          messagePrep2 = concat(messagePrep1, "", '\0');
        }

        if(count > 0){
          char *mprep = mms;
          mms = concat(mprep, messagePrep2, '\0');
          free(mprep);
        }
        else{
          mms = concat("", messagePrep2, '\0');
        }
        count++;

        free(thead);
        free(temp);
        free(messagePrep1);

        // char *tmp = concat()
        //printf("%s\n", head->value);
        head = head->next;
      }
      message = concat("success", mms,'>');
      free(mms);
      free(path);
    }
  }
  return message;
}

int createFunction(char *str){
  //returns 0 for success, 1 for file present error, 2 for manifest error
	struct stat stats;
  int result = -1;
	if (stat( str, &stats) == -1) {
    int mkdirResult = mkdir(str, 0700);
    char *maniFilePath = concat("./",str, '\0');
    char *fp = concat(maniFilePath, ".manifest", '/');
    char *maniContent = "version:0\n";
    int manifestWrite = fileWriter(fp, maniContent, strlen(maniContent), '1');
    if(mkdirResult != -1 && manifestWrite != -1){
      printf("Created successfully\n");
      result = 0;
    }
    else {
      result = 2;
    }
	}
	else{
    //file was present error
    result = 1;
		printf("File present error\n");
	}
  return result;
}


int remove_directory(char *path)
{
   DIR *_dir = opendir(path);
   int rvalue = -1;
   if (_dir)
   {
      struct dirent *pdirent;
      rvalue = 0;
      while (!rvalue && (pdirent=readdir(_dir)))
      {
          int rvalue2 = -1;

          if (!strcmp(pdirent->d_name, ".") || !strcmp(pdirent->d_name, ".."))
          {
             continue;
          }
           struct stat statbuf;
           char *fpath = concat(path, pdirent->d_name, '/');
           if (!stat(fpath, &statbuf)){
              if (S_ISDIR(statbuf.st_mode)){
                 rvalue2 = remove_directory(fpath);
              }
              else{
                 rvalue2 = unlink(fpath);
              }
           }

           free(fpath);
          rvalue = rvalue2;
      }
      closedir(_dir);
   }
   //empty dir
   if (!rvalue)
   {
      rvalue = rmdir(path);
   }

   return rvalue;
}

int destroyFunction(char *str){
  //returns 0 for success, 1 for dir was not found,2 for deletion was not done,
  //
  struct stat stats;
  int result = -1;
  if (stat( str, &stats) != -1){
    int rs = remove_directory(str);
    if(rs != 0){
      //resulted in failure
      result = 2;
      printf("%s could not been deleted.\n", str);
    }
    else{
      //success
      result = 0;
      printf("%s deleted.\n", str);
    }
  }
  else{
    printf("%s not present.\n", str);
    result = 1;
  }
  return result;
}

int rollback(char *pname, char *version){
  struct stat stats;
  int result = -1;
  if (stat( pname, &stats) != -1){
    int _version = atoi(version);
    DIR *_dir = opendir(pname);
    struct dirent *de;
    while((de = readdir(_dir)) != NULL) {
      if(!isdigit(*de->d_name))
          continue;
      int x = atoi(de->d_name);
      if(x>_version){
        char *dpath = concat(pname,de->d_name, '/');
        int localresult = remove_directory(dpath);
        if(localresult != 0){
          result = 2;
        }
      }
    }
    if(result == -1)
      result = 0;
  }
  else{
    result = 1;
  }
  return result;
}

char *currentVersionFunction(char *fname){
  char *message = NULL;
  struct stat stats;
  char *numpath = NULL;
  if (stat( fname, &stats) != -1){
    DIR *_dir = opendir(fname);
    struct dirent *de;
    int highest = 0;
    char hasManifest = '0';
    while((de = readdir(_dir)) != NULL) {
      if(strcmp(de->d_name, ".manifest") == 0){
        hasManifest = '1';
      }
      if(!isdigit(*de->d_name))
          continue;
      int version = atoi(de->d_name);
      if(highest < version){
        highest = version;
        numpath = de->d_name;
      }
    }
    if(highest == 0 && hasManifest == '1'){
      char *st = "success><";
      char *stpt2 = concat(st, fname, '\0');
      char *path_ = concat(stpt2, ".manifest", '/');
      message = concat(path_, "><version:0\n", '\0');
      free(stpt2);
      free(path_);
    }
    else{
      char *fpath1 = concat(fname, numpath, '/');
      char *fpath2 = concat(fpath1, ".manifest>", '/');
      char *str = fileReader(fpath2);


      char *tempPath = concat("success><", fpath2, '\0');
      message = concat(tempPath, str, '<');
      free(fpath1);
      free(fpath2);
      free(str);
      free(tempPath);
    }
  }
  return message;
}

void func(int sockfd)
{
	char *readString = sockReader(sockfd);
    //printf("\n%s\n", readString);
	int functionType = functionDeterminer(readString);
  int result = -1;
  char *message = NULL;
	printf("%d\n", functionType);
	switch(functionType){
		case 1:{
      char *fname = subString(readString, ':', '1');
      char *mssg = checkoutFunction(fname);
      free(fname);
      if(mssg != NULL){
        message = msgPreparer(mssg);
        free(mssg);
      }
	    break;
    }
		case 2:{
      char *temp;
			char *proj = subString(readString, ':', '1');
			proj[strlen(proj)] = '\0';//might cause seg faults

			char *path_manifest = concat(proj, ".manifest", '\0');

  			int fd = open(path_manifest, O_RDONLY);

  			char buffer[1024];
  			char *_manifest;
  			int buffer_len = 0;
  			int _manifest_len = 0;
			if (fd > 0) {
				while ((buffer_len = read(fd, buffer, 1023)) > 0) {
					_manifest_len += buffer_len;
					_manifest = realloc(_manifest, (_manifest_len + 1) * sizeof(char));
					strncat(_manifest, buffer, buffer_len);
					_manifest[_manifest_len] = '\0';
    			}

        		temp = concat("success>", _manifest, '<');
    			close(fd);
  			}
  			else {
  				temp = "error><manifest could not be found.";
  			}
  			message = msgPreparer(temp);
        break;
    }

		case 3:
    {
      char *proj = subString(readString, ':', '1');
			char *update = subString(proj, ':', '1');
			char buffer[1024];
			int i = 0;
			while(*update++) {
				if (*update == 'M') {
					update += 2;
					while (*update != '\n') buffer[i++] = *update++;
					buffer[i] = '\0';
					char *temp = concat(buffer, _read(buffer), ':');
					message = realloc(message, strlen(message) + strlen(temp) + 1);
					message = concat(message, temp, '\n');
				}
				else if (*update == 'A') {
					update += 2;
					while (*update != '\n') buffer[i++] = *update++;
					buffer[i] = '\0';
					char *temp = concat(buffer, _read(buffer), ':');
					message = realloc(message, strlen(message) + strlen(temp) + 1);
					message = concat(message, temp, '\n');
				}
			}
      break;
		}
		case 4:
			break;
		case 5:
			break;
		case 6:{
      char *fname = subString(readString, ':', '1');
      char *temp;
			result = createFunction(fname);
      if(result == 0){
        //temp = "<success>";

        char *_path = concat("<", fname, '\0');
        char *path_ = concat(_path, ".manifest", '/');
        char *_path2 = concat(path_, "><version:0\n", '\0');
        temp = concat("success>", _path2, '\0');
      }else if(result == 1){
        temp = "error><file was present.";
      }else if(result == 2){
        temp = "error><manifest had an error.";
      }else{
        temp = "error><Unknown error occured with create.";
      }


      message = msgPreparer(temp);
			break;
    }
		case 7:{
      char *fname = subString(readString, ':', '1');
      char *temp;
      result = destroyFunction(fname);
      if(result == 0){
        temp = "success";
      }else if(result == 1){
        temp = "error><Project not found.";
      }else if(result == 2){
        temp = "error><Project could not been deleted.";
      }else{
        temp = "error><Unknown error occured with delete.";
      }
      message = msgPreparer(temp);
			break;
    }
		case 10:
			{
        char *fname = subString(readString, ':', '1');
        char *temp = currentVersionFunction(fname);
        message = msgPreparer(temp);
        // char *temp;
  			// char *proj = subString(readString, ':', '1');
  			// proj[strlen(proj)] = '\0';//might cause seg faults
        //
  			// char *path_manifest = concat(proj, ".manifest", '\0');
        //
  			// int fd = open(path_manifest, O_RDONLY);
        //
  			// char buffer[1024];
  			// char *_manifest;
  			// int buffer_len = 0;
  			// int _manifest_len = 0;
  			// if (fd > 0) {
  			// 	while ((buffer_len = read(fd, buffer, 1023)) > 0) {
    		// 			_manifest_len += buffer_len;
    		// 			_manifest = realloc(_manifest, (_manifest_len + 1) * sizeof(char));
    		// 			strncat(_manifest, buffer, buffer_len);
    		// 			_manifest[_manifest_len] = '\0';
    		// 	}
    		// 	close(fd);
  			// }
  			// message = (char*)malloc(_manifest_len + 1);
  			// memcpy(message, _manifest, _manifest_len);
  			// free(_manifest);

        break;
    }

		case 11:{
      //char *fname = subString(readString, ':', '0');
      char *tmp = subString(readString, ':', '1');
      char *fname = subString(tmp, ':', '0');
      char *version = subString(tmp, ':', '1');
      rollback(fname, version);

			break;
    }
		case 12:
			break;
	}


  if(message != NULL){
    write(sockfd, message, sizeof(message)*strlen(message));
    free(message);
  }
  free(readString);
}


void compress(){

}


// Driver function
int main(int argc, char **argv)
{


    if(argc<2){
        printf("Please enter port number\n");
        return 0;
    }

    char *_port = argv[1];
    int portNo = 0;
    if(strlen(_port)>=4){
        portNo = atoi(_port);
    }
    else{
        printf("Please enter a port number greater than 8k\n");
        return 0;
    }
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
    	printf("socket creation failed...\n");
    	exit(0);
    }
    else
    	printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(portNo);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    	printf("socket bind failed...\n");
    	exit(0);
    }
    else
    	printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
    	printf("Listen failed...\n");
    	exit(0);
    }
    else{
    	printf("Server listening..\n");
           len = sizeof(cli);


    	// Accept the data packet from client and verification
    	connfd = accept(sockfd, (SA*)&cli, &len);
    	if (connfd < 0) {
    		printf("server acccept failed...\n");
    		exit(0);
    	}
    	else
    		printf("server acccept the client...\n");

    	// Function for chatting between client and server
    	func(connfd);

    	// After chatting close the socket

    }
    close(sockfd);
    return 0;
}
