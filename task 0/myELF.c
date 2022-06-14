//Created by Roey & Amit

#include <stdio.h>
#include <unistd.h>
#include <elf.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


//global variable
int debug = 0;
int currFD=-1;
char* currentFilenameOpen=NULL;
void* map_start; /* will point to the start of the memory mapped file */
struct stat fd_stat; /* this is needed to  the size of the file */
Elf32_Ehdr *header; /* this will point to the header structure */

typedef struct {
    char *name;
    void (*fun)();
}fun_desc;
void toggleDebugMode();
void examineElf();
void quit ();
void stubs();
int displayMenu (fun_desc menu[]);

int main(int argc, char **argv){
    fun_desc menu[] = { { "Toggle Debug Mode", toggleDebugMode }, { "Examine ELF File", examineElf }, { "Print Section Names", stubs},
                        { "Print Symbols", stubs }, { "Quit", quit } ,{ NULL, NULL } };

    while (1) {
        int bounds = displayMenu(menu);
        int op;
        scanf("%d", &op);
        if (op >= 0 && op < bounds){
            fprintf(stdout, "Within bounds\n" );
            menu[op].fun();
        }
        else
            fprintf(stdout, "Not within bounds\n" );
        printf("\n");
    }


}

void toggleDebugMode(){
    if(debug){
        printf("Debug flag now off\n");
        debug=0;

    }
    else{
        printf("Debug flag now on\n");
        debug=1;
    }
}

int LoadFile(){
    char filename[100];
    int fd;
    fscanf(stdin,"%s",filename);
    if((fd = open(filename, O_RDWR)) < 0) {
        perror("error in open");
        exit(-1);
    }
    if(fstat(fd, &fd_stat) != 0 ) {
        perror("stat failed");
        exit(-1);
    }
    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, fd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        exit(-4);
    }
    if(currFD!=-1){close(currFD);}
    currFD=fd;
    strcpy((char*)&currentFilenameOpen,(char*)filename);
    return currFD;
}

int isELFfile(Elf32_Ehdr* header){
    if(strncmp((char*)header->e_ident,(char*)ELFMAG, 4)==0){
        return 1;
    }
    return 0;
}

char* dataEncSch(Elf32_Ehdr* header){
    switch (header->e_ident[5]){
        case ELFDATANONE:
            return "invalid data encoding";
            break;
        case ELFDATA2LSB:
            return "2's complement, little endian";
            break;
        case ELFDATA2MSB:
            return "2's complement, big endian";
            break;
        default:
            return "NO DATA";
            break;
    }
}

void examineElf(){
    printf("Enter file name: ");
    if(LoadFile()==-1){exit(EXIT_FAILURE);}
    header = (Elf32_Ehdr *) map_start;
    if(isELFfile(header)){
        printf("Magic:\t\t\t\t %X %X %X\n", header->e_ident[EI_MAG0],header->e_ident[EI_MAG1],header->e_ident[EI_MAG2]);
        printf("Data:\t\t\t\t %s\n",dataEncSch(header));
        printf("Enty point address:\t\t 0x%x\n",header->e_entry);
        printf("Start of section headers:\t %d (bytes into file)\n",header->e_shoff);
        printf("Number of section headers:\t %d\n",  header->e_shnum);
        printf("Size of section headers:\t %d (bytes)\n",header->e_shentsize);
        printf("Start of program headers:\t %d (bytes into file)\n",header->e_phoff);
        printf("Number of program headers:\t %d\n",header->e_phnum);
        printf("Size of program headers:\t %d (bytes)\n",header->e_phentsize);
    }
    else{
        printf("This is not ELF file\n");
        munmap(map_start, fd_stat.st_size);
        close(currFD);
        currFD=-1;
        currentFilenameOpen=NULL;
    }
}

void quit () {
    if (debug) { printf("quitting..\n");}
    exit(0);
}

void stubs(){
    printf("not implemented yet");
}

int displayMenu (fun_desc menu[]){
    fprintf(stdout, "Choose action:\n");
    int i=0 ;
    while(menu[i].name != NULL) {
        fprintf(stdout, "%d) %s\n", i, menu[i].name);
        i++;
    }
    fprintf(stdout, "Option: ");
    return i;
}








