all: ../proxy/a.out

a.out:
	gcc -g main.c src/cache.c src/proxy.c src/http_request.c src/threadpool.c -lpthread -o a.out

clean:
	rm -f *.o a.out