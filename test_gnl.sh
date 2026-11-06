#!/usr/bin/env bash
# Tests pour get_next_line (partie obligatoire).
# Usage : ./test_gnl.sh /chemin/vers/ton/repo_gnl
# À garder HORS du repo rendu.

GNL_DIR="${1:-.}"
HERE="$(cd "$(dirname "$0")" && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

GREEN='\033[32m'; RED='\033[31m'; YELLOW='\033[33m'; RESET='\033[0m'
PASS=0; FAIL=0

ok()   { printf "${GREEN}OK${RESET}  %s\n" "$1"; PASS=$((PASS + 1)); }
ko()   { printf "${RED}KO${RESET}  %s\n" "$1"; FAIL=$((FAIL + 1)); }
info() { printf "${YELLOW}%s${RESET}\n" "$1"; }

TIMEOUT=""
command -v timeout >/dev/null && TIMEOUT="timeout 10"

# ---------- Fichiers de test ----------
F="$TMP/files"; mkdir -p "$F"
: > "$F/vide.txt"
printf '\n' > "$F/un_saut.txt"
printf '\n\n\n\n' > "$F/que_des_sauts.txt"
printf 'hello\n' > "$F/une_ligne.txt"
printf 'hello' > "$F/une_ligne_sans_nl.txt"
printf 'hello\nworld' > "$F/fin_sans_nl.txt"
printf 'ligne1\nligne2\n\nligne4\n' > "$F/plusieurs.txt"
printf 'a\nbb\nccc\ndddd\neeeee\n' > "$F/courtes.txt"
{ head -c 20000 /dev/zero | tr '\0' 'x'; printf '\nshort\n'; } > "$F/ligne_longue.txt"
{ head -c 50000 /dev/zero | tr '\0' 'y'; } > "$F/longue_sans_nl.txt"
seq 1 10000 > "$F/gros_fichier.txt"
awk 'BEGIN { for (i = 1; i <= 300; i++) { s = ""; for (j = 0; j < (i * 37) % 200; j++) s = s "z"; print s } }' > "$F/longueurs_variees.txt"

SRCS="$GNL_DIR/get_next_line.c $GNL_DIR/get_next_line_utils.c"
CFLAGS="-Wall -Wextra -Werror"

# ---------- Compilation sans -D BUFFER_SIZE ----------
info "== Compilation sans -D BUFFER_SIZE =="
if cc $CFLAGS -I "$GNL_DIR" "$HERE/main_test.c" $SRCS -o "$TMP/gnl_default" 2> "$TMP/err.txt"; then
	ok "compile sans BUFFER_SIZE"
else
	ko "compile sans BUFFER_SIZE"; cat "$TMP/err.txt"
fi

# ---------- Tests par BUFFER_SIZE ----------
for BS in 1 2 3 5 42 1024 9999 10000000; do
	info "== BUFFER_SIZE=$BS =="
	BIN="$TMP/gnl_$BS"
	if ! cc $CFLAGS -D BUFFER_SIZE=$BS -I "$GNL_DIR" "$HERE/main_test.c" $SRCS -o "$BIN" 2> "$TMP/err.txt"; then
		ko "compilation"; cat "$TMP/err.txt"; continue
	fi

	for FILE in "$F"/*; do
		NAME="$(basename "$FILE")"

		$TIMEOUT "$BIN" "$FILE" > "$TMP/out.txt" 2> "$TMP/stderr.txt"
		CODE=$?
		if [ $CODE -eq 0 ] && cmp -s "$TMP/out.txt" "$FILE"; then
			ok "fichier   $NAME"
		else
			ko "fichier   $NAME (code $CODE)"; head -c 300 "$TMP/stderr.txt"
		fi

		$TIMEOUT "$BIN" < "$FILE" > "$TMP/out.txt" 2>/dev/null
		if [ $? -eq 0 ] && cmp -s "$TMP/out.txt" "$FILE"; then
			ok "stdin     $NAME"
		else
			ko "stdin     $NAME"
		fi

		cat "$FILE" | $TIMEOUT "$BIN" > "$TMP/out.txt" 2>/dev/null
		if [ $? -eq 0 ] && cmp -s "$TMP/out.txt" "$FILE"; then
			ok "pipe      $NAME"
		else
			ko "pipe      $NAME"
		fi
	done

	if $TIMEOUT "$BIN" --invalid 2> "$TMP/stderr.txt"; then
		ok "fd invalides"
	else
		ko "fd invalides"; cat "$TMP/stderr.txt"
	fi
done

# ---------- Fuites mémoire ----------
if command -v valgrind >/dev/null; then
	for BS in 1 42; do
		info "== Valgrind, BUFFER_SIZE=$BS =="
		BIN="$TMP/gnl_$BS"
		[ -x "$BIN" ] || continue
		for NAME in plusieurs.txt fin_sans_nl.txt vide.txt ligne_longue.txt; do
			if valgrind -q --leak-check=full --errors-for-leak-kinds=definite,indirect \
				--error-exitcode=42 "$BIN" "$F/$NAME" > /dev/null 2> "$TMP/vg.txt"; then
				ok "valgrind  $NAME"
			else
				ko "valgrind  $NAME"; head -n 20 "$TMP/vg.txt"
			fi
		done
	done
else
	info "valgrind absent : tests de fuites ignorés"
fi

echo
printf "Résultat : ${GREEN}%d OK${RESET}, ${RED}%d KO${RESET}\n" "$PASS" "$FAIL"
[ $FAIL -eq 0 ]
