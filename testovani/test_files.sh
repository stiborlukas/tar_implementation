#!/bin/bash

# Vytvoření testovací složky
mkdir -p files
cd files

# Vytvoření normálního archivu
touch file1 file2 file3 file4
tar -f normal.tar -c file1 file2 file3 file4

# Vytvoření archivu s velkým souborem (pro testování base-256 kódování velikosti)
dd if=/dev/zero of=bigfile bs=1M count=100 2>/dev/null
tar -f big.tar -c bigfile

# Vytvoření archivu s adresářem (pro testování nepodporovaného typu)
mkdir testdir
tar -f dir.tar -c testdir

# Vytvoření neúplného archivu (pro testování neočekávaného EOF)
tar -f temp.tar -c file1
dd if=temp.tar of=partial.tar bs=512 count=2 2>/dev/null

# Vytvoření archivu s jedním chybějícím nulovým blokem
tar -f temp.tar -c file1
BLOCKS=$(($(wc -c < temp.tar)/512 - 1))
dd if=temp.tar of=one-zero-block-missing.tar bs=512 count=$BLOCKS 2>/dev/null

# Úklid dočasných souborů
rm -f temp.tar bigfile
rm -rf testdir

cd ..
