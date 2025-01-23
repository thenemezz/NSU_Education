#define _GNU_SOURCE
#define _XOPEN_SOURCE 600
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ucontext.h>
#include <unistd.h>

#define STACK_SIZE 4096 * 64
#define PAGE_SIZE 4096
#define NAME_SIZE 16
#define MAX_THREADS 100

typedef struct uthread {
    char name[NAME_SIZE];
    void (*thread_func)(void *);
    void *arg;
    ucontext_t uctx;
    int finished;
    void *stack;
    void (*on_finish)(void *);
} uthread_t;

uthread_t *uthreads[MAX_THREADS];
int uthread_count = 0;
int uthread_cur = -1;

ucontext_t main_context;

void *create_stack(size_t size) {
    void *stack = mmap(NULL, size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (stack == MAP_FAILED) {
        perror("mmap stack failed");
        exit(1);
    }

    return stack;
}

void free_stack(void *stack, size_t size) {
    if (munmap(stack, size) == -1) {
        perror("munmap stack failed");
    }
}

int find_next_thread() {
    int active_threads[MAX_THREADS];
    int active_count = 0;

    for (int i = 0; i < uthread_count; i++) {
        if (uthreads[i] != NULL && !uthreads[i]->finished) {
            active_threads[active_count++] = i;
        }
    }
    if (active_count == 0) {
        printf("Scheduler: No active threads\n");
        return -1;
    }
    printf("Scheduler: Active threads:\n");
    for (int i = 0; i < active_count; i++) {
        printf("%s ", uthreads[active_threads[i]]->name);
    }
    printf("\n");

    // Если текущий поток не определен, возвращаем первый активный
    if (uthread_cur == -1) {
        printf("Scheduler: Current thread not defined, choosing the first active thread\n");
        return active_threads[0];
    }

    // Проверяем, входит ли текущий поток в список активных
    int current_thread_index = -1;
    for (int i = 0; i < active_count; i++) {
        if (active_threads[i] == uthread_cur) {
            current_thread_index = i;
            break;
        }
    }

    // Если текущий поток не активен, ищем следующий активный поток
    if (current_thread_index == -1) {
        printf("Scheduler: Current thread '%s' is not active, searching for the next active thread.\n", uthreads[uthread_cur]->name);

        // Ищем ближайший активный поток после текущего (по индексу)
        for (int i = 0; i < uthread_count; i++) {
            // Проверяем, является ли поток активным и идет ли он после текущего
            if (uthreads[i] != NULL && !uthreads[i]->finished) {
                if (i > uthread_cur) {
                    printf("Scheduler: Next active thread: %s\n", uthreads[i]->name);
                    return i;
                }
            }
        }

        // Если после текущего нет активных потоков, начинаем с первого активного потока
        printf("Scheduler: No active threads after '%s', restarting from the first active thread.\n", uthreads[uthread_cur]->name);
        return active_threads[0];
    }

    // Если текущий поток активен, выбираем следующий
    int next_thread_index = (current_thread_index + 1) % active_count;
    return active_threads[next_thread_index];
}


void scheduler(void) {
    printf("Scheduler: Start. Current thread: %s\n", (uthread_cur == -1) ? "None" : uthreads[uthread_cur]->name);

    int next_thread = find_next_thread();

    if (next_thread == -1) {
        printf("Scheduler: No active threads. Returning to main.\n");
        setcontext(&main_context);
        return;
    }

    uthread_t *prev_thread = (uthread_cur != -1) ? uthreads[uthread_cur] : NULL;
    uthread_t *next_uthread = uthreads[next_thread];

    printf("Scheduler: Switching from '%s' --> '%s'\n",
           prev_thread ? prev_thread->name : "Main",
           next_uthread->name);

    uthread_cur = next_thread;

    // Переключение контекста
    if (prev_thread && !prev_thread->finished) {
        swapcontext(&prev_thread->uctx, &next_uthread->uctx);
    } else {
        setcontext(&next_uthread->uctx);
    }
}

void thread_wrapper(void *arg) {
    uthread_t *thread = (uthread_t *)arg;

    if (thread->thread_func) {
        thread->thread_func(thread->arg);
    }

    thread->finished = 1;
    printf("Thread wrapper: '%s' finished\n", thread->name);

    if (thread->on_finish) {
        thread->on_finish(thread->arg);
    }

    // Переход к следующему потоку
    scheduler();
}

void uthread_create(uthread_t **ut, char *name,
                    void (*thread_func)(void *),
                    void *arg,
                    void (*on_finish)(void *)) {
    if (uthread_count >= MAX_THREADS) {
        fprintf(stderr, "Max threads limit reached\n");
        exit(1);
    }

    uthread_t *new_ut = malloc(sizeof(uthread_t));
    memset(new_ut, 0, sizeof(uthread_t));

    new_ut->stack = create_stack(STACK_SIZE);

    int ret = getcontext(&new_ut->uctx);
    if (ret == -1) {
        perror("getcontext failed");
        exit(1);
    }

    new_ut->uctx.uc_stack.ss_sp = new_ut->stack;
    new_ut->uctx.uc_stack.ss_size = STACK_SIZE;
    new_ut->uctx.uc_link = NULL;

    makecontext(&new_ut->uctx, (void (*)(void))thread_wrapper, 1, new_ut);

    strncpy(new_ut->name, name, NAME_SIZE - 1);
    new_ut->thread_func = thread_func;
    new_ut->arg = arg;
    new_ut->on_finish = on_finish;
    new_ut->finished = 0;

    uthreads[uthread_count++] = new_ut;
    *ut = new_ut;
}

void uthread_scheduler(int sig, siginfo_t *si, void *u) {
    printf("Signal handler: Received signal\n");
    alarm(1);
    scheduler();
}

void mythread1(void *arg) {
    char **args = (char **)arg;
    char *thread_name = args[0];
    int work_time = atoi(args[1]);

    printf("mythread: started: name '%s', work time %d seconds\n", thread_name, work_time);

    for (int i = 0; i < work_time; i++) {
        printf("mythread: name '%s' is working (%d/%d)\n", thread_name, i + 1, work_time);
        sleep(1);
    }

    printf("mythread: name '%s' finished\n", thread_name);
}

void thread_finished(void *arg) {
    char **args = (char **)arg;
    printf("Thread finished: %s\n", args[0]);
}

int main() {
    uthread_t *ut[MAX_THREADS];
    char *args[MAX_THREADS][2];

    char *preset_args[4][2] = {
        {"thread 1", "10"},
        {"thread 2", "3"},
        {"thread 3", "10"},
        {"thread 4", "5"}
    };

    for (int i = 0; i < 4; i++) {
        args[i][0] = strdup(preset_args[i][0]);
        args[i][1] = strdup(preset_args[i][1]);
    }

    // Создаем еще 96 потоков с временем работы 3 секунды
    for (int i = 4; i < MAX_THREADS; i++) {
        char name[NAME_SIZE];
        snprintf(name, NAME_SIZE, "thread %d", i + 1);

        args[i][0] = strdup(name);
        args[i][1] = strdup("3");
    }

    getcontext(&main_context);

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = uthread_scheduler;
    act.sa_flags = SA_SIGINFO;

    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction failed");
        return 1;
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        uthread_create(&ut[i], args[i][0], mythread1, args[i], thread_finished);
    }

    alarm(1);

    uthread_cur = 1;
    swapcontext(&main_context, &uthreads[1]->uctx);

    printf("Main: All threads finished, exiting...\n");
    return 0;
}
