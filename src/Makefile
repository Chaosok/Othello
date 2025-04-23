CC = gcc
CFLAGS = -Wall -Wextra -std=c99 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lSDL2_ttf -lm -lSDL2_image -lSDL2_mixer

SRC_DIR = .
OBJ_DIR = obj
TEST_DIR = ../test

SRC_FILES = \
	jeu/fonc_jeu.c \
	jeu/fonc_sauv.c \
	graphique/plateau.c \
	IA/fonc_ia.c \
	graphique/menus.c \
	graphique/othelloGraph.c \
	jeu/reseau.c

OBJ = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

#
#
#

all: othello test

othello: $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#
# Tests
#

TESTS = test_changerJoueur test_score

test: $(TESTS)

test_changerJoueur: $(OBJ_DIR)/test_changerJoueur.o $(OBJ_DIR)/jeu/fonc_jeu.o
	$(CC) $^ -o $@

test_score: $(OBJ_DIR)/test_score.o $(OBJ_DIR)/jeu/fonc_jeu.o
	$(CC) $^ -o $@

$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c jeu/fonc_jeu.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

#
# Nettoyage
#

clean:
	rm -f $(OBJ_DIR)/*/*.o
	rm -f $(OBJ_DIR)/*.o

delete: clean
	rm -f othello $(TESTS)