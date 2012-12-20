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

#ifndef _csvlib_h
#define _csvlib_h

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>

///return value for params
enum retval{
    ADD=2,
    DEL,
    SIN,
    LIST,
    FIND,
    PLIST
};

///enumerator for listing months
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

///datatype for storing birthdate
typedef struct date{
    int day;
    TMonth month;
    int year;
}TDate;

///TContact is structure for contacts, part of a dualy linked list TContact_list
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

///TContact_list is a header for dualy linked list of contacts
typedef struct contact_list{
    struct contact *first;
    struct contact *last;
}TContact_list;

///insertlast parses the string 'line' and creates new contact in dualy linked list 'contact_list'
///@param TContact_list header of dualy linked list
///@param *char string containing contact info in .csv format
///@return on success function adds contact to 'contact_list' and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int insertlast(TContact_list *contact_list, char *line);

///sort_list sorts contact_list alphabeticaly by first name
///@param TContact_list header of dualy linked list
///@return on success function sorts 'contact_list' alphabeticaly and returns EXIT_SUCCESS
int sort_list(TContact_list *contact_list);

///swaps two contacts in contact_list
///@param TContact_list header of dualy linked list
///@param TContact first contact to be swapped
///@param TContact second contact to be swapped
///@return on success function swaps contacts 'contact_list' and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int swap_contacts(TContact_list *contact_list, TContact *contact1, TContact *contact2);

///add_to_list inserts new contact from command line to contact_list
///@param *char[] command line argument array
///@param TContact_list header of dualy linked list
///@return on success function adds contact to 'contact_list' and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int add_to_list(const char *argv[], TContact_list *contact_list);

///removes contact with specified UID from list
///@param TContact_list header of dualy linked list
///@param size_t contact UID
///@return on success function removes contact from 'contact_list' and returns EXIT_SUCCESS, on failure function returns EXIT_FAILURE
int remove_from_list(TContact_list *contact_list, size_t UID);

///finds contacts which are containing specific string
///@param TContact_list header of dualy linked list
///@param *char searched string
///@return on success function removes contacts not containing searched string from 'contact_list' and returns EXIT_SUCCESS
int find_in_list(TContact_list *contact_list, char *argv);

///function print_list prints entire doubly-linked list to stdout
///@param TContact_list header of dualy linked list
///@return EXIT_SUCCESS
int print_list(TContact_list *contact_list);

///free_list deallocates memory from doubly-linked list contact_list
///@param TContact_list header of dualy linked list
///@return EXIT_SUCCESS
int free_list(TContact_list *contact_list);

///save_csv saves the doubly-linked list to a .csv file
///@param TContact_list header of dualy linked list
///@return EXIT_SUCCESS
int save_csv(TContact_list *contact_list);

///generate_html generates a .html file from doubly-linked list contact_list
///@param TContact_list header of dualy linked list
///@return EXIT_SUCCESS or EXIT_FAILURE
int generate_html(TContact_list *contact_list);

///generate_plist generates a .plist file from doubly-linked list contact_list
///@param TContact_list header of dualy linked list
///@return EXIT_SUCCESS or EXIT_FAILURE
int generate_plist(TContact_list *contact_list);

///function print_single creates a .html output for a single contact with specific UID
///@param TContact_list header of dualy linked list
///@param size_t contact UID
///@return EXIT_SUCCESS or EXIT_FAILURE
int print_single(TContact_list *contact_list, size_t UID);

///function get_data reads a file containing the .csv database and creates a doubly linked list out of the records
///@param TContact_list header of dualy linked list
///@return on success function adds contacts from .csv file to 'contact_list' and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int get_data(TContact_list *contact_list);

///function get_line reads *FILE and returns a pointer to a string (single line of undefined length)
///@param FILE file open for reading
///@param **char pointer to string
///@return on success function sets the poiner to string containing one line from file and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int get_line(FILE *file, char **ip);

///set global variable DIR to parent directory
///@return on success function saves path to working directory to DIR and returns EXIT_SUCCESS, on failure function sets errno and returns EXIT_FAILURE
int get_dir(void);

#endif
