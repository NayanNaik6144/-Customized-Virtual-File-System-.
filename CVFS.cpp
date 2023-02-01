/*
####################################################################
##      Customized Virtual File System Application                ##
##      Name - Nayan Suresh Naik                                  ##
####################################################################
*/

/*
####################################################################
##              ALL Required  Header Files                        ##
##                                                                ##
####################################################################
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<unistd.h>
#include<io.h>


/*
####################################################################
##                 Definging all required MACROS                  ##
##                                                                ##
####################################################################
*/


#define MAXINODE     50  // defining Max inode size is 50 in 

#define READ         1   // 1 for read
#define WRITE        2   // 2 for write
#define MAXFILESIZE 1024 // size of file instd formmat that is 1kb(1024Bytes)

#define REGULAR      1   // file type regular 
#define SPECIAL      2   // file type special


#define START    0       // start 0
#define CURRENT  1       // Cuurent 1
#define END      2       // end 2



/*
################################################000####################
##               Creating SuperBlock structure                       ##
##                                                                   ##
#######################################################################
*/

typedef struct superblock   // size of the superblock structure is 8 bytes
{

    int TotalInodes; // Total nodes contain in Disk Inode List block
    int FreeInode;   // Total free nodes contain in Disk Inode List block

} SUPERBLOCK , *PSUPERBLOCK;  


/*
################################################000####################
##               Creating  structure of Inode                        ##
##                                                                   ##
#######################################################################
*/

# pragma pack(1) 
typedef struct inode // size of the superblock structure is 88 bytes
{
    char FileName[50];    // size is 50 for char // file name stored in that array 

    int InodeNumber;      //4 byte // assign number to Inode 

    int FileSize;         //4 Bytes // Std file size in the format of multiple of 1kb ( 1024 bytes ) 

    int FileActualSize;  //4 bytes // it is allocated when we create file and write data inside file 

    int FileType;        // 4 byte regular or special

    char *Buffer;        // 8 bytes for char pointer // Buffer for to stored write data in the file

    int LinkCount : 5 ;       // 4 bytes for linking count

    int ReferenceCount : 5;  // 4 bytes for reference count

    int permission : 5;      // 4 bytes for read write 

    struct inode *next ;  // 8 bytes for self referential pointer which refer to next struct inode 

} INODE , *PINODE , **PPINODE;


/*
################################################000####################
##               Creating  structure of filetable                    ##
##                                                                   ##
#######################################################################
*/

# pragma pack(1) 
typedef struct filetbale  // size of the superblock structure is 24 bytes
{

    int readoffset ;    // 4 bytes for it , from where to read 
    int writeoffset ;   // 4 bytes from where to write
    int count : 5 ;         // 4 bytes remain 1 Through out the program
    int mode :5 ;          // 4 bytes mode of file 1 , 2 , 3

    PINODE ptrinode;  //8 bytes // here file table point to inode 

}FILETABLE , *PFILETABLE;

/*
################################################000####################
##               Creating  structure of UFDT                         ##
##                                                                   ##
#######################################################################
*/

typedef struct ufdt
{

    PFILETABLE ptrfiletable;  // Create UFDT structure // pointer which point to Filetable  

} UFDT;

/*
################################################000####################
##               Creating  Globale Varilas                           ##
##                                                                   ##
#######################################################################

*/

UFDT UFDTArr[MAXINODE];			// Create Array Of Structure i.e Array Of Pointer// user area 

SUPERBLOCK SUPERBLOCKobj;		// global variable

PINODE head = NULL;				// global pointer 

/*
######################################################################
##               Definations of all Functions                       ##
##                                                                  ##
######################################################################

*/


void InitialiseSuperBlock(); // use to initialize the supper block which mentain how many nodes are free and used

PINODE Get_Inode(char*);   // here we get node

void CreateDILB();         // here we create link list of inode which in format of singly link list

/*
This if condtions for count =1 ;
which conatins -> ls_file() ,CloseAllFile(), system("cls") , DisplayHelp() , backup()
*/

/*
The ls is the list command in Linux. It will show the full list or content of your directory.
Just type ls and press the enter key. The whole content will be shown.
*/
void ls_file();  

/*

*/
void CloseAllFile();

void DisplayHelp();

void backup();

 /*
This if condtions for count =2 ;
which conatins -> stat_file(command[1]) , fstat_file(atoi(command[1])), 
                CloseFileByName(command[1]) , rm_File(command[1]) , man(command[1]),
                GetFDFromName(command[1]) , WriteFile(fd,arr,ret) truncate_File(command[1])
*/

int stat_file( char * );

int fstat_file( int );

int CloseFileByName(char *);

int rm_File( char * );

void man(char *);

int GetFDFromName(char *);

int WriteFile(int , char * , int  );

int truncate_File( char * );


/*
This if condtions for count =3 ;
which conatins  ->  CreateFile(command[1], atoi(command[2]));  OpenFile(command[1], atoi(command[2])); 
                    GetFDFromName(command[1]);  ReadFile(fd, ptr, atoi(command[2])) ,
*/

int CreateFile( char* , int  );

int OpenFile( char* , int );

int GetFDFromName(char *);

int ReadFile( int , char * , int  );

/*
This if condtions for count =4 ;
which conatins  -> int GetFDFromName(char *);int LseekFile( int , int , int );

*/
int GetFDFromName(char *);
int LseekFile( int , int , int );



/*
######################################################################
##          Definations and Initilaization of SUPERBLOCK            ##
##                                                                  ##
######################################################################
*/

void InitialiseSuperBlock()  // C
{
    int i=0;

    while( i < MAXINODE)                   // this loop is used to set all the pointers at null
    {
        UFDTArr[i].ptrfiletable = NULL ; 
		i++;
    }

    SUPERBLOCKobj.TotalInodes = MAXINODE ;
    SUPERBLOCKobj.FreeInode   = MAXINODE;
	
	printf("Supper block creation is Completed..................... \n\n");

}

/*
######################################################################
##          Definations and Initilaization of Get_Inode             ##
##                                                                  ##
######################################################################
*/

PINODE Get_Inode(char * name)  // C
{
	PINODE temp = head;
	int i = 0; 
	

	// filter 
	if(name == NULL)
	{
		return NULL; 
	}
	
	while(temp != NULL)
	{
		if(strcmp(name, temp -> FileName) == 0) // if used to check given file is exit of not 
		{
				break;
		}
		temp = temp -> next;
	}
	return temp; // return null bcz there is no such file exit in the DIL
}

/*
######################################################################
##          Definations and Initilaization of CreateDILB            ##
##                                                                  ##
######################################################################
*/
void CreateDILB()  // C  
{
    int i =0;
    PINODE newn = NULL;
    PINODE temp = head ;   // Insert Last Function From Linkedlist

    while(i <= MAXINODE)  // while loop eksobat 50 Inode tayar karun ghenar in lingly link list format
	{
        // Dynamic memeory alloction 
		newn = (PINODE)malloc(sizeof(INODE));  // 86 Bytes Of Memory Get Allocated
		
		    newn -> LinkCount = newn -> ReferenceCount = 0;

        	newn -> FileType  = newn -> FileSize       = 0; 

        	newn -> Buffer = NULL;  // Buffer doesnot contain any data inside in it 

        	newn -> next = NULL;   

        	newn -> InodeNumber = i;  // here assign of node number
		
		if(temp == NULL)
		{
			head = newn;
			temp = head;
		}
		else
		{
			temp -> next = newn;

			temp = temp -> next;
		}

		i++;
	}

	printf("DILB created successfully............\n");  // execute after creating 50 Inode inside 

}

/*
################################################000####################
##              Ls_File( )                                           ##
##                                                                   ##
#######################################################################

*/

void ls_file() // C
{
	int i = 0;

	PINODE temp = head; // temp node for traversal 

	if(SUPERBLOCKobj.FreeInode == MAXINODE) // if condtion check where supperblock have any free inode or not 
	{
		printf("Error : There are no files\n");
		return;
	} 

	printf("\nFile Name\tInode number\tFile size\tLink count\tRemaingFileSize\n");
	printf("--------------------------------------------------------------------------------------------------------------\n");

	while(temp != NULL)
	{
		if(temp -> FileType != 0) // file type will be regular 1 or special 2
		{
			printf("%s\t\t%d\t\t%d\t\t%d\t\t%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount,temp->FileSize- temp->FileActualSize );
		}
	    temp = temp -> next;
	}
	printf("--------------------------------------------------------------------------------------------------------------\n"); 

}

/*
################################################000####################
##              CloseAllFile()                                       ##
##                                                                   ##
#######################################################################

*/

void CloseAllFile() // C
{
	int i = 0;

	while(i < MAXINODE)  // while( 0 < 50)
	{
		if(UFDTArr[i].ptrfiletable != NULL)
		{
			UFDTArr[i].ptrfiletable -> readoffset = 0;

			UFDTArr[i].ptrfiletable -> writeoffset = 0;

            (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount) = 0;
        }

		i++;
	} 
    	printf("All Files Are Closed Successfully\n");

}

/*
################################################000####################
##              DisplayHelp()                                        ##
##                                                                   ##
#######################################################################

*/

void DisplayHelp() // C
{
    printf("----------------------------------------------------------------");
	printf("ls          :   To List out all files\n"); 
    printf("clear       :   To clear console\n"); 
    printf("open        :   To open the file\n"); 
    printf("close       :   To close the file\n"); 
    printf("closeall    :   To close all opened files\n"); 
    printf("read        :   To Read the contents from file\n"); 
    printf("write       :   To Write contents into file\n"); 
    printf("exit        :   To Terminate file system\n"); 
    printf("stat        :   To Display information of file using name\n"); 
    printf("fstat       :   To Display information of file using file descriptor\n"); 
    printf("truncate    :   To Remove all data from file\n"); 
    printf("rm          :   To Delet the file\n");
    printf("backup      :   To Take Backup Of All Created Files\n");
	printf("----------------------------------------------------------------");
}

/*
################################################000####################
##              backup()                                             ##
##                                                                   ##
#######################################################################

*/

void backup()  // C // our project is Virtual File system which work only on Ram but for storing data which are created by we its need to store n hard disk
{
	PINODE temp = head;

		int fd = 0; // File discriptor

		while(temp != NULL)
		{
			if(temp -> FileType != 0)
			{
				fd = creat(temp -> FileName, 0777);
				write(fd, temp -> Buffer, temp -> FileActualSize);
			}
			temp = temp -> next;
		}
		printf("Successfully Get The Backup Of All Created Files...\n");
}



/*
################################################000####################
##              stat_file( char * )                                  ##
##                                                                   ##
#######################################################################
*/
int stat_file( char * name ) // C
{
	PINODE temp = head;

	int i = 0; 

	if(name == NULL) 
    	{
        	return -1;
    	}
	
	while(temp != NULL)
	{
		if(strcmp(name, temp -> FileName) == 0)
		{
				break;
		}

		temp = temp -> next;
	} 

	if(temp == NULL) 
	{
		return -2; 
	}

	printf("\n---------Statistical Information about file-----------\n");
	
    printf("File name         :%s\n", temp -> FileName);
	printf("Inode Number      :%d\n", temp -> InodeNumber);
	printf("File size         :%d\n", temp -> FileSize);
	printf("Actual File size  :%d\n", temp -> FileActualSize);
	printf("Link count        :%d\n", temp -> LinkCount);
	printf("Reference count   :%d\n", temp -> ReferenceCount); 

	if(temp -> permission == 1)
	{
		printf("File Permission : Read only\n");
	}
		
	else if(temp -> permission == 2)
	{
		printf("File Permission : Write\n");
	}
		
	else if(temp -> permission == 3)
	{
		printf("File Permission : Read & Write\n");
	}
		
	printf(" ------------------------------------------------------\n\n"); 
	
	return 0; 
}

/*
################################################000####################
##             fstat_file( int )                                     ##
##                                                                   ##
#######################################################################
*/

int fstat_file( int fd) //C
{
    PINODE temp = head; 

	int i = 0; 

	if(fd < 0)	
	{
		return -1;
    }
 
	if(UFDTArr[fd].ptrfiletable == NULL)
	{
		return -2;
	}	
		 
	temp = UFDTArr[fd].ptrfiletable -> ptrinode;

	printf("\n---------Statistical Information about file---------\n");
	
   	printf("File name           :%s\n", temp -> FileName);
	printf("Inode Number        :%d\n", temp -> InodeNumber);
	printf("File size           :%d\n", temp -> FileSize);
	printf("Actual File size    :%d\n", temp -> FileActualSize);
	printf("Link count          :%d\n", temp -> LinkCount);
	printf("Reference count     :%d\n", temp -> ReferenceCount); 
	
	if(temp -> permission == 1)
    	{
        	printf("File Permission : Read only\n");
    	}
	
	else if(temp -> permission == 2)
    	{
        	printf("File Permission : Write\n");
    	}
		
	else if(temp -> permission == 3)
    	{
        	printf("File Permission : Read & Write\n");
    	}
		
	printf("-----------------------------------------------------\n\n");
	return 0; 
}
/*
################################################000####################
##             CloseFileByName(char *)                               ##
##                                                                   ##
#######################################################################
*/

int CloseFileByName(char * name) // C
{
	int i = 0;

	i = GetFDFromName(name);

	if(i == -1)
    	{
        	return printf("All Open Files Are Closed......................\n"); 
    	}

    	if((UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount) == 0)
    	{
        	return -2;
    	}
	
	UFDTArr[i].ptrfiletable -> readoffset = 0;

	UFDTArr[i].ptrfiletable -> writeoffset = 0;

	(UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount) = 0; 

	printf("File Closed Succesfully........................................\n");

	return 0; 
}
/*
################################################000####################
##             rm_File( char * )                                     ##
##                                                                   ##
#######################################################################
*/

int rm_File( char * name ) //C
{
	int fd = 0; 

	fd = GetFDFromName(name);

	if(fd == -1)
	{
		return -1;
	} 
	
	(UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount)--;

	if(UFDTArr[fd].ptrfiletable -> ptrinode -> LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable -> ptrinode -> FileType = 0;

		strcpy(UFDTArr[fd].ptrfiletable -> ptrinode -> FileName," ");

		UFDTArr[fd].ptrfiletable -> ptrinode-> ReferenceCount=0;

		UFDTArr[fd].ptrfiletable -> ptrinode-> permission=0;

		UFDTArr[fd].ptrfiletable -> ptrinode-> FileActualSize=0;

		free(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer);

		free(UFDTArr[fd].ptrfiletable);
	} 
 	
	UFDTArr[fd].ptrfiletable = NULL;

	(SUPERBLOCKobj.FreeInode)++; 
	
	printf("File Successfully Deleted................\n");
}
/*
################################################000####################
##             man(char *)                                           ##
##                                                                   ##
#######################################################################
*/

void man(char * name) //C
{
	if(name == NULL) 
    {
        return;
    }

    if(strcmp(name, "create") == 0) 
    {
        printf("Description : Used To Create New Regular File\n");
        printf("Usage       : create File_Name, Permission\n");
    }

    else if(strcmp(name, "read") == 0) 
    {
        printf("Description : Used to read data from regular file\n");
        printf("Usage       : read File_name, No_Of_Bytes_To_Read\n");
    }

    else if(strcmp(name, "write") == 0) 
    {
        printf("Description : Used to write data into regular file\n"); 
        printf("Usage       : write File_name\n After this enter the data that we want to write\n");
    }

    else if(strcmp(name, "ls") == 0) 
    {
        printf("Description : Used to list all information of files\n"); 
        printf("Usage       : ls\n");
    }

    else if(strcmp(name, "stat") == 0) 
    {
        printf("Description : Used to display information of file\n"); 
        printf("Usage       : stat File_name\n");
    }

    else if(strcmp(name, "fstat") == 0) 
    {
        printf("Description : Used to display information of file\n"); 
        printf("Usage       : stat File_Descriptor\n");
    }

    else if(strcmp(name, "truncate") == 0) 
    {
        printf("Description  : Used to remove data from file\n"); 
        printf("Usage        : truncate File_name\n");
    }

    else if(strcmp(name, "open") == 0) 
    {
        printf("Description : Used to open existing file\n"); 
        printf("Usage       : open File_name mode\n");
    }

    else if(strcmp(name, "close") == 0)
    {
        printf("Description : Used to close opened file\n"); 
        printf("Usage       : close File_name\n"); 
    }

    else if(strcmp(name, "closeall") == 0) 
    {
        printf("Description : Used to close all opened file\n"); 
        printf("Usage       : closeall\n");
    }

    else if(strcmp(name, "lseek") == 0) 
    {
        printf("Description : Used to change file offset\n"); 
        printf("Usage       : lseek File_Name ChangeInOffset StartPoint\n");
    }

    else if(strcmp(name, "rm") == 0) 
    {
        printf("Description : Used to delete the file\n"); 
        printf("Usage       : rm File_Name\n");
    }

    else if(strcmp(name,"backup") == 0)
    {
        printf("Description : Used To Take Backup Of All Files Created\n");
        printf("Usage       : backup\n");
    }

    else
    {
        printf("ERROR : No manual entry available.\n"); 
    }
}



/*
################################################000####################
##              WriteFile(int , char * , int  )                      ##
##                                                                   ##
#######################################################################
*/

int WriteFile(int fd , char * arr , int isize  )  // C
{
	if(((UFDTArr[fd].ptrfiletable -> mode) != WRITE) && ((UFDTArr[fd].ptrfiletable -> mode) != READ + WRITE)) 
	{
        return -1; 
    }

	if(((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != WRITE) && ((UFDTArr[fd].ptrfiletable -> ptrinode -> permission) != READ + WRITE))	
	{
        return -1; 
    }

	if((UFDTArr[fd].ptrfiletable -> writeoffset) == MAXFILESIZE) 
	{
        return -2; 
    } 
	
	if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileType) != REGULAR)	
	{
        return -3; 
    }

	if(((UFDTArr[fd].ptrfiletable -> ptrinode -> FileSize) - (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)) < isize)
	{
		return -4;
	}

	strncpy((UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> writeoffset), arr, isize); 
	
	(UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset ) + isize; 
	
	(UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + isize;

	return isize; 
}
/*
################################################000####################
##              truncate_File( char * )                              ##
##                                                                   ##
#######################################################################
*/

int truncate_File( char * name )  // C
{
	int fd = GetFDFromName(name);

	if(fd == -1)
	{
		return -1;
	}

	memset(UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer, 0, MAXFILESIZE);

	UFDTArr[fd].ptrfiletable -> readoffset = 0;

	UFDTArr[fd].ptrfiletable -> writeoffset = 0;

	UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize = 0; 

	printf("Data Succesfully Removed................................................\n");
}
/*
################################################000####################
##            CreateFile( char* name , int permission  )             ##
##                                                                   ##
#######################################################################
*/
// int CreateFile( char* name , int permission  ) 
// char * name = name of file 
// int permission  = permission for file operations  1 for read , 2 for write , 3 combination for both read and write 
int CreateFile( char* name , int permission  )  //  9C
{
	int i = 0;

	// temp used to traveler node 
	PINODE temp = head; 


	// filter used for wrong input 
	if((name == NULL) || (permission == 0) || (permission > 3)) // permission 3 means (Read and write both)
	{
		return -1; 
	}

	// filter used for availabilty of Inode
	if(SUPERBLOCKobj.FreeInode == 0) // if There is no Free Inode in supperblock  
	{
		return -2; 
	}	 
	
	(SUPERBLOCKobj.FreeInode)--;  // here is used to reduce count of free node when we use Inode

	// filter used to check file is exit or not 
	if(Get_Inode(name) != NULL)  // Here we call Get_Inode where we passed name to check whether file is exit or not 
	{
		return -3; 
	}

	while(temp != NULL)  // 
	{
		if(temp -> FileType == 0) // 
		{
				break;
		}

		temp = temp -> next;
	} 

	while(i < MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		{
				break;
		}

		i++;
	} 

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE)); 

	if(UFDTArr[i].ptrfiletable == NULL)
	{
		return -4;
	}

	UFDTArr[i].ptrfiletable -> count       = 1;
	UFDTArr[i].ptrfiletable -> mode        = permission; // 3
	UFDTArr[i].ptrfiletable -> readoffset  = 0;
	UFDTArr[i].ptrfiletable -> writeoffset = 0; 
	
	UFDTArr[i].ptrfiletable -> ptrinode    = temp; 
	

	// strcpy(kashyat copy karaych , kay karaych );
	strcpy(UFDTArr[i].ptrfiletable -> ptrinode -> FileName, name);

	UFDTArr[i].ptrfiletable -> ptrinode -> FileType       = REGULAR; // 1

	UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount = 1;
	UFDTArr[i].ptrfiletable -> ptrinode -> LinkCount      = 1;

	UFDTArr[i].ptrfiletable -> ptrinode -> FileSize       = MAXFILESIZE;

	UFDTArr[i].ptrfiletable -> ptrinode -> FileActualSize = 0; // just file is created 

	UFDTArr[i].ptrfiletable -> ptrinode -> permission     = permission; // 3 both read and write 

	// Buffer is sued to stored the data in the created file 
	UFDTArr[i].ptrfiletable -> ptrinode -> Buffer         = (char *)malloc(MAXFILESIZE);   
	
	/*
	void *memset(void *dest, int c, size_t count);
	Description

	The memset() function sets the first count bytes of dest to the value c. 
	The value of c is converted to an unsigned character.

	Return Value

    The memset() function returns a pointer to dest.
	*/
    memset(UFDTArr[i].ptrfiletable -> ptrinode -> Buffer, 0, 1024);

	return i; 
}
/*
################################################000####################
##            OpenFile( char* name  , int mode )                     ##
##                                                                   ##
#######################################################################
*/

int OpenFile( char* name  , int mode )  // C
{
	int i = 0;
	PINODE temp = NULL; 

	if(name == NULL || mode <= 0)
	{
		return -1; 
	}

	temp = Get_Inode(name);
	
    if(temp == NULL)
	{
        return -2; 
    } 

	if(temp -> permission < mode) // ( 0 , 1 , 2 < 3)
	{
        	return -3; 
    	}
 
	while(i < MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		{
				break;
		}

		i++;
	} 

	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));

	if(UFDTArr[i].ptrfiletable == NULL)	
	{
        return -1; 
    }
	
	UFDTArr[i].ptrfiletable -> count = 1;
	UFDTArr[i].ptrfiletable -> mode = mode;
	
	if(mode == READ + WRITE)
	{
		UFDTArr[i].ptrfiletable -> readoffset = 0;
		UFDTArr[i].ptrfiletable -> writeoffset = 0;
	}

	else if(mode == READ) // 1
	{
		UFDTArr[i].ptrfiletable -> readoffset = 0;
	}

	else if(mode == WRITE) //2
	{
		UFDTArr[i].ptrfiletable -> writeoffset = 0;
	}
	
	 UFDTArr[i].ptrfiletable -> ptrinode = temp;

	 (UFDTArr[i].ptrfiletable -> ptrinode -> ReferenceCount)++; 
	
	return i; 

    printf("File Opened Successfully\n");

}
/*
################################################000####################
##              GetFDFromName(char * name)                           ##
##                                                                   ##
#######################################################################
*/
int GetFDFromName(char * name)  //C
{
	int i = 0;
	while(i < MAXINODE)
	{
		if(UFDTArr[i].ptrfiletable != NULL)
		{
			if(strcmp((UFDTArr[i].ptrfiletable -> ptrinode -> FileName), name) == 0)
			{
				break;
			}
        }

		i++;
	} 

	if(i == MAXINODE)	
	{
		return -1;
	}
	else	
	{
		return i;
	}
}
/*
################################################000####################
##              ReadFile( int fd , char * arr , int isize )          ##
##                                                                   ##
#######################################################################
*/
int ReadFile( int fd , char * arr , int isize )  // C
{  
	int read_size = 0; 

	if(UFDTArr[fd].ptrfiletable == NULL)
	{
		return -1; 
	}	 

	if(UFDTArr[fd].ptrfiletable -> mode != READ && UFDTArr[fd].ptrfiletable -> mode != READ + WRITE)
	{
		return -2; 
	} 

	if(UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ && UFDTArr[fd].ptrfiletable -> ptrinode -> permission != READ + WRITE)
	{
		return -2; 
	}  

	if(UFDTArr[fd].ptrfiletable -> readoffset == UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) 
    {
		return -3; 
	}	

	if(UFDTArr[fd].ptrfiletable -> ptrinode -> FileType != REGULAR)	
	{
		return -4;         
	} 

	if(UFDTArr[fd].ptrfiletable -> ptrinode -> FileType != REGULAR)
	{
		return -5;
	}
	
	read_size = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) - (UFDTArr[fd].ptrfiletable -> readoffset);
	
    if(read_size < isize)
	{
		strncpy(arr, (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), read_size); 
		UFDTArr[fd].ptrfiletable -> readoffset = UFDTArr[fd].ptrfiletable -> readoffset + read_size;
	}

	else
	{
		strncpy(arr, (UFDTArr[fd].ptrfiletable -> ptrinode -> Buffer) + (UFDTArr[fd].ptrfiletable -> readoffset), isize); 
		(UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + isize;
	} 

	return isize; 
}
/*
################################################000####################
##              LseekFile(    )                                      ##
##                                                                   ##
#######################################################################
*/
int LseekFile( int fd , int size , int from)   // C
{
    if((fd<0) || (from > 2))
	{
		return -1; 
	}	
		
	if(UFDTArr[fd].ptrfiletable == NULL) 
	{
		return -1; 
	}

	if(UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount == 0)
	{
		return -2;
	}
	
	if((UFDTArr[fd].ptrfiletable -> mode == READ) || (UFDTArr[fd].ptrfiletable -> mode == READ+WRITE))
	{
		if(from == CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) > UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize)	
			{
                return -1; 
            }
	
			if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) < 0)
			{
                return -1; 
            }
		
			(UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> readoffset) + size;
		}

		else if(from == START)
		{
			if(size > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))	
			{
                return -1; 
            }
			
            if(size < 0) 
			{
                return -1; 
            }

			(UFDTArr[fd].ptrfiletable -> readoffset) = size;
		}

		else if(from == END)
		{
			if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size > MAXFILESIZE) 
			{
                return -1; 
            }

			if(((UFDTArr[fd].ptrfiletable -> readoffset) + size) < 0) 
			{
                return -1;
            }

			(UFDTArr[fd].ptrfiletable -> readoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
		}
	}

	else if(UFDTArr[fd].ptrfiletable -> mode == WRITE)
	{
		if(from == CURRENT)
		{
			if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > MAXFILESIZE)	
			{
                return -1;
            }	

			if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0)	
			{
                return -1; 
            }

			if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
			{
				(UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
				(UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> writeoffset) + size;
			}
		}	

		else if(from == START)	
		{	
			if(size > MAXFILESIZE)
			{
				return -1;
			}	
				
			if(size < 0)	
			{
                return -1;
            }

			if(size > (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize))
			{
				(UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) = size;
		        (UFDTArr[fd].ptrfiletable -> writeoffset) = size;
			}
		}

		else if(from == END)
		{
			if((UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size > MAXFILESIZE)
			{
				return -1;
            }

			if(((UFDTArr[fd].ptrfiletable -> writeoffset) + size) < 0) 
			{
				return -1;
            }

			(UFDTArr[fd].ptrfiletable -> writeoffset) = (UFDTArr[fd].ptrfiletable -> ptrinode -> FileActualSize) + size;
		}
	} 
    	printf("Successfully Changed\n");
}


/*
################################################000####################
##              Main Function                                        ##
##                                                                   ##
#######################################################################
*/
int main()
{
    printf("Size of inode is : %d\n",sizeof(INODE));  // 20
    printf("Size of Super Block is : %d\n",sizeof(SUPERBLOCK));  // 20
    printf("Size Fletbale : %d\n",sizeof(filetbale));   // 21
    printf("Size ufdt : %d\n",sizeof(ufdt));   // 21


    char *ptr = NULL;
    int  ret=0 ;
    int  fd=0;     // file discriptor 
    int  count =0;
    char command[4][80] ;
    char str[80];
    char arr[MAXFILESIZE];

    /*
     Step 1
     second compartment of os (Supper block is block which contains infromations about inodes and blocks)
    */
    
      InitialiseSuperBlock(); 
    
    /*
       Step 2
	   Hard disk varil 3rd block disk inode list block which in format of singly link list
    */
	
    CreateDILB();

    /*
       Step 3
	   infine while loop will termitnate after we intenstionally ->Infifnit listening loop
    */

    while(1) 
	{
		fflush(stdin);       // used to stdin keyborad flush keborad buffer

		strcpy(str, "");    // strcpy -> inbuild string function 
		
		printf("\nCustomised VFS : > "); 

        // files gets ( str madhe 80 ana from keyboard)

		fgets(str, 80, stdin);     // scanf("%[^'\n']s",str);special scanf function 


        // sscanf() -> use for tokenization , 
		// count used to catch number of word which is given by programmer  
		// maximum token generated by code is 4
		// i am Nayan  sscanf braek in to 3 different tokens and count will be 3

		count = sscanf(str,"%s %s %s %s", command[0], command[1], command[2], command[3]); 
        
        /*
        This if condtions for count =1 ;

        which conatins ls_file() ,CloseAllFile(), system("cls") , DisplayHelp() , backup()

        */
        if(count == 1)          // when count is 1 means , programmer type single word                                
		{
            // Nested if conditions 
			// iner if used to comapere the command which having count 1 
			//command[0] = ls 
			// if(strcmp(ls, "ls") == 0) // ls ls are equal 

			// (strcmp(command[0], "ls") which is case sensitive 
			// (stricmp(command[0], "ls") which is case inSensitive 
			if(strcmp(command[0], "ls") == 0) // command is ls
			{
				ls_file(); // at 275
			}

			else if(strcmp(command[0], "closeall") == 0) // command is closeall
			{
				CloseAllFile(); 
				continue;
			}

			else if(strcmp(command[0], "clear") == 0) // command is clear
			{
				system("cls"); // at cls is used to clear the screen 
				continue;
			}

			else if(strcmp(command[0], "help") == 0) // command is help
			{
				DisplayHelp(); // at 237
				continue;
			}

            else if(strcmp(command[0], "backup") == 0) // command is backup
            {
                backup(); // at 1137
                continue;
            }

			else if(strcmp(command[0], "exit") == 0) // command is exit
			{
				printf("Terminating the Customised Virtual File System\n");
				break; // loop brake
			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}

        /*
        This if condtions for count =2 ;

        which conatins  stat_file(command[1]) , fstat_file(atoi(command[1])), 

        CloseFileByName(command[1]) , rm_File(command[1]) , man(command[1]),
        
        GetFDFromName(command[1]) , WriteFile(fd,arr,ret) truncate_File(command[1])



        */

        else if(count == 2)  // enter in this if conditions when word count is 2
		{
			if(strcmp(command[0], "stat") == 0) // strcmp used to comapre the string which is inbulid function 
			{
				ret = stat_file(command[1]);  // At 1041
				if(ret == -1)
                		{
                   	 		printf("ERROR : Incorrect parameters\n");
                		}
					
				if(ret == -2)
                		{
                    			printf("ERROR : There is no such file\n");
                		}
                		continue;
			}

			else if(strcmp(command[0], "fstat") == 0) //fstat 0
			{
				ret = fstat_file(atoi(command[1])); // At 981
				if(ret == -1)
                		{
                   	 		printf("ERROR : Incorrect parameters\n");
                		}
					
				if(ret == -2)
                		{
                    			printf("ERROR : There is no such file\n");
                		}
                		continue;
			}

			else if(strcmp(command[0], "close") == 0)
			{	
				ret = CloseFileByName(command[1]); // At 743

				if(ret == -1)
                		{
                    			printf("ERROR : There is no such file\n");
                		}

                		if (ret == -2)
                		{
                    			printf("The File Is Already Closed\n");
                		}
                		continue;
			}

			else if(strcmp(command[0], "rm") == 0) 
			{
				ret = rm_File(command[1]); // At 490

				if(ret == -1)
                		{
                    			printf("ERROR : There is no such file\n");
					continue;
                		}
			}

			else if(strcmp(command[0], "man") == 0)
			{
				man(command[1]); // At 133
			}

			else if(strcmp(command[0], "write") == 0)
			{
				fd = GetFDFromName(command[1]); // At 267
				
                		if(fd == -1)
				{
					printf("Error : Incorrect parameter\n");
					continue;
				}

                if(UFDTArr[fd].ptrfiletable -> ptrinode -> ReferenceCount == 0)
                {
                    printf("ERROR : File is Not Opened\n");
                }
                
                else
                {
                    printf("Enter the data : \n");
				    scanf("%[^\n]", arr); 
                }
				//fflush(stdin); // empty input buffer
	
				ret = strlen(arr); // AT

				if(ret == 0)
				{
					printf("Error : Incorrect parameter\n");
					continue;
				}

				ret = WriteFile(fd,arr,ret); // 0, Adress, 4 // AT

				if(ret == -1)
                		{
                    			printf("ERROR : Permission denied\n");
                		}
					
				if(ret == -2)
                		{
                    			printf("ERROR : There is no sufficient memory to write\n");
                		}
					
				if(ret == -3)
                		{
                    			printf("ERROR : It is not regular file\n");
                		}

				if(ret == -4)
				{
					printf("ERROR:There is no sufficient memory Available\n");
				}

                		if(ret > 0)
				{
					printf("Sucessfully : %d bytes written\n", ret);
				}
			}

			else if(strcmp(command[0], "truncate") == 0)
			{
				ret = truncate_File(command[1]);  // At 1108
				

				if(ret == -1)
                		{
                    			printf("Error : Incorrect parameter\n");
                		}
			}	

			else	
			{	
				printf("\nERROR : Command not found !!!\n");
			}	 
            		continue;  // used to normal flow of the program and 
		}

        /*
        This if condtions for count =3 ;

        which conatins  ->  CreateFile(command[1], atoi(command[2]));  OpenFile(command[1], atoi(command[2])); 
                            GetFDFromName(command[1]);  ReadFile(fd, ptr, atoi(command[2])) ,
        */
    

        else if(count == 3)  // command ->here 3 token used  
		{
			//create Demo.txt 3
			// command[0]= create , command[1]=Demo.txt , command[2]= 3 
			// if(strcmp(command[0]= create , "create") == 0) true 

			if(strcmp(command[0], "create") == 0)
			{
				// ret = CreateFile(create , 3)); // atoi() — Convert Character String to Integer
				/*
				The atoi() function returns an int value that is produced by interpreting the input characters as a number. 
				The return value is 0 if the function cannot convert the input to a value of that type.
				The return value is undefined in the case of an overflow.
				*/

				ret = CreateFile(command[1], atoi(command[2])); // ASCII to Integer
		
				if(ret >= 0)
			    {
					printf("File is successfully created with file descriptor : %d\n", ret);
				}
					
				if(ret == -1)
				{
						printf("ERROR : Incorrect parameters\n");
				}
					
				if(ret == -2)
                		{
                    			printf("ERROR : There is no inodes\n");
                		}
					
				if(ret == -3)
				{
						printf("ERROR : File already exists\n");
				}
					
				if(ret == -4)
				{
						printf("ERROR : Memory allocation failure\n");
				}
				continue;
			}

			else if(strcmp(command[0], "open") == 0)
			{
				ret = OpenFile(command[1], atoi(command[2]));  // AT
		
				if(ret >= 0)
                		{
                    			printf("File is successfully opened with file descriptor : %d\n", ret);
                		}
					
				if(ret == -1)
                		{
                    			printf("ERROR : Incorrect parameters\n");
                		}
					
				if(ret == -2)
                		{
                   	 		printf("ERROR : File not present\n");
                		}
					
				if(ret == -3)
                		{
                    			printf("ERROR : Permission denied\n");
                		}
                		continue;
			}

			else if(strcmp(command[0], "read") == 0)
			{
				fd = GetFDFromName(command[1]);  // At

				if(fd == -1)
				{
					printf("Error : Incorrect parameter\n");
					continue;
				}

				ptr = (char *)malloc(sizeof(atoi(command[2])) + 1);

				if(ptr == NULL)
				{
					printf("Error : Memory allocation failure\n");
					continue;
				}

				ret = ReadFile(fd, ptr, atoi(command[2]));  // At
				
                if(ret == -1)
                {
                    printf("ERROR : File not existing\n");
                }
					
				if(ret == -2)
                		{
                    			printf("ERROR : Permission denied\n");
                		}
					
				if(ret == -3)
                		{
                    			printf("ERROR : Reached at end of file\n");
                		}
					
				if(ret == -4)
                		{
                    			printf("ERROR : It is not regular file\n");
                		}	

                		if (ret == -5)
                		{
                    			printf("ERROR : File is not opened\n");
                		}
					
				if(ret == 0)
                		{
                    			printf("ERROR : File empty\n");
                		}
					
				if(ret > 0)
				{
					write(2, ptr, ret);  // 0 for stdin, 1 for stdout  // At
				}
				continue;
			}

			else	
			{	
				printf("\nERROR : Command not found !!!\n");
				continue;
			}	
		}

    /*
        This if condtions for count =4;

        which conatins  ->   GetFDFromName(command[1]); LseekFile(fd, atoi(command[2]), atoi(command[3]))
                            
        */

	else if(count == 4)
		{
			if(strcmp(command[0], "lseek") == 0)
			{
				fd = GetFDFromName(command[1]);  // AT

				if(fd == -1)
				{
					printf("Error : Incorrect parameter\n");
					continue;
				}
				
				ret = LseekFile(fd, atoi(command[2]), atoi(command[3])); // At 807
				
				if(ret == -1)
				{
					printf("ERROR : Unable to perform lseek\n");
				}

				if (ret == -2)
				{
						printf("ERROR : File is not opened\n");
				}
			}

			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}

		else
		{
			printf("\nERROR : Command not found !!!\n");
			continue;
		}

	}	// end of while loop


   return 0;
}
