all: hashgen gen_search search

hashgen: hashgen.c common.c
	gcc -o hashgen hashgen.c

gen_search: generate_search.c common.c
	gcc -o gen_search generate_search.c

search: search.c common.c
	gcc -o search search.c

clean:
	rm -f hashgen gen_search search