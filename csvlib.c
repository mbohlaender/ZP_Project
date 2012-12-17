//
//  dlist.c.c
//  contact_list_cmd
//
//  Created by Salamander on 17.12.12.
//  Copyright (c) 2012 Salamander. All rights reserved.
//

#include "csvlib.h"

const char *SINGLE =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"> <html>\n"
"<head>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> <title>Contact</title>\n"
"</head>\n"
"<body>\n";

const char *HEADER =
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\"> <html>\n"
"<head>\n"
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"> <title>Contact_list</title>\n"
"</head>\n"
"<body>\n"
"<table width=\"1000\">\n"
"<tr><th width=\"99\">Name</th><th width=\"99\">Surname</th><th width=\"99\">Company</th><th width=\"150\">Mobile</th><th width=\"99\">Email</th><th width=\"99\">Date of birth</th><th width=\"99\">Picture</th></tr>\n";

const char *FOOTER =
"</table>\n"
"</body>\n"
"</html>\n";

char DIR[1024];
size_t MAXUID = 0;

//swap two contacts in contact_list
int swap_contacts(TContact_list *contact_list, TContact *contact1, TContact *contact2){
    if((contact1 == contact2) || (contact1 == NULL) || (contact2 == NULL)){
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

int sort_list(TContact_list *contact_list){
    size_t swapped = 1;
    if(contact_list->first == contact_list->last)
        return EXIT_SUCCESS;
    
    while(swapped){
        for(TContact *contact = contact_list->first->next; contact !=NULL; contact = contact->next){
            if(strcmp(contact->name, contact->prev->name) < 0){
                swap_contacts(contact_list, contact->prev, contact);
                swapped++;
            }
        }
        swapped--;
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
    strncpy(newcontact->name, pch, strlen(pch));
    pch = strtok (NULL, ",");
    strncpy(newcontact->surname, pch, strlen(pch));
    pch = strtok (NULL, ",");
    strncpy(newcontact->company, pch, strlen(pch));
    pch = strtok(NULL, ",");
    strncpy(newcontact->mobile, pch, strlen(pch));
    pch = strtok (NULL, ",");
    strncpy(newcontact->email, pch, strlen(pch));
    pch = strtok (NULL, ",");
    sscanf(pch, "%d/%d/%d", (int *)&newcontact->dob.month, &newcontact->dob.day, &newcontact->dob.year);
    pch = strtok(NULL, ",");
    strncpy(newcontact->image, pch, strlen(pch));
    
    if((pch = strtok(NULL, "")) != NULL){
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
    strcpy(newcontact->mobile, argv[5]);
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
    size_t deleted = 0;
    TContact *contact = contact_list->first;
    
    while(contact != NULL){
        if(UID == contact->UID){
            if(contact == contact_list->first){
                contact_list->first = contact->next;
                contact->next->prev = NULL;
                free(contact);
                deleted++;
            }
            else if(contact == contact_list->last){
                contact_list->last = contact->prev;
                contact->prev = NULL;
                free(contact);
                deleted++;
            }
            else{
                contact->prev->next = contact->next;
                contact->next->prev = contact->prev;
                free(contact);
                deleted++;
            }
        }
        contact = contact->next;
    }
    if(deleted == 0) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

//function print_list prints entire doubly-linked list to stdin
int print_list(TContact_list *contact_list){
    TContact *contact = contact_list->first;
    while(contact != NULL){
        printf("Name:%s Surname:%s Company:%s Mobile:%s Email:%s Date of birth:%d.%d.%d Picture:%s\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, contact->image);
        contact = contact->next;
    }
    return EXIT_SUCCESS;
}

//free_list deallocates memory from doubly-linked list contact_list
int free_list(TContact_list *contact_list){
    TContact *current = NULL;
    while(contact_list->first != NULL){
        current = contact_list->first;
        contact_list->first = contact_list->first->next;
        free(current);
    }
    return EXIT_SUCCESS;
}

//function print_single creates a .html output for a single contact with specific UID
int print_single(TContact_list *contact_list, size_t UID){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);FILE * file;
    char OUTHTML[1024], PATH[1024], imgpath[1024];
    TContact *contact = contact_list->first;
    
    strcpy(OUTHTML, DIR);
    strcpy(PATH, DIR);
    strcat(OUTHTML,"/record.html");
    strcat(PATH, "/resources/");
    
    file = fopen(OUTHTML, "w");
    if(file == NULL)
        return EXIT_FAILURE;
    
    printf( "%d/%d/%d\n", t->tm_mon+1, t->tm_mday, t->tm_year+1900 );
    
    fprintf(file,"%s", SINGLE);
    
    while(contact != NULL){
        if(UID == contact->UID){
            strcpy(imgpath, PATH);
            strcat(imgpath, contact->image);
            if( access(imgpath, F_OK ) == -1 ){
                strcpy(contact->image, "default.gif");
                errno = 0;
            }
            fprintf(file,"<table width=\"500\" border=\"0\">\n<tr>\n<td colspan=\"2\" style=\"background-color:#A9A9A9;\">\n<h1>%s %s</h1>\n</td>\n</tr>\n<tr>\n<td style=\"background-color:#DCDCDC;width:100px;text-align:top;\"><b>Company:</b><br>%s<br><br><b>Phone number:</b><br>%s<br><br><b>Email:</b><br>%s<br><br><b>Birthday:</b><br>%d.%d.%d</td>\n<td style=\"background-color:#EEEEEE;height:200px;width:400px;text-align:top;\">\n <img width=50 height=50 src=%s%s></td>\n</tr>\n<tr>\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, PATH, contact->image);
        }
        contact = contact->next;
    }
    fclose(file);
    return EXIT_SUCCESS;
}

//generate_html generates a .html file from doubly-linked list contact_list
int generate_html(TContact_list *contact_list){
    FILE * file;
    char OUTHTML[1024], PATH[1024], imgpath[1024];
    TContact *contact = contact_list->first;
    
    strcpy(OUTHTML, DIR);
    strcpy(PATH, DIR);
    strcat(OUTHTML,"/contacts.html");
    strcat(PATH, "/resources/");
    
    file = fopen(OUTHTML, "w");
    if(file == NULL)
        return EXIT_FAILURE;
    
    fprintf(file,"%s", HEADER);
    
    
    while(contact != NULL){
        strcpy(imgpath, PATH);
        strcat(imgpath, contact->image);
        if( access(imgpath, F_OK ) == -1 ){
            strcpy(contact->image, "default.gif");
            errno = 0;
        }
        fprintf(file,"<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%d.%d.%d</td><td><img width=50 height=50 src=%s%s></td></tr>", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, PATH, contact->image);
        contact = contact->next;
    }
    fprintf(file,"%s", FOOTER);
    fclose(file);
    return EXIT_SUCCESS;
}

//save_csv saves the doubly-linked list to a .csv file
int save_csv(TContact_list *contact_list){
    FILE * csv;
    char FILENAMEV[1024];
    TContact *contact = contact_list->first;
    
    strcpy(FILENAMEV, DIR);
    strcat(FILENAMEV, "/contacts_copy.csv");
    
    csv = fopen(FILENAMEV, "w");
    
    while(contact != NULL){
        fprintf(csv, "%ld,%s,%s,%s,%s,%s,%d/%d/%d,%s\n",contact->UID, contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.month, contact->dob.day, contact->dob.year, contact->image);
        contact = contact->next;
    }
    fclose(csv);
    return EXIT_SUCCESS;
}


//handle_errors is a function for dealing with errno
int handle_errors(void){
    switch(errno){
        case 0:
            return EXIT_SUCCESS;
            break;
        default:
            perror("Error");
            errno = 0;
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//get current directory
int get_dir(){
    getcwd(DIR, sizeof(DIR));
    return EXIT_SUCCESS;
}

//function get_line reads *FILE and returns a pointer to a string (single line of undefined length)
int get_line(FILE *file, char **ip){
    char *line = NULL, *tmp = NULL;
    char ch = 1;
    int size = 0, index = 0;
    
    while((ch != '\n')&&(!feof(file))){
        ch = getc(file);
        if((ch != '\n') && (ch != '\0')&&(!feof(file))){
            if(size-1 <=index){
                size += 32;
                tmp = (char *)realloc(line, sizeof(char)*size);
                if(tmp == NULL){
                    errno = ENOMEM;
                    return EXIT_FAILURE;
                }
                line = tmp;
                *ip = line;
            }
            if(isprint(ch))line[index++] = ch;
        }
        if(line != NULL)line[index] = '\0';
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
            if(line != NULL){
                insertlast(contact_list, line);
            }
            free(line);
        }
    }
    
    if(fclose(file) == EOF){
        if(errno == 0)errno = EBADF;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

//get_mode checks params for [ --add, --delete, --list, --plist, --single ] and returns apropriate return code, on error sets errno code
int get_mode(int argc, const char *argv[]){
    switch(argc){
        case 2:
            if(strcmp (argv[1], "--list") == 0)
                return LIST;
            if(strcmp (argv[1], "--plist") == 0)
                return PLIST;
            else{
                errno = EINVAL;
                return EXIT_FAILURE;
            }
            break;
        case 3:
            if(strcmp (argv[1], "--delete") == 0)
                return DEL;
            else if(strcmp (argv[1], "--single") == 0)
                return SIN;
            else if(strcmp (argv[1], "--find") == 0) //not implemented
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
    
    return EXIT_SUCCESS;
}

