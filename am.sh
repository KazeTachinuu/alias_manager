#!/bin/sh

Version="7.0.0"
alias_file="$HOME/.my_aliases.txt"

IsAliasInLine() {
    local line="$1"
    local alias_name="$2"
    local i=6
    local j=0

    while [ "${line:$i:1}" != "" ] && [ "${alias_name:$j:1}" != "" ] && [ "${line:$i:1}" = "${alias_name:$j:1}" ]; do
        ((i++))
        ((j++))
    done

    [ "${line:$i:1}" != "" ] && [ "${alias_name:$j:1}" = "" ] && [ "${line:$i:1}" = "=" ]
}

create_alias() {
    local alias_name="$1"
    local alias_command="$2"

    if [ -z "$HOME" ]; then
        echo "Failed to get the HOME directory"
        return
    fi

    alias_file_path="$HOME/.my_aliases.txt"

    if [ ! -f "$alias_file_path" ]; then
        touch "$alias_file_path"
    fi


    if [[ $alias_command == *"'"* ]]; then
        alias_command="${alias_command//\'\\\'}"
        echo "alias $alias_name=\"$alias_command\"" >>"$alias_file_path"
    else
        echo "alias $alias_name='$alias_command'" >>"$alias_file_path"
    fi

    echo "Alias '$alias_name' with command '$alias_command' created successfully!"
    source $alias_file_path
}


remove_alias() {
    local alias_name="$1"
    local is_forced="$2"

    if [ -z "$HOME" ]; then
        echo "Failed to get the HOME directory"
        return
    fi

    alias_file_path="$HOME/.my_aliases.txt"

    if [ ! -f "$alias_file_path" ]; then
        echo "Alias '$alias_name' not found."
        return
    fi

    if grep -q "^alias $alias_name=" "$alias_file_path"; then
        if [ "$is_forced" -eq 0 ]; then
            alias_command=$(grep "^alias $alias_name=" "$alias_file_path" | sed 's/^alias //')
            echo "Removing alias:" 
            echo "alias $alias_command"
            read -p "Do you want to remove this alias? (Y/n): " confirmation
            confirmation="${confirmation,,}"  # Convert to lowercase
            if [ "$confirmation" != "y" ] && [ "$confirmation" != "yes" ] && [ "$confirmation" != "" ]; then
                echo "Aborted."
                return
            fi
        fi

        sed -i "/^alias $alias_name=/d" "$alias_file_path"
        echo "Alias '$alias_name' removed successfully!"
    else
        echo "Alias '$alias_name' not found."
    fi
}

list_aliases() {
    if [ -z "$HOME" ]; then
        echo "Failed to get the HOME directory"
        return
    fi

    alias_file_path="$HOME/.my_aliases.txt"

    if [ ! -f "$alias_file_path" ]; then
        echo "No aliases found."
        return
    fi

    echo "Aliases:"
    grep "^alias" "$alias_file_path" | sed 's/^alias //'
}

find_aliases() {
    local str="$1"

    if [ -z "$HOME" ]; then
        echo "Failed to get the HOME directory"
        return
    fi

    alias_file_path="$HOME/.my_aliases.txt"

    if [ ! -f "$alias_file_path" ]; then
        echo "No aliases found."
        return
    fi

    echo "Aliases matching \"$str\":"
    grep "^alias" "$alias_file_path" | grep "$str" | sed 's/^alias //'
}

show_version() {
    echo "aliasmanager v$Version"
}

show_help() {
    show_version
    echo "Manage your aliases in ~/.my_aliases.txt"
    echo
    echo "   aliasmanager add <ALIAS_NAME> <COMMAND>     add alias"
    echo "   aliasmanager rm <ALIAS_NAME> [-f|--force]   remove alias"
    echo "   aliasmanager ls                             list all aliases"
    echo "   aliasmanager ls <STRING>                    list all aliases matching <STRING>"
    echo
    echo "    -V --version             Show version of this program"
    echo "    -h --help                Show this page of help"
}

if [ "$#" -lt 1 ]; then
    show_help
    exit 1
fi

case "$1" in
    add)
        if [ "$#" -ne 3 ]; then
            echo "Usage: $0 add <ALIAS_NAME> <COMMAND>"
            exit 1
        fi
        create_alias "$2" "$3"
        ;;
    rm)
        if [ "$#" -lt 2 ] || [ "$#" -gt 4 ]; then
            echo "Usage: $0 rm <ALIAS_NAME> [-f|--force]"
            exit 1
        fi
        is_forced=0
        if [ "$#" -eq 4 ] && { [ "$3" == "--force" ] || [ "$3" == "-f" ]; }; then
            is_forced=1
        fi
        remove_alias "$2" "$is_forced"
        ;;
    ls)
        if [ "$#" -eq 1 ]; then
            list_aliases
        elif [ "$#" -eq 2 ]; then
            find_aliases "$2"
        else
            echo "Usage: $0 ls [STRING]"
            exit 1
        fi
        ;;
    -h|--help)
        show_help
        exit 0
        ;;
    -V|--version)
        show_version
        exit 0
        ;;
    *)
        echo "Unknown subcommand '$1'"
        echo "Usage: $0 <SUBCOMMAND> <ARGS>"
        echo
        echo "Consider using '$0 -h | --help' for more information"
        exit 1
        ;;
esac

exit 0

