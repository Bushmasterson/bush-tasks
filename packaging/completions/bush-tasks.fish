# fish completion for bush-tasks
complete -c bush-tasks -f

complete -c bush-tasks -n '__fish_use_subcommand' -a 'add'      -d 'Add a new task'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'del'      -d 'Delete a task'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'edit'     -d 'Edit task text'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'sub'      -d 'Add a subtask'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'priority' -d 'Change task priority'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'status'   -d 'Change task status'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'tasks'    -d 'List all tasks'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'clear'    -d 'Delete all tasks'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'exit'     -d 'Quit'
complete -c bush-tasks -n '__fish_use_subcommand' -a 'quit'     -d 'Quit'

complete -c bush-tasks -n '__fish_seen_subcommand_from priority' -a 'low medium high urgent'
complete -c bush-tasks -n '__fish_seen_subcommand_from status'   -a 'pending done postponed'
