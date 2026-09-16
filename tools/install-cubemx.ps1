<#
.SYNOPSIS
    Устанавливает STM32CubeMX 6.0.1, 6.2.0 и 6.4.0 рядом друг с другом.

.DESCRIPTION
    Инсталлятор CubeMX собран на IzPack и поддерживает автоматическую
    установку по XML-скрипту — без единого диалога. Но он требует прав
    администратора: без них молча завершается с кодом 1, а при запуске
    вручную показывает «Unsufficient rights to install».

    Скрипт ставит каждую версию в свой каталог C:\ST\STM32CubeMX_<версия>,
    поэтому версии не конфликтуют между собой.

.NOTES
    ЗАПУСКАТЬ ОТ ИМЕНИ АДМИНИСТРАТОРА.

        Правый клик на PowerShell -> «Запуск от имени администратора», затем:
        cd <корень репозитория>
        powershell -ExecutionPolicy Bypass -File tools\install-cubemx.ps1

    Если автоматическая установка какой-то версии не сработает, скрипт
    скажет об этом и назовёт файл, который нужно запустить вручную.
    Панели IzPack сверены с версией 6.0.1; у 6.2.0 и 6.4.0 полезная
    нагрузка упакована, и их набор панелей может отличаться.
#>

[CmdletBinding()]
param(
    # Куда ставить. По умолчанию рядом с уже установленным STM32CubeIDE.
    [string]$InstallRoot = 'C:\ST',
    # Ограничиться отдельными версиями, например: -Only 6.4.0
    [string[]]$Only
)

$ErrorActionPreference = 'Stop'

# --- права администратора -------------------------------------------------
$id = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object Security.Principal.WindowsPrincipal($id)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host 'Нужны права администратора.' -ForegroundColor Red
    Write-Host 'Запустите PowerShell от имени администратора и повторите:'
    Write-Host '  powershell -ExecutionPolicy Bypass -File tools\install-cubemx.ps1'
    exit 1
}

$root = Join-Path $PSScriptRoot 'installers\cubemx'

$versions = @(
    @{ Version = '6.0.1'; Exe = '6.0.1\SetupSTM32CubeMX-6.0.1.exe' }
    @{ Version = '6.2.0'; Exe = '6.2.0\SetupSTM32CubeMX-6.2.0-Win.exe' }
    @{ Version = '6.4.0'; Exe = '6.4.0\SetupSTM32CubeMX-6.4.0-Win.exe' }
)
if ($Only) { $versions = $versions | Where-Object { $Only -contains $_.Version } }

$ok = @(); $failed = @()

foreach ($v in $versions) {
    $ver = $v.Version
    $exe = Join-Path $root $v.Exe
    $xml = Join-Path $root "auto-install-$ver.xml"
    $dest = Join-Path $InstallRoot "STM32CubeMX_$ver"

    Write-Host ''
    Write-Host "=== STM32CubeMX $ver ===" -ForegroundColor Cyan

    if (Test-Path $dest) { Write-Host "  уже установлен: $dest"; $ok += $ver; continue }
    if (-not (Test-Path $exe)) { Write-Host "  НЕТ ФАЙЛА: $exe" -ForegroundColor Red; $failed += $ver; continue }
    if (-not (Test-Path $xml)) { Write-Host "  НЕТ СКРИПТА: $xml" -ForegroundColor Red; $failed += $ver; continue }

    # Путь установки прописан внутри XML; если InstallRoot изменён — правим на лету.
    $xmlToUse = $xml
    if ($InstallRoot -ne 'C:\ST') {
        $xmlToUse = Join-Path $env:TEMP "auto-install-$ver.xml"
        (Get-Content $xml -Raw).Replace("C:\ST\STM32CubeMX_$ver", $dest) |
            Set-Content $xmlToUse -Encoding UTF8
    }

    Write-Host "  установка в $dest ..."
    $p = Start-Process -FilePath $exe -ArgumentList "`"$xmlToUse`"" -Wait -PassThru -NoNewWindow
    Start-Sleep -Seconds 2

    if (Test-Path $dest) {
        Write-Host "  готово (код $($p.ExitCode))" -ForegroundColor Green
        $ok += $ver
    } else {
        Write-Host "  автоматическая установка не сработала, код $($p.ExitCode)" -ForegroundColor Yellow
        Write-Host "  запустите вручную и пройдите мастером: $exe"
        $failed += $ver
    }
}

Write-Host ''
Write-Host '=== ИТОГ ===' -ForegroundColor Cyan
if ($ok)     { Write-Host ("установлены: " + ($ok -join ', ')) -ForegroundColor Green }
if ($failed) { Write-Host ("не установлены: " + ($failed -join ', ')) -ForegroundColor Yellow }

Write-Host ''
Write-Host 'Проверка комплекта окружения: python tools\verify.py'
if ($failed) { exit 1 } else { exit 0 }
