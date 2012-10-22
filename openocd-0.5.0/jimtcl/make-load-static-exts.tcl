#!/usr/bin/env tclsh

# Usage: make-load-static-exts extname ... >load-static-exts.c

# Creates load-static-exts.c based on the configured static extensions

# There are some dependencies on static extensions which require
# a certain load order. Do this by setting priorities and sorting.

array set pri {
    stdlib 0
    readdir 1
    glob 2
    oo 1
    tree 2
    pack 1
    binary 2
}

foreach i $argv {
    set p 1
    if {[info exists pri($i)]} {
        set p $pri($i)
    }
    lappend exts [list $p $i]
}
set exts [lsort $exts]

puts {
/* autogenerated - do not edit */
#include "jim.h"
#include "jimautoconf.h"
int Jim_InitStaticExtensions(Jim_Interp *interp)
}
puts \{

foreach e $exts {
    set ext [lindex $e 1]
    puts "\textern int Jim_${ext}Init(Jim_Interp *);"
    puts "\tJim_${ext}Init(interp);"
}

puts "\treturn JIM_OK;"

puts \}
