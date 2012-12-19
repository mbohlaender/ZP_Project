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

int main(int argc, const char *argv[])
{
    TContact_list contact_list = {contact_list.first = NULL, contact_list.last = NULL};
    get_dir();
    
    switch(get_mode(argc, argv)){
        case DEL:
            get_data(&contact_list);
            remove_from_list(&contact_list, atoi(argv[2]));
            save_csv(&contact_list);
            free_list(&contact_list);
            break;
        case ADD:
            get_data(&contact_list);
            add_to_list(argv, &contact_list);
            save_csv(&contact_list);
            free_list(&contact_list);
            break;
        case LIST:
            get_data(&contact_list);
            sort_list(&contact_list);
            print_list(&contact_list);
            save_csv(&contact_list);
            generate_html(&contact_list);
            free_list(&contact_list);
            break;
        case SIN:
            get_data(&contact_list);
            print_single(&contact_list, atoi(argv[2]));
            free_list(&contact_list);
            break;
        case FIND:
            get_data(&contact_list);
            find_in_list(&contact_list, (char *)argv[2]);
            //print_list(&contact_list);
            generate_html(&contact_list);
            free_list(&contact_list);
            break;
        case PLIST:
            get_data(&contact_list);
            sort_list(&contact_list);
            //print_list(&contact_list);
            save_csv(&contact_list);
            //generate_plist(&contact_list);
            free_list(&contact_list);
            break;
        default:
            handle_errors();
            return EXIT_FAILURE;
    }
    if(handle_errors() != EXIT_SUCCESS) return EXIT_FAILURE;
    else return EXIT_SUCCESS;
}