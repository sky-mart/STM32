#/bin/sh

# to create a symlink
# ln -fsv $(pwd)/scripts/pre-commit-hook.sh $(pwd)/.git/hooks/pre-commit

link_path=$(readlink $0)
if [ -z link_path ]; then
    script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
else
    script_dir=$(CDPATH= cd -- "$(dirname -- "$link_path")" && pwd)
fi

$script_dir/cpp_file_style.sh --check --staged
