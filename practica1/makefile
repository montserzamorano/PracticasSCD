## ---------------------------------------------------------------------
## SCD. Makefile para la práctica 1.
## ---------------------------------------------------------------------

.PHONY:  pcFIFO, pcLIFO, fu, msg, clean, todo
.SUFFIXES:

params_comp:= -g -Wall
libs:=

msg:
	@echo "usa 'make pcFIFO' para compilar y ejecutar el programa de los productores - consumidores usando una cola FIFO."
	@echo "usa 'make pcLIFO' para compilar y ejecutar el programa de los productores - consumidores usando una cola LIFO."
	@echo "usa 'make fu' para compilar y ejecutar el programa de los fumadores y el estanquero."
	@echo "usa 'make clean' para borrar todos los archivos creados al compilar."
	@echo "usa 'make todo' para recompilar (si es necesario) los dos programas."

## ---------------------------------------------------------------------

pcFIFO: prod-consFIFO
	./prod-consFIFO

prod-consFIFO: prod-consFIFO.cpp
	g++ -o prod-consFIFO $(params_comp)  prod-consFIFO.cpp -lpthread

## ---------------------------------------------------------------------

pcLIFO: prod-consLIFO
	./prod-consLIFO

prod-consLIFO: prod-consLIFO.cpp
	g++ -o prod-consLIFO $(params_comp)  prod-consLIFO.cpp -lpthread

## ---------------------------------------------------------------------
fu: fumadores
	./fumadores

fumadores: fumadores.cpp
	g++ -o fumadores $(params_comp) fumadores.cpp -lpthread

## ---------------------------------------------------------------------

todo:
	make fumadores
	make prod-consFIFO
	make prod-consLIFO

## ---------------------------------------------------------------------

clean:
	rm -rf *.o fumadores prod-consFIFO prod-consLIFO

## ---------------------------------------------------------------------
