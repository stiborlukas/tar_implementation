#!/bin/bash

# Funkce pro testování
run_test() {
    echo "=== Test: $1 ==="
    echo "$ $2"
    eval "$2"
    echo "Exit code: $?"
    echo ""
}

# 1. Základní výpis
run_test "Základní výpis" "./mytar -f files/normal.tar -t"

# 2. Výpis konkrétních souborů
run_test "Výpis konkrétních souborů" "./mytar -f files/normal.tar -t file1 file4"

# 3. Výpis neexistujících souborů
run_test "Výpis neexistujících souborů" "./mytar -f files/normal.tar -t file6 file1"

# 4. Testování chybějících voleb
run_test "Bez voleb" "./mytar"
run_test "Bez -f" "./mytar -t"
run_test "Bez -t" "./mytar -f files/normal.tar"

# 5. Neplatné volby
run_test "Neplatná volba" "./mytar -f files/normal.tar -X"

# 6. Neexistující soubor archivu
run_test "Neexistující archiv" "./mytar -f nonexistent.tar -t"

# 7. Nepodporovaný typ hlavičky
run_test "Archiv s adresářem" "./mytar -f files/dir.tar -t"

# 8. Neúplný archiv
run_test "Neúplný archiv" "./mytar -f files/partial.tar -t"

# 9. Jeden chybějící nulový blok
run_test "Jeden chybějící nulový blok" "./mytar -f files/one-zero-block-missing.tar -t"

# 10. Testování velkého souboru (base-256 kódování)
run_test "Velký soubor" "./mytar -f files/big.tar -t"
