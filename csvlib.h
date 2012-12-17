//
//  dlist.h
//  contact_list_cmd
//
//  Created by Salamander on 17.12.12.
//  Copyright (c) 2012 Salamander. All rights reserved.
//

#ifndef _csvlib_h
#define _csvlib_h

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

//return value for params
enum retval{
    ADD=2,
    DEL,
    SIN,
    LIST,
    FIND,
    PLIST
};

//datatype for listing months
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

//datatype for storing birthdate
typedef struct date{
    int day;
    TMonth month;
    int year;
}TDate;

//TContact is structure for a single contact, part of a dualy linked list TContact_list
typedef struct contact{
    size_t UID;
    TDate dob;
    char name[64];
    char surname[64];
    char company[64];
    char mobile[64];
    char email[64];
    char image[64];
    struct contact *next;
    struct contact *prev;
}TContact;

//TContact_list is a header for dualy linked list of contacts
typedef struct contact_list{
    struct contact *first;
    struct contact *last;
}TContact_list;

int insertlast(TContact_list *contact_list, char *line);

int sort_list(TContact_list *contact_list);

int swap_contacts(TContact_list *contact_list, TContact *contact1, TContact *contact2);

int add_to_list(const char *argv[], TContact_list *contact_list);

int remove_from_list(TContact_list *contact_list, size_t UID);

int print_list(TContact_list *contact_list);

int free_list(TContact_list *contact_list);

int save_csv(TContact_list *contact_list);

int generate_html(TContact_list *contact_list);

int print_single(TContact_list *contact_list, size_t UID);

int get_mode(int argc, const char *argv[]);

int get_data(TContact_list *contact_list);

int get_line(FILE *file, char **ip);

int get_dir();

int handle_errors(void);

#endif
