CC = g++
RM = rm
#Nom du programme
PROG=prog
#Nom du dossier source
srcdir=src/
#Nom du dossier bin
bindir=bin/
#Nom du dossier doc
docdir=doc/
#Nom du dossier sauvegarde
savedir=save/
#Récupère tous les fichiers .cpp
SRC=$(wildcard $(srcdir)*.cpp)
#Crée la liste des fichiers .o à partir des .cpp
OBJ=$(subst $(srcdir), $(bindir),$(SRC:.cpp=.o))
#Récupère tous les fichiers .hpp
HEAD=$(wildcard $(srcdir)*.hpp)

all : $(PROG)

#Compile le programme à partir des fichiers objets
$(PROG) : $(OBJ) 
	$(CC) -Wall $^ -o $@ -lm -lpthread 

#Crée les fichiers objets à partir des fichiers .cpp et .hpp
$(bindir)%.o : $(srcdir)%.cpp $(srcdir)%.hpp 
	$(CC) -Wall -std=c++14 -c $< -o $@ -lpthread 


.PHONY : clean

clean :			#Supprime les fichiers objets et le fichier programme
	$(RM) $(PROG) $(OBJ)

.PHONY : save

save :			#Crée un backup des fichiers sources dans le dosier de sauvegarde
	cp $(srcdir)*.cpp $(savedir)
	cp $(srcdir)*.hpp $(savedir)

.PHONY : retore

restore :			#restaure les fichiers sources depuis le dossier de sauvegarde
	cp $(savedir)*.cpp $(srcdir)
	cp $(savedir)*.hpp $(srcdir)

.PHONY : give

give :		#Créer une archive contenant les fichiers sources, les docs, et le Makefile
	mkdir martinetch-tp1
	cp Makefile martinetch-tp1/
	cp -r src/ martinetch-tp1/
	cp -r bin/ martinetch-tp1/
	cp README martinetch-tp1/
	tar czvf martinetch-tp1.tar.gz martinetch-tp1/

.PHONY : init

init :
	mkdir save/ doc/ src/ bin/
	touch src/main.cpp #notre programme de test
	touch src/main.hpp
	touch src/Vecteur.cpp
	touch src/Vecteur.hpp
