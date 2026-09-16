<#
.SYNOPSIS
    Устанавливает STM32CubeMX 6.0.1, 6.2.0 и 6.4.0 рядом друг с другом.

.DESCRIPTION
    Скрипт делает две вещи:

    1. Ставит Java Runtime Environment 8, если её нет. Лаунчер инсталлятора
       CubeMX ищет JRE в реестре (HKLM\SOFTWARE\JavaSoft\Java Runtime
       Environment) и без неё показывает «This application requires a Java
       Runtime Environment 1.8.0_45 (64-bit)». JRE внутри STM32CubeIDE
       в реестре не значится, поэтому не подходит.

    2. Ставит три версии CubeMX в автоматическом режиме IzPack — без единого
       диалога, каждую в свой каталог C:\ST\STM32CubeMX_<версия>.

    Три версии держатся рядом намеренно: проекты создавались разными
    версиями, а перегенерация чужой версией меняет содержимое Core/
    сильнее, чем ожидается.

.NOTES
    ЗАПУСКАТЬ ОТ ИМЕНИ АДМИНИСТРАТОРА — иначе инсталлятор CubeMX завершается
    с кодом 1 молча, ничего не выводя в консоль.

        powershell -ExecutionPolicy Bypass -File tools\install-cubemx.ps1

    Набор панелей IzPack сверен с версией 6.0.1, где полезная нагрузка
    инсталлятора доступна для чтения. У 6.2.0 и 6.4.0 она упакована, и набор
    панелей может отличаться — тогда скрипт сообщит о неудаче и назовёт файл
    для ручного запуска.
#>

[CmdletBinding()]
param(
    # Куда ставить CubeMX. По умолчанию рядом с уже установленным STM32CubeIDE.
    [string]$InstallRoot = 'C:\ST',
    # Ограничиться отдельными версиями, например: -Only 6.4.0
    [string[]]$Only,
    # Не ставить JRE, даже если она не найдена.
    [switch]$SkipJre
)

$ErrorActionPreference = 'Stop'
$toolsRoot = $PSScriptRoot
$cubeRoot = Join-Path $toolsRoot 'installers\cubemx'

# --- права администратора -------------------------------------------------
$id = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = New-Object Security.Principal.WindowsPrincipal($id)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host 'Нужны права администратора.' -ForegroundColor Red
    Write-Host 'Запустите PowerShell от имени администратора и повторите:'
    Write-Host '  powershell -ExecutionPolicy Bypass -File tools\install-cubemx.ps1'
    exit 1
}

# --- 1. Java Runtime Environment -----------------------------------------
function Test-RegisteredJre {
    foreach ($key in @(
        'HKLM:\SOFTWARE\JavaSoft\Java Runtime Environment',
        'HKLM:\SOFTWARE\JavaSoft\JRE',
        'HKLM:\SOFTWARE\WOW6432Node\JavaSoft\Java Runtime Environment'
    )) {
        if (Test-Path $key) {
            $sub = Get-ChildItem $key -ErrorAction SilentlyContinue
            if ($sub) { return $true }
        }
    }
    return $false
}

Write-Host ''
Write-Host '=== Java Runtime Environment ===' -ForegroundColor Cyan

if (Test-RegisteredJre) {
    Write-Host '  JRE уже зарегистрирована в реестре'
} elseif ($SkipJre) {
    Write-Host '  JRE не найдена, но установка пропущена по ключу -SkipJre' -ForegroundColor Yellow
} else {
    $msi = Get-ChildItem -Path (Join-Path $toolsRoot 'installers') -Filter 'OpenJDK8U-jre_x64_windows_hotspot_*.msi' |
           Select-Object -First 1
    if (-not $msi) {
        Write-Host '  НЕТ ФАЙЛА: tools\installers\OpenJDK8U-jre_x64_windows_hotspot_*.msi' -ForegroundColor Red
        Write-Host '  Источник указан в tools\manifest.json (позиция jre8).'
        exit 1
    }

    Write-Host "  установка $($msi.Name) ..."
    # FeatureOracleJavaSoft создаёт ключи реестра HKLM\SOFTWARE\JavaSoft,
    # по которым лаунчер CubeMX и ищет JRE.
    $features = 'FeatureMain,FeatureEnvironment,FeatureJarFileRunWith,FeatureJavaHome,FeatureOracleJavaSoft'
    $log = Join-Path $env:TEMP 'jre8-install.log'
    $p = Start-Process -FilePath 'msiexec.exe' -Wait -PassThru -NoNewWindow -ArgumentList @(
        '/i', "`"$($msi.FullName)`"", '/qn', '/norestart', "ADDLOCAL=$features", '/l*v', "`"$log`""
    )
    if ($p.ExitCode -ne 0) {
        Write-Host "  msiexec завершился с кодом $($p.ExitCode); журнал: $log" -ForegroundColor Red
        exit 1
    }
    if (Test-RegisteredJre) {
        Write-Host '  готово, JRE зарегистрирована' -ForegroundColor Green
    } else {
        Write-Host '  msiexec отработал, но ключей JavaSoft в реестре нет' -ForegroundColor Yellow
        Write-Host "  журнал: $log"
    }
}

# --- 2. STM32CubeMX -------------------------------------------------------
$versions = @(
    @{ Version = '6.0.1'; Exe = '6.0.1\SetupSTM32CubeMX-6.0.1.exe' }
    @{ Version = '6.2.0'; Exe = '6.2.0\SetupSTM32CubeMX-6.2.0-Win.exe' }
    @{ Version = '6.4.0'; Exe = '6.4.0\SetupSTM32CubeMX-6.4.0-Win.exe' }
)
if ($Only) { $versions = $versions | Where-Object { $Only -contains $_.Version } }

$ok = @(); $failed = @()

foreach ($v in $versions) {
    $ver = $v.Version
    $exe = Join-Path $cubeRoot $v.Exe
    $xml = Join-Path $cubeRoot "auto-install-$ver.xml"
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
