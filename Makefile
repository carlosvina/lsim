## Modulo:  Makefile
## Fecha:   8/2/12

.SUFFIXES: .o .c .cc


CCOMPILER = g++
CFLAGS   = -g  -Wall
CXXFLAGS = "-O3"
LIBS = -lm
SRCS = main.cc Definiciones.cc varaleat.cc Cola.cc
OBJECTS = main.o Promedio.o Definiciones.o varaleat.o Cola.o
EXEC = practica3


all: $(EXEC)

$(EXEC): $(OBJECTS)
	@echo [Creando ejecutable]
	$(CCOMPILER) $(CFLAGS) $(CXXFLAGS) -o $(EXEC) $(OBJECTS) $(LIBS)
	@chmod 755 $(EXEC)

.cc.o:
	@echo Compilando $<
	@$(CCOMPILER) $(CFLAGS) $(INCLUDE_PATH) -c $<
	@echo Hecho!!

clean:
	@echo [Haciendo limpieza]
	@rm -f *~ main.o Definiciones.o varaleat.o Cola.o core
	@rm -f $(EXEC) $(GENERADOS)



#end Makefile


             

