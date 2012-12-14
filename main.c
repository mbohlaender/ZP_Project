/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *HEADER =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"> <html>\n"
"<head>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> <title>Contact_list</title>\n"
"</head>\n"
"<body>\n"
"<table width=\"700\">\n"
"<tr><th width=\"99\">Name</th><th width=\"99\">Surname</th><th width=\"99\">Company</th><th width=\"99\">Mobile</th><th width=\"99\">Email</th><th width=\"99\">Date of birth</th><th width=\"99\">Picture</th></tr>\n";

const char *FOOTER =
"</table>\n"
"</body>\n"
"</html>\n";

size_t MAXUID = 0;
char DIR[1024];

//return value for params
enum retval{
    ADD=2,
    DEL,
    FIND,
    LIST
};

typedef enum month{
    JAN =1,
    FEB,
    MAR,
    APR,
    MAY,
    JUN,
    JUL,
    AUG,
    SEP,
    OCT,
    NOV,
    DEC
}TMonth;

typedef struct date{
    int day;
    TMonth month;
    int year;
}TDate;

//TContact is structure for a cingle contact, part of a dualy linked list TContact_list
typedef struct contact{
    size_t UID;
    double mobile;
    TDate dob;
    char name[30];
    char surname[30];
    char company[30];
    char email[50];
    char image[50];
    struct contact *next;
    struct contact *prev;
}TContact;

//TContact_list is a geader for dualy linked list of contacts
typedef struct contact_list{
    struct contact *first;
    struct contact *last;
}TContact_list;

//handle_errors is a function for dealing with errno
int handle_errors(void){
    switch(errno){
        case 0:
            return EXIT_SUCCESS;
            break;
        default:
            perror("Error");
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//get current directory
int get_dir(){
    getcwd(DIR, sizeof(DIR));
    return EXIT_SUCCESS;
}

//swap two contacts in contact_list
int swap_contacts(TContact_list *contact_list, TContact *contact1, TContact *contact2){
    if(contact1 == contact2){
        errno = EINVAL;
        return EXIT_FAILURE;
    }
    
    if(contact1 == contact_list->first)
        contact_list->first = contact2;
    if(contact2 == contact_list->last)
        contact_list->last = contact1;
    
    TContact *tmp = NULL;
    
    tmp = contact1->next;
    
    contact1->next = contact2->next;
    contact2->next = tmp;
    if(contact1->next != NULL){
        contact1->next->prev = contact1;
    }
    if(contact2->next != NULL){
        contact2->next->prev = contact2;
    }
    tmp = contact1->prev;
    contact1->prev = contact2->prev;
    contact2->prev = tmp;
    if(contact1->prev != NULL){
        contact1->prev->next = contact1;
    }
    if(contact2->prev != NULL){
        contact2->prev->next = contact2;
    }  
    return EXIT_SUCCESS;
}

//insertlast parses the string and creates new contact in contact_list
int insertlast(TContact_list *contact_list, char *line){
    char *pch = NULL;
    TContact *newcontact = (TContact *)calloc(1,sizeof(TContact));
    if(newcontact == NULL){
        errno = ENOMEM;
        return EXIT_FAILURE;
    }
    pch = strtok(line, ",");
    newcontact->UID = strtoul(pch, NULL, 10);
    if(newcontact->UID > MAXUID)
        MAXUID = newcontact->UID;
    pch = strtok (NULL, ",");
    strcpy(newcontact->name, pch);
    pch = strtok (NULL, ",");
    strcpy(newcontact->surname, pch);
    pch = strtok (NULL, ",");
    strcpy(newcontact->company, pch);
    pch = strtok(NULL, ",");
    newcontact->mobile = atol(pch);
    pch = strtok (NULL, ",");
    strcpy(newcontact->email, pch);
    pch = strtok (NULL, ",");
    sscanf(pch, "%d.%d.%d", &newcontact->dob.day, (int *)&newcontact->dob.month, &newcontact->dob.year);
    pch = strtok(NULL, ",");
    strcpy(newcontact->image, pch);
    
    if((pch = strtok(NULL, ",")) != NULL){
        free(newcontact);
        errno = ENFILE;
        return EXIT_FAILURE;
    }
    
    newcontact->next = NULL;
    
    if(contact_list->last == NULL){
        contact_list->first = newcontact;
        contact_list->last = newcontact;
        newcontact->prev = NULL;
    }
    else{
        contact_list->last->next = newcontact;
        newcontact->prev = contact_list->last;
        contact_list->last = newcontact;
    }
    return EXIT_SUCCESS;
}

//add_to_list inserts new contact from command line to contact_list
int add_to_list(const char *argv[], TContact_list *contact_list){
    TContact *newcontact = (TContact *)calloc(1,sizeof(TContact));
    if(newcontact == NULL){
        errno = ENOMEM;
        return EXIT_FAILURE;
    }
    
    newcontact->UID = ++MAXUID;
    strcpy(newcontact->name, argv[2]);
    strcpy(newcontact->surname, argv[3]);
    strcpy(newcontact->company, argv[4]);
    newcontact->mobile = atol(argv[5]);
    strcpy(newcontact->email, argv[6]);
    sscanf(argv[7], "%d.%d.%d", &newcontact->dob.day, (int *)&newcontact->dob.month, &newcontact->dob.year);
    strcpy(newcontact->image, "\"");
    strcat(newcontact->image, argv[8]);
    strcat(newcontact->image, "\"");
    newcontact->next = NULL;
    
    if(contact_list->last == NULL){
        contact_list->first = newcontact;
        contact_list->last = newcontact;
        newcontact->prev = NULL;
    }
    else{
        contact_list->last->next = newcontact;
        newcontact->prev = contact_list->last;
        contact_list->last = newcontact;
    }
    return EXIT_SUCCESS;
}

//removes contact with specified UID from list
int remove_from_list(TContact_list *contact_list, size_t UID){
    for(TContact *contact = contact_list->first; contact !=NULL; contact = contact->next){
        if(UID == contact->UID){
            if(contact == contact_list->first){
                contact_list->first = contact->next;
                contact->next->prev = NULL;
                free(contact);
            }
            else if(contact == contact_list->last){
                contact_list->last = contact->prev;
                contact->prev = NULL;
                free(contact);
            }
            else{
                contact->prev->next = contact->next;
                contact->next->prev = contact->prev;
                free(contact);
            }
        }
    }
    return EXIT_SUCCESS;
}

//function get_line reads *FILE and returns a pointer to a string (single line of undefined length)
int get_line(FILE *file, char **ip){
    char *line, *tmp;
    char ch = 1;
    int size = 1, index = 0;
    line = (char *)malloc(sizeof(char));
    
    while((ch != '\n')&&(!feof(file))){
        ch = getc(file);
        if((ch != '\n') && (ch != '\0')&&(!feof(file))){
            if(size-1 <=index){
                size +=5;
                tmp = (char *)realloc(line, sizeof(char)*size);
                if(tmp == NULL){
                    errno = ENOMEM;
                    return EXIT_FAILURE;
                }
                line = tmp;
                *ip = line;
            }
            line[index++] = ch;
        }
    }
    return EXIT_SUCCESS;
}

//function get_data reads a file containing the .csv database and creates a doubly linked list out of the records
int get_data(TContact_list *contact_list){
    FILE *file;
    char *line = NULL;
    char FILENAME[1024];
    
    strcpy(FILENAME, DIR);
    strcat(FILENAME, "/contacts.csv");
    
    file = fopen(FILENAME, "r");
    if(file == NULL){
        if(errno == 0)errno = ENOENT;
        return EXIT_FAILURE;
    }
    
    while (!feof(file)){
        line = NULL;
        if(get_line(file, &line) != EXIT_FAILURE){
            if(line !=NULL){
                insertlast(contact_list, line);
                free(line);
            }
        }
    }
    
    if(fclose(file) == EOF){
        if(errno == 0)errno = EBADF;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//get_mode checks command line for --add or --delete and returns apropriate return code, on error sets errno code
int get_mode(int argc, const char *argv[]){
    switch(argc){
        case 2:
            if(strcmp (argv[1], "--list") == 0)
                return LIST;
            else{
                errno = EINVAL;
                return EXIT_FAILURE;
            }
            break;
        case 3:
            if(strcmp (argv[1], "--delete") == 0)
                return DEL;
            if(strcmp (argv[1], "--find") == 0)
                return FIND;
            else{
                errno = EINVAL;
                return EXIT_FAILURE;
            }
            break;
        case 9:
            if(strcmp (argv[1], "--add") == 0)
                return ADD;
            else{
                errno = EINVAL;
                return EXIT_FAILURE;
            }
            break;
        default:
            errno = EINVAL;
            return EXIT_FAILURE;
    }
    
    errno = EINVAL;
    return EXIT_FAILURE;
}

//function print_list prints entire doubly-linked list
int print_list(TContact_list *contact_list){
    for(TContact *contact = contact_list->first; contact !=NULL; contact = contact->next){
        printf("Name:%s Surname:%s Company:%s Mobile:%.f Email:%s Date of birth:%d.%d.%d Picture:%s\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, contact->image);
    }
    return EXIT_SUCCESS;
}

//free_list deallocates memory from doubly-linked list contact_list
int free_list(TContact_list *contact_list){
    for(TContact *contact = contact_list->first; contact !=NULL; contact = contact->next){
        contact_list->first = contact->next;
        free(contact);
    }
    return EXIT_SUCCESS;
}

//generate_html generates a .html file from doubly-linked list contact_list
int generate_html(TContact_list *contact_list){
    FILE * file;
    char OUTHTML[1024];
    
    strcpy(OUTHTML, DIR);
    strcat(OUTHTML,"/contacts.html");
    
    file = fopen(OUTHTML, "w");
    if(file == NULL)
        return EXIT_FAILURE;
    
    fprintf(file,"%s", HEADER);
    for(TContact *contact = contact_list->first; contact !=NULL; contact = contact->next){
        fprintf(file,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%.f</td><td>%s</td><td>%d.%d.%d</td><td><img width=50 height=50 src=\"%s%s></td></tr>", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, DIR, contact->image+1);
    }
    fprintf(file,"%s", FOOTER);
    fclose(file);
    return EXIT_SUCCESS;
}

//save_csv saves the doubly-linked list to a .csv file
int save_csv(TContact_list *contact_list){
    FILE * csv;
    char FILENAMEV[1024];
    
    strcpy(FILENAMEV, DIR);
    strcat(FILENAMEV, "/contacts_copy.csv");
    
    csv = fopen(FILENAMEV, "w");
    for(TContact *contact = contact_list->first; contact !=NULL; contact = contact->next){
        fprintf(csv, "%zd,%s,%s,%s,%.f,%s,%d.%d.%d,%s\n",contact->UID, contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, contact->image);
    }
    fclose(csv);
    return EXIT_SUCCESS;
}

int main(int argc, const char *argv[])
{
    TContact_list contact_list = {contact_list.first = NULL, contact_list.last = NULL};
    get_dir();
    
    switch(get_mode(argc, argv)){
        case DEL:
            get_data(&contact_list);
            remove_from_list(&contact_list, atoi(argv[2]));
            //print_list(&contact_list);
            save_csv(&contact_list);
            free_list(&contact_list);
            break;
        case ADD:
            get_data(&contact_list);
            add_to_list(argv, &contact_list);
            //print_list(&contact_list);
            save_csv(&contact_list);
            //generate_html(&contact_list);
            free_list(&contact_list);
            break;
        case LIST:
            get_data(&contact_list);
            print_list(&contact_list);
            generate_html(&contact_list);
            free_list(&contact_list);
            break;
        default:
            handle_errors();
            return EXIT_FAILURE;
    }
    if(handle_errors() != EXIT_SUCCESS) return EXIT_FAILURE;
    else return EXIT_SUCCESS;
}