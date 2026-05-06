#!/usr/bin/env bash
set -u

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

EXEC='./practica2SG'

if [[ ! -x "$EXEC" ]]; then
  echo -e "${RED}Error:${NC} no existe o no es ejecutable: $EXEC"
  echo "Compila antes (por ejemplo: make -j4)."
  exit 1
fi

declare -a TEST_LEVEL=()
declare -a TEST_ID=()
declare -a TEST_CMD=()
declare -A LEVEL_COUNT=([0]=0 [1]=0 [2]=0 [3]=0 [4]=0 [5]=0 [6]=0)

add_test() {
  local lvl="$1"
  local cmd="$2"
  LEVEL_COUNT[$lvl]=$((LEVEL_COUNT[$lvl] + 1))
  TEST_LEVEL+=("$lvl")
  TEST_ID+=("N${lvl}.${LEVEL_COUNT[$lvl]}")
  TEST_CMD+=("$cmd")
}

# ========================== TESTS DEFINIDOS ==========================
while IFS='|' read -r lvl cmd; do
  [[ -z "${lvl// }" ]] && continue
  add_test "$lvl" "$cmd"
done <<'TESTS'
0|./practica2SG -m ./mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 7 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa30.map -n 0 -i 24 10 2 -t 17 17 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/2ez.map -n 0 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa30.map -n 0 -i 16 9 2 -t 16 14 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa50.map -n 0 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa75.map -n 0 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 0 -i 34 39 7 -t 13 34 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/bosque_venenoso.map -n 0 -i 12 21 1 -t 12 3 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/gemini2.map -n 0 -i 19 26 3 -t 22 23 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/2ez.map -n 0 -i 10 16 3 -t 25 18 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/mapaop.map -n 0 -i 15 55 0 -t 15 33 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/luminalia25.map -n 0 -i 55 55 4 -t 56 46 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/minivertiguito.map -n 0 -i 20 23 1 -t 6 8 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/minivertiguito.map -n 0 -i 8 10 5 -t 6 8 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/F_islas.map -n 0 -i 59 56 2 -t 59 57 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
0|./practica2SG -m ./mapas/F_islas.map -n 0 -i 12 3 2 -t 92 88 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/minivertiguito.map -n 1 -i 7 4 0 -t 24 24 7 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
1|./practica2SG -m ./mapas/gemini2.map -n 1 -i 20 8 0 -t 13 13 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa30.map -n 1 -i 16 7 7 -t 17 16 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa50.map -n 1 -i 38 10 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 1 -i 45 25 0 -t 45 26 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/2ez.map -n 1 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/bosque_prohibido.map -n 1 -i 4 11 0 -t 9 10 2 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
1|./practica2SG -m ./mapas/2ez.map -n 1 -i 19 26 3 -t 11 27 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 1000
1|./practica2SG -m ./mapas/chess.map -n 1 -i 38 41 2 -t 31 44 3 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/nuevo_mapa40.map -n 1 -i 33 33 3 -t 33 35 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapaop.map -n 1 -i 69 63 6 -t 24 67 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa75.map -n 1 -i 69 3 2 -t 40 24 6 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa75_espirales.map -n 1 -i 6 14 2 -t 66 71 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/islas_cambio_climatico.map -n 1 -i 47 54 0 -t 47 55 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
1|./practica2SG -m ./mapas/mapa100.map -n 1 -i 47 54 0 -t 48 54 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000
2|./practica2SG -m ./mapas/2ez.map -n 2 -i 23 19 2 -t 14 25 6 -seed 0 -Tiempo 3000 -Ambiental 2219 -Energia 8445 -O 14 16
2|./practica2SG -m ./mapas/mapaop.map -n 2 -i 69 62 5 -t 20 45 6 -seed 0 -Tiempo 3000 -Ambiental 674 -Energia 5941 -O 53 4
2|./practica2SG -m ./mapas/vertigo.map -n 2 -i 12 28 6 -t 87 45 3 -seed 0 -Tiempo 3000 -Ambiental 2752 -Energia 9316 -O 82 11
2|./practica2SG -m ./mapas/mapa75.map -n 2 -i 55 30 3 -t 12 53 2 -seed 0 -Tiempo 3000 -Ambiental 1919 -Energia 9247 -O 51 53
2|./practica2SG -m ./mapas/scape26.map -n 2 -i 20 15 0 -t 22 12 1 -seed 0 -Tiempo 3000 -Ambiental 2303 -Energia 7662 -O 22 21
2|./practica2SG -m ./mapas/mapa75_espirales.map -n 2 -i 24 68 1 -t 23 22 1 -seed 0 -Tiempo 3000 -Ambiental 2222 -Energia 9163 -O 24 66
2|./practica2SG -m ./mapas/mapa75.map -n 2 -i 35 35 6 -t 49 19 5 -seed 0 -Tiempo 3000 -Ambiental 60 -Energia 4920 -O 51 35
2|./practica2SG -m ./mapas/parchis.map -n 2 -i 81 53 7 -t 40 62 5 -seed 0 -Tiempo 3000 -Ambiental 674 -Energia 7897 -O 50 51
2|./practica2SG -m ./mapas/mapaop.map -n 2 -i 51 42 6 -t 69 39 5 -seed 0 -Tiempo 3000 -Ambiental 1510 -Energia 7923 -O 65 15
2|./practica2SG -m ./mapas/mapa50.map -n 2 -i 10 16 0 -t 15 38 5 -seed 0 -Tiempo 3000 -Ambiental 885 -Energia 5036 -O 41 37
2|./practica2SG -m ./mapas/vertigo.map -n 2 -i 15 27 2 -t 26 58 2 -seed 0 -Tiempo 3000 -Ambiental 1701 -Energia 4866 -O 26 41
2|./practica2SG -m ./mapas/F_islas.map -n 2 -i 51 10 2 -t 56 86 6 -seed 0 -Tiempo 3000 -Ambiental 1225 -Energia 9135 -O 68 82
2|./practica2SG -m ./mapas/luminalia25.map -n 2 -i 49 32 1 -t 90 59 0 -seed 0 -Tiempo 3000 -Ambiental 751 -Energia 9954 -O 33 43
2|./practica2SG -m ./mapas/mapa30.map -n 2 -i 16 12 1 -t 24 18 3 -seed 0 -Tiempo 3000 -Ambiental 2687 -Energia 6375 -O 5 21
2|./practica2SG -m ./mapas/mapa30_26.map -n 2 -i 16 23 3 -t 16 20 7 -seed 0 -Tiempo 3000 -Ambiental 2624 -Energia 9779 -O 25 17
2|./practica2SG -m ./mapas/islas_cambio_climatico.map -n 2 -i 25 96 3 -t 61 27 6 -seed 0 -Tiempo 3000 -Ambiental 145 -Energia 8847 -O 56 76
2|./practica2SG -m ./mapas/mapa75_espirales.map -n 2 -i 25 47 2 -t 27 10 6 -seed 0 -Tiempo 3000 -Ambiental 1171 -Energia 8666 -O 16 41
3|./practica2SG -m ./mapas/mapa30.map -n 3 -i 5 55 2 -t 10 10 4 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 12 5
3|./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Tiempo 3000 -Ambiental 2362 -Energia 8505 -O 31 54
3|./practica2SG -m ./mapas/mapa75.map -n 3 -i 5 5 2 -t 8 5 2 -seed 0 -Tiempo 3000 -Ambiental 2362 -Energia 8505 -O 31 50
3|./practica2SG -m ./mapas/minivertiguito.map -n 3 -i 14 17 7 -t 5 12 3 -seed 0 -Tiempo 3000 -Ambiental 1747 -Energia 7118 -O 25 16
3|./practica2SG -m ./mapas/islas25.map -n 3 -i 11 7 3 -t 49 58 2 -seed 0 -Tiempo 3000 -Ambiental 2400 -Energia 5947 -O 55 68
3|./practica2SG -m ./mapas/paldea25.map -n 3 -i 88 18 3 -t 70 49 4 -seed 0 -Tiempo 3000 -Ambiental 861 -Energia 4664 -O 69 71
3|./practica2SG -m ./mapas/mapa30.map -n 3 -i 17 17 3 -t 19 19 5 -seed 0 -Tiempo 3000 -Ambiental 2461 -Energia 3813 -O 22 6
3|./practica2SG -m ./mapas/vertigo.map -n 3 -i 52 44 2 -t 50 13 3 -seed 0 -Tiempo 3000 -Ambiental 2812 -Energia 5966 -O 42 8
3|./practica2SG -m ./mapas/parchis.map -n 3 -i 29 34 0 -t 34 11 6 -seed 0 -Tiempo 3000 -Ambiental 476 -Energia 4331 -O 22 29
3|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 3 -i 13 6 3 -t 27 38 4 -seed 0 -Tiempo 3000 -Ambiental 2533 -Energia 8505 -O 34 28
3|./practica2SG -m ./mapas/mapa30_26.map -n 3 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 7342 -O 26 3
3|./practica2SG -m ./mapas/luminalia25.map -n 3 -i 22 50 1 -t 61 41 7 -seed 0 -Tiempo 3000 -Ambiental 2818 -Energia 4312 -O 12 96
3|./practica2SG -m ./mapas/islas_cambio_climatico.map -n 3 -i 87 23 4 -t 65 17 0 -seed 0 -Tiempo 3000 -Ambiental 2922 -Energia 6305 -O 4 28
3|./practica2SG -m ./mapas/minivertiguito.map -n 3 -i 11 19 1 -t 25 9 1 -seed 0 -Tiempo 3000 -Ambiental 2109 -Energia 5874 -O 11 13
3|./practica2SG -m ./mapas/mapaop.map -n 3 -i 56 45 0 -t 51 38 3 -seed 0 -Tiempo 3000 -Ambiental 2796 -Energia 9842 -O 11 28
3|./practica2SG -m ./mapas/mapa30.map -n 3 -i 21 9 6 -t 21 10 2 -seed 0 -Tiempo 3000 -Ambiental 200 -Energia 4054 -O 9 24
3|./practica2SG -m ./mapas/luminalia25.map -n 3 -i 51 30 0 -t 32 71 6 -seed 0 -Tiempo 3000 -Ambiental 1834 -Energia 5531 -O 58 38
4|./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
4|./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
4|./practica2SG -m ./mapas/minivertiguito.map -n 4 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
4|./practica2SG -m ./mapas/mapa30_26.map -n 4 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
4|./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3
4|./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 22 13
4|./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 3000 -Energia 3000 -O 22 13
4|./practica2SG -m ./mapas/mapaop.map -n 4 -i 48 43 1 -t 52 38 1 -seed 0 -Tiempo 3000 -Ambiental 2280 -Energia 3403 -O 26 22
4|./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 1550 -Energia 7422 -O 5 23
4|./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 19 15
4|./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 18 15
4|./practica2SG -m ./mapas/mapa75_espirales.map -n 4 -i 56 10 4 -t 52 30 4 -seed 0 -Tiempo 3000 -Ambiental 1700 -Energia 3648 -O 29 24
4|./practica2SG -m ./mapas/luminalia25.map -n 4 -i 83 62 0 -t 39 75 0 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 7552 -O 63 61
4|./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 900 -Energia 3000 -O 8 24
4|./practica2SG -m ./mapas/gemini2.map -n 4 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 8 24
4|./practica2SG -m ./mapas/mapaop.map -n 4 -i 48 43 1 -t 52 38 1 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3403 -O 26 22
4|./practica2SG -m ./mapas/mapa30.map -n 4 -i 26 12 5 -t 23 4 4 -seed 0 -Tiempo 3000 -Ambiental 650 -Energia 7422 -O 16 8
5|./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
5|./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
5|./practica2SG -m ./mapas/minivertiguito.map -n 5 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
5|./practica2SG -m ./mapas/mapa30_26.map -n 5 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
5|./practica2SG -m ./mapas/vertigo.map -n 5 -i 80 75 2 -t 56 24 5 -seed 0 -Tiempo 3000 -Ambiental 2688 -Energia 9517 -O 4 36
5|./practica2SG -m ./mapas/mapa30.map -n 5 -i 25 20 6 -t 26 23 6 -seed 0 -Tiempo 3000 -Ambiental 1804 -Energia 6361 -O 14 17
5|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 5 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 1533 -Energia 4092 -O 30 24
5|./practica2SG -m ./mapas/mapa75_espirales.map -n 5 -i 69 69 7 -t 3 69 5 -seed 0 -Tiempo 3000 -Ambiental 1417 -Energia 4150 -O 16 10
5|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 5 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 3533 -Energia 4092 -O 46 18
5|./practica2SG -m ./mapas/mapa75.map -n 5 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 865 -Energia 5574 -O 52 32
5|./practica2SG -m ./mapas/mapa100.map -n 5 -i 13 33 3 -t 82 73 6 -seed 0 -Tiempo 3000 -Ambiental 1719 -Energia 4581 -O 25 61
5|./practica2SG -m ./mapas/mapa75.map -n 5 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 5574 -O 48 16
5|./practica2SG -m ./mapas/mapaop.map -n 5 -i 42 36 1 -t 42 37 1 -seed 0 -Tiempo 3000 -Ambiental 2280 -Energia 3403 -O 26 22
5|./practica2SG -m ./mapas/islas_cambio_climatico.map -n 5 -i 27 94 7 -t 52 92 4 -seed 0 -Tiempo 3000 -Ambiental 2107 -Energia 4383 -O 11 10
5|./practica2SG -m ./mapas/mapa50.map -n 5 -i 17 27 6 -t 41 23 1 -seed 0 -Tiempo 3000 -Ambiental 2836 -Energia 3699 -O 32 16
5|./practica2SG -m ./mapas/gemini2.map -n 5 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3
5|./practica2SG -m ./mapas/luminalia25.map -n 5 -i 83 62 0 -t 39 75 0 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 7552 -O 63 61
6|./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 648 -Energia 3000 -O 19 11
6|./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 17 22
6|./practica2SG -m ./mapas/minivertiguito.map -n 6 -i 16 8 3 -t 26 9 0 -seed 0 -Tiempo 3000 -Ambiental 1000 -Energia 3000 -O 3 3
6|./practica2SG -m ./mapas/mapa30_26.map -n 6 -i 13 26 5 -t 16 9 6 -seed 0 -Tiempo 3000 -Ambiental 2364 -Energia 3000 -O 26 3
6|./practica2SG -m ./mapas/vertigo.map -n 6 -i 80 75 2 -t 56 24 5 -seed 0 -Tiempo 3000 -Ambiental 2688 -Energia 9517 -O 4 36
6|./practica2SG -m ./mapas/mapa30.map -n 6 -i 25 20 6 -t 26 23 6 -seed 0 -Tiempo 3000 -Ambiental 1804 -Energia 6361 -O 14 17
6|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 1533 -Energia 4092 -O 30 24
6|./practica2SG -m ./mapas/mapa75_espirales.map -n 6 -i 69 69 7 -t 3 69 5 -seed 0 -Tiempo 3000 -Ambiental 1417 -Energia 4150 -O 16 10
6|./practica2SG -m ./mapas/mapa50_cuadricula.map -n 6 -i 25 38 6 -t 15 17 1 -seed 0 -Tiempo 3000 -Ambiental 3533 -Energia 4092 -O 46 18
6|./practica2SG -m ./mapas/mapa75.map -n 6 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 865 -Energia 5574 -O 52 32
6|./practica2SG -m ./mapas/mapa100.map -n 6 -i 13 33 3 -t 82 73 6 -seed 0 -Tiempo 3000 -Ambiental 1719 -Energia 4581 -O 25 61
6|./practica2SG -m ./mapas/mapa75.map -n 6 -i 29 68 6 -t 14 52 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 5574 -O 48 16
6|./practica2SG -m ./mapas/mapaop.map -n 6 -i 42 36 1 -t 42 37 1 -seed 0 -Tiempo 3000 -Ambiental 2280 -Energia 3403 -O 26 22
6|./practica2SG -m ./mapas/islas_cambio_climatico.map -n 6 -i 27 94 7 -t 52 92 4 -seed 0 -Tiempo 3000 -Ambiental 2107 -Energia 4383 -O 11 10
6|./practica2SG -m ./mapas/mapa50.map -n 6 -i 17 27 6 -t 41 23 1 -seed 0 -Tiempo 3000 -Ambiental 2836 -Energia 3699 -O 32 16
6|./practica2SG -m ./mapas/gemini2.map -n 6 -i 10 21 1 -t 20 4 2 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 3000 -O 26 3
6|./practica2SG -m ./mapas/luminalia25.map -n 6 -i 83 62 0 -t 39 75 0 -seed 0 -Tiempo 3000 -Ambiental 1500 -Energia 7552 -O 63 61
TESTS
# ====================================================================

choose_levels() {
  local option
  echo -e "${BLUE}Selecciona modo de ejecución:${NC}" >&2
  echo "1) Ejecutar todos los niveles (0-6)" >&2
  echo "   -> Lanza todos los tests definidos en el script para los niveles 0,1,2,3,4,5 y 6." >&2
  echo "2) Ejecutar un nivel específico" >&2
  echo "   -> Lanza únicamente los tests del nivel que indiques (por ejemplo, solo nivel 2)." >&2
  echo "3) Ejecutar una lista de niveles (ej: 0,1,4)" >&2
  echo "   -> Lanza los tests de varios niveles concretos, separados por comas." >&2
  read -r -p "Opción [1-3]: " option

  case "$option" in
    1)
      echo "0 1 2 3 4 5 6"
      ;;
    2)
      read -r -p "Nivel [0-6]: " lv
      if [[ ! "$lv" =~ ^[0-6]$ ]]; then
        echo -e "${RED}Nivel inválido.${NC}" >&2
        return 1
      fi
      echo "$lv"
      ;;
    3)
      read -r -p "Lista de niveles (separados por coma): " lvls
      lvls="${lvls//,/ }"
      local out=()
      for x in $lvls; do
        [[ "$x" =~ ^[0-6]$ ]] && out+=("$x")
      done
      if [[ ${#out[@]} -eq 0 ]]; then
        echo -e "${RED}No hay niveles válidos.${NC}" >&2
        return 1
      fi
      # únicos
      awk '!seen[$0]++' <<<"${out[*]}" | tr '\n' ' '
      ;;
    *)
      echo -e "${RED}Opción inválida.${NC}" >&2
      return 1
      ;;
  esac
}

classify_fail_reason() {
  local out="$1"
  local ec="$2"

  if grep -Eqi "CPU|tiempo de cómputo|tiempo de computo|timeout|SIGXCPU" <<<"$out"; then
    echo "Fallo por CPU"
    return
  fi

  if grep -Eqi "Se agotaron los instantes de simulacion|Instantes consumidos: *3000" <<<"$out"; then
    echo "Fallo por Instantes"
    return
  fi

  if grep -Eqi "\[Req\]|Req\]|requerid|eficiencia" <<<"$out"; then
    echo "Fallo por Eficiencia"
    return
  fi

  if [[ "$ec" -ne 0 ]]; then
    echo "Error de ejecución (exit $ec)"
  else
    echo "Fallo (motivo no identificado)"
  fi
}

is_success() {
  local out="$1"
  # primero descartamos casos explícitos de no completado
  if grep -Eqi "NO completado|No completado" <<<"$out"; then
    return 1
  fi
  grep -Eqi "completado con Exito|Conexion de tuberias establecida" <<<"$out"
}

main() {
  local selected
  if ! selected="$(choose_levels)"; then
    exit 1
  fi

  declare -A RUN_LVL=()
  for lv in $selected; do RUN_LVL["$lv"]=1; done

  declare -a IDX_TO_RUN=()
  for i in "${!TEST_LEVEL[@]}"; do
    [[ -n "${RUN_LVL[${TEST_LEVEL[$i]}]:-}" ]] && IDX_TO_RUN+=("$i")
  done

  if [[ ${#IDX_TO_RUN[@]} -eq 0 ]]; then
    echo -e "${RED}No hay tests para esa selección.${NC}"
    exit 1
  fi

  local total="${#IDX_TO_RUN[@]}"
  local cur=0

  declare -a PASSED_IDS=()
  declare -a FAILED_LINES=()

  echo -e "\n${YELLOW}Ejecutando $total tests...${NC}\n"

  for idx in "${IDX_TO_RUN[@]}"; do
    cur=$((cur + 1))
    local id="${TEST_ID[$idx]}"
    local cmd="${TEST_CMD[$idx]}"

    printf "[%03d/%03d] %-8s " "$cur" "$total" "$id"

    local out
    out="$(eval "$cmd" 2>&1)"
    local ec=$?

    if is_success "$out"; then
      echo -e "${GREEN}OK${NC}"
      PASSED_IDS+=("$id")
    else
      local reason
      reason="$(classify_fail_reason "$out" "$ec")"
      echo -e "${RED}FAIL${NC} - $reason"
      FAILED_LINES+=("$id -> $reason")
    fi
  done

  echo
  echo -e "${GREEN}===== LISTA DE TESTS PASADOS =====${NC}"
  if [[ ${#PASSED_IDS[@]} -eq 0 ]]; then
    echo "(ninguno)"
  else
    printf '%s\n' "${PASSED_IDS[@]}"
  fi

  echo
  echo -e "${RED}===== LISTA DE TESTS FALLIDOS =====${NC}"
  if [[ ${#FAILED_LINES[@]} -eq 0 ]]; then
    echo "(ninguno)"
  else
    printf '%s\n' "${FAILED_LINES[@]}"
  fi

  echo
  read -r -p "¿Guardar resumen en calificacion.txt? (s/N): " save
  if [[ "$save" =~ ^[sS]$ ]]; then
    {
      echo "===== LISTA DE TESTS PASADOS ====="
      if [[ ${#PASSED_IDS[@]} -eq 0 ]]; then
        echo "(ninguno)"
      else
        printf '%s\n' "${PASSED_IDS[@]}"
      fi
      echo
      echo "===== LISTA DE TESTS FALLIDOS ====="
      if [[ ${#FAILED_LINES[@]} -eq 0 ]]; then
        echo "(ninguno)"
      else
        printf '%s\n' "${FAILED_LINES[@]}"
      fi
    } > calificacion.txt
    echo -e "${BLUE}Resumen guardado en calificacion.txt${NC}"
  fi
}

main "$@"
