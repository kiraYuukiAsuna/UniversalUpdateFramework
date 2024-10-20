# 格式化指定目录下的所有 C/C++ 源文件并忽略 .gitignore 中指定的文件/目录
# 使用 clang-format 工具进行格式化，需要提前安装 clang-format 并将其添加到环境变量中
# 用法: 在 PowerShell 中执行 .\format_all.ps1 [RootDir]，默认为当前目录

param (
    [string]$RootDir = (Get-Location).Path
)

function ConvertGitignorePatternToRegex($pattern) {
    # Escape special regex characters
    $pattern = [regex]::Escape($pattern)
    # Handle special gitignore patterns
    $pattern = $pattern -replace '\\\*\\\*/', '(\\/|\\/.*\\/)'    # **/ -> (\/|\/.*\/)
    $pattern = $pattern -replace '\\\*', '.*'                     # * -> .*
    $pattern = $pattern -replace '\\\?', '.'                      # ? -> .
    
    if ($pattern.EndsWith('/')) {
        $pattern = '^' + $pattern.TrimEnd('/') + '.*'
    } else {
        $pattern = '^' + $pattern + '$'
    }
    
    return $pattern
}

function ShouldIgnoreFile($filePath) {
    # Convert Windows paths to Unix paths for pattern matching
    $filePath = $filePath.Replace('\', '/')
    foreach ($regex in $ignoreRegexList) {
        if ($filePath -match $regex) {
            return $true
        }
    }
    return $false
}

# 初始化忽略模式
$ignorePatterns = @()
$ignoreRegexList = @()

# 读取 .gitignore 文件并解析出应忽略的文件/目录模式
$gitignorePath = Join-Path $RootDir ".gitignore"

if (Test-Path -Path $gitignorePath) {
    $ignorePatterns = Get-Content $gitignorePath | Where-Object { $_ -and $_ -notlike '#*' -and $_ -ne '' }
    $ignoreRegexList = $ignorePatterns | ForEach-Object { ConvertGitignorePatternToRegex $_ }
}

# 获取所有需要格式化的文件
$files = Get-ChildItem -Path $RootDir -Recurse -Include *.cpp, *.h, *.c, *.hpp

foreach ($file in $files) {
    $relativePath = [System.IO.Path]::GetRelativePath($RootDir, $file.FullName)
    
    # 将忽略判断放入条件语句内部处理
    if ($ignoreRegexList.Count -gt 0) {
        if (ShouldIgnoreFile $relativePath) {
            Write-Output ("Ignoring: " + $relativePath)
            continue
        }
    }
    
    # 格式化每个文件
    Write-Output ("Formatting: " + $relativePath)
    & clang-format -i $file.FullName
}

Write-Output "Formatting completed!"
