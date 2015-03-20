//
//  main.c
//  forward.cgi
//
//  Created by Maxthon Chan on 3/13/15.
//  Copyright (c) 2015 DreamCity. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <json-c/json.h>

#ifdef FASTCGI
#include <fcgi_stdio.h>
#endif

void cgi_crash(int value)
{
    printf("Status: %1$d\n"
           "Content-Type: text/html\n"
           "\n"
           "<DOCTYPE html>\n"
           "<html><head>\n"
           "<title>Redirector Error: %1$d</title>\n"
           "</head><body>\n"
           "<h1>Error %1$d</h1>\n"
           "</body></html>\n", value);
    exit(0);
}

int main(int argc, const char * argv[])
{
#ifdef FASTCGI
    while (FCGI_Accept() >= 0)
#endif
    {
        // What is the user asking for?
        char *target = getenv("QUERY_STRING");
        if (!target)
            target = "";
        
        char *targetFile = getenv("FORWARD_JSON");
        if (!targetFile || !strlen(targetFile))
            targetFile = "/etc/forward.json";
        
        char *remainder = strchr(target, '/');
        if (remainder)
        {
            *remainder = 0;
            remainder++;
        }
        
        // Open the config file
        int fd = open(targetFile, O_RDONLY);
        if (fd < 0)
            cgi_crash(500);
        
        // Get the size of the file
        struct stat st;
        if (fstat(fd, &st))
            cgi_crash(500);
        
        // Allocate the buffer
        char *string = malloc(st.st_size);
        if (!string)
            cgi_crash(500);
        
        // Read all bytes
        if (read(fd, string, st.st_size) != st.st_size)
            cgi_crash(500);
        
        // Close the file
        close(fd);
        
        // Parse the JSON
        struct json_object *object = json_tokener_parse(string);
        if (!object)
            cgi_crash(500);
        
        // We expect a dictionary at the root
        if (!json_object_is_type(object, json_type_object))
            cgi_crash(500);
        
        // Search the string.
        struct lh_table *table = json_object_get_object(object);
        if (!table)
            cgi_crash(500);
        
        struct lh_entry *entry = lh_table_lookup_entry(table, target);
        if (!entry)
            cgi_crash(404);
        
        struct json_object *target_address_boxed = (struct json_object *)entry->v;
        if (!target_address_boxed)
            cgi_crash(500);
        
        // We expect a string here.
        if (!json_object_is_type(target_address_boxed, json_type_string))
            cgi_crash(500);
        
        // Redirect.
        const char *target_address = json_object_get_string(target_address_boxed);
        if (remainder && strlen(remainder))
        {
            if (target_address[strlen(target_address) - 1] != '/')
                printf("Status: 302\n"
                       "Location: %s/%s\n\n", target_address, remainder);
            else
                printf("Status: 302\n"
                       "Location: %s%s\n\n", target_address, remainder);
        }
        else
            printf("Status: 302\n"
                   "Location: %s\n\n", target_address);
        
        json_object_put(object);
        free(string);
        
    }
}
