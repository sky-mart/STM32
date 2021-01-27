#!/bin/sh

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

function is_cpp_style_bad()
{
    local cpp_file=$1
    clang-format -style=file --output-replacements-xml $cpp_file \
	| grep -q "</replacement>"
}

function diff_cpp_style()
{
    local cpp_file=$1
    echo $cpp_file
    clang-format $cpp_file | colordiff $cpp_file -
}

function fix_cpp_style()
{
    local cpp_file=$1
    clang-format -i $cpp_file
    echo "$cpp_file -- FIXED"
}

function usage()
{
    echo "Usage:"
    echo "$0 --check|--diff|--fix --file <file> | --staged"
    echo "$0 -c|-d|-x -f <file> | -s"
}

while [[ $# -gt 0 ]] ; do
    key="$1"
    case $key in
	-h|--help)
	    usage
	    exit 0
	    ;;
	-c|--check)
	    operation=check
	    shift
	    ;;
	-d|--diff)
	    operation=diff
	    shift
	    ;;
	-x|--fix)
	    operation=fix
	    shift
	    ;;
	-f|--file)
	    cpp_file="$2"
	    shift
	    shift
	    ;;
	-s|--staged)
	    cpp_file=staged
	    shift
	    ;;
    esac
done

[ -z "$operation" ] && (echo "Pick operation" ; usage; exit 1)
[ -z "$cpp_file" ] && (echo "Specify a file" ; usage; exit 1)

if [ "$cpp_file" == "staged" ]; then
    file_list=$(git diff --cached --name-only | grep -E ".*\.c|.*\.h")
else
    file_list=$cpp_file
fi

git_top_dir=$(git rev-parse --show-toplevel)

case $operation in
    check)
	bad=false
	for file in $file_list; do
	    abs_file="$git_top_dir/$file"
	    if is_cpp_style_bad $abs_file ; then
		bad=true
		diff_cpp_style $abs_file
	    fi
	done
	[ "$bad" == "true" ] && exit 1 || exit 0
	;;
    diff)
	for file in $file_list; do
	    abs_file="$git_top_dir/$file"
	    diff_cpp_style $abs_file
	done
	;;
    fix)
	for file in $file_list; do
	    abs_file="$git_top_dir/$file"
	    fix_cpp_style $abs_file
	done
	;;
esac

