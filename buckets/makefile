all: hashgen gen_search search sort

hashgen: hashgen.c common.c
	gcc -o hashgen hashgen.c

gen_search: generate_search.c common.c
	gcc -o gen_search generate_search.c

search: search.c common.c
	gcc -o search search.c

sort: sort.c common.c
	gcc -o sort sort.c

clean:
	rm -f hashgen gen_search search sort testsort