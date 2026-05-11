# Convert-QmToTs.ps1
$QtBin = "C:\Qt\Qt5.15.2\5.15.2\msvc2019\bin"
$lconvert = Join-Path $QtBin "lconvert.exe"
$lrelease = Join-Path $QtBin "lrelease.exe"

$transDir = "translations"
if (-not (Test-Path $transDir)) {
    Write-Error "找不到 translations 目录"
    exit 1
}

Push-Location $transDir

$oldContexts = @("main", "InfoList")

Get-ChildItem -Filter "*.qm" | ForEach-Object {
    $qm = $_.FullName
    $base = $_.BaseName
    $tsFile = "$base.ts"
    Write-Host "Processing $($_.Name) ..."

    # 1. qm -> ts
    # 不使用 -target-encoding，依赖默认（应该是 utf-8）
    & $lconvert -i $qm -o $tsFile 2>&1 | Out-Null
    if (-not $?) {
        Write-Warning "lconvert failed for $($_.Name), skipping"
        return
    }

    # 2. 读取并替换上下文
    try {
        $content = Get-Content $tsFile -Raw -Encoding UTF8
        if (-not $content) { throw "File empty" }
        foreach ($old in $oldContexts) {
            $content = $content -replace "<name>$old</name>", '<name>CDiskHealthMainWin</name>'
        }
        # 写回 UTF-8 无 BOM
        $utf8NoBom = New-Object System.Text.UTF8Encoding $false
        [System.IO.File]::WriteAllText((Resolve-Path $tsFile), $content, $utf8NoBom)
    } catch {
        Write-Warning "Failed to process $tsFile : $_"
        return
    }

    # 3. ts -> qm
    & $lrelease $tsFile
    if (-not $?) {
        Write-Warning "lrelease failed for $tsFile"
    } else {
        Write-Host "Successfully updated $($_.Name)"
    }

    # 可选删除 ts
    # Remove-Item $tsFile
}

Pop-Location
Write-Host "All done."