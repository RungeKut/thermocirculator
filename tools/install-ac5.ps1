<#
.SYNOPSIS
    Ставит Arm Compiler 5.06u7 в существующую установку Keil MDK.

.DESCRIPTION
    MDK 5.37 и новее больше не поставляются с Arm Compiler 5, а оба ST-проекта
    требуют именно его: в их .uvprojx стоит <uAC6>0</uAC6>. Без него сборка
    обрывается на «uses ARM-Compiler 'Default Compiler Version 5' which is
    not available».

    Сам компилятор при этом продолжает работать в MDK 5.39 — его достаточно
    положить рядом и зарегистрировать в TOOLS.INI.

    Скрипт распаковывает ARMCC из комплекта в <Keil>\ARM\ARMCC и добавляет
    строку ARMCCPATH в <Keil>\TOOLS.INI. Существующий TOOLS.INI сохраняется
    как TOOLS.INI.bak.

.NOTES
    Архив tools\installers\ARMCC-5.06u7.7z собран из дистрибутива Keil MDK 5.36
    (последняя версия, содержавшая Arm Compiler 5). Распаковать дистрибутив
    можно без установки:

        7z x KeilMDK536.exe -oРАСПАКОВКА
        # компилятор окажется в РАСПАКОВКА\ARM\ARMCC

    Лицензирование MDK — на стороне владельца проекта. Компилятор проверяет
    лицензию при запуске и без неё работать не будет.

    Проверка успеха — совпадение размера эталонной прошивки:
    STM32L072 = 27 796 байт, STM32L071 = 27 544 байта.
#>

[CmdletBinding()]
param(
    [string]$KeilRoot = 'C:\Keil_v5',
    [string]$SevenZip = 'C:\Program Files\7-Zip\7z.exe'
)

$ErrorActionPreference = 'Stop'

$armDir  = Join-Path $KeilRoot 'ARM\ARMCC'
$toolsIni = Join-Path $KeilRoot 'TOOLS.INI'
$archive = Join-Path $PSScriptRoot 'installers\ARMCC-5.06u7.7z'
$entry   = 'ARMCCPATH1=".\ARMCC" ("V5.06 update 7 (build 960)")'

Write-Host ''
Write-Host '=== Arm Compiler 5.06u7 ===' -ForegroundColor Cyan

if (-not (Test-Path $toolsIni)) {
    Write-Host "  не найден $toolsIni — проверьте -KeilRoot" -ForegroundColor Red
    exit 1
}

# --- 1. распаковка --------------------------------------------------------
if (Test-Path (Join-Path $armDir 'bin\armcc.exe')) {
    Write-Host '  компилятор уже распакован'
} else {
    if (-not (Test-Path $archive)) {
        Write-Host "  НЕТ ФАЙЛА: $archive" -ForegroundColor Red
        Write-Host '  Источник указан в tools\manifest.json (позиция ac5).'
        exit 1
    }
    if (-not (Test-Path $SevenZip)) {
        Write-Host "  не найден 7-Zip: $SevenZip" -ForegroundColor Red
        exit 1
    }
    Write-Host "  распаковка в $armDir ..."
    & $SevenZip x -y -o"$(Join-Path $KeilRoot 'ARM')" $archive | Out-Null
    if (-not (Test-Path (Join-Path $armDir 'bin\armcc.exe'))) {
        Write-Host '  распаковка не дала armcc.exe' -ForegroundColor Red
        exit 1
    }
    Write-Host '  распакован' -ForegroundColor Green
}

# --- 2. регистрация в TOOLS.INI ------------------------------------------
$ini = Get-Content $toolsIni -Raw -Encoding Default

if ($ini -match [regex]::Escape('".\ARMCC"')) {
    Write-Host '  уже зарегистрирован в TOOLS.INI'
} else {
    Copy-Item $toolsIni "$toolsIni.bak" -Force
    # Встраиваем сразу после строки регистрации Arm Compiler 6.
    $ini = $ini -replace '(?m)^(ARMCCPATH0=.*)$', "`$1`r`n$entry"
    if ($ini -notmatch [regex]::Escape('".\ARMCC"')) {
        Write-Host '  в TOOLS.INI нет строки ARMCCPATH0, добавить не удалось' -ForegroundColor Red
        Write-Host "  добавьте вручную в секцию [ARM]:  $entry"
        exit 1
    }
    Set-Content $toolsIni -Value $ini -Encoding Default -NoNewline
    Write-Host '  зарегистрирован в TOOLS.INI (прежний сохранён как TOOLS.INI.bak)' -ForegroundColor Green
}

# --- 3. проверка ----------------------------------------------------------
Write-Host ''
Write-Host 'Проверьте сборкой эталонного проекта:' -ForegroundColor Cyan
Write-Host '  & "C:\Keil_v5\UV4\UV4.exe" -j0 -b STM32L072CBT6\MDK-ARM\STM32L072CBT6.uvprojx -o build.log'
Write-Host 'Размер должен совпасть с эталоном: Total ROM Size = 27 796 байт.'

exit 0
