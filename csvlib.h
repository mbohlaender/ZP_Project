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

//insertlast parses the string and creates new contact in contact_list
int insertlast(TContact_list *contact_list, char *line);

int sort_list(TContact_list *contact_list);

//swap two contacts in contact_list
int swap_contacts(TContact_list *contact_list, TContact *contact1, TContact *contact2);

//add_to_list inserts new contact from command line to contact_list
int add_to_list(const char *argv[], TContact_list *contact_list);

//removes contact with specified UID from list
int remove_from_list(TContact_list *contact_list, size_t UID);

int find_in_list(TContact_list *contact_list, char *argv);

//function print_list prints entire doubly-linked list to stdin
int print_list(TContact_list *contact_list);

//free_list deallocates memory from doubly-linked list contact_list
int free_list(TContact_list *contact_list);

//save_csv saves the doubly-linked list to a .csv file
int save_csv(TContact_list *contact_list);

//generate_html generates a .html file from doubly-linked list contact_list
int generate_html(TContact_list *contact_list);

//function print_single creates a .html output for a single contact with specific UID
int print_single(TContact_list *contact_list, size_t UID);

//function get_data reads a file containing the .csv database and creates a doubly linked list out of the records
int get_data(TContact_list *contact_list);

//function get_line reads *FILE and returns a pointer to a string (single line of undefined length)
int get_line(FILE *file, char **ip);

//set DIR to current directory
int get_dir();

#endif
