# bash completion for bush-tasks
_bush_tasks() {
    local cur prev
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"

    case "$prev" in
        priority)
            COMPREPLY=($(compgen -W "low medium high urgent" -- "$cur"))
            return
            ;;
        status)
            COMPREPLY=($(compgen -W "pending done postponed" -- "$cur"))
            return
            ;;
    esac

    if [ "$COMP_CWORD" -eq 1 ]; then
        COMPREPLY=($(compgen -W "add del edit sub priority status tasks clear exit quit" -- "$cur"))
    fi
}

complete -F _bush_tasks bush-tasks
