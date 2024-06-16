# Parse command-line arguments
param (
    [string]$Task = "default"
)

# Default task
if ($Task -eq "default") {
    $Task = "build"
}

# Define tasks and their dependencies
$tasks = @{
    "config-conan" = @{
        Action = {
            conan install . --build=missing
        }
    }
    "config" = @{
        Action = {
            cmake --preset=conan-default
        }
        Dependencies = @("config-conan")
    }
    "build" = @{
        Action = {
            cmake --build --preset=conan-release
        }
        Dependencies = @("config")
    }
}

# Function to execute tasks
function Invoke-Task {
    param (
        [string]$taskName
    )

    if (-not $tasks.ContainsKey($taskName)) {
        Write-Error "Task '$taskName' not found."
        return
    }

    # Run dependencies first
    foreach ($dependency in $tasks[$taskName].Dependencies) {
        Invoke-Task $dependency
    }

    # Execute the task
    Write-Output "Executing task '$taskName'..."
    $tasks[$taskName].Action.Invoke()
}

# Invoke the specified task
Invoke-Task $Task
