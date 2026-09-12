# PowerShell completion for bush-tasks
Register-ArgumentCompleter -Native -CommandName bush-tasks -ScriptBlock {
  param($wordToComplete, $commandAst, $cursorPosition)

  $commands = @('add', 'del', 'edit', 'sub', 'priority', 'status', 'tasks', 'clear', 'exit', 'quit')
  $priorities = @('low', 'medium', 'high', 'urgent')
  $statuses = @('pending', 'done', 'postponed')

  $tokens = $commandAst.CommandElements | ForEach-Object { $_.ToString() }
  $prev = if ($tokens.Count -ge 2) { $tokens[-1] } else { '' }

  $candidates = switch ($prev) {
    'priority' { $priorities }
    'status' { $statuses }
    default {
      if ($tokens.Count -le 2) { $commands } else { @() }
    }
  }

  $candidates | Where-Object { $_ -like "$wordToComplete*" } | ForEach-Object {
    [System.Management.Automation.CompletionResult]::new($_, $_, 'ParameterValue', $_)
  }
}
