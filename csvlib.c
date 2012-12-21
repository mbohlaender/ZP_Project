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

const char *PLIST_HEADER =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
"<plist version=\"1.0\">\n"
"<array>\n";

const char *FOOTER =
"</table>\n"
"</body>\n"
"</html>\n";

const char *PLIST_FOOTER =
"</array>\n"
"</plist>\n";

size_t MAXUID = 0;
char DIR[1024];

int get_dir(void){
    char *pch;
    getcwd(DIR, sizeof(DIR));
    pch = strrchr(DIR, '/');
    DIR[pch-DIR] = '\0';
    return EXIT_SUCCESS;
}

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
    strcat(newcontact->image, argv[2]);
    strcat(newcontact->image, ".gif");
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

int remove_from_list(TContact_list *contact_list, size_t UID){
    size_t deleted = 0;
    TContact *contact = contact_list->first;
    
    while(contact != NULL){
        if(UID == contact->UID){
            if((contact == contact_list->first) && (contact == contact_list->last)){
                contact_list->first = NULL;
                contact_list->last = NULL;
                free(contact);
                deleted++;
            }
            else if(contact == contact_list->first){
                contact_list->first = contact->next;
                contact->next->prev = NULL;
                free(contact);
                deleted++;
            }
            else if(contact == contact_list->last){
                contact_list->last = contact->prev;
                contact->prev->next = NULL;
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

int delete_by_name(TContact_list *contact_list, char *name, char *surname){
    TContact *contact = contact_list->first;
    size_t deleted = 0;
    
    while(contact != NULL){
        if((strcasecmp(name, contact->name) == 0) && (strcasecmp(surname, contact->surname) == 0) && deleted == 0)
            remove_from_list(contact_list, contact->UID);
        contact = contact->next;
    }
    return EXIT_SUCCESS;
}

int find_in_list(TContact_list *contact_list, char *argv){
    TContact *contact = contact_list->first;
    while(contact != NULL){
        if((strcasestr(contact->name, argv) == NULL) && (strcasestr(contact->surname, argv) == NULL) && (strcasestr(contact->company, argv) == NULL) && (strcasestr(contact->email, argv) == NULL))
            remove_from_list(contact_list, contact->UID);
        contact = contact->next;
    }

    return EXIT_SUCCESS;
}

int print_list(TContact_list *contact_list){
    TContact *contact = contact_list->first;
    while(contact != NULL){
        printf("Name:%s Surname:%s Company:%s Mobile:%s Email:%s Date of birth:%d.%d.%d Picture:%s\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, contact->image);
        contact = contact->next;
    }
    return EXIT_SUCCESS;
}

int free_list(TContact_list *contact_list){
    TContact *current = NULL;
    while(contact_list->first != NULL){
        current = contact_list->first;
        contact_list->first = contact_list->first->next;
        free(current);
    }
    return EXIT_SUCCESS;
}

int print_single(TContact_list *contact_list, size_t UID){
    time_t now = time(NULL);
    struct tm *t = localtime(&now);FILE * file;
    char OUTHTML[1024], PATH[1024], imgpath[1024];
    TContact *contact = contact_list->first;
    
    strcpy(OUTHTML, DIR);
    strcpy(PATH, DIR);
    strcat(OUTHTML,"/vystupnidata/record.html");
    strcat(PATH, "/vstupnidata/resources/");
    
    file = fopen(OUTHTML, "w");
    if(file == NULL)
        return EXIT_FAILURE;
    
    fprintf(file,"%s", SINGLE);
    
    while(contact != NULL){
        if(UID == contact->UID){
            strcpy(imgpath, PATH);
            strcat(imgpath, contact->image);
            if( access(imgpath, F_OK ) == -1 ){
                strcpy(contact->image, "default.gif");
                errno = 0;
            }
            if((t->tm_mon+1 == contact->dob.month) && (t->tm_mday == contact->dob.day))
                fprintf(file,"<table width=\"400\" border=\"0\">\n<tr>\n<td colspan=\"2\" style=\"background-color:#A9A9A9;\">\n<h1>%s %s</h1>\n</td>\n</tr>\n<tr>\n<td style=\"background-color:#DCDCDC;width:100px;text-align:top;\"><b>Company:</b><br>%s<br><br><b>Phone number:</b><br>%s<br><br><b>Email:</b><br>%s<br><br><b>Birthday:</b><br>%d.%d.%d</td>\n<td style=\"background-color:#EEEEEE;height:200px;width:200px;text-align:top;\">\n <img width=200 height=200 src=%s%s></td>\n</tr>\n<tr>\n<td colspan=\"2\" style=\"background-color:#FFA500;text-align:center;\">\nIt's %s's birthday today !</td>\n</tr>\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, PATH, contact->image, contact->name);
            else fprintf(file,"<table width=\"400\" border=\"0\">\n<tr>\n<td colspan=\"2\" style=\"background-color:#A9A9A9;\">\n<h1>%s %s</h1>\n</td>\n</tr>\n<tr>\n<td style=\"background-color:#DCDCDC;width:100px;text-align:top;\"><b>Company:</b><br>%s<br><br><b>Phone number:</b><br>%s<br><br><b>Email:</b><br>%s<br><br><b>Birthday:</b><br>%d.%d.%d</td>\n<td style=\"background-color:#EEEEEE;height:200px;width:200px;text-align:top;\">\n <img width=200 height=200 src=%s%s></td>\n</tr>\n<tr>\n", contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, PATH, contact->image);
        }
        contact = contact->next;
    }
    fprintf(file,"%s", FOOTER);
    fclose(file);
    return EXIT_SUCCESS;
}

int generate_html(TContact_list *contact_list){
    FILE * file;
    char OUTHTML[1024], PATH[1024], imgpath[1024];
    TContact *contact = contact_list->first;
    
    strcpy(OUTHTML, DIR);
    strcpy(PATH, DIR);
    strcat(OUTHTML,"/vystupnidata/contacts.html");
    strcat(PATH, "/vstupnidata/resources/");
    
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

int generate_plist(TContact_list *contact_list){
    FILE * file;
    char OUTPLIST[1024], PATH[1024];
    TContact *contact = contact_list->first;
    
    strcpy(OUTPLIST, DIR);
    strcpy(PATH, DIR);
    strcat(OUTPLIST,"/vystupnidata/contacts.plist");
    strcat(PATH, "/vstupnidata/resources/");
    
    file = fopen(OUTPLIST, "w");
    if(file == NULL)
        return EXIT_FAILURE;
    
    fprintf(file,"%s", PLIST_HEADER);
    
    while(contact != NULL){
        fprintf(file,"\t<dict>\n\t\t<key>Company</key>\n\t\t<string>%s</string>\n\t\t<key>Email</key>\n\t\t<string>%s</string>\n\t\t<key>dateofBirth</key>\n\t\t<string>%d.%d.%d</string>\n\t\t<key>firstName</key>\n\t\t<string>%s</string>\n\t\t<key>lastName</key>\n\t\t<string>%s</string>\n\t\t<key>phoneNumber</key>\n\t\t<string>%s</string>\n\t</dict>\n", contact->company, contact->email, contact->dob.day, contact->dob.month, contact->dob.year, contact->name, contact->surname, contact->mobile);
        contact = contact->next;
    }
    fprintf(file,"%s", PLIST_FOOTER);
    fclose(file);

    return EXIT_SUCCESS;
}

int save_csv(TContact_list *contact_list){
    FILE * csv;
    char FILENAMEV[1024];
    TContact *contact = contact_list->first;
    
    strcpy(FILENAMEV, DIR);
    strcat(FILENAMEV, "/vstupnidata/contacts.csv");
    
    csv = fopen(FILENAMEV, "w");
    
    while(contact != NULL){
        fprintf(csv, "%ld,%s,%s,%s,%s,%s,%d/%d/%d,%s\n",contact->UID, contact->name, contact->surname, contact->company, contact->mobile, contact->email, contact->dob.month, contact->dob.day, contact->dob.year, contact->image);
        contact = contact->next;
    }
    fclose(csv);
    return EXIT_SUCCESS;
}

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

int get_data(TContact_list *contact_list){
    FILE *file;
    char *line = NULL;
    char FILENAME[1024];
    
    strcpy(FILENAME, DIR);
    strcat(FILENAME, "/vstupnidata/contacts.csv");
    
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


